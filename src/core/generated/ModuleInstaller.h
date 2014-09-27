
#ifndef _MODULE_INSTALLER_H_
#define _MODULE_INSTALLER_H_

#include <co/RefPtr.h>
#include <co/IReflector.h>
#include <co/IModulePart.h>

namespace co {

class ModuleInstaller
{
public:
	inline static ModuleInstaller& instance() { return sm_instance; }

	enum TypeId
	{
		TypeId_ArrayType,
		TypeId_Component,
		TypeId_CppBlockAnnotation,
		TypeId_CSLError,
		TypeId_DocumentationAnnotation,
		TypeId_Enum,
		TypeId_ExceptionType,
		TypeId_Field,
		TypeId_IAnnotated,
		TypeId_IAnnotation,
		TypeId_IArray,
		TypeId_IClassType,
		TypeId_IComponent,
		TypeId_ICompositeType,
		TypeId_ICppBlock,
		TypeId_IDocumentation,
		TypeId_IDynamicServiceProvider,
		TypeId_IDynamicTypeProvider,
		TypeId_IEnum,
		TypeId_IException,
		TypeId_IField,
		TypeId_IInclude,
		TypeId_IInterface,
		TypeId_IllegalArgumentException,
		TypeId_IllegalCastException,
		TypeId_IllegalNameException,
		TypeId_IllegalStateException,
		TypeId_IMember,
		TypeId_IMethod,
		TypeId_IMethodBuilder,
		TypeId_IModule,
		TypeId_IModuleManager,
		TypeId_IModulePart,
		TypeId_IModulePartLoader,
		TypeId_INamespace,
		TypeId_INativeClass,
		TypeId_IncludeAnnotation,
		TypeId_Interface,
		TypeId_IObject,
		TypeId_IParameter,
		TypeId_IPort,
		TypeId_IRecordType,
		TypeId_IReflector,
		TypeId_IService,
		TypeId_IServiceManager,
		TypeId_IStruct,
		TypeId_ISystem,
		TypeId_IType,
		TypeId_ITypeBuilder,
		TypeId_ITypeManager,
		TypeId_ITypeTransaction,
		TypeId_Method,
		TypeId_MethodBuilder,
		TypeId_MissingInputException,
		TypeId_MissingServiceException,
		TypeId_Module,
		TypeId_ModuleLoadException,
		TypeId_ModuleManager,
		TypeId_ModulePartLoader,
		TypeId_Namespace,
		TypeId_NativeClass,
		TypeId_NoSuchPortException,
		TypeId_NotSupportedException,
		TypeId_Parameter,
		TypeId_Port,
		TypeId_ServiceManager,
		TypeId_Struct,
		TypeId_System,
		TypeId_Type,
		TypeId_TypeBuilder,
		TypeId_TypeLoadException,
		TypeId_TypeManager,
		TypeId_TypeTransaction,
		TypeId_Uuid,
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
co::IReflector* __createArrayTypeReflector();
co::IReflector* __createComponentReflector();
co::IReflector* __createCppBlockAnnotationReflector();
co::IReflector* __createCSLErrorReflector();
co::IReflector* __createDocumentationAnnotationReflector();
co::IReflector* __createEnumReflector();
co::IReflector* __createExceptionTypeReflector();
co::IReflector* __createFieldReflector();
co::IReflector* __createIAnnotatedReflector();
co::IReflector* __createIAnnotationReflector();
co::IReflector* __createIArrayReflector();
co::IReflector* __createIClassTypeReflector();
co::IReflector* __createIComponentReflector();
co::IReflector* __createICompositeTypeReflector();
co::IReflector* __createICppBlockReflector();
co::IReflector* __createIDocumentationReflector();
co::IReflector* __createIDynamicServiceProviderReflector();
co::IReflector* __createIDynamicTypeProviderReflector();
co::IReflector* __createIEnumReflector();
co::IReflector* __createIExceptionReflector();
co::IReflector* __createIFieldReflector();
co::IReflector* __createIIncludeReflector();
co::IReflector* __createIInterfaceReflector();
co::IReflector* __createIllegalArgumentExceptionReflector();
co::IReflector* __createIllegalCastExceptionReflector();
co::IReflector* __createIllegalNameExceptionReflector();
co::IReflector* __createIllegalStateExceptionReflector();
co::IReflector* __createIMemberReflector();
co::IReflector* __createIMethodReflector();
co::IReflector* __createIMethodBuilderReflector();
co::IReflector* __createIModuleReflector();
co::IReflector* __createIModuleManagerReflector();
co::IReflector* __createIModulePartReflector();
co::IReflector* __createIModulePartLoaderReflector();
co::IReflector* __createINamespaceReflector();
co::IReflector* __createINativeClassReflector();
co::IReflector* __createIncludeAnnotationReflector();
co::IReflector* __createInterfaceReflector();
co::IReflector* __createIObjectReflector();
co::IReflector* __createIParameterReflector();
co::IReflector* __createIPortReflector();
co::IReflector* __createIRecordTypeReflector();
co::IReflector* __createIReflectorReflector();
co::IReflector* __createIServiceReflector();
co::IReflector* __createIServiceManagerReflector();
co::IReflector* __createIStructReflector();
co::IReflector* __createISystemReflector();
co::IReflector* __createITypeReflector();
co::IReflector* __createITypeBuilderReflector();
co::IReflector* __createITypeManagerReflector();
co::IReflector* __createITypeTransactionReflector();
co::IReflector* __createMethodReflector();
co::IReflector* __createMethodBuilderReflector();
co::IReflector* __createMissingInputExceptionReflector();
co::IReflector* __createMissingServiceExceptionReflector();
co::IReflector* __createModuleReflector();
co::IReflector* __createModuleLoadExceptionReflector();
co::IReflector* __createModuleManagerReflector();
co::IReflector* __createModulePartLoaderReflector();
co::IReflector* __createNamespaceReflector();
co::IReflector* __createNativeClassReflector();
co::IReflector* __createNoSuchPortExceptionReflector();
co::IReflector* __createNotSupportedExceptionReflector();
co::IReflector* __createParameterReflector();
co::IReflector* __createPortReflector();
co::IReflector* __createServiceManagerReflector();
co::IReflector* __createStructReflector();
co::IReflector* __createSystemReflector();
co::IReflector* __createTypeReflector();
co::IReflector* __createTypeBuilderReflector();
co::IReflector* __createTypeLoadExceptionReflector();
co::IReflector* __createTypeManagerReflector();
co::IReflector* __createTypeTransactionReflector();
co::IReflector* __createUuidReflector();

} // namespace co

#endif
