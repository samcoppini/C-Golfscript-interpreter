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

Bigint get_randint(Bigint max_val) {
  if (bigint_is_zero(&max_val)) {
    return new_bigint();
  }
  else {
    Bigint rand_num = bigint_with_digits(max_val.length);
    for (uint32_t i = 0; i < max_val.length; i++) {
      if (i + 1 < max_val.length)
        rand_num.digits[i] = get_random();
      else
        rand_num.digits[i] = get_random() % max_val.digits[i];
    }
    return rand_num;
  }
}
