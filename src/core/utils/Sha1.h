#ifndef _CORAL_UTILS_SHA1_H_
#define _CORAL_UTILS_SHA1_H_

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

void init(Context *context);
void update(Context *context, const uint8_t *data, uint32_t length);
void final(Context *context, uint8_t digest[DIGEST_SIZE]);

}  // namespace sha1

#endif
