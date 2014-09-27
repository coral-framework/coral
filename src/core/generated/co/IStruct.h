
#ifndef _CO_ISTRUCT_H_
#define _CO_ISTRUCT_H_

#include <co/Common.h>
#include <co/IRecordType.h>

// co.IStruct Mapping:
namespace co {

class IStruct : public co::IRecordType
{
public:
	virtual ~IStruct() {;}
};

typedef co::RefPtr<IStruct> IStructRef;

} // namespace co

namespace co {
template<> struct kindOf<co::IStruct> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<co::IStruct> { static const char* get() { return "co.IStruct"; } };
} // namespace co

#endif // _CO_ISTRUCT_H_
