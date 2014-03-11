#ifndef _CO_ITYPETRANSACTION_H_
#define _CO_ITYPETRANSACTION_H_

#include <co/Common.h>
#include <co/IService.h>

// Forward Declarations:
namespace co {
	class ITypeBuilder; typedef co::RefPtr<ITypeBuilder> ITypeBuilderRef;
} // namespace co
// End Of Forward Declarations

// co.ITypeTransaction Mapping:
namespace co {

class ITypeTransaction : public co::IService
{
public:
	virtual ~ITypeTransaction() {;}

	virtual co::TSlice<co::ITypeBuilder*> getTypeBuilders() = 0;

	virtual void commit() = 0;

	virtual void rollback() = 0;
};

typedef co::RefPtr<ITypeTransaction> ITypeTransactionRef;

} // namespace co

namespace co {
template<> struct kindOf<co::ITypeTransaction> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<co::ITypeTransaction> { static const char* get() { return "co.ITypeTransaction"; } };
} // namespace co

#endif // _CO_ITYPETRANSACTION_H_
