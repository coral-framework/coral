#include "SHA1.h"
#include <algorithm>

namespace co {

namespace {

inline uint32 rol(uint32 value, uint8 bits) {
  return (value << bits) | (value >> (32 - bits));
}

inline uint32 blk0(SHA1::Block64& b64, uint8 i) {
#if CORAL_BYTE_ORDER == CORAL_LITTLE_ENDIAN
  return (b64.l[i] = (rol(b64.l[i], 24) & 0xFF00FF00) |
                     (rol(b64.l[i], 8) & 0x00FF00FF));
#else
  return b64.l[i];
#endif
}

inline uint32 blk(SHA1::Block64& b64, uint8 i) {
  return (b64.l[i & 15] = rol(b64.l[(i + 13) & 15] ^ b64.l[(i + 8) & 15] ^
                                  b64.l[(i + 2) & 15] ^ b64.l[i & 15],
                              1));
}

inline void R0(uint32 v, uint32& w, uint32 x, uint32 y, uint32& z, uint8 i,
               SHA1::Block64& b64) {
  z += ((w & (x ^ y)) ^ y) + blk0(b64, i) + 0x5A827999 + rol(v, 5);
  w = rol(w, 30);
}

inline void R1(uint32 v, uint32& w, uint32 x, uint32 y, uint32& z, uint8 i,
               SHA1::Block64& b64) {
  z += ((w & (x ^ y)) ^ y) + blk(b64, i) + 0x5A827999 + rol(v, 5);
  w = rol(w, 30);
}

inline void R2(uint32 v, uint32& w, uint32 x, uint32 y, uint32& z, uint8 i,
               SHA1::Block64& b64) {
  z += (w ^ x ^ y) + blk(b64, i) + 0x6ED9EBA1 + rol(v, 5);
  w = rol(w, 30);
}

inline void R3(uint32 v, uint32& w, uint32 x, uint32 y, uint32& z, uint8 i,
               SHA1::Block64& b64) {
  z += (((w | x) & y) | (w & x)) + blk(b64, i) + 0x8F1BBCDC + rol(v, 5);
  w = rol(w, 30);
}

inline void R4(uint32 v, uint32& w, uint32 x, uint32 y, uint32& z, uint8 i,
               SHA1::Block64& b64) {
  z += (w ^ x ^ y) + blk(b64, i) + 0xCA62C1D6 + rol(v, 5);
  w = rol(w, 30);
}

// Worker function that hashes a 64-byte block.
void transform(uint32 state[5], SHA1::Block64& b64) {
  // copy context->state[] to working vars
  uint32 a = state[0];
  uint32 b = state[1];
  uint32 c = state[2];
  uint32 d = state[3];
  uint32 e = state[4];

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

}  // namespace

void SHA1::reset() {
  _state[0] = 0x67452301;
  _state[1] = 0xEFCDAB89;
  _state[2] = 0x98BADCFE;
  _state[3] = 0x10325476;
  _state[4] = 0xC3D2E1F0;
  _count[0] = 0;
  _count[1] = 0;
}

void SHA1::add(const uint8* data, size_t size) {
  size_t j = (_count[0] >> 3) & 63;

  if ((_count[0] += size << 3) < (size << 3)) _count[1]++;

  _count[1] += (size >> 29);

  size_t i = 0;
  if ((j + size) > 63) {
    i = 64 - j;
    std::copy(data, data + i, &_buffer.c[j]);
    transform(_state, _buffer);

    Block64 b64;
    for (; i + 63 < size; i += 64) {
      std::copy(data + i, data + i + 64, b64.c);
      transform(_state, b64);
    }

    j = 0;
  }

  std::copy(data + i, data + size, &_buffer.c[j]);
}

void SHA1::finish() {
  uint8 finalCount[8];
  for (uint8 i = 0; i < 8; ++i) {
    finalCount[i] = static_cast<uint8>(
        (_count[i >= 4 ? 0 : 1] >> ((3 - (i & 3)) * 8)) & 255);
  }
  add("\200", 1);
  while ((_count[0] & 504) != 448) {
    add("\0", 1);
  }
  add(finalCount, 8);
}

void SHA1::getDigest(uint8 digest[DIGEST_SIZE]) const {
  for (uint8 i = 0; i < DIGEST_SIZE; ++i)
    digest[i] =
        static_cast<uint8>((_state[i >> 2] >> ((3 - (i & 3)) * 8)) & 255);
}

void SHA1::getDigest(std::string& str) const {
  uint8 digest[DIGEST_SIZE];
  getDigest(digest);

  static const char HEX_CHARS[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                     '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
  str.clear();
  str.reserve(DIGEST_SIZE * 2);
  for (uint8 i = 0; i < DIGEST_SIZE; ++i) {
    uint8 v = digest[i];
    str.push_back(HEX_CHARS[v / 16]);
    str.push_back(HEX_CHARS[v % 16]);
  }
}

}  // namespace co
