#ifndef _CO_EXCEPTION_H_
#define _CO_EXCEPTION_H_

#include <co/TypeTraits.h>
#include <exception>
#include <sstream>

namespace co {

/*!
	\brief Base class for all Coral exceptions.
 */
class CORAL_EXPORT Exception : public std::exception
{
public:
	//! Constructs an Exception with an empty message.
	inline Exception()
	{;}

	//! Constructs an Exception with the specified message.
	inline Exception( const std::string& message ) : _message( message )
	{;}

	//! Constructs a copy of an Exception.
	inline Exception( const Exception& other )
		: std::exception(), _message( other._message )
	{;}

	//! Destructor.
	virtual ~Exception() throw();

	//! Returns the exception's fully-qualified type name.
	virtual const char* getTypeName() const;

	//! Returns the exception message.
	inline const std::string& getMessage() const { return _message; }

	//! Sets the exception message.
	inline void setMessage( const std::string& message ) { _message = message; }

	//! Returns the exception message as a C string, for compatibility with std::exception.
	virtual const char* what() const throw();

private:
	// copies are forbidden
	Exception& operator=( const Exception& );

private:
	std::string _message;
};

template<> struct kindOf<Exception> : public kindOfBase<TK_EXCEPTION> {};

} // namespace co

/*!
	Macro to raise an exception with a std::ostream-style formatted message.
	You must \c \#include \<sstream\> to use this macro.
	\relates co::Exception
 */
#define CORAL_THROW( exceptionType, message ) \
	{ \
		std::stringstream throwMessageStringStream; \
		throwMessageStringStream << message; \
		throw exceptionType( throwMessageStringStream.str() ); \
	}

#endif // _CO_EXCEPTION_H_
