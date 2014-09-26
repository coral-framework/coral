local coIInclude = co.Type "co.IInclude"

local function template( writer, c, t )
	c.header( writer, c, "Adapter generated for native class '", t.fullName, "'" )

	writer( [[

#ifndef _]], t.fullNameUpperUnderline, [[_ADAPTER_H_
#define _]], t.fullNameUpperUnderline, [[_ADAPTER_H_

]] )

	local include = t[coIInclude]
	if include then
		writer( "#include <", include.value, "> // added by co.Include()\n\n" )
	end

	t:writeIncludesAndFwdDecls( writer )

	writer( "\n" )

	c.utils.openNamespaces( writer, c.moduleName )

	writer( [[

/*!
	Implement this class to adapt ']], t.cppName, [[' to the Coral type system.
 */
class ]], t.name, [[_Adapter
{
public:
]] )

	if #t.fields > 0 then
		writer( "\t//------ Fields ------\n" )
		for i, a in ipairs( t.fields ) do
			writer( "\n\tstatic ", t.formatResult( a.type ), " ", t.formatAccessor( "get", a.name ), "( ", t.cppName, "& instance );\n" )
			if not a.isReadOnly then
				writer( "\tstatic void ", t.formatAccessor( "set", a.name ), "( ", t.cppName,
					"& instance, ", t.formatInput( a.type ), " value );\n" )
			end
		end
	end

	if #t.methods > 0 then
		writer( "\n\t//------ Methods ------\n" )
		for i, m in ipairs( t.methods ) do
			writer( "\n\tstatic ", t.formatResult( m.returnType ), " ", m.name, "( ", t.cppName, "& instance" )
			for i, p in ipairs( m.parameters ) do
				writer( ", ", ( p.isOut and t.formatOutput or t.formatInput)( p.type ), " ", p.name )
			end
			writer( " );" )
		end
	end

	writer( "\n};\n\n" )

	c.utils.closeNamespaces( writer, c.moduleName )

	writer( [[

#endif // _]], t.fullNameUpperUnderline, [[_ADAPTER_H_
]] )

end

return template
