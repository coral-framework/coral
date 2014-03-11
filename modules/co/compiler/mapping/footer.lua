local function template( writer, c, t )

	if t.kind ~= 'TK_NATIVECLASS' then
		writer( "\n" )
		c.utils.closeNamespaces( writer, t.namespace.fullName )
	end

	writer( [[

namespace co {
template<> struct kindOf<]], t.cppName, [[> : public kindOfBase<]], t.kind, [[> {};
template<> struct nameOf<]], t.cppName, [[> { static const char* get() { return "]], t.fullName, [["; } };
} // namespace co

#endif // _]], t.fullNameUpperUnderline, [[_H_
]] )

end

return template
