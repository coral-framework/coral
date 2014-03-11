#ifndef _CO_ITYPE_H_
#define _CO_ITYPE_H_

#include <co/Common.h>
#include <co/Uuid.h>
#include <co/TypeKind.h>
#include <co/IType.h>
#include <co/IAnnotated.h>

// Forward Declarations:
namespace co {
	class INamespace; typedef co::RefPtr<INamespace> INamespaceRef;
	class IReflector; typedef co::RefPtr<IReflector> IReflectorRef;
} // namespace co
// End Of Forward Declarations

// co.IType Mapping:
namespace co {

class IType : public co::IAnnotated
{
public:
	virtual ~IType() {;}

	virtual co::Uuid getBinarySignature() = 0;

	virtual co::IReflector* getCurrentReflector() = 0;

	virtual std::string getFullName() = 0;

	virtual co::Uuid getFullSignature() = 0;

	virtual co::TypeKind getKind() = 0;

	virtual std::string getName() = 0;

	virtual co::INamespace* getNamespace() = 0;

	virtual co::IReflector* getReflector() = 0;

	virtual void setReflector( co::IReflector* reflector ) = 0;

	virtual bool isA( co::IType* type ) = 0;
};

typedef co::RefPtr<IType> ITypeRef;

} // namespace co

namespace co {
template<> struct kindOf<co::IType> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<co::IType> { static const char* get() { return "co.IType"; } };
} // namespace co

#endif // _CO_ITYPE_H_
