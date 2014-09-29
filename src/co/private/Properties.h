#ifndef _CO_PRIVATE_PROPERTIES_H_
#define _CO_PRIVATE_PROPERTIES_H_

#include <co/Platform.h>
#include <istream>
#include <string>
#include <map>

namespace co {

/*!
  Parses a string dictionary from a (Java-like) ".properties" stream.
  Format reference: http://en.wikipedia.org/wiki/.properties

  Deviation: escape ('\') characters are currently ignored except
    for line continuations (i.e. escaping end-of-lines).
 */
class CORAL_EXPORT Properties {
 public:
  Properties();

  /*!
    Retrieves the value associated with the specified \a key.
    If the key is not found, returns an empty string.
   */
  const std::string& getProperty(const std::string& key);

  /*!
    Retrieves the value associated with the specified \a key.
    If the key is not found, returns \a defaultValue.
   */
  const std::string& getProperty(const std::string& key,
                                 const std::string& defaultValue);

  /*!
    Reads entries from a ".properties"-formatted stream.
    \note new entries override existing entries.
    \return number of entries read.
   */
  int load(std::istream& instream);

  /*!
    Reads entries from a ".properties" file.
    \note new entries override existing entries.
    \return number of entries read, or -1 if the file could not be opened.
   */
  int load(const std::string& filename);

  //! Clears the dictionary.
  void clear();

 private:
  int processLine(std::string& line);

 private:
  typedef std::map<std::string, std::string> Dictionary;
  Dictionary _properties;
};

}  // namespace co

#endif  // _CO_PRIVATE_PROPERTIES_H_
