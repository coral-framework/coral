local function template( writer, c )
	c.header( writer, c, "Bootstrap functions generated for module '", c.moduleName, "'" )

	writer( [[

#include <co/RefPtr.h>
#include <co/IModulePart.h>
#include <co/reserved/LibraryManager.h>

]] )

	c.revisionCheck( writer, c )

	writer( [[
extern "C" CORAL_DLL_EXPORT
const char* coral_module_query_verification_data()
{
	return  "pattern=CORAL_MODULE_VERIFICATION_DATA\n"
			"version=]], co.version, [[\n"
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
]] )

	for t in pairs( c.dependencies ) do
		writer( '\t\t{ "', t.fullName, '", "', t.binarySignature:getString(), '" },\n' )
	end

	writer( [[
		{ NULL, NULL }
	};
	return s_dependencies;
}

]] )

	c.utils.openNamespaces( writer, c.moduleName )

	writer( [[

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

]] )

	c.utils.closeNamespaces( writer, c.moduleName )

	writer( [[

extern "C" CORAL_DLL_EXPORT
co::IModulePart* coral_module_part_instance()
{
	if( !]], c.moduleNS, [[::sg_instance.isValid() )
	{
		assert( ]], c.moduleNS, [[::sg_refCount == 0 );
		]], c.moduleNS, [[::sg_instance = ]], c.moduleNS, [[::createModulePart();
	}
	return ]], c.moduleNS, [[::sg_instance.get();
}
]] )
end

return template
