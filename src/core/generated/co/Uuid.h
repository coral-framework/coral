#ifndef _CO_UUID_H_
#define _CO_UUID_H_

#include <co/Common.h>


		#include <co/reserved/Uuid.h>

	
namespace co {
template<> struct kindOf<co::Uuid> : public kindOfBase<TK_NATIVECLASS> {};
template<> struct nameOf<co::Uuid> { static const char* get() { return "co.Uuid"; } };
} // namespace co

#endif // _CO_UUID_H_
