#ifndef _CO_SYSTEMSTATE_H_
#define _CO_SYSTEMSTATE_H_

#include <co/Common.h>

// co.SystemState Mapping:
namespace co {

enum SystemState
{
	SystemState_None,
	SystemState_Initializing,
	SystemState_Integrating,
	SystemState_Integrated,
	SystemState_IntegratingPresentation,
	SystemState_Running,
	SystemState_Disintegrating,
	__SystemState__FORCE_SIZEOF_UINT32 = 0xFFFFFFFF
};

} // namespace co

namespace co {
template<> struct kindOf<co::SystemState> : public kindOfBase<TK_ENUM> {};
template<> struct nameOf<co::SystemState> { static const char* get() { return "co.SystemState"; } };
} // namespace co

#endif // _CO_SYSTEMSTATE_H_
