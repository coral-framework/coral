
#ifndef _CO_IENUM_H_
#define _CO_IENUM_H_

#include <co/Common.h>
#include <co/IType.h>

// co.IEnum Mapping:
namespace co {

class IEnum : public co::IType
{
public:
	virtual ~IEnum() {;}

	virtual co::TSlice<std::string> getIdentifiers() = 0;

	virtual co::int16 getValueOf( const std::string& id ) = 0;
};

typedef co::RefPtr<IEnum> IEnumRef;

} // namespace co

namespace co {
template<> struct kindOf<co::IEnum> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<co::IEnum> { static const char* get() { return "co.IEnum"; } };
} // namespace co

#endif // _CO_IENUM_H_
