
#ifndef _CO_ISYSTEM_H_
#define _CO_ISYSTEM_H_

#include <co/Common.h>
#include <co/SystemState.h>
#include <co/IService.h>

// Forward Declarations:
namespace co {
	class IModuleManager; typedef co::RefPtr<IModuleManager> IModuleManagerRef;
	class IServiceManager; typedef co::RefPtr<IServiceManager> IServiceManagerRef;
	class ITypeManager; typedef co::RefPtr<ITypeManager> ITypeManagerRef;
} // namespace co
// End Of Forward Declarations

// co.ISystem Mapping:
namespace co {

class ISystem : public co::IService
{
public:
	virtual ~ISystem() {;}

	// Code From <c++ Block:
	
		// Sets the system up and running without loading any modules.
		inline void setup() { setup( Slice<std::string>() ); }

		// Sets the system up and running with the specified modules.
		inline void setup( Slice<std::string> requiredModules )
		{
			setupBase( requiredModules );
			setupPresentation();
		}
	
	// End Of c++> Block

	virtual co::IModuleManager* getModules() = 0;

	virtual co::IServiceManager* getServices() = 0;

	virtual co::SystemState getState() = 0;

	virtual co::ITypeManager* getTypes() = 0;

	virtual void setupBase( co::Slice<std::string> requiredModules ) = 0;

	virtual void setupPresentation() = 0;

	virtual void tearDown() = 0;
};

typedef co::RefPtr<ISystem> ISystemRef;

} // namespace co

namespace co {
template<> struct kindOf<co::ISystem> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<co::ISystem> { static const char* get() { return "co.ISystem"; } };
} // namespace co

#endif // _CO_ISYSTEM_H_
