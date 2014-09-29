
#ifndef _CO_INAMESPACE_H_
#define _CO_INAMESPACE_H_

#include <co/Common.h>
#include <co/INamespace.h>
#include <co/TypeKind.h>
#include <co/IService.h>

// Forward Declarations:
namespace co {
	class IModule; typedef co::RefPtr<IModule> IModuleRef;
	class IType; typedef co::RefPtr<IType> ITypeRef;
	class ITypeBuilder; typedef co::RefPtr<ITypeBuilder> ITypeBuilderRef;
} // namespace co
// End Of Forward Declarations

// co.INamespace Mapping:
namespace co {

class INamespace : public co::IService
{
public:
	virtual ~INamespace() {;}

	virtual co::TSlice<co::INamespace*> getChildNamespaces() = 0;

	virtual std::string getFullName() = 0;

	virtual co::IModule* getModule() = 0;

	virtual std::string getName() = 0;

	virtual co::INamespace* getParentNamespace() = 0;

	virtual co::TSlice<co::IType*> getTypes() = 0;

	virtual co::INamespace* defineChildNamespace( const std::string& name ) = 0;

	virtual co::ITypeBuilder* defineType( const std::string& name, co::TypeKind kind ) = 0;

	virtual co::INamespace* findChildNamespace( const std::string& name ) = 0;

	virtual co::IType* findType( const std::string& name ) = 0;
};

typedef co::RefPtr<INamespace> INamespaceRef;

} // namespace co

namespace co {
template<> struct kindOf<co::INamespace> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<co::INamespace> { static const char* get() { return "co.INamespace"; } };
} // namespace co

#endif // _CO_INAMESPACE_H_
