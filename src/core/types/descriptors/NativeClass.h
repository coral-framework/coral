#ifndef _CO_NATIVECLASS_H_
#define _CO_NATIVECLASS_H_

#include "ClassType.h"
#include "NativeClass_Base.h"

namespace co {

/*!
	Implements co.INativeClass.
 */
class NativeClass : public ClassType<NativeClass_Base>
{
public:
	virtual ~NativeClass();
};

} // namespace co

#endif
