local coICppBlock = co.Type "co.ICppBlock"

local function template( writer, c, t )
	writer( "class ", t.name, " : public ", t.baseType.cppName )
	writer( "\n{\npublic:\n\tvirtual ~", t.name, "() {;}" )

	local cppBlock = t[coICppBlock]
	if cppBlock then
		writer( "\n\n\t// Code From <c++ Block:\n" )
		writer( "\t", cppBlock.value, "\n" )
		writer( "\t// End Of c++> Block" )
	end

	-- Field Accessors
	for i, a in ipairs( t.fields ) do
		writer( "\n\n\tvirtual ", t.formatResult( a.type ), " ", t.formatAccessor( "get", a.name ), "() = 0;" )
		if not a.isReadOnly then
			writer( "\n\n\tvirtual void ", t.formatAccessor( "set", a.name ),
						"( ", t.formatInput( a.type ), " ", a.name, " ) = 0;" )
		end
	end

	-- Methods
	for i, m in ipairs( t.methods ) do
		writer( "\n\n\tvirtual ", t.formatResult( m.returnType ), " ", m.name, "(" )
		local params = m.parameters
		if #params > 0 then
			writer( " " )
			for i, p in ipairs( params ) do
				local paramType = ( p.isOut and t.formatOutput or t.formatInput )( p.type )
				writer( paramType, " ", p.name, ( i < #params and ', ' or '' ) )
			end
			writer( " " )
		end
		writer( ") = 0;" )
	end

	writer( "\n};\n\ntypedef co::RefPtr<", t.name,"> ", t.name, "Ref;\n" )
end

return template
