local type = type
local assert = assert
local ipairs = ipairs
local tostring = tostring

local coICppBlock = co.Type "co.ICppBlock"
local coIDocumentation = co.Type "co.IDocumentation"

-- General Helpers

local function getDoc( t, memberName )
	local doc = t[coIDocumentation]
	if not doc then return '' end
	if memberName then
		return doc:getDocFor( memberName )
	else
		return doc.value
	end
end

-- Helpers for writing JSON
local escape = {["\\"]="\\\\", ['"']='\\"',['\r']='',['\n']='\\n',['\t']='\\t'}
local function write( writer, v )
	local tp = type( v )
	if tp == 'string' then
		writer( '"', ( v:gsub('["\\\r\n\t]', escape ) ), '"' )
	elseif tp == 'number' or tp == 'boolean' then
		writer( tostring( v ) )
	elseif tp == 'nil' then
		writer 'null'
	else
		error( "you can't write a " .. tp .. " directly to JSON" )
	end
end
local levelStack = { 0 } -- counts the number of elements per level
local function pushLevel( writer )
	levelStack[#levelStack + 1] = 0
end
local function popLevel( writer )
	levelStack[#levelStack] = nil
end
local function writeSeparator( writer )
	local count = levelStack[#levelStack] + 1
	levelStack[#levelStack] = count
	if count > 1 then writer ',' end
end
local function writeKey( writer, key )
	assert( type( key ) == 'string', "key must be a string" )
	writer( '"', key, '":' )
end
local function startArray( writer, key )
	writeSeparator( writer )
	if key then writeKey( writer, key ) end
	writer '['
	pushLevel( writer )
end
local function writeElem( writer, value )
	writeSeparator( writer )
	write( writer, value )
end
local function endArray( writer )
	writer ']'
	popLevel( writer )
end
local function startObject( writer, key )
	writeSeparator( writer )
	if key then writeKey( writer, key ) end
	writer '{'
	pushLevel( writer )
end
local function writePair( writer, key, value )
	writeSeparator( writer )
	writeKey( writer, key )
	write( writer, value )
end
local function writeDoc( writer, t, memberName )
	local doc = getDoc( t, memberName )
	writePair( writer, 'doc', doc )
end
local function writeCppBlock( writer, t )
	local cppBlock = t[coICppBlock]
	writePair( writer, 'cpp', cppBlock and cppBlock.value or "" )
end
local function endObject( writer )
	writer '}'
	popLevel( writer )
end

-- Exporters for each type kind
local export = {}

function export.TK_ENUM( writer, t )
	startArray( writer, 'identifiers')
	for i, id in ipairs( t.identifiers ) do
		startObject( writer )
		writePair( writer, 'id', id )
		writeDoc( writer, t, id )
		endObject( writer )
	end
	endArray( writer )
end

function export.TK_EXCEPTION( writer, t )
	-- empty
end

local function writeFields( writer, t )
	for i, field in ipairs( t.fields ) do
		startObject( writer )
		writePair( writer, 'kind', 'field' )
		writePair( writer, 'name', field.name )
		writePair( writer, 'type', field.type.fullName )
		writePair( writer, 'isReadOnly', field.isReadOnly )
		writeDoc( writer, t, field.name )
		endObject( writer )
	end
end

function export.TK_STRUCT( writer, t )
	startArray( writer, 'members' )
	writeFields( writer, t )
	endArray( writer )
end

local function writeMethods( writer, t )
	for i, method in ipairs( t.methods ) do
		startObject( writer )

		writePair( writer, 'kind', 'method' )
		writePair( writer, 'name', method.name )
		local ret = method.returnType
		writePair( writer, 'returnType', ret and ret.fullName or "void" )
		writeDoc( writer, t, method.name )

		startArray( writer, 'parameters' )
		for i, param in ipairs( method.parameters ) do
			startObject( writer )
			writePair( writer, 'name', param.name )
			writePair( writer, 'type', param.type.fullName )
			writePair( writer, 'mode', (param.isIn and 'in' or '')..(param.isOut and 'out' or '') )
			endObject( writer )
		end
		endArray( writer )

		startArray( writer, 'exceptions' )
		for i, ex in ipairs( method.exceptions ) do
			writeElem( writer, ex.fullName )
		end
		endArray( writer )

		endObject( writer )
	end
end

function export.TK_NATIVECLASS( writer, t )
	startArray( writer, 'members' )
	writeFields( writer, t )
	writeMethods( writer, t )
	endArray( writer )
	writeCppBlock( writer, t )
end

function export.TK_INTERFACE( writer, t )
	writePair( writer, 'base', t.baseType and t.baseType.fullName or nil )
	startArray( writer, 'members' )
	writeFields( writer, t )
	writeMethods( writer, t )
	endArray( writer )
	writeCppBlock( writer, t )
end

function export.TK_COMPONENT( writer, t )
	startArray( writer, 'members' )
	for i, port in ipairs( t.ports ) do
		startObject( writer )
		writePair( writer, 'kind', port.isFacet and 'facet' or 'receptacle' )
		writePair( writer, 'name', port.name )
		writePair( writer, 'type', port.type.fullName )
		writeDoc( writer, t, port.name )
		endObject( writer )
	end
	endArray( writer )
end

local function template( writer, c )
	startObject( writer )
	writePair( writer, 'name', c.moduleName )
	writePair( writer, 'datetime', c.utils.currentDateTime() )
	startArray( writer, 'types' )
	for i, t in ipairs( c.types ) do
		startObject( writer )
		writePair( writer, 'name', t.fullName )
		writePair( writer, 'kind', t.kindName )
		writeDoc( writer, t )
		export[t.kind]( writer, t )
		endObject( writer )
	end
	endArray( writer );
	endObject( writer )
end

return template
