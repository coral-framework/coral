#ifndef _CO_IEXCEPTION_H_
#define _CO_IEXCEPTION_H_

#include <co/Common.h>
#include <co/IType.h>

// co.IException Mapping:
namespace co {

class IException : public co::IType
{
public:
	virtual ~IException() {;}
};

typedef co::RefPtr<IException> IExceptionRef;

} // namespace co

namespace co {
template<> struct kindOf<co::IException> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<co::IException> { static const char* get() { return "co.IException"; } };
} // namespace co

#endif // _CO_IEXCEPTION_H_
