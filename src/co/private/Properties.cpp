#include "Properties.h"
#include <cstring>
#include <fstream>

namespace {
const char* BLANK_CHARS = " \t\f";
const char* COMMENT_CHARS = "#!";
const char* KEY_TERMINATION_CHARS = "=: \t\f";
}

namespace co {

Properties::Properties() {
  // empty
}

const std::string& Properties::getProperty(const std::string& key) {
  static const std::string emptyString;
  return getProperty(key, emptyString);
}

const std::string& Properties::getProperty(const std::string& key,
                                           const std::string& defaultValue) {
  Dictionary::iterator it = _properties.find(key);
  return it != _properties.end() ? it->second : defaultValue;
}

int Properties::load(std::istream& input) {
  std::string line;
  std::string logicalLine;
  int numRead = 0;

  while (std::getline(input, line)) {
    size_t start = line.find_first_not_of(BLANK_CHARS);
    if (start != std::string::npos) {
      bool lineContinuation = (*line.rbegin() == '\\');
      size_t end = lineContinuation ? line.size() - 1 : std::string::npos;
      logicalLine.append(line, start, end - start);
      if (lineContinuation) continue;
    }

    if (!logicalLine.empty()) {
      numRead += processLine(logicalLine);
      logicalLine.clear();
    }
  }

  return numRead;
}

int Properties::load(const std::string& filename) {
  std::ifstream file;
  file.open(filename.c_str());

  if (!file.is_open()) return -1;

  return load(file);
}

int Properties::processLine(std::string& line) {
  size_t first = line.find_first_not_of(BLANK_CHARS);
  if (first == std::string::npos || strchr(COMMENT_CHARS, line[first]))
    return 0;  // empty or comment line

  size_t end = line.find_first_of(KEY_TERMINATION_CHARS, first);
  std::string key(line, first, end - first);

  std::string value;
  if (end != std::string::npos) {
    first = line.find_first_not_of(BLANK_CHARS, end);
    if (strchr(KEY_TERMINATION_CHARS, line[first]))
      first = line.find_first_not_of(BLANK_CHARS, first + 1);
    if (first != std::string::npos)
      value.append(line, first, std::string::npos);
  }

  _properties[key] = value;

  return 1;
}

}  // namespace co
