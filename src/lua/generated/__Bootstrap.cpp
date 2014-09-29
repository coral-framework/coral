
#include <co/RefPtr.h>
#include <co/IModulePart.h>
#include <co/private/LibraryManager.h>

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
		{ "co.INativeClass", "3cf167b40150ee87aff452b22c457c5d77683fe8" },
		{ "lua.IState", "86a4f7b6995b48a037babecd111beefa3bcd8521" },
		{ "lua.ILauncher", "d35a92568f3e2dc7daefb089e1663785900bd4a0" },
		{ "co.IPort", "3487e1598d90276712fbc4608ccf8e56a6c057af" },
		{ "co.IStruct", "1ab135e4a8960630e454a971cc804c948b964c8b" },
		{ "lua.ScriptedAnnotation", "2d711f2da3876ba10ebaa3e416c6738ba32a9571" },
		{ "co.IModulePart", "348bdaa4e607bad5aed7cc6cf1b2d62caf1cc4e1" },
		{ "lua.Universe", "8472e99df2e5e029a22da45e375da51fa88dcb0c" },
		{ "co.ISystem", "c302fd46baff72ade2f68481f770592efbefa172" },
		{ "lua.IScripted", "25baa08a335ec0f7e30490890df7cdf46d6403b9" },
		{ "lua.lua", "5c27d67ce0868ed6486c25ef2b5c8dbd0550d936" },
		{ "co.IEnum", "acb1fdadb3161a650f0063d2433c57819a0bd4ba" },
		{ "co.IModulePartLoader", "6869b745ac2c6888a83ed60ad86d192804da6b26" },
		{ "co.IDynamicServiceProvider", "d867747ddf23a40966787bcaefcdbebcdbdfaed9" },
		{ "co.IDynamicTypeProvider", "f51e8a8c0c8aea416a6bbc3f2aece5748f97b31c" },
		{ "lua.Component", "346f2d36372225292ff1948e17d9eb983b4da26b" },
		{ "co.IReflector", "6497091d3a4a54004ec979582afc7a63e6c1f0a9" },
		{ "co.IService", "a87005d5ef73f5396c8e802c66fd63f1a7a9f4f7" },
		{ "co.IInterface", "4030441a712ba9a609fd075ffdc4e747141e4d9b" },
		{ "lua.ModulePartLoader", "7f792a01e9a166935abd3da9f3eb57cf8fb8660d" },
		{ "lua.IInterceptor", "fcbd5ab2ee880dc8db5749efae67fc885cc129ad" },
		{ "co.IComponent", "3b61d8c4305e721fc7daad9a49e2f636e34d53ad" },
		{ "co.IException", "34fdee0f4ed6bfdedb692dc1aa933ff5bdc7700f" },
		{ "co.IArray", "65a4fa0f29c748976a5a72dc3cd58a423461f75d" },
		{ "lua.Exception", "2a4cf6f1260134df397cd132eab1d510ae9896c6" },
		{ "lua.ModulePart", "5c27d67ce0868ed6486c25ef2b5c8dbd0550d936" },
		{ "lua.Launcher", "c5ee5505b0659f1f42a0828832d102b487303d89" },
		{ "co.INamespace", "2b181c47dd791023d026edea0feea45738bef891" },
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
