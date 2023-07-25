
#ifndef KL_RAND_H
#define KL_RAND_H
#include <stdint.h>

struct xorshift128_state {
  uint32_t x[4];
};

uint32_t xorshift32(uint32_t *state);

uint64_t xorshift64(uint64_t *state);

uint32_t xorshift128(struct xorshift128_state *state);

static struct xorshift128_state *
xorshift128_state_init_64(struct xorshift128_state *state, uint64_t upper_seed,
                          uint64_t lower_seed);

uint64_t xorshift64s(uint64_t *state);

uint64_t splitmix64(uint64_t *state);

void random_init(uint64_t seed);

uint32_t xorshift32_E();
uint64_t xorshift64_E();
uint64_t xorshift64s_E();
uint32_t xorshift128_E();
uint64_t splitmix64_E();

#endif
