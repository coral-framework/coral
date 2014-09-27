#ifndef _CO_SHA1_H_
#define _CO_SHA1_H_

#include <cstddef>
#include <cstdint>

namespace sha1 {

using std::uint8_t;
using std::uint32_t;

union Block64 {
  uint8_t c[64];
  uint32_t l[16];
};

struct Context {
  uint32_t state[5];
  uint32_t count[2];
  Block64 buffer;
};

const uint8_t DIGEST_SIZE = 20;

void init(Context* context);

// Run your data through this.
void update(Context* context, const uint8_t* data, size_t length);

// Add padding and return the message digest.
void final(Context* context, uint8_t digest[DIGEST_SIZE]);

inline void update(Context* context, const char* data, size_t length) {
  update(context, reinterpret_cast<const uint8_t*>(data), length);
}

}  // namespace sha1

#endif
