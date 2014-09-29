
#include "ModuleInstaller.h"
#include <co/IType.h>

co::ModuleInstaller co::ModuleInstaller::sm_instance;

co::ModuleInstaller::ModuleInstaller()
{
	// empty
}

co::ModuleInstaller::~ModuleInstaller()
{
	// empty
}

void co::ModuleInstaller::initialize()
{
	for( unsigned int i = 0; i < TypeCount; ++i )
	{
		if( !_reflectors[i].isValid() )
			_reflectors[i] = createReflector( static_cast<TypeId>( i ) );
	}
}

void co::ModuleInstaller::install()
{
	initialize();

	for( unsigned int i = 0; i < TypeCount; ++i )
	{
		co::IReflector* reflector = _reflectors[i].get();
		reflector->getType()->setReflector( reflector );
	}
}

void co::ModuleInstaller::uninstall()
{
	for( unsigned int i = 0; i < TypeCount; ++i )
	{
		if( _reflectors[i].isValid() )
		{
			_reflectors[i]->getType()->setReflector( NULL );
			_reflectors[i] = NULL;
		}
	}
}

co::IReflector* co::ModuleInstaller::createReflector( TypeId typeId )
{
	co::IReflector* res = NULL;
	switch( typeId )
	{
	case TypeId_ArrayType: res = co::__createArrayTypeReflector(); break;
	case TypeId_Component: res = co::__createComponentReflector(); break;
	case TypeId_CppBlockAnnotation: res = co::__createCppBlockAnnotationReflector(); break;
	case TypeId_CSLError: res = co::__createCSLErrorReflector(); break;
	case TypeId_DocumentationAnnotation: res = co::__createDocumentationAnnotationReflector(); break;
	case TypeId_Enum: res = co::__createEnumReflector(); break;
	case TypeId_ExceptionType: res = co::__createExceptionTypeReflector(); break;
	case TypeId_Field: res = co::__createFieldReflector(); break;
	case TypeId_IAnnotated: res = co::__createIAnnotatedReflector(); break;
	case TypeId_IAnnotation: res = co::__createIAnnotationReflector(); break;
	case TypeId_IArray: res = co::__createIArrayReflector(); break;
	case TypeId_IClassType: res = co::__createIClassTypeReflector(); break;
	case TypeId_IComponent: res = co::__createIComponentReflector(); break;
	case TypeId_ICompositeType: res = co::__createICompositeTypeReflector(); break;
	case TypeId_ICppBlock: res = co::__createICppBlockReflector(); break;
	case TypeId_IDocumentation: res = co::__createIDocumentationReflector(); break;
	case TypeId_IDynamicServiceProvider: res = co::__createIDynamicServiceProviderReflector(); break;
	case TypeId_IDynamicTypeProvider: res = co::__createIDynamicTypeProviderReflector(); break;
	case TypeId_IEnum: res = co::__createIEnumReflector(); break;
	case TypeId_IException: res = co::__createIExceptionReflector(); break;
	case TypeId_IField: res = co::__createIFieldReflector(); break;
	case TypeId_IInclude: res = co::__createIIncludeReflector(); break;
	case TypeId_IInterface: res = co::__createIInterfaceReflector(); break;
	case TypeId_IllegalArgumentException: res = co::__createIllegalArgumentExceptionReflector(); break;
	case TypeId_IllegalCastException: res = co::__createIllegalCastExceptionReflector(); break;
	case TypeId_IllegalNameException: res = co::__createIllegalNameExceptionReflector(); break;
	case TypeId_IllegalStateException: res = co::__createIllegalStateExceptionReflector(); break;
	case TypeId_IMember: res = co::__createIMemberReflector(); break;
	case TypeId_IMethod: res = co::__createIMethodReflector(); break;
	case TypeId_IMethodBuilder: res = co::__createIMethodBuilderReflector(); break;
	case TypeId_IModule: res = co::__createIModuleReflector(); break;
	case TypeId_IModuleManager: res = co::__createIModuleManagerReflector(); break;
	case TypeId_IModulePart: res = co::__createIModulePartReflector(); break;
	case TypeId_IModulePartLoader: res = co::__createIModulePartLoaderReflector(); break;
	case TypeId_INamespace: res = co::__createINamespaceReflector(); break;
	case TypeId_INativeClass: res = co::__createINativeClassReflector(); break;
	case TypeId_IncludeAnnotation: res = co::__createIncludeAnnotationReflector(); break;
	case TypeId_Interface: res = co::__createInterfaceReflector(); break;
	case TypeId_IObject: res = co::__createIObjectReflector(); break;
	case TypeId_IParameter: res = co::__createIParameterReflector(); break;
	case TypeId_IPort: res = co::__createIPortReflector(); break;
	case TypeId_IRecordType: res = co::__createIRecordTypeReflector(); break;
	case TypeId_IReflector: res = co::__createIReflectorReflector(); break;
	case TypeId_IService: res = co::__createIServiceReflector(); break;
	case TypeId_IServiceManager: res = co::__createIServiceManagerReflector(); break;
	case TypeId_IStruct: res = co::__createIStructReflector(); break;
	case TypeId_ISystem: res = co::__createISystemReflector(); break;
	case TypeId_IType: res = co::__createITypeReflector(); break;
	case TypeId_ITypeBuilder: res = co::__createITypeBuilderReflector(); break;
	case TypeId_ITypeManager: res = co::__createITypeManagerReflector(); break;
	case TypeId_ITypeTransaction: res = co::__createITypeTransactionReflector(); break;
	case TypeId_Method: res = co::__createMethodReflector(); break;
	case TypeId_MethodBuilder: res = co::__createMethodBuilderReflector(); break;
	case TypeId_MissingInputException: res = co::__createMissingInputExceptionReflector(); break;
	case TypeId_MissingServiceException: res = co::__createMissingServiceExceptionReflector(); break;
	case TypeId_Module: res = co::__createModuleReflector(); break;
	case TypeId_ModuleLoadException: res = co::__createModuleLoadExceptionReflector(); break;
	case TypeId_ModuleManager: res = co::__createModuleManagerReflector(); break;
	case TypeId_ModulePartLoader: res = co::__createModulePartLoaderReflector(); break;
	case TypeId_Namespace: res = co::__createNamespaceReflector(); break;
	case TypeId_NativeClass: res = co::__createNativeClassReflector(); break;
	case TypeId_NoSuchPortException: res = co::__createNoSuchPortExceptionReflector(); break;
	case TypeId_NotSupportedException: res = co::__createNotSupportedExceptionReflector(); break;
	case TypeId_Parameter: res = co::__createParameterReflector(); break;
	case TypeId_Port: res = co::__createPortReflector(); break;
	case TypeId_ServiceManager: res = co::__createServiceManagerReflector(); break;
	case TypeId_Struct: res = co::__createStructReflector(); break;
	case TypeId_System: res = co::__createSystemReflector(); break;
	case TypeId_Type: res = co::__createTypeReflector(); break;
	case TypeId_TypeBuilder: res = co::__createTypeBuilderReflector(); break;
	case TypeId_TypeLoadException: res = co::__createTypeLoadExceptionReflector(); break;
	case TypeId_TypeManager: res = co::__createTypeManagerReflector(); break;
	case TypeId_TypeTransaction: res = co::__createTypeTransactionReflector(); break;
	default: assert( false );
	};
	assert( res != NULL );
	return res;
}
