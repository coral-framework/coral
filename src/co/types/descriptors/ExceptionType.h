#ifndef _EXCEPTIONTYPE_H_
#define _EXCEPTIONTYPE_H_

#include "Type.h"
#include "ExceptionType_Base.h"

namespace co {

/*!
	Implements co.IException.
 */
class ExceptionType : public Type<ExceptionType_Base>
{
public:
	virtual ~ExceptionType();
};

} // namespace co

#endif
