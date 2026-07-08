// SipHash-2-4 implementation
// this only works on little-endian machines
#include <stdint.h>

typedef uint64_t u64;
typedef uint16_t u16;
typedef uint8_t u8;

#define ROTL(x, b) (u64)(((x) << (b)) | ((x) >> (64 - (b))))

#define SIPROUND                                                               \
  do {                                                                         \
    v0 += v1;                                                                  \
    v1 = ROTL(v1, 13);                                                         \
    v1 ^= v0;                                                                  \
    v0 = ROTL(v0, 32);                                                         \
    v2 += v3;                                                                  \
    v3 = ROTL(v3, 16);                                                         \
    v3 ^= v2;                                                                  \
    v0 += v3;                                                                  \
    v3 = ROTL(v3, 21);                                                         \
    v3 ^= v0;                                                                  \
    v2 += v1;                                                                  \
    v1 = ROTL(v1, 17);                                                         \
    v1 ^= v2;                                                                  \
    v2 = ROTL(v2, 32);                                                         \
  } while (0)

void init_siphash(u64 *vo, u64 *v1, u64 *v2, u64 *v3, u8 *key);
u64 siphash(u8 *key, u8 *m, unsigned long b);
