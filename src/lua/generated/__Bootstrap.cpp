
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
		{ "co.ISystem", "C013961B-C619-5ADE-4099585B87793227" },
		{ "co.IReflector", "3486D717-3C62-1264-E055C9262B5C1807" },
		{ "lua.IInterceptor", "1C54379C-1DCE-B0BA-AA965AE2E4236B86" },
		{ "lua.Component", "A36A742D-7FE2-E1DF-35248B7FD70BA610" },
		{ "lua.Exception", "72C713E7-D28F-7B48-17CEFABE287A12A5" },
		{ "co.IDynamicTypeProvider", "3D3DC782-8317-AA58-D6F995A33C5F58CA" },
		{ "co.IPort", "434EBDBB-27F2-1468-134A13B744743C04" },
		{ "co.IModulePartLoader", "684AA3C3-6259-B315-45ADE2FC7A7997D6" },
		{ "co.INamespace", "26669318-19CB-C96C-CE00D0302AAA04A1" },
		{ "lua.ILauncher", "AB5BCC33-77F8-BA66-89B5D572DE5D7409" },
		{ "co.IArray", "DE7AD38C-41BB-CCFD-13F42B94DB8319C6" },
		{ "co.INativeClass", "7A83D645-62F7-876C-B45D1433A14ED38F" },
		{ "co.IComponent", "CF4D0334-D002-520C-10D225319B386E2C" },
		{ "co.IInterface", "1393D9C9-F753-FFB8-26B38239AA4F6971" },
		{ "lua.Launcher", "0128854A-082B-5996-49B01DD65F652593" },
		{ "lua.ModulePartLoader", "A82CAF30-EF73-EA16-60037E5245E4DBB7" },
		{ "co.IStruct", "2FD6C6C0-12ED-3A37-D56734BD20AC248C" },
		{ "lua.ModulePart", "3588D0DB-0233-E7A4-AACA553A3C684549" },
		{ "co.IModulePart", "9AAF5D48-EB66-9BE7-80458A992D636279" },
		{ "co.IEnum", "F0F4A131-38C8-C274-DBB5F0C786D829D2" },
		{ "lua.IScripted", "9B34BCD4-E7C6-5153-C44367E4E38C9AFC" },
		{ "lua.ScriptedAnnotation", "C99281FA-929A-C827-7AFAD170E44E864B" },
		{ "co.IException", "7CE9E958-31B9-FCD4-9FAEA95716FE0311" },
		{ "lua.lua", "3588D0DB-0233-E7A4-AACA553A3C684549" },
		{ "lua.Universe", "F44BDB76-B3D4-F4D5-5CDDBB8C86E853D2" },
		{ "lua.IState", "94D3AEAA-BB3A-CEFD-03C57AD3CA9C798B" },
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
