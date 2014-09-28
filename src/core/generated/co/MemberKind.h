
#ifndef _CO_MEMBERKIND_H_
#define _CO_MEMBERKIND_H_

#include <co/Common.h>

// co.MemberKind Mapping:
namespace co {

enum class MemberKind : co::int16 {
  Field,
  Method,
  Port,
};

} // namespace co

namespace co {
template<> struct kindOf<co::MemberKind> : public kindOfBase<TK_ENUM> {};
template<> struct nameOf<co::MemberKind> { static const char* get() { return "co.MemberKind"; } };
} // namespace co

#endif // _CO_MEMBERKIND_H_
