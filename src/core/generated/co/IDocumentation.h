
#ifndef _CO_IDOCUMENTATION_H_
#define _CO_IDOCUMENTATION_H_

#include <co/Common.h>
#include <co/IAnnotation.h>

// co.IDocumentation Mapping:
namespace co {

class IDocumentation : public co::IAnnotation
{
public:
	virtual ~IDocumentation() {;}

	virtual std::string getValue() = 0;

	virtual void setValue( const std::string& value ) = 0;

	virtual void addDocFor( const std::string& member, const std::string& doc ) = 0;

	virtual std::string getDocFor( const std::string& member ) = 0;
};

typedef co::RefPtr<IDocumentation> IDocumentationRef;

} // namespace co

namespace co {
template<> struct kindOf<co::IDocumentation> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<co::IDocumentation> { static const char* get() { return "co.IDocumentation"; } };
} // namespace co

#endif // _CO_IDOCUMENTATION_H_
