--------------------------------------------------------------------------------
-- The Lua module's initialization script
--------------------------------------------------------------------------------

-- localize commonly used functions
local type = type
local error = error
local pairs = pairs
local ipairs = ipairs
local tostring = tostring
local setmetatable = setmetatable
local strMatch = string.match
local tconcat = table.concat

local debug = require "debug"
local rawgetmetatable = debug.getmetatable

local co = co
local coNew = co.new
local coLogLM = co.logLM
local coGetType = co.getType
local coFindScript = co.findScript
local coTypeManager = co.system.types
local coTypeTransaction = coTypeManager.transaction
local coNewComponentType = co.newComponentType
local coNewComponentInstance = co.newComponentInstance

--------------------------------------------------------------------------------
-- Because this module script is executed manually, it must register itself.
--------------------------------------------------------------------------------

package.loaded.lua = true

--------------------------------------------------------------------------------
-- Set the Lua search path to our 'lua/packages' dir.
--------------------------------------------------------------------------------

local thisScriptFileName = ...
local thisModuleDir = thisScriptFileName:match( "(.+)__init%.lua$" )
local luaPackagesDir = thisModuleDir .. 'packages'
package.path = luaPackagesDir .. "/?.lua"

--------------------------------------------------------------------------------
-- Install our own loader function into Lua's package.searchers table
--------------------------------------------------------------------------------

table.insert( package.searchers, 2, function( moduleName )
	local filename = coFindScript( moduleName )
	if filename then
		local chunk, err = loadfile( filename )
		if err then
			error( err, 0 )
		end
		return chunk
	end
end )

--------------------------------------------------------------------------------
-- co.log( [level,] arg1, arg2, ... )
--------------------------------------------------------------------------------

local LogLevel = { 'INFO', 'WARNING', 'ERROR', 'FATAL' }

-- reverse mapping
for i, v in ipairs( LogLevel ) do
	LogLevel[v] = i
end

co.LogLevel = LogLevel

function co.log( level, message, catchExtras )
	if level == nil then error( "nothing to log", 2 ) end
	if catchExtras ~= nil then error( "co.log() takes only 2 arguments", 2 ) end
	if not message then
		level, message = 'WARNING', level
	end
	coLogLM( assert( LogLevel[level], "invalid log level" ), message )
end

--[[----------------------------------------------------------------------------

type = co.Type[typeName]

co.Type is a smart table that fetches and caches Coral types. It can be
indexed or called, always with a fully-qualified type name. For example:
	local type = co.Type[fullTypeName]
	local type = co.Type "my.InterfaceName"

]]

local coTypeMT = {}

function coTypeMT.__index( t, typeName )
	local res = coGetType( typeName )
	t[typeName] = res
	return res
end

function coTypeMT.__call( t, typeName )
	return t[typeName]
end

local coType = setmetatable( {}, coTypeMT )
co.Type = coType


--[[----------------------------------------------------------------------------

typeName = co.typeOf( coralValue )

Returns the fully-qualified type name of a Coral value. Works for Coral structs,
native classes, services and objects. For everything else, returns nil.

]]

local function coTypeOf( v )
	local res = rawgetmetatable( v )
	if res then
		res = res.__coraltype
	end
	return res
end

co.typeOf = coTypeOf


--[[----------------------------------------------------------------------------

co.raise( exceptionType, message )

Raises a Coral exception of the given type, with the specified message.

]]

co.raise = function( exceptionType, message )
	error( "{" .. exceptionType .. "}" .. message, 0 )
end

--[[----------------------------------------------------------------------------

exceptionType, message = co.getException( err )

Extracts the type and message of a Coral exception caught from Lua.
If 'err' is not a Coral exception, returns nil and tostring( err ).

]]

co.getException = function( err )
	local errStr = tostring( err )
	local exceptionType, message = strMatch( errStr, "^{(.-)}(.*)$" )
	return exceptionType, ( message or errStr )
end


--[[----------------------------------------------------------------------------

service = co.getService( serviceName[, client] )

Performs all kinds of service lookups: global, specialized by client type,
or specialized by client instance.

Parameter 'serviceName' is a fully-qualified interface type name (string),
indicating the kind of service you are trying to obtain.

If parameter 'client' is omitted, this function will attempt to get the
global provider of the specified type of service. Otherwise, \a client must
be either the name (string) or an instance of another service.

If \a client is an interface name (string), co.getService() will pick the most
specialized service available for clients of this type. Otherwise, if \a client
is a service, co.getService() will pick the most specialized service available
for the component instance providing the service, considering all its facets.

Examples:

	local itf = co.getService( "gfx.IDrawer" )
	Gets the global (or default) instance of 'gfx.IDrawer'. Much like a singleton.

	local itf = co.getService( "gfx.IDrawer", "obj.IPlayer" )
	Gets an instance of 'gfx.IDrawer' specialized for an 'obj.IPlayer'.

	local itf = co.getService( "gfx.IDrawer", obj )
	Gets an instance of 'gfx.IDrawer' specialized for this 'obj' instance.

]]

local serviceManager = co.system.services

