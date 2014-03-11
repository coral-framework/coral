#ifndef _CO_IPARAMETER_H_
#define _CO_IPARAMETER_H_

#include <co/Common.h>
#include <co/IService.h>

// Forward Declarations:
namespace co {
	class IType; typedef co::RefPtr<IType> ITypeRef;
} // namespace co
// End Of Forward Declarations

// co.IParameter Mapping:
namespace co {

class IParameter : public co::IService
{
public:
	virtual ~IParameter() {;}

	virtual bool getIsIn() = 0;

	virtual bool getIsOut() = 0;

	virtual std::string getName() = 0;

	virtual co::IType* getType() = 0;
};

typedef co::RefPtr<IParameter> IParameterRef;

} // namespace co

namespace co {
template<> struct kindOf<co::IParameter> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<co::IParameter> { static const char* get() { return "co.IParameter"; } };
} // namespace co

#endif // _CO_IPARAMETER_H_
