
#ifndef _CO_IARRAY_H_
#define _CO_IARRAY_H_

#include <co/Common.h>
#include <co/IType.h>

// co.IArray Mapping:
namespace co {

class IArray : public co::IType
{
public:
	virtual ~IArray() {;}

	virtual co::IType* getElementType() = 0;
};

typedef co::RefPtr<IArray> IArrayRef;

} // namespace co

namespace co {
template<> struct kindOf<co::IArray> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<co::IArray> { static const char* get() { return "co.IArray"; } };
} // namespace co

#endif // _CO_IARRAY_H_
