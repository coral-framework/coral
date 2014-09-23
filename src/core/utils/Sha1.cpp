// C++ re-implementation of the SHA-1 algorithm (based on public domain code)

#include "Sha1.h"
#include <co/Platform.h>
#include <algorithm>

namespace {
  
using namespace sha1;

uint32_t rol(uint32_t value, uint8_t bits) {
  return (value << bits) | (value >> (32 - bits));
}

uint32_t blk0(Block64 &b64, uint8_t i) {
#if CORAL_BYTE_ORDER == CORAL_LITTLE_ENDIAN
  return (b64.l[i] = (rol(b64.l[i], 24) & 0xFF00FF00) |
                     (rol(b64.l[i], 8) & 0x00FF00FF));
#else
  return b64.l[i];
#endif
}

uint32_t blk(Block64 &b64, uint8_t i) {
  return (b64.l[i & 15] = rol(b64.l[(i + 13) & 15] ^ b64.l[(i + 8) & 15] ^
                                  b64.l[(i + 2) & 15] ^ b64.l[i & 15],
                              1));
}

void R0(uint32_t v, uint32_t &w, uint32_t x, uint32_t y, uint32_t &z, uint8_t i,
        Block64 &b64) {
  z += ((w & (x ^ y)) ^ y) + blk0(b64, i) + 0x5A827999 + rol(v, 5);
  w = rol(w, 30);
}

void R1(uint32_t v, uint32_t &w, uint32_t x, uint32_t y, uint32_t &z, uint8_t i,
        Block64 &b64) {
  z += ((w & (x ^ y)) ^ y) + blk(b64, i) + 0x5A827999 + rol(v, 5);
  w = rol(w, 30);
}

void R2(uint32_t v, uint32_t &w, uint32_t x, uint32_t y, uint32_t &z, uint8_t i,
        Block64 &b64) {
  z += (w ^ x ^ y) + blk(b64, i) + 0x6ED9EBA1 + rol(v, 5);
  w = rol(w, 30);
}

void R3(uint32_t v, uint32_t &w, uint32_t x, uint32_t y, uint32_t &z, uint8_t i,
        Block64 &b64) {
  z += (((w | x) & y) | (w & x)) + blk(b64, i) + 0x8F1BBCDC + rol(v, 5);
  w = rol(w, 30);
}

void R4(uint32_t v, uint32_t &w, uint32_t x, uint32_t y, uint32_t &z, uint8_t i,
        Block64 &b64) {
  z += (w ^ x ^ y) + blk(b64, i) + 0xCA62C1D6 + rol(v, 5);
  w = rol(w, 30);
}

//! Worker function that hashes a 64-byte block.
void transform(uint32_t state[5], Block64 &b64) {
  // copy context->state[] to working vars
  uint32_t a = state[0];
  uint32_t b = state[1];
  uint32_t c = state[2];
  uint32_t d = state[3];
  uint32_t e = state[4];

  // 4 rounds of 20 operations each (loop unrolled)
  R0(a, b, c, d, e, 0, b64);
  R0(e, a, b, c, d, 1, b64);
  R0(d, e, a, b, c, 2, b64);
  R0(c, d, e, a, b, 3, b64);
  R0(b, c, d, e, a, 4, b64);
  R0(a, b, c, d, e, 5, b64);
  R0(e, a, b, c, d, 6, b64);
  R0(d, e, a, b, c, 7, b64);
  R0(c, d, e, a, b, 8, b64);
  R0(b, c, d, e, a, 9, b64);
  R0(a, b, c, d, e, 10, b64);
  R0(e, a, b, c, d, 11, b64);
  R0(d, e, a, b, c, 12, b64);
  R0(c, d, e, a, b, 13, b64);
  R0(b, c, d, e, a, 14, b64);
  R0(a, b, c, d, e, 15, b64);
  R1(e, a, b, c, d, 16, b64);
  R1(d, e, a, b, c, 17, b64);
  R1(c, d, e, a, b, 18, b64);
  R1(b, c, d, e, a, 19, b64);
  R2(a, b, c, d, e, 20, b64);
  R2(e, a, b, c, d, 21, b64);
  R2(d, e, a, b, c, 22, b64);
  R2(c, d, e, a, b, 23, b64);
  R2(b, c, d, e, a, 24, b64);
  R2(a, b, c, d, e, 25, b64);
  R2(e, a, b, c, d, 26, b64);
  R2(d, e, a, b, c, 27, b64);
  R2(c, d, e, a, b, 28, b64);
  R2(b, c, d, e, a, 29, b64);
  R2(a, b, c, d, e, 30, b64);
  R2(e, a, b, c, d, 31, b64);
  R2(d, e, a, b, c, 32, b64);
  R2(c, d, e, a, b, 33, b64);
  R2(b, c, d, e, a, 34, b64);
  R2(a, b, c, d, e, 35, b64);
  R2(e, a, b, c, d, 36, b64);
  R2(d, e, a, b, c, 37, b64);
  R2(c, d, e, a, b, 38, b64);
  R2(b, c, d, e, a, 39, b64);
  R3(a, b, c, d, e, 40, b64);
  R3(e, a, b, c, d, 41, b64);
  R3(d, e, a, b, c, 42, b64);
  R3(c, d, e, a, b, 43, b64);
  R3(b, c, d, e, a, 44, b64);
  R3(a, b, c, d, e, 45, b64);
  R3(e, a, b, c, d, 46, b64);
  R3(d, e, a, b, c, 47, b64);
  R3(c, d, e, a, b, 48, b64);
  R3(b, c, d, e, a, 49, b64);
  R3(a, b, c, d, e, 50, b64);
  R3(e, a, b, c, d, 51, b64);
  R3(d, e, a, b, c, 52, b64);
  R3(c, d, e, a, b, 53, b64);
  R3(b, c, d, e, a, 54, b64);
  R3(a, b, c, d, e, 55, b64);
  R3(e, a, b, c, d, 56, b64);
  R3(d, e, a, b, c, 57, b64);
  R3(c, d, e, a, b, 58, b64);
  R3(b, c, d, e, a, 59, b64);
  R4(a, b, c, d, e, 60, b64);
  R4(e, a, b, c, d, 61, b64);
  R4(d, e, a, b, c, 62, b64);
  R4(c, d, e, a, b, 63, b64);
  R4(b, c, d, e, a, 64, b64);
  R4(a, b, c, d, e, 65, b64);
  R4(e, a, b, c, d, 66, b64);
  R4(d, e, a, b, c, 67, b64);
  R4(c, d, e, a, b, 68, b64);
  R4(b, c, d, e, a, 69, b64);
  R4(a, b, c, d, e, 70, b64);
  R4(e, a, b, c, d, 71, b64);
  R4(d, e, a, b, c, 72, b64);
  R4(c, d, e, a, b, 73, b64);
  R4(b, c, d, e, a, 74, b64);
  R4(a, b, c, d, e, 75, b64);
  R4(e, a, b, c, d, 76, b64);
  R4(d, e, a, b, c, 77, b64);
  R4(c, d, e, a, b, 78, b64);
  R4(b, c, d, e, a, 79, b64);

  // add the working vars back into context.state[]
  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;
  state[4] += e;
}

}  // anonymous namespace

