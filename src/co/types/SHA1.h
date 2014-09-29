#ifndef _CO_SHA1_H_
#define _CO_SHA1_H_

#include <co/Platform.h>
#include <cstring>
#include <string>

namespace co {

// Calculates SHA-1 hashes.
class CORAL_EXPORT SHA1 {
 public:
  static const uint8 DIGEST_SIZE = 20;

  union Block64 {
    uint8 c[64];
    uint32 l[16];
  };

  // Default constructor.
  SHA1() { reset(); }

  // Resets the algorithm.
  void reset();

  // Feeds data to the algorithm.
  void add(const uint8* data, size_t size);
  template <typename T> void add(const T* data, size_t size) {
    add(reinterpret_cast<const uint8*>(data), size);
  }
  inline void add(const char* cstr) { add(cstr, strlen(cstr)); }
  inline void add(const std::string& str) { add(str.c_str(), str.size()); }
  template <typename T> void add(const T& object) {
    add(&object, sizeof(object));
  }

  // Adds the required padding. MUST be called before getDigest().
  void finish();

  // Gets the result in binary form.
  void getDigest(uint8 digest[DIGEST_SIZE]) const;

  // Gets the result in string form (40 lowercase hex characters).
  void getDigest(std::string& str) const;

 private:
  uint32 _state[5];
  uint32 _count[2];
  Block64 _buffer;
};

}  // namespace co

#endif  // _CO_SHA1_H_
