
#ifndef _CO_IRECORDTYPE_H_
#define _CO_IRECORDTYPE_H_

#include <co/Common.h>
#include <co/ICompositeType.h>

// Forward Declarations:
namespace co {
	class IField; typedef co::RefPtr<IField> IFieldRef;
} // namespace co
// End Of Forward Declarations

// co.IRecordType Mapping:
namespace co {

class IRecordType : public co::ICompositeType
{
public:
	virtual ~IRecordType() {;}

	virtual co::TSlice<co::IField*> getFields() = 0;
};

typedef co::RefPtr<IRecordType> IRecordTypeRef;

} // namespace co

namespace co {
template<> struct kindOf<co::IRecordType> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<co::IRecordType> { static const char* get() { return "co.IRecordType"; } };
} // namespace co

#endif // _CO_IRECORDTYPE_H_
