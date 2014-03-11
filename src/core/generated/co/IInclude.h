#ifndef _CO_IINCLUDE_H_
#define _CO_IINCLUDE_H_

#include <co/Common.h>
#include <co/IAnnotation.h>

// co.IInclude Mapping:
namespace co {

class IInclude : public co::IAnnotation
{
public:
	virtual ~IInclude() {;}

	virtual std::string getValue() = 0;

	virtual void setValue( const std::string& value ) = 0;
};

typedef co::RefPtr<IInclude> IIncludeRef;

} // namespace co

namespace co {
template<> struct kindOf<co::IInclude> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<co::IInclude> { static const char* get() { return "co.IInclude"; } };
} // namespace co

#endif // _CO_IINCLUDE_H_
