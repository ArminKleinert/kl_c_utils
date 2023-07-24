#include <stdint.h>

#ifdef __builtin_clzl
int u64_cl1(uint64_t value) { return __builtin_clzl(~value); }
#else
int u64_cl1(uint64_t value) {
  unsigned int nbits = sizeof(value) * 8 - 1;
  unsigned int bitIndex = nbits;
  while (bitIndex > 0 && (value & (1 << bitIndex)) != 0) {
    bitIndex--;
  }
  return nbits - bitIndex;
}
#endif

#ifdef __builtin_clz
int u32_cl1(uint32_t value) { return __builtin_clz(~value); }
#else
int u32_cl1(uint32_t value) {
  unsigned int nbits = sizeof(value) * 8 - 1;
  unsigned int bitIndex = nbits;
  while (bitIndex > 0 && (value & (1 << bitIndex)) != 0) {
    bitIndex--;
  }
  return nbits - bitIndex;
}
#endif

int u8_cl1(uint8_t value) {
  unsigned int nbits = 7; // 8 - 1
  unsigned int bitIndex = nbits;
  while (bitIndex > 0 && (value & (1 << bitIndex)) != 0) {
    bitIndex--;
  }
  return nbits - bitIndex;
}
