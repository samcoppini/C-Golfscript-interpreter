// random.c
// Contains functions for generating random numbers

#include <time.h>
#include "golf.h"

uint64_t rng_state[16];
int pos;

void init_rng() {
  rng_state[0] = time(NULL);
  for (int i = 1; i < 16; i++) {
    rng_state[i] = 1812433253 * (rng_state[i - 1] ^ rng_state[i - 1] >> 30);
  }
}

uint64_t get_random() {
  uint64_t s0 = rng_state[pos];
  pos = (pos + 1) & 0xF;
  uint64_t s1 = rng_state[pos];
  s1 ^= s1 << 31;
  rng_state[pos] = s1 ^ s0 ^ (s1 >> 11) ^ (s0 >> 30);
  return rng_state[pos] * 0x106689d45497fdb5;
}

int64_t get_randint(int64_t max_val) {
  if (max_val < 1) {
    return 0;
  }
  else {
    return get_random() % max_val;
  }
}