function co.getService( serviceName, client )
	local serviceType = coType[serviceName]
	if not client then
		return serviceManager:getService( serviceType )
	elseif type( client ) == 'string' then
		return serviceManager:getServiceForType( serviceType, coType[client] )
	else
		return serviceManager:getServiceForInstance( serviceType, client )
	end
end


--[[----------------------------------------------------------------------------

componentPrototype = co.Component( desc )

Defines a Coral component implemented in Lua.

Receives a table describing the component, with three fields:
	desc.name = a fully-qualified type name for the component.
	desc.provides = a map of facet names to interface names.
	desc.receives = a map of receptacle names to interface names.
Either 'provides' or 'receives' can be omitted, but at least one port must
be defined. When providing both tables, beware of port name clashes.

The 'desc' table is turned into a component prototype table and returned as
the function result. A fresh component prototype table contains only the
component's facets, as facet prototype tables.

Prototype tables should be populated with Lua methods to implement the component.
A method will only affect a specific facet if it is added to the facet's prototype
table; conversely, it will affect all facets if added to the component's prototype
table. For example:
	local MyComponent = co.Component{ ... }
	function MyComponent:doSomething() print "all interfaces" end
	function MyComponent.someItf:doSomething() print "just someItf" end

A component can be instantiated by calling the component's prototype table. For example:
	local instance = MyComponent() -- either no args, or...
	local instance = MyComponent{} -- a table, which is re-used as the component instance.
	local instance = MyComponent{ initial = 1, values = 2, areOK = 3 }

It is also possible to instantiate a Lua component from C++ by using the component's
type reflector. Therefore, co::newInstance() should also work for Lua components.

]]

local function checkComponentInterfaces( provides, receives )
	local ok = false

	-- turn type names into type instances
	if provides then
		for k, v in pairs( provides ) do
			ok = true
			local t = coType[v]
			local r = t.reflector -- force reflectors to be loaded
			provides[k] = t
		end
	end

	if receives then
		for k, v in pairs( receives ) do
			ok = true
			receives[k] = coType[v]
			if provides and provides[k] then
				error( "receptacle '" .. k .. "' clashes with a facet name" )
			end
		end
	end

	if not ok then
		error( "no interface was specified" )
	end
end

local function defineComponentType( ns, typeName, provides, receives )
	local typeBuilder = ns:defineType( typeName, 'TK_COMPONENT' )

	if provides then
		for facetName, facetType in pairs( provides ) do
			typeBuilder:definePort( facetName, facetType, true )
		end
	end

	if receives then
		for receptacleName, receptacleType in pairs( receives ) do
			typeBuilder:definePort( receptacleName, receptacleType, false )
		end
	end

	return typeBuilder:createType()
end

local function handleComponentSpecification( t )
	local fullName = t.name
	if not fullName then
		error( "you must specify a component name", 3 )
	end

	local provides = t.provides
	local receives = t.receives
	local ok, err = pcall( checkComponentInterfaces, provides, receives )
	if not ok then
		error( "invalid component interfaces: " .. tostring( err ), 3 )
	end

	local nsName, typeName = fullName:match( '(.+)%.([^%.]+)$' )
	if not typeName then
		nsName = ''
		typeName = fullName
	end

	local ns = coTypeManager:getNamespace( nsName )
	if ns:findType( typeName ) then
		error( "component name '" .. fullName .. "' clashes with an existing type", 3 )
	end

	local ok, ct = pcall( defineComponentType, ns, typeName, provides, receives )
	if ok then
		coTypeTransaction:commit()
	else
		coTypeTransaction:rollback()
		error( "could not define a new component type: " .. tostring( ct ), 3 )
	end

	-- we reuse 't' as the component prototype table
	t.name = nil
	t.provides = nil
	t.receives = nil

	return ct
end

local ComponentMT = {}

function co.Component( t )
	local componentType

	local argType = type( t )
	if argType == 'table' then
		componentType = handleComponentSpecification( t )
	elseif argType == 'string' then
		componentType = coType[t]
		if componentType.kind ~= 'TK_COMPONENT' then
			error( "type '" .. t .. "' is not a component", 2 )
		end
		if componentType.currentReflector ~= nil then
			error( "component '" .. t .. "' already has a reflector and cannot be re-implemented", 2 )
		end
		t = {}
	else
		error( "illegal argument to co.Component()", 2 )
	end

	t.__reflector = coNewComponentType( componentType, t )

	-- a component prototype table is the MT for its facet and object tables
	t.__index = function( _, k ) return t[k] end

	-- create prototype tables for the facets
	for i, facet in ipairs( componentType.facets ) do
		t[facet.name] = setmetatable( {}, t )
	end

	return setmetatable( t, ComponentMT )
end

function ComponentMT.__call( componentTable, objectTable )
	objectTable = objectTable or {}

	local object = coNewComponentInstance( componentTable.__reflector, objectTable )
	objectTable.object = object

	setmetatable( objectTable, componentTable )

	local constructor = componentTable.__init
	if constructor then
		constructor( objectTable )
	end

	return object
end
