local function template( writer, c, t )
	c.header( writer, c, "C++ mapping generated for type '", t.fullName, "'" )

	writer( [[

#ifndef _]], t.fullNameUpperUnderline, [[_H_
#define _]], t.fullNameUpperUnderline, [[_H_

#include <co/Common.h>
]] )

	if t.kind == 'TK_NATIVECLASS' then
		return
	end

	t:writeIncludesAndFwdDecls( writer )
	writer( "\n// ", t.fullName, " Mapping:\n" )
	c.utils.openNamespaces( writer, t.namespace.fullName )
	writer( "\n" )
end

return template
