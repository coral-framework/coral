local function template( writer, c, t )
	if c.moduleName == 'co' or c.moduleName == 'lua' then
		return
	end

	writer( [[
#if !defined( CORAL_COMPILER_OUTPUT_REVISION )
#error "The header file <co/Config.h> was not included."
#elif CORAL_COMPILER_OUTPUT_REVISION != ]], co.compilerOutputRevision, "\n", [[
#error "This file was generated using the Coral Compiler v]], co.version, [[. It"
#error "cannot be used with the include files from this version of Coral."
#error "(The code generation rules have changed too much.)"
#endif

]] )
end

return template
