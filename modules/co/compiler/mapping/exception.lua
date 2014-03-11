local function template( writer, c, t )
	writer( [[
class CORAL_EXPORT_EXCEPTION ]], t.name, [[ : public co::Exception
{
public:
	]], t.name, [[()
	{;}

	]], t.name, [[( const std::string& message )
		: co::Exception( message )
	{;}

	virtual ~]], t.name, [[() throw()
	{;}

	inline const char* getTypeName() const { return "]], t.fullName, [["; }
};
]] )
end

return template
