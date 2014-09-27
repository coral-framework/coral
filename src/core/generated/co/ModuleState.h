
#ifndef _CO_MODULESTATE_H_
#define _CO_MODULESTATE_H_

#include <co/Common.h>

// co.ModuleState Mapping:
namespace co {

enum ModuleState
{
	ModuleState_None,
	ModuleState_Initialized,
	ModuleState_Integrated,
	ModuleState_PresentationIntegrated,
	ModuleState_Disintegrated,
	ModuleState_Disposed,
	ModuleState_Aborted,
	__ModuleState__FORCE_SIZEOF_UINT32 = 0xFFFFFFFF
};

} // namespace co

namespace co {
template<> struct kindOf<co::ModuleState> : public kindOfBase<TK_ENUM> {};
template<> struct nameOf<co::ModuleState> { static const char* get() { return "co.ModuleState"; } };
} // namespace co

#endif // _CO_MODULESTATE_H_
