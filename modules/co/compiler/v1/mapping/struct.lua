local function estimateFieldSize( t )
	local predefinedSizes = {
		TK_ANY			= co.Type.any.reflector.size,
		TK_BOOL			= co.Type.bool.reflector.size,
		TK_INT8			= co.Type.int8.reflector.size,
		TK_INT16		= co.Type.int16.reflector.size,
		TK_INT32		= co.Type.int32.reflector.size,
		TK_INT64		= co.Type.int64.reflector.size,
		TK_DOUBLE		= co.Type.double.reflector.size,
		TK_STRING		= co.Type.string.reflector.size,
		TK_ARRAY		= co.Type["string[]"].reflector.size,
		TK_ENUM			= co.Type["co.TypeKind"].reflector.size,
		TK_NATIVECLASS	= 4 * 8, -- average size
		TK_INTERFACE	= ( co.buildKey:find( "x86_64" ) and 8 or 4 ),
	}

	local kind = t.kind
	if kind == 'TK_STRUCT' then
		local res = 0
		for i, a in ipairs( t.fields ) do
			res = res + estimateFieldSize( a.type )
		end
		return res
	else
		return predefinedSizes[kind]
	end
end

local function template( writer, c, t )
	writer( "struct ", t.name, "\n{\n" )

	local fields = t.fields
	local fieldSizes = {}
	local sortedIndices = {}
	for i, field in ipairs( fields ) do
		sortedIndices[i] = i
		fieldSizes[i] = estimateFieldSize( field.type )
	end

	-- sort fields by size, descending
	table.sort( sortedIndices, function( a, b )
		return fieldSizes[a] > fieldSizes[b]
	end )

	for _, i in ipairs( sortedIndices ) do
		local field = fields[i]
		writer( "\t", t.formatField( field.type ), " ", field.name, "; // estimated size of ", fieldSizes[i], " bytes\n" )
	end

	writer( "\n\t//! Default constructor.\n\t", t.name, "() :\n" )
	for ii, i in ipairs( sortedIndices ) do
		local field = fields[i]
		writer( "\t\t", field.name, "()", ( ii < #sortedIndices and ",\n" or "\n" ) )
	end
	writer( [[
	{;}

	//! Equality test operator.
	inline bool operator==( const ]], t.name, [[& o ) const
	{
		return (
]] )

	for ii, i in ipairs( sortedIndices ) do
		local field = fields[i]
		writer( "\t\t\t", field.name, " == o.", field.name, ( ii < #sortedIndices and " &&\n" or "\n" ) )
	end

	writer( [[
		);
	}

	//! Inequality test operator.
	inline bool operator!=( const ]], t.name, [[& o ) const { return !( *this == o ); }

	//! ADL-based overload for swap() calls.
	friend inline void swap( ]], t.name, [[& a, ]], t.name, [[& b )
	{
		using std::swap;
]] )

	for ii, i in ipairs( sortedIndices ) do
		local field = fields[i]
		writer( "\t\tswap( a.", field.name, ", b.", field.name, " );\n" )
	end

	writer( [[
	}
};
]] )

end

return template
