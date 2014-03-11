-------------------------------------------------------------------------------
-- Coral Compiler Utility Functions
-------------------------------------------------------------------------------

local M = {}

function M.currentDateTime()
	return os.date "%Y-%m-%d %H:%M:%S"
end

function M.toCppName( coralTypeName )
	return coralTypeName:gsub( '%.', '::' )
end

local function toHeaderName( coralTypeName )
	return coralTypeName:gsub( '%.', '/' ) .. '.h'
end

M.toHeaderName = toHeaderName

local function openBlocks( writer, keyword, fullName )
	for name in fullName:gmatch( "([^%.]+)%.?" ) do
		writer( keyword, " ", name, " {\n" )
	end
end

M.openBlocks = openBlocks

function M.openNamespaces( writer, ns )
	openBlocks( writer, "namespace", ns )
end

local function closeBlocks( writer, keyword, fullName, it )
	if not it then
		it = fullName:gmatch( "([^%.]+)%.?" )
	end
	local name = it()
	if name then
		closeBlocks( writer, keyword, fullName, it )
		writer( "} // ", keyword, " ", name, "\n" )
	end
end

M.closeBlocks = closeBlocks

function M.closeNamespaces( writer, ns )
	closeBlocks( writer, "namespace", ns )
end

function M.formatAnyGet( toType, isOut )
	if toType.kind ~= 'TK_ANY' or isOut then
		return ".get< " .. ( isOut and M.formatOutput or M.formatInput )( toType ) .. " >()"
	end
	return ""
end

function M.formatAccessor( prefix, fieldName, suffix )
	return prefix .. fieldName:sub( 1, 1 ):upper() .. fieldName:sub( 2 ) .. ( suffix or "" )
end

function M.formatField( t )
	local kind = t.kind
	if kind == 'TK_ANY' then
		return "co::AnyValue"
	elseif kind == 'TK_ARRAY' then
		return "std::vector<" .. M.formatField( t.elementType ) .. ">"
	elseif kind == 'TK_INTERFACE' then
		return t.cppName .. "Ref"
	else
		return t.cppName
	end
end

function M.formatResult( t )
	if not t then return "void" end
	local kind = t.kind
	if kind == 'TK_INTERFACE' then
		return t.cppName .. "*"
	elseif kind == 'TK_ARRAY' then
		local elem = t.elementType
		if elem.kind == 'TK_INTERFACE' then
			return "co::TSlice<" .. elem.cppName .. "*>"
		else
			return "co::TSlice<" .. M.formatField( elem ) .. ">"
		end
	else
		return M.formatField( t )
	end
end

function M.formatInput( t )
	local kind = t.kind
	if kind == 'TK_ARRAY' then
		local elem = t.elementType
		return "co::Slice<" .. elem.cppName .. ( elem.kind == 'TK_INTERFACE' and '*' or '' ) .. ">"
	elseif kind == 'TK_INTERFACE' then
		return t.cppName .. "*"
	elseif kind == 'TK_ANY' or kind == 'TK_STRING'
		or kind == 'TK_STRUCT' or kind == 'TK_NATIVECLASS' then
		return "const " .. t.cppName .. "&"
	else -- ( kind >= co::TK_BOOL && kind <= co::TK_DOUBLE ) || kind == co::TK_ENUM
		return t.cppName
	end
end

function M.formatOutput( t )
	return M.formatField( t ) .. "&"
end

function M.includeHeader( t, header )
	if type( header ) ~= 'string' then
		header = toHeaderName( header.fullName )
	end
	local included = t.includedHeaders
	if not included[header] then
		included[header] = true
		included[#included + 1] = header
	end
end

function M.addForwardDecl( t, type )
	if t._type ~= type._type then
		t.forwardDeclTypes[type.fullName] = type
	end
end

------ includeType() ----------------------------------------------------------

local include = {}

function M.includeType( t, type )
	local f = include[type.kind]
	if f then f( t, type ) end
end

function include.TK_ANY( t, type )
	t:includeHeader( "co/Any.h" )
end

function include.TK_ARRAY( t, type )
	type = type.elementType
	if t.kind == 'TK_STRUCT' then
		-- force inclusion (instead of potential forward-decl)
		-- but only if the element type is not a basic type
		if include[type.kind] then
			t:includeHeader( type )
		end
	else
		t:includeType( type )
	end
end

function include.TK_ENUM( t, type )
	t:includeHeader( type )
end

include.TK_EXCEPTION = include.TK_ENUM

function include.TK_STRUCT( t, type )
	if t.kind == 'TK_STRUCT' then
		t:includeHeader( type )
	else
		t:addForwardDecl( type )
	end
end

local coIInclude = co.Type "co.IInclude"

function include.TK_NATIVECLASS( t, type )
	if t.kind == 'TK_STRUCT' then
		local include = type[coIInclude]
		if include then
			t:includeHeader( include.value )
		end
	end
	t:includeHeader( type )
end

function include.TK_INTERFACE( t, type )
	if t.kind == 'TK_STRUCT' then
		t:includeHeader( type )
	elseif t.kind == 'TK_INTERFACE' and t.isA( t._type, type._type ) then
		t:includeHeader( type ) -- t inherits from type
	else
		t:addForwardDecl( type )
	end
end

------ writeIncludesAndFwdDecls() ----------------------------------------------------------

function M.writeIncludesAndFwdDecls( t, writer )
	local headers = t.includedHeaders
	for i = 1, #headers do
		writer( "#include <", headers[i], ">\n" )
	end

	if next( t.forwardDeclTypes ) then
		writer( "\n// Forward Declarations:\n" )

		-- gather all type names into an array and sort them
		local fullTypeNames = {}
		for fullName, type in pairs( t.forwardDeclTypes ) do
			fullTypeNames[#fullTypeNames + 1] = fullName
		end

		table.sort( fullTypeNames )

		local depth = 0
		local lastNS = ''
		local indent = ''
		local s, e

		-- closes namespaces from 'lastNS' until it contains 'ns'
		local function closeNamespacesUntilItContains( ns )
			while true do
				s, e = ns:find( lastNS, 1, true )
				if s == 1 then break end
				local prefix, name = lastNS:match( '(.+)%.([^%.]+)$' )
				if prefix then
					lastNS = prefix
				else
					name = lastNS
					lastNS = ''
				end
				depth = depth - 1
				indent = ( "\t" ):rep( depth )
				writer( indent, "} // namespace ", name, "\n" )
			end
		end

		for _, fullName in ipairs( fullTypeNames ) do
			local type = t.forwardDeclTypes[fullName]
			local ns = type.namespace.fullName
			if ns ~= lastNS then
				closeNamespacesUntilItContains( ns )

				-- ns is within lastNS, just open the necessary namespaces
				local namespacesToOpen = ns:sub( e + 1 )
				for name in namespacesToOpen:gmatch( '([^%.]+)' ) do
					writer( indent, "namespace ", name, " {\n" )
					depth = depth + 1
					indent = ( '\t' ):rep( depth )
				end
				lastNS = ns
			end
			local name = type.name
			if type.kind == 'TK_STRUCT' then
				writer( indent, "struct ", name, ";\n" )
			else
				writer( indent, "class ", name, "; typedef co::RefPtr<", name, "> ", name, "Ref;\n" )
			end
		end

		closeNamespacesUntilItContains( '' )

		writer( "// End Of Forward Declarations\n" )
	end
end

return M
