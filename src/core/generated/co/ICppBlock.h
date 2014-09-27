
#ifndef _CO_ICPPBLOCK_H_
#define _CO_ICPPBLOCK_H_

#include <co/Common.h>
#include <co/IAnnotation.h>

// co.ICppBlock Mapping:
namespace co {

class ICppBlock : public co::IAnnotation
{
public:
	virtual ~ICppBlock() {;}

	virtual std::string getValue() = 0;

	virtual void setValue( const std::string& value ) = 0;
};

typedef co::RefPtr<ICppBlock> ICppBlockRef;

} // namespace co

namespace co {
template<> struct kindOf<co::ICppBlock> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<co::ICppBlock> { static const char* get() { return "co.ICppBlock"; } };
} // namespace co

#endif // _CO_ICPPBLOCK_H_
