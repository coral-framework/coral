#ifndef _CO_STRINGTOKENIZER_H_
#define _CO_STRINGTOKENIZER_H_

#include <co/Platform.h>
#include <string>

namespace co {

/*!
	Breaks a string into a stream of tokens.
	A token is a string delimited by an arbitrary set of characters.
 */
class CORAL_EXPORT StringTokenizer
{
public:
	/*!
		Constructs a string tokenizer for the specified string.
		The argument \a delimiters indicates a list of characters that might be used to separate the tokens.
	 */
	StringTokenizer( const std::string& str, const std::string& delimiters );

	//! Returns the current token.
	inline const std::string& getToken() const { return _token; }

	/*!
		Moves to the next token in the string.
		Returns false when the end of the string is reached (i.e. there are no more tokens).
	 */
	bool nextToken();

private:
	std::string _str;
	std::string _delimiters;
	std::string _token;
	size_t _offset;
};

} // namespace co

#endif // _CO_STRINGTOKENIZER_H_
