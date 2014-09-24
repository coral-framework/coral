#ifndef _CO_STRINGTOKENIZER_H_
#define _CO_STRINGTOKENIZER_H_

#include <string>

namespace co {

// Breaks a string into a stream of tokens.
// A token is a string delimited by any character in 'delimiters'.
class StringTokenizer {
 public:
  StringTokenizer(std::string str, std::string delimiters)
      : _str(std::move(str)), _delimiters(std::move(delimiters)), _offset(0) {}

  // Moves to the next token in the string.
  // Returns false when the end of the string is reached (no more tokens).
  bool getNext(std::string& token) {
    // skip delimiters at beginning
    size_t startPos = _str.find_first_not_of(_delimiters, _offset);
    if (startPos == std::string::npos) {
      _offset = _str.length();
      return false;
    }

    // find first delimiter after startPos
    size_t endPos = _str.find_first_of(_delimiters, startPos);
    if (endPos == std::string::npos) {
      _offset = _str.length();
      token.assign(_str, startPos, _offset);
      return true;
    }

    token.assign(_str, startPos, endPos - startPos);
    _offset = endPos;

    return true;
  }

 private:
  std::string _str;
  std::string _delimiters;
  size_t _offset;
};

}  // namespace co

#endif  // _CO_STRINGTOKENIZER_H_