namespace sha1 {

void init(Context *context) {
  context->state[0] = 0x67452301;
  context->state[1] = 0xEFCDAB89;
  context->state[2] = 0x98BADCFE;
  context->state[3] = 0x10325476;
  context->state[4] = 0xC3D2E1F0;
  context->count[0] = context->count[1] = 0;
}

// Run your data through this.
void update(Context *context, const uint8_t *data, uint32_t length) {
  uint32_t j = (context->count[0] >> 3) & 63;

  if ((context->count[0] += length << 3) < (length << 3))
    context->count[1]++;

  context->count[1] += (length >> 29);

  uint32_t i = 0;
  if ((j + length) > 63) {
    i = 64 - j;
    std::copy(data, data + i, &context->buffer.c[j]);
    transform(context->state, context->buffer);

    Block64 b64;
    for (; i + 63 < length; i += 64) {
      std::copy(data + i, data + i + 64, b64.c);
      transform(context->state, b64);
    }

    j = 0;
  }

  std::copy(data + i, data + length, &context->buffer.c[j]);
}

// Add padding and return the message digest.
void final(Context *context, uint8_t digest[DIGEST_SIZE]) {
  uint8_t finalCount[8];
  for (uint8_t i = 0; i < 8; ++i) {
    // endian independent
    finalCount[i] = static_cast<uint8_t>(
        (context->count[i >= 4 ? 0 : 1] >> ((3 - (i & 3)) * 8)) & 255);
  }

  update(context, reinterpret_cast<const uint8_t *>("\200"), 1);

  while ((context->count[0] & 504) != 448) {
    update(context, reinterpret_cast<const uint8_t *>("\0"), 1);
  }

  update(context, finalCount, 8);

  for (uint8_t i = 0; i < DIGEST_SIZE; ++i) {
    digest[i] = static_cast<uint8_t>(
        (context->state[i >> 2] >> ((3 - (i & 3)) * 8)) & 255);
  }
}

}  // namespace sha1
