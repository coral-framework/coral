#ifndef _CO_MEMBERKIND_H_
#define _CO_MEMBERKIND_H_

#include <co/Common.h>

// co.MemberKind Mapping:
namespace co {

enum MemberKind
{
	MK_FIELD,
	MK_METHOD,
	MK_PORT,
	__MemberKind__FORCE_SIZEOF_UINT32 = 0xFFFFFFFF
};

} // namespace co

namespace co {
template<> struct kindOf<co::MemberKind> : public kindOfBase<TK_ENUM> {};
template<> struct nameOf<co::MemberKind> { static const char* get() { return "co.MemberKind"; } };
} // namespace co

#endif // _CO_MEMBERKIND_H_
