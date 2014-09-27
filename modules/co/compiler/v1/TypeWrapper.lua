-------------------------------------------------------------------------------
-- A TypeWrapper is a smart proxy to a real co.IType instance. Its purpose is
-- to augment types with efficient access to extra (computed) fields and
-- methods, useful when writing template scripts.
-------------------------------------------------------------------------------

local utils = require "co.compiler.v1.utils"

local TypeWrapper = {}	-- the TypeWrapper class
local autoFields = {}	-- table of auto-field handlers
local wrappedTypes = {}	-- cache of TypeWrapper instances, indexed by type

-- Auxiliary function to wrap field values, so a chain of nested field
-- accesses will still return properly wrapped co.Types.
local GenericWrapper = {}
local wrappedObjects = {}
local function wrapValue( v )
	local typeOfV = type( v )
	if typeOfV == 'userdata' and v.facet then
		if v.namespace then
			v = TypeWrapper:wrap( v )
		else
			local wrapped = wrappedObjects[v]
			if not wrapped then
				wrapped = setmetatable( { v }, GenericWrapper )
				wrappedObjects[v] = wrapped
			end
			v = wrapped
		end
	elseif typeOfV == 'table' then
		for i = 1, #v do
			v[i] = wrapValue( v[i] )
		end
	end
	return v
end

function GenericWrapper.__index( obj, key )
	local v = wrapValue( obj[1][key] )
	obj[key] = v
	return v
end

function GenericWrapper.__tostring( obj )
	return "GenericWrapper(" .. tostring( obj[1] ) .. ")"
end

function TypeWrapper.__index( t, key )
	local v

	-- if the key is not a string, assume it's a type and get an annotation
	if type( key ) ~= 'string' then
		v = t._type:findAnnotation( key )
	else
		-- if key matches an auto field, compute it
		local autoField = autoFields[key]
		if autoField then
			v = autoField( t )
		else
			-- otherwise, if key matches a function in the 'utils' module, use it;
			v = utils[key]

			-- otherwise, resolve it as a member of the _type instance;
			if not v then
				v = wrapValue( t._type[key] )
			end
		end
	end

	-- cache the value for future accesses
	t[key] = v
	return v
end

function TypeWrapper.__tostring( t )
	return "TypeWrapper(" .. tostring( t._type ) .. ")"
end

function TypeWrapper:wrap( type )
	assert( type and type.namespace,  "wrap() expects a co.IType as argument" )
	local self = wrappedTypes[type]
	if not self then
		self = setmetatable( { _type = type }, TypeWrapper )
		wrappedTypes[type] = self
	end
	return self
end

-------------------------------------------------------------------------------
-- Auto-Field Handlers
-------------------------------------------------------------------------------

local typeKindToCpp = {
	TK_ANY		= "co::Any",
	TK_BOOL		= "bool",
	TK_INT8		= "co::int8",
	TK_INT16	= "co::int16",
	TK_INT32	= "co::int32",
	TK_INT64	= "co::int64",
	TK_DOUBLE	= "double",
	TK_STRING	= "std::string",
}

function autoFields.cppName( t )
	if t.kind == 'TK_ARRAY' then
		return t.elementType.cppName .. "[]"
	else
		return typeKindToCpp[t.kind] or utils.toCppName( t.fullName )
	end
end

local typeKindNames = {
	TK_ENUM			= "enum",
	TK_STRUCT		= "struct",
	TK_NATIVECLASS	= "native class",
	TK_INTERFACE	= "interface",
	TK_COMPONENT	= "component",
	TK_EXCEPTION	= "exception",
}

function autoFields.kindName( t )
	return typeKindNames[t.kind] or "unknown"
end

function autoFields.fullNameUnderline( t )
	-- my.type.Name -> my_type_Name
	return t.fullName:gsub( '%.', '_' )
end

