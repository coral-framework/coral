local function template( writer, c, sourceFiles )
	c.header( writer, c, "All-in-one source file generated for module '", c.moduleName, "'" )

	writer( "\n" )

	for i, sourceFile in ipairs( sourceFiles ) do
		writer( '#include "', sourceFile, '"\n' )
	end

end

return template
