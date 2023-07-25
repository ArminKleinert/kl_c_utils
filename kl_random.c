#include "kl_random.h"
#include <stdint.h>

uint32_t xorshift32_state_global;
uint64_t xorshift64_state_global;
struct xorshift128_state xorshift128_state_global;
uint64_t xorshift64s_state_global;
uint64_t splitmix64_state_global;

uint32_t xorshift32(uint32_t *state) {
  uint32_t x = *state;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  *state = x;
  return x;
}

uint64_t xorshift64(uint64_t *state) {
  uint64_t x = *state;
  x ^= x << 13;
  x ^= x >> 7;
  x ^= x << 17;
  *state = x;
  return x;
}

uint32_t xorshift128(struct xorshift128_state *state) {
  uint32_t t = state->x[3];

  uint32_t s = state->x[0];
  state->x[3] = state->x[2];
  state->x[2] = state->x[1];
  state->x[1] = s;

  t ^= t << 11;
  t ^= t >> 8;
  return state->x[0] = t ^ s ^ (s >> 19);
}

static struct xorshift128_state *
xorshift128_state_init_64(struct xorshift128_state *state, uint64_t upper_seed,
                          uint64_t lower_seed) {
  state->x[0] = upper_seed & 0xFFFFULL;
  state->x[1] = upper_seed & 0xFFFF0000ULL;
  state->x[2] = lower_seed & 0xFFFFULL;
  state->x[3] = lower_seed & 0xFFFF0000ULL;
  return state;
}

uint64_t xorshift64s(uint64_t *state) {
  uint64_t x = *state;
  x ^= x >> 12;
  x ^= x << 25;
  x ^= x >> 27;
  x = x * 0x2545F4914F6CDD1DULL;
  *state = x;
  return x;
}

uint64_t splitmix64(uint64_t *state) {
  uint64_t result = (*state += 0x9E3779B97f4A7C15ULL);
  result = (result ^ (result >> 30)) * 0xBF58476D1CE4E5B9ULL;
  result = (result ^ (result >> 27)) * 0x94D049BB133111EBULL;
  result = result ^ (result >> 31);
  *state = result;
  return result;
}

void random_init(uint64_t seed) {
  if (seed == 0) {
    xorshift32_state_global = 0xF1A6;
    xorshift64_state_global = 0xDEAD10CC;
    xorshift128_state_init_64(&xorshift128_state_global, 0xBADDCAFE,
                              0xDEADDEAD);
    xorshift64s_state_global = 0xDEADBEEF;
    splitmix64_state_global = 0xFEE1DEAD;
  } else {
    xorshift32_state_global = seed & 0xFFFF;
    xorshift64_state_global = seed;
    uint64_t temp = seed;
    xorshift128_state_init_64(&xorshift128_state_global, splitmix64(&temp),
                              seed);
    xorshift64s_state_global = seed;
    splitmix64_state_global = seed;
  }
}

// Defines a global version of a compatible algorithm.
// For example,
//      DEFINE_INPLACE_RANDOM_KL_ALGORITHM(uint64_t, xorshift64s)
// will create
//      uint64_t xorshift64s_E(&xorshift64s_state_global);
// Luckily, this macro is undefined after use.
#define DEFINE_INPLACE_RANDOM_KL_ALGORITHM(type, name)                         \
  type name##_E() { return name(&name##_state_global); }

DEFINE_INPLACE_RANDOM_KL_ALGORITHM(uint32_t, xorshift32)
DEFINE_INPLACE_RANDOM_KL_ALGORITHM(uint64_t, xorshift64)
DEFINE_INPLACE_RANDOM_KL_ALGORITHM(uint64_t, xorshift64s)
DEFINE_INPLACE_RANDOM_KL_ALGORITHM(uint32_t, xorshift128)
DEFINE_INPLACE_RANDOM_KL_ALGORITHM(uint64_t, splitmix64)
#undef DEFINE_INPLACE_RANDOM_KL_ALGORITHM

/*
#include <stdio.h>
int main() {
  random_init(0);
  printf("%u\n", xorshift32_E());
  printf("%lu\n", xorshift64_E());
  printf("%u\n", xorshift128_E());
  printf("%lu\n", xorshift64s_E());
  printf("%lu\n", splitmix64_E());

  printf("%u\n", xorshift32_E());
  printf("%lu\n", xorshift64_E());
  printf("%u\n", xorshift128_E());
  printf("%lu\n", xorshift64s_E());
  printf("%lu\n", splitmix64_E());
}
*/
