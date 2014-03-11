#include <co/RefPtr.h>
#include <co/IModulePart.h>
#include <co/reserved/LibraryManager.h>

extern "C" CORAL_DLL_EXPORT
const char* coral_module_query_verification_data()
{
	return  "pattern=CORAL_MODULE_VERIFICATION_DATA\n"
			"version=0.8.0\n"
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
		{ "lua.Universe", "9CDB6763-A7BD-F50D-70BD8BC6AD9A9F2C" },
		{ "lua.Launcher", "7F834A71-EAAB-D59E-B3033EF5A65E5039" },
		{ "lua.lua", "01D65DFD-7AA0-052B-DD549599474CDA86" },
		{ "co.IModulePartLoader", "8B980092-72DF-55EC-62E80071B0C8F068" },
		{ "co.IObject", "3D4B59E2-C5B2-853B-A6CCCDAB8623F1D5" },
		{ "co.ISystem", "CB0DACE2-D193-3523-559D84D4449BFC23" },
		{ "co.IArray", "FCA4443F-B957-450A-5A1023C9D0107A9C" },
		{ "lua.IInterceptor", "DDD3E015-D99A-7514-8AD032842617EFFA" },
		{ "lua.Component", "E497DE0A-29D3-751C-AE3781ABED8FF944" },
		{ "co.IPort", "AEC40CE7-1408-B51F-BB806FDB2462BD87" },
		{ "co.IMethod", "44B63D27-C983-A52D-C753EEED1B0E8BFE" },
		{ "co.IComponent", "D18EC513-AD23-F507-77ED6A93870ED06F" },
		{ "co.IDynamicTypeProvider", "F28777B9-4350-A550-56A76D2AB610247D" },
		{ "co.INamespace", "B8D7E517-710A-65B0-C2F038DEAE33D03C" },
		{ "co.IField", "6AB731E3-2DB4-0598-1BCAD486513C6848" },
		{ "lua.IState", "EFAF47D2-645C-75F5-CF472FB7BE502604" },
		{ "lua.ILauncher", "23A662E5-3C8C-8557-87D2FE9D63065340" },
		{ "co.IReflector", "0C8E90FF-D9DB-259F-EACD322FE8FC2322" },
		{ "co.INativeClass", "0DC8B4B4-CF31-65DD-24B3C5908D6E753B" },
		{ "co.IException", "B98A3083-6BE3-E589-8B062D3DF1D27D73" },
		{ "lua.IScripted", "83A12D1D-7486-A5A4-91AB2CD71C3C73B4" },
		{ "co.IInterface", "AF1AA505-F84E-D538-20A03A480574DA71" },
		{ "co.IStruct", "8BCD67AF-6A9E-A5CE-9B6F064422A7749E" },
		{ "co.IModulePart", "B9E33D80-2B2C-8569-8DCF30F02B0FA556" },
		{ "co.IDynamicServiceProvider", "0551D753-11FE-55B8-18F2586276A17951" },
		{ "lua.ModulePartLoader", "DDF98C90-C138-35CD-E18539A3465C9798" },
		{ "co.IEnum", "2DAA0A2A-E74E-25C3-5D692724983DE421" },
		{ "lua.ModulePart", "01D65DFD-7AA0-052B-DD549599474CDA86" },
		{ "lua.Exception", "E494AEF9-CB92-959B-260612E42B651281" },
		{ "lua.ScriptedAnnotation", "5F5A08CA-6992-25AD-BE3668A0DE92ED66" },
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
