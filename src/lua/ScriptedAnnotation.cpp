#include "LuaState.h"
#include "ScriptedAnnotation_Base.h"
#include <lua/Exception.h>
#include <sstream>

namespace lua {

class ScriptedAnnotation : public ScriptedAnnotation_Base
{
public:
	ScriptedAnnotation()
	{
		// empty constructor
	}

	virtual ~ScriptedAnnotation()
	{
		// empty destructor
	}

	std::string getValue()
	{
		return _scriptName;
	}

	void setValue( const std::string& value )
	{
		_scriptName = value;
	}

	void provideReflectorFor( co::IType* type )
	{
		if( !type || type->getKind() != co::TK_COMPONENT )
			throw lua::Exception( "this annotation can only be added to components" );

		if( _scriptName.empty() )
			throw lua::Exception( "this annotation requires a script name as argument" );

		assert( type->getCurrentReflector() == NULL );

		lua_State* L = LuaState::getL();
		LuaState::require( L, _scriptName );
		lua_pop( L, 1 );

		if( type->getCurrentReflector() == NULL )
			CORAL_THROW( lua::Exception, "script '" << _scriptName
				<< "' does not implement '" << type->getFullName() << "'" );
	}

private:
	std::string _scriptName;
};

CORAL_EXPORT_COMPONENT( ScriptedAnnotation, ScriptedAnnotation );

} // namespace lua
