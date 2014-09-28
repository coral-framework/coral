
#include <co/RefPtr.h>
#include <co/IModulePart.h>
#include <co/reserved/LibraryManager.h>

extern "C" CORAL_DLL_EXPORT
const char* coral_module_query_verification_data()
{
	return  "pattern=CORAL_MODULE_VERIFICATION_DATA\n"
			"version=0.8.1\n"
			"buildkey=" CORAL_BUILD_KEY "\n"
			"buildmode=" CORAL_BUILD_MODE "\n";
}

struct TypeDependency
{
	const char* name;
	const char* binarySignature;
};

extern "C" CORAL_DLL_EXPORT
const TypeDependency* coral_module_query_dependencies()
{
	static const TypeDependency s_dependencies[] = {
		{ "lua.ScriptedAnnotation", "0B419A58-A802-ED23-EEE1503BDF350736" },
		{ "co.IStruct", "EA8AAABB-0B9C-9134-358828640D7BFA2A" },
		{ "lua.ModulePart", "F151DAE5-31A6-4C05-4D37C8A8B7259AE2" },
		{ "co.IReflector", "D968BAB6-FBFA-5BE2-9277194A10D96021" },
		{ "co.IModulePartLoader", "F58B8ADB-CC55-B208-1C54FBFBF779BA5A" },
		{ "co.IDynamicTypeProvider", "08023B76-C478-1673-EF20960645C3C14C" },
		{ "co.INativeClass", "9E7EABE4-7D53-A4F9-9B0E1EB817B5F456" },
		{ "co.IInterface", "19003250-27E6-0964-FDEF6E5451455999" },
		{ "lua.IScripted", "88922ADA-913F-148F-E0A88FB708F719AC" },
		{ "co.IComponent", "D39D9740-3C85-7FD2-61BEBC9BDF0CB65E" },
		{ "co.IException", "255453FB-6071-C969-2DFE3F39362B5AF8" },
		{ "co.INamespace", "26AD6E0C-E400-24EA-673AFD4D3E0ADF28" },
		{ "lua.Exception", "2A4CF6F1-2601-34DF-397CD132EAB1D510" },
		{ "lua.ILauncher", "7F1AC5FD-7188-7D96-6C0BCB0AA02400A6" },
		{ "lua.Universe", "1D3232C7-8F8D-348D-6355E9AF78B6130D" },
		{ "co.IEnum", "14861701-A9F5-4022-2DA4C208259D0B2C" },
		{ "lua.ModulePartLoader", "9581D641-B4CD-F5CE-F1F5F2689A9FF024" },
		{ "lua.lua", "F151DAE5-31A6-4C05-4D37C8A8B7259AE2" },
		{ "co.ISystem", "ACB03A3D-1ED8-E143-2C3C98D3FBE93BDF" },
		{ "lua.IInterceptor", "C80FD872-18BD-D54C-F1ABC37714CFFA81" },
		{ "co.IArray", "D88E1E35-C0C8-D4C8-291A2984FF9BCA13" },
		{ "co.IModulePart", "29CDC521-F960-21F2-4673B393807D7D09" },
		{ "lua.IState", "A439014A-3154-E0A9-E35F65B96CBCD403" },
		{ "lua.Launcher", "204DFA1F-3247-049C-7F6124DF853456B7" },
		{ "lua.Component", "B50C7C5F-5AB1-5726-AC2AEDCC2FA3AF19" },
		{ NULL, NULL }
	};
	return s_dependencies;
}

namespace lua {

// The module's IModulePart instance
co::RefPtr<co::IModulePart> sg_instance;

// The module's internal reference count
co::int32 sg_refCount( 0 );

void moduleRetain()
{
	++sg_refCount;
}

void moduleRelease()
{
	// is the module's IModulePart the only active reference?
	if( --sg_refCount == 1 )
	{
		assert( sg_instance.isValid() );
		co::LibraryManager::release( sg_instance.get() );
	}

	// module retain/release mismatch?
	assert( sg_refCount >= 0 );
}

// implemented by CORAL_EXPORT_MODULE_PART()
co::IModulePart* createModulePart();

} // namespace lua

extern "C" CORAL_DLL_EXPORT
co::IModulePart* coral_module_part_instance()
{
	if( !lua::sg_instance.isValid() )
	{
		assert( lua::sg_refCount == 0 );
		lua::sg_instance = lua::createModulePart();
	}
	return lua::sg_instance.get();
}
