#ifndef _MODULE_INSTALLER_H_
#define _MODULE_INSTALLER_H_

#include <co/RefPtr.h>
#include <co/IReflector.h>
#include <co/IModulePart.h>

namespace lua {

class ModuleInstaller
{
public:
	inline static ModuleInstaller& instance() { return sm_instance; }

	enum TypeId
	{
		TypeId_Component,
		TypeId_Exception,
		TypeId_IInterceptor,
		TypeId_ILauncher,
		TypeId_IScripted,
		TypeId_IState,
		TypeId_Launcher,
		TypeId_lua,
		TypeId_ModulePart,
		TypeId_ModulePartLoader,
		TypeId_ScriptedAnnotation,
		TypeId_Universe,
		TypeCount
	};

public:
	//! Instantiates Reflectors for the module types that still don't have an assigned IReflector.
	void initialize();

	//! Returns the IReflector that should be assigned to a module type.
	inline co::IReflector* getReflector( TypeId typeId ) { return _reflectors[typeId].get(); }

	//! Sets the IReflector instance that should be assigned to module type.
	//! \warning This method can only be called \b before calling install().
	inline void setReflector( TypeId typeId, co::IReflector* reflector ) { _reflectors[typeId] = reflector; }

	//! Assigns the reflectors to their corresponding co::IType instances.
	//! This method skips any co::IType instance that already has an assigned IReflector.
	void install();

	//! Removes references to the Reflectors from their corresponding co::IType instances.
	//! This ignores any co::IType instance whose current IReflector was not assigned by this class.
	void uninstall();

private:
	ModuleInstaller();
	~ModuleInstaller();

	co::IReflector* createReflector( TypeId typeId );

private:
	static ModuleInstaller sm_instance;

private:
	co::RefPtr<co::IReflector> _reflectors[TypeCount];
};

// Module's reflector creation functions:
co::IReflector* __createComponentReflector();
co::IReflector* __createExceptionReflector();
co::IReflector* __createIInterceptorReflector();
co::IReflector* __createILauncherReflector();
co::IReflector* __createIScriptedReflector();
co::IReflector* __createIStateReflector();
co::IReflector* __createLauncherReflector();
co::IReflector* __createluaReflector();
co::IReflector* __createModulePartReflector();
co::IReflector* __createModulePartLoaderReflector();
co::IReflector* __createScriptedAnnotationReflector();
co::IReflector* __createUniverseReflector();

} // namespace lua

#endif
