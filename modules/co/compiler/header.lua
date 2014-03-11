local function template( writer, c, ... )

	if c.moduleName == 'co' or c.moduleName == 'lua' then
		return
	end

	writer( "/******************************************************************************\n** " )
	writer( ... )
	writer( [[

**
** Created: ]], c.utils.currentDateTime(), '\n', [[
**      by: Coral Compiler version ]], co.version, '\n', [[
**
** WARNING! All changes made in this file will be lost when recompiling!
******************************************************************************/
]] )

end

return template
