
#ifndef _CO_IMODULEPARTLOADER_H_
#define _CO_IMODULEPARTLOADER_H_

#include <co/Common.h>
#include <co/IService.h>

// Forward Declarations:
namespace co {
	class IModulePart; typedef co::RefPtr<IModulePart> IModulePartRef;
} // namespace co
// End Of Forward Declarations

// co.IModulePartLoader Mapping:
namespace co {

class IModulePartLoader : public co::IService
{
public:
	virtual ~IModulePartLoader() {;}

	virtual bool canLoadModulePart( const std::string& moduleName ) = 0;

	virtual co::IModulePart* loadModulePart( const std::string& moduleName ) = 0;
};

typedef co::RefPtr<IModulePartLoader> IModulePartLoaderRef;

} // namespace co

namespace co {
template<> struct kindOf<co::IModulePartLoader> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<co::IModulePartLoader> { static const char* get() { return "co.IModulePartLoader"; } };
} // namespace co

#endif // _CO_IMODULEPARTLOADER_H_