function autoFields.fullNameUpperUnderline( t )
	-- my.type.Name -> MY_TYPE_NAME
	return t.fullName:upper():gsub( '%.', '_' )
end

local typeKindToInterfaceName = {
	TK_ARRAY		= "IArray",
	TK_ENUM			= "IEnum",
	TK_STRUCT		= "IStruct",
	TK_NATIVECLASS	= "INativeClass",
	TK_INTERFACE	= "IInterface",
	TK_COMPONENT	= "IComponent",
	TK_EXCEPTION	= "IException",
}

function autoFields.typeInterfaceName( t )
	return typeKindToInterfaceName[t.kind] or "IType"
end

function autoFields.headerName( t )
	return utils.toHeaderName( t.fullName )
end

function autoFields.fullSignatureStr( t )
	return t.fullSignature:getString()
end

local coIDynamicTypeProvider = co.Type "co.IDynamicTypeProvider"
function autoFields.isDynamic( t )
	return t[coIDynamicTypeProvider] ~= nil
end

function autoFields.hasReflector( t )
	return not t.isDynamic and t.kind ~= 'TK_ENUM'
end

function autoFields.includedHeaders( t )
	autoFields.traverse( t )
	return rawget( t, 'includedHeaders' )
end

function autoFields.forwardDeclTypes( t )
	autoFields.traverse( t )
	return rawget( t, 'forwardDeclTypes' )
end

function autoFields.typesNeededInReflector( t )
	local forwardDeclTypes = t.forwardDeclTypes
	if t.kind ~= 'TK_INTERFACE' then
		return forwardDeclTypes
	end

	-- copy forwardDeclTypes into a new table
	local neededTypes = {}
	for fullName, type in pairs( forwardDeclTypes ) do
		neededTypes[fullName] = type
	end

	local function addType( type )
		if type.kind == 'TK_ARRAY' then
			type = type.elementType
		end
		local kind = type.kind
		if kind == 'TK_STRUCT' or kind == 'TK_NATIVECLASS' or kind == 'TK_INTERFACE' then
			neededTypes[type.fullName] = type
		end
	end

	-- add forward-decl types from all supertypes
	for i, super in ipairs( t.superTypes ) do
		if super.fullName == 'co.IService' then
			break
		end

		for i, field in ipairs( super.fields ) do
			addType( field.type )
		end

		for i, method in ipairs( super.methods ) do
			local returnType = method.returnType
			if returnType then
				addType( returnType )
			end
			for i, param in ipairs( method.parameters ) do
				addType( param.type )
			end
		end
	end

	return neededTypes
end

-------------------------------------------------------------------------------
-- Helper traversal methods to gather data about a type
-------------------------------------------------------------------------------
local traverse = {}

function autoFields.traverse( t )
	t.includedHeaders = {}
	t.forwardDeclTypes = {}
	assert( traverse[t.kind] )( t )
end

function traverse.TK_ENUM( t )
	-- empty
end

function traverse.TK_EXCEPTION( t )
	t:includeHeader( "co/Exception.h" )
end

function traverse.TK_STRUCT( t )
	for i, field in ipairs( t.fields ) do
		t:includeType( field.type )
	end
end

local function traverseClassType( t )
	for i, a in ipairs( t.fields ) do
		t:includeType( a.type )
	end

	for i, m in ipairs( t.methods ) do
		local returnType = m.returnType
		if returnType then
			t:includeType( returnType )
		end
		for i, p in ipairs( m.parameters ) do
			t:includeType( p.type )
		end
	end
end

function traverse.TK_NATIVECLASS( t )
	t:includeHeader( t.headerName )
	traverseClassType( t )
end

function traverse.TK_INTERFACE( t )
	traverseClassType( t )
	if t.baseType then
		t:includeHeader( t.baseType )
	end
end

function traverse.TK_COMPONENT( t )
	for i, itf in ipairs( t.ports ) do
		t:includeHeader( itf.type )
	end
end

return TypeWrapper
