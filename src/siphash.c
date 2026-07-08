#include "../include/siphash.h"
#include <string.h>

void init_siphash(u64 *vo, u64 *v1, u64 *v2, u64 *v3, u8 *key) {
  u64 k0, k1;
  memcpy(&k0, key, 8);
  memcpy(&k1, key + 8, 8);
  *vo = k0 ^ 0x736f6d6570736575;
  *v1 = k1 ^ 0x646f72616e646f6d;
  *v2 = k0 ^ 0x6c7967656e657261;
  *v3 = k1 ^ 0x7465646279746573;
}

u64 siphash(u8 *key, u8 *m, unsigned long b) {
  u64 v0, v1, v2, v3;
  init_siphash(&v0, &v1, &v2, &v3, key);
  int blocks = b / 8;
  int remainder = b % 8;
  for (size_t i = 0; i < blocks; i++) {
    u64 mi;
    memcpy(&mi, m + i * 8, 8);
    v3 ^= mi;
    SIPROUND;
    SIPROUND;
    v0 ^= mi;
  }
  // b mod 256 is the last 8 bits
  u64 last = ((u64)(b & 0xff)) << 56;
  for (size_t i = 0; i < remainder; i++) {
    last |= ((u64)m[(blocks * 8) + i]) << (i * 8);
  }
  v3 ^= last;
  SIPROUND;
  SIPROUND;
  v2 ^= 0xff;
  SIPROUND;
  SIPROUND;
  SIPROUND;
  SIPROUND;
  return v0 ^ v1 ^ v2 ^ v3;
}
