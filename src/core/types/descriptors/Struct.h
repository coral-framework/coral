#ifndef _CO_STRUCT_H_
#define _CO_STRUCT_H_

#include "ClassType.h"
#include "Struct_Base.h"

namespace co {

/*!
	Implements co.IStruct.
 */
class Struct : public ClassType<Struct_Base>
{
public:
	virtual ~Struct();
};

} // namespace co

#endif
