
#ifndef _CO_MODULESTATE_H_
#define _CO_MODULESTATE_H_

#include <co/Common.h>

// co.ModuleState Mapping:
namespace co {

enum class ModuleState : co::int16 {
  None,
  Initialized,
  Integrated,
  Disintegrated,
  Disposed,
  Aborted,
};

} // namespace co

namespace co {
template<> struct kindOf<co::ModuleState> : public kindOfBase<TK_ENUM> {};
template<> struct nameOf<co::ModuleState> { static const char* get() { return "co.ModuleState"; } };
} // namespace co

#endif // _CO_MODULESTATE_H_
