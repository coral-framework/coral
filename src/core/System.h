#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include "System_Base.h"
#include "ServiceManager.h"
#include "modules/ModuleManager.h"
#include "types/TypeManager.h"

namespace co {

/*!
	Implements co.ISystem.
 */
class System : public System_Base
{
public:
	System();
	virtual ~System();

	// internal methods:
	void initialize();

	// ISystem methods:
	SystemState getState();
	ITypeManager* getTypes();
	IModuleManager* getModules();
	IServiceManager* getServices();
	void setupBase( Slice<std::string> requiredModules );
	void setupPresentation();
	void tearDown();

private:
	SystemState _state;
	RefPtr<TypeManager> _types;
	RefPtr<ModuleManager> _modules;
	RefPtr<ServiceManager> _services;
};

} // namespace co

#endif
