
#ifndef _CO_SYSTEMSTATE_H_
#define _CO_SYSTEMSTATE_H_

#include <co/Common.h>

// co.SystemState Mapping:
namespace co {

enum class SystemState : co::int16 {
  None,
  Initializing,
  Integrating,
  Running,
  Disintegrating,
};

} // namespace co

namespace co {
template<> struct kindOf<co::SystemState> : public kindOfBase<TK_ENUM> {};
template<> struct nameOf<co::SystemState> { static const char* get() { return "co.SystemState"; } };
} // namespace co

#endif // _CO_SYSTEMSTATE_H_
