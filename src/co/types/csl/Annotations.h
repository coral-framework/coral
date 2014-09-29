#ifndef _CO_ANNOTATIONS_H_
#define _CO_ANNOTATIONS_H_

#include "CppBlockAnnotation_Base.h"
#include "DocumentationAnnotation_Base.h"
#include <map>

namespace co {

//---------- @co.CppBlock ------------------------------------------------------

class CppBlockAnnotation : public CppBlockAnnotation_Base
{
public:
	CppBlockAnnotation();

	virtual ~CppBlockAnnotation();

	std::string getValue();
	void setValue( const std::string& value );

private:
	std::string _value;
};

//---------- @co.Documentation -------------------------------------------------

class DocumentationAnnotation : public DocumentationAnnotation_Base
{
public:
	DocumentationAnnotation();

	virtual ~DocumentationAnnotation();

	std::string getValue();
	void setValue( const std::string& value );

	void addDocFor( const std::string& element, const std::string& text );
	std::string getDocFor( const std::string& element );

private:
	std::string _value;

	typedef std::map<std::string, std::string> DocMap;
	DocMap _docMap;
};

} // namespace co

#endif
