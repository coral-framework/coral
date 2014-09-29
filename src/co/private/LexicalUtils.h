#ifndef _CO_PRIVATE_LEXICALUTILITIES_H_
#define _CO_PRIVATE_LEXICALUTILITIES_H_

#include <co/Platform.h>

#include <string>
#include <vector>

namespace co {

/*!
  \brief Lexical utility functions in a static class.
 */
class CORAL_EXPORT LexicalUtils {
 public:
  /*!
    Returns whether the passed parameter \a str is a valid identifier.
    Identifiers must start with a letter, which can then be followed by more
    letters,
    numbers and the underscore symbol.
   */
  static bool isValidIdentifier(const std::string& str);

  /*!
    Returns whether the passed \a identifier is a valid field name.
    Field names must start with lower case.
    \warning this method assumes the passed parameter is already a valid
    identifier.
    \sa isValidIdentifier
   */
  static bool isValidFieldName(const std::string& identifier);

  //! Enumerates the kind of field accessors.
  enum Accessor {
    Getter = 1,  //!< Getter method.
    Setter = 2   //!< Setter method.
  };

  /*!
    Generates an accessor name given a field name.
    \warning This method assumes \a methodName is a valid identifier.
   */
  static void formatAccessor(const std::string& fieldName, Accessor accessor,
                             std::string& methodName);

  /*!
    Parses a method name looking for a specific acessor name pattern.
    The passed method name will be searched for the patterns created by the
    formatAccessor() method.
    Only the kind of accessors specified in the parameter \a accessorKinds will
    be searched for.

    If a prefix 'get' or 'set' is found, the subsequent character must be a
    capital letter
    (or this method will return -1). If a valid pattern is found, the prefix is
    removed and
    the first letter is converted back to lowercase.

    \param[in] methodName an identifier representing a method name.
    \param[in] accessorKinds a bitfield with one or more of the following
    values:
      - co::LexicalUtils::Getter meaning that the method should search for a \e
    getter.
      - co::LexicalUtils::Setter meaning that the method should search for a \e
    setter.
    \param[out] fieldName the parsed field name, starting with a lower case
    letter.

    \return One of the following values:
      - co::LexicalUtils::Getter if a \e getter name was parsed;
      - co::LexicalUtils::Setter if a \e setter name was parsed;
      - -1 if \a methodName was not a valid accessor name.

    \warning This method assumes \a methodName is a valid identifier.
   */
  static int parseAccessor(const std::string& methodName, int accessorKinds,
                           std::string& fieldName);
};

}  // namespace co

#endif  // _CO_PRIVATE_LEXICALUTILITIES_H_
