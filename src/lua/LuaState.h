#ifndef _LUASTATE_H_
#define _LUASTATE_H_

#include "Universe_Base.h"
#include <lua.hpp>

namespace lua {

/*!
	A static component class that manages a Lua universe and
	provides methods to exchange values between Lua and Coral.
 
	All instances of this component share the same state.
 
	\warning
	All functions taking a lua_State in this module must be called with either
	the lua_State returned by LuaState::getL(), or a Lua thread	created by it.
	Currently we do NOT support working with multiple independent lua_States.
 */
class LuaState : public Universe_Base
{
public:
	//! Returns our Lua universe (a main thread).
	static inline lua_State* getL() { return sm_L; }

	//! Creates and sets up our Lua universe.
	static void setup();

	//! Tears down our Lua universe, releasing all memory.
	static void tearDown();

	//! Debug method: prints the Lua stack to stdout.
	static void dumpStack( lua_State* L );

	/*!
		Looks for a Lua script with the given \a name in the Coral path.
		The search follows the same conventions used for locating Lua modules.
		Returns false if there is no script with the given name.
	 */
	static bool findScript( lua_State*L, const std::string& name, std::string& filename );

	//! Pushes a chunk loaded from a file. On error, raises a lua::Exception.
	static void loadFile( lua_State* L, const std::string& filename );

	/*!
		Pushes the result of <tt>require( moduleName )</tt> in Lua.
		\throw lua::Exception if \c require() signals an error.
	 */
	static void require( lua_State* L, const std::string& moduleName );

	/*!
		Calls a Lua function using the same conventions as lua_call().
		On error, raises a lua::Exception.
	 */
	static void call( lua_State* L, int numArgs, int numResults );

	//! Pushes any Coral value onto the Lua stack.
	//@{
	static void push( lua_State* L, co::Any var );
	static void push( lua_State* L, const std::string& str );
	static void push( lua_State* L, co::IService* itf );
	static void push( lua_State* L, co::IObject* object );
	//@}

	/*!
		Puts the value at \a index in the Lua stack into \a var.
		\throw lua::Exception if the value cannot be converted to a Coral value.
	 */
	static void get( lua_State* L, int index, const co::Any& var );

public:
    LuaState();
	virtual ~LuaState();

	co::TSlice<lua::IInterceptor*> getInterceptors();
	void addInterceptor( lua::IInterceptor* interceptor );
	void removeInterceptor( lua::IInterceptor* interceptor );

	bool findScript( const std::string& name, std::string& filename );

	co::int32 call( const std::string& moduleName, const std::string& functionName,
		co::Slice<co::Any> args, co::Slice<co::Any> results );

	void collectGarbage();

private:
	template<typename BindingClass, typename InstanceType>
	static inline void pushInstance( lua_State* L, InstanceType* ptr );

	static void pushInstancesTable( lua_State* L );

	static void pushArray( lua_State* L, const co::Any& var );
	static void getArray( lua_State* L, int index, const co::Any& array );
	
	static co::int32 getEnumIdentifier( lua_State* L, int index, co::IEnum* enumType );

	static void checkType( lua_State* L, int index, int expectedType );
	static void raiseException( lua_State* L, int errorCode );

private:
	static lua_State* sm_L;
	static int sm_instancesTableRegIdx;
};

} // namespace lua

#endif
