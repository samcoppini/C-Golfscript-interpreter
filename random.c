// random.c
// Contains functions for generating random numbers

#include <time.h>
#include "golf.h"

// The internal state used by the RNG
static uint64_t rng_state[16];

// The position in the rng_state array that will be used to produce the
// next random number
static unsigned pos;

// Seeds the random number generator with the current time
void init_rng() {
  rng_state[0] = time(NULL);
  for (int i = 1; i < 16; i++) {
    rng_state[i] = 1812433253 * (rng_state[i - 1] ^ rng_state[i - 1] >> 30);
  }
}

// Implements the xorshift1024* algorithm to produce a random 64-bit integer
uint64_t get_random() {
  uint64_t s0 = rng_state[pos];
  pos = (pos + 1) % 16;
  uint64_t s1 = rng_state[pos];
  s1 ^= s1 << 31;
  rng_state[pos] = s1 ^ s0 ^ (s1 >> 11) ^ (s0 >> 30);
  return rng_state[pos] * 1181783497276652981;
}

// Returns a number in the range of [0, max_val)
// Returns 0 if max_val is negative
Bigint get_randint(Bigint max_val) {
  if (bigint_is_zero(&max_val) || max_val.is_negative) {
    return new_bigint();
  }
  else {
    Bigint rand_num = bigint_with_digits(max_val.length);
    for (uint32_t i = 0; i < max_val.length; i++) {
      if (i + 1 < max_val.length) {
        rand_num.digits[i] = get_random();
      }
      else {
        uint64_t result = get_random();
        // Prevent modulo bias
        while (result > (UINT64_MAX - (UINT64_MAX % max_val.digits[i]))) {
            result = get_random();
        }
        rand_num.digits[i] = result % max_val.digits[i];
      }
    }
    return rand_num;
  }
}
