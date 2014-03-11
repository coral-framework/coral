#ifndef _CO_ITYPEBUILDER_H_
#define _CO_ITYPEBUILDER_H_

#include <co/Common.h>
#include <co/TypeKind.h>
#include <co/IService.h>

// Forward Declarations:
namespace co {
	class IInterface; typedef co::RefPtr<IInterface> IInterfaceRef;
	class IMethodBuilder; typedef co::RefPtr<IMethodBuilder> IMethodBuilderRef;
	class INamespace; typedef co::RefPtr<INamespace> INamespaceRef;
	class IType; typedef co::RefPtr<IType> ITypeRef;
} // namespace co
// End Of Forward Declarations

// co.ITypeBuilder Mapping:
namespace co {

class ITypeBuilder : public co::IService
{
public:
	virtual ~ITypeBuilder() {;}

	virtual co::TypeKind getKind() = 0;

	virtual co::INamespace* getNamespace() = 0;

	virtual std::string getTypeName() = 0;

	virtual co::IType* createType() = 0;

	virtual void defineBaseType( co::IType* baseType ) = 0;

	virtual void defineField( const std::string& name, co::IType* type, bool isReadOnly ) = 0;

	virtual void defineIdentifier( const std::string& name ) = 0;

	virtual co::IMethodBuilder* defineMethod( const std::string& name ) = 0;

	virtual void definePort( const std::string& name, co::IInterface* type, bool isFacet ) = 0;
};

typedef co::RefPtr<ITypeBuilder> ITypeBuilderRef;

} // namespace co

namespace co {
template<> struct kindOf<co::ITypeBuilder> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<co::ITypeBuilder> { static const char* get() { return "co.ITypeBuilder"; } };
} // namespace co

#endif // _CO_ITYPEBUILDER_H_
