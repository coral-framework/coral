#ifndef _CO_IANNOTATION_H_
#define _CO_IANNOTATION_H_

#include <co/Common.h>
#include <co/IService.h>

// co.IAnnotation Mapping:
namespace co {

class IAnnotation : public co::IService
{
public:
	virtual ~IAnnotation() {;}
};

typedef co::RefPtr<IAnnotation> IAnnotationRef;

} // namespace co

namespace co {
template<> struct kindOf<co::IAnnotation> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<co::IAnnotation> { static const char* get() { return "co.IAnnotation"; } };
} // namespace co

#endif // _CO_IANNOTATION_H_
