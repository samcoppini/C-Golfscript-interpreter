// bigint.c
// Contains functions for the manipulation of arbitrary-sized integers

#include <assert.h>
#include <stdlib.h>
#include "golf.h"

// Returns a new bigint, initialized to zero
Bigint new_bigint() {
  Bigint num = {calloc(1, sizeof(uint64_t)), 1, 1, false};
  return num;
}

void free_bigint(Bigint *num) {
  free(num->digits);
}

// Returns a new bigint, all digits initially set to zero, but with a certain
// pre-set length
Bigint bigint_with_digits(uint32_t num_digits) {
  uint32_t to_allocate = 1;
  while (num_digits > to_allocate) {
    to_allocate <<= 1;
  }
  Bigint num = {
    .digits = calloc(to_allocate, sizeof(uint64_t)),
    .length = num_digits,
    .allocated = to_allocate,
    .is_negative = false
  };
  return num;
}

Bigint bigint_from_int64(int64_t int_val) {
  Bigint num = new_bigint();
  if (int_val < 0) {
    int_val *= -1;
    num.is_negative = true;
  }
  num.digits[0] = int_val;
  return num;
}

Bigint bigint_from_uint64(uint64_t int_val) {
  Bigint num = new_bigint();
  num.digits[0] = int_val;
  return num;
}

Bigint bigint_from_string(const String *str) {
  Bigint result = new_bigint();
  Bigint ten = bigint_from_int64(10);

  uint32_t start = (str->str_data[0] == '-' ? 1: 0);
  for (uint32_t i = start; i < str->length; i++) {
    Bigint digit = bigint_from_int64(str->str_data[i] - '0');
    Bigint result_temp = bigint_multiply(&result, &ten);

    free_bigint(&result);
    bigint_add(&result_temp, &digit);
    result = result_temp;
    free_bigint(&digit);
  }

  if (str->str_data[0] == '-' && !bigint_is_zero(&result)) {
    result.is_negative = true;
  }

  free_bigint(&ten);

  return result;
}

String bigint_to_string(const Bigint *num) {
  String num_str = new_string();
  if (bigint_is_zero(num)) {
    string_add_char(&num_str, '0');
    return num_str;
  }
  else {
    Bigint num_copy = copy_bigint(num);
    Bigint ten_quintillion = bigint_from_uint64(10000000000000000000ULL);

    while (!bigint_is_zero(&num_copy)) {
      Bigint remainder, quotient;
      bigint_divmod(&num_copy, &ten_quintillion, &quotient, &remainder);
      for (uint32_t i = 0; i < 19; i++) {
        if (remainder.digits[0] != 0 || !bigint_is_zero(&quotient)) {
          string_add_char(&num_str, (remainder.digits[0] % 10) + '0');
          remainder.digits[0] /= 10;
        }
      }
      free_bigint(&remainder);
      free_bigint(&num_copy);
      num_copy = quotient;
    }

    if (num->is_negative) {
      string_add_char(&num_str, '-');
    }
    string_reverse(&num_str);

    free_bigint(&num_copy);
    free_bigint(&ten_quintillion);

    return num_str;
  }
}

bool bigint_fits_in_uint32(const Bigint *num) {
  return num->length == 1 && num->digits[0] < (1ULL << 32);
}

uint32_t bigint_to_uint32(const Bigint *num) {
  assert(bigint_fits_in_uint32(num));
  assert(!num->is_negative);

  return num->digits[0];
}

Bigint copy_bigint(const Bigint *to_copy) {
  Bigint new_num = bigint_with_digits(to_copy->length);
  for (uint32_t i = 0; i < to_copy->length; i++) {
    new_num.digits[i] = to_copy->digits[i];
  }
  new_num.is_negative = to_copy->is_negative;
  return new_num;
}

bool bigint_is_zero(const Bigint *num) {
  return num->length == 1 && num->digits[0] == 0;
}

// Adds a digit to the front of a bigint, initializing the digit to zero, and
// allocating additional memory for the number if need be
static inline void bigint_add_digit(Bigint *num) {
  if (num->length == num->allocated) {
    num->allocated <<= 1;
    num->digits = realloc(num->digits, sizeof(uint64_t) * num->allocated);
  }
  num->digits[num->length] = 0;
  num->length++;
}

// Removes all the zeros from the front of a bigint, and corrects a bigint
// if it's "negative zero"
static inline void bigint_remove_leading_zeros(Bigint *num) {
  for (uint32_t i = num->length - 1; i > 0; i--) {
    if (num->digits[i] != 0) {
      return;
    }
    else {
      num->length--;
    }
  }
  // Correct negative zero
  if (num->digits[0] == 0)
    num->is_negative = false;
}

static inline void bigint_do_increment(Bigint *num) {
  bool carry = true;
  for (uint32_t i = 0; carry && i < num->length; i++) {
    num->digits[i]++;
    carry = (num->digits[i] == 0);
  }
  if (carry) {
    bigint_add_digit(num);
    num->digits[num->length - 1] = 1;
  }
}

static inline void bigint_do_decrement(Bigint *num) {
  if (bigint_is_zero(num)) {
    num->digits[0] = 1;
    num->is_negative = !num->is_negative;
  }
  else {
    bool borrow = true;
    for (uint32_t i = 0; borrow && i < num->length; i++) {
      borrow = (num->digits[i] == 0);
      num->digits[i]--;
    }
    bigint_remove_leading_zeros(num);
  }
}

void bigint_increment(Bigint *num) {
  if (num->is_negative) {
    bigint_do_decrement(num);
  }
  else {
    bigint_do_increment(num);
  }
}

void bigint_decrement(Bigint *num) {
  if (num->is_negative) {
    bigint_do_increment(num);
  }
  else {
    bigint_do_decrement(num);
  }
}

static inline void bigint_bitshift_left(Bigint *num) {
  bigint_add_digit(num);
  uint64_t carry = 0;
  for (uint32_t i = 0; i < num->length; i++) {
    uint64_t old_carry = carry;
    carry = num->digits[i] >> 63;
    num->digits[i] <<= 1;
    num->digits[i] |= old_carry;
  }
  bigint_remove_leading_zeros(num);
}

// Compares the absolute values of two bigints, returning 1 if the first
// is larger, -1 if the second is larger, and 0 if they're equal
static int bigint_compare_absolute(const Bigint *a, const Bigint *b) {
  if (a->length != b->length) {
    if (a->length > b->length)
      return 1;
    else
      return -1;
  }
  else {
    for (int64_t i = a->length - 1; i >= 0; i--) {
      if (a->digits[i] > b->digits[i])
        return 1;
      else if (a->digits[i] < b->digits[i])
        return -1;
    }
    return 0;
  }
}

// Compares two bigints to each other, returning 1 if the first bigint is
// bigger, -1 if the second bigint is larger, or 0 if they are equal
int bigint_compare(const Bigint *a, const Bigint *b) {
  if (a->is_negative) {
    if (b->is_negative) {
      return bigint_compare_absolute(a, b) * -1;
    }
    else {
      return -1;
    }
  }
  else if (b->is_negative) {
    return 1;
  }
  else {
    return bigint_compare_absolute(a, b);
  }
}

// Returns a bigint's two's complement representation
// Used only for bitwise operations which would result in erroneous
// results if the bigints weren't in two's complement representation
static Bigint bigint_convert_to_twos_complement(const Bigint *num) {
  Bigint twos_num = copy_bigint(num);
  if (twos_num.digits[twos_num.length - 1] & (1ULL << 63))
    bigint_add_digit(&twos_num);

  if (twos_num.is_negative) {
    for (uint32_t i = 0; i < twos_num.length; i++) {
      twos_num.digits[i] = ~twos_num.digits[i];
    }
    bigint_decrement(&twos_num);
  }
  return twos_num;
}

// Converts a number in two's complement to the representation for Bigints
static void bigint_convert_from_twos_complement(Bigint *num) {
  if (num->digits[num->length - 1] & (1ULL << 63)) {
    num->is_negative = true;
    bigint_increment(num);
    for (uint32_t i = 0; i < num->length; i++) {
      num->digits[i] = ~num->digits[i];
    }
  }
  else {
    num->is_negative = false;
  }
  bigint_remove_leading_zeros(num);
}

// Performs a bitwise or on two bigints
Bigint bigint_or(const Bigint *a, const Bigint *b) {
  Bigint num1 = bigint_convert_to_twos_complement(a);
  Bigint num2 = bigint_convert_to_twos_complement(b);

  Bigint result = bigint_with_digits(max(num1.length, num2.length));
  for (uint32_t i = 0; i < result.length; i++) {
    if (i < num1.length)
      result.digits[i] |= num1.digits[i];
    else if (num1.is_negative)
      result.digits[i] |= UINT64_MAX;

    if (i < num2.length)
      result.digits[i] |= num2.digits[i];
    else if (num2.is_negative)
      result.digits[i] |= UINT64_MAX;
  }

  free_bigint(&num1);
  free_bigint(&num2);
  bigint_convert_from_twos_complement(&result);

  return result;
}

// Performs a bitwise and on two bigints
Bigint bigint_and(const Bigint *a, const Bigint *b) {
  Bigint num1 = bigint_convert_to_twos_complement(a);
  Bigint num2 = bigint_convert_to_twos_complement(b);

  Bigint result = bigint_with_digits(max(num1.length, num2.length));
  for (uint32_t i = 0; i < result.length; i++) {
    if (i >= num1.length) {
      if (num1.is_negative)
        result.digits[i] = num2.digits[i];
      else
        break;
    }
    else if (i >= num2.length) {
      if (num2.is_negative)
        result.digits[i] = num1.digits[i];
      else
        break;
    }
    else {
      result.digits[i] = num1.digits[i] & num2.digits[i];
    }
  }

  free_bigint(&num1);
  free_bigint(&num2);
  bigint_convert_from_twos_complement(&result);

  return result;
}

// Performs a bitwise xor on two bigints
Bigint bigint_xor(const Bigint *a, const Bigint *b) {
  Bigint num1 = bigint_convert_to_twos_complement(a);
  Bigint num2 = bigint_convert_to_twos_complement(b);

  Bigint result = bigint_with_digits(max(num1.length, num2.length));
  for (uint32_t i = 0; i < result.length; i++) {
    if (i < num1.length)
      result.digits[i] ^= num1.digits[i];
    else if (num1.is_negative)
      result.digits[i] ^= UINT64_MAX;

    if (i < num2.length)
      result.digits[i] ^= num2.digits[i];
    else if (num2.is_negative)
      result.digits[i] ^= UINT64_MAX;
  }

  free_bigint(&num1);
  free_bigint(&num2);
  bigint_convert_from_twos_complement(&result);

  return result;
}

// Adds one bigint to another. Does not handle signs correctly, just blindly
// adds the digits to each other
static void bigint_do_add(Bigint *a, const Bigint *b) {
  uint32_t digit = 0;
  bool carry = false;

  while (digit < b->length || carry) {
    if (digit >= a->length) {
      bigint_add_digit(a);
    }

    bool overflowed = false;

    if (b->length > digit) {
      a->digits[digit] += b->digits[digit];
      overflowed = (a->digits[digit] < b->digits[digit]);
    }

    if (carry) {
      a->digits[digit]++;
      overflowed |= (a->digits[digit] == 0);
    }

    carry = overflowed;
    digit++;
  }
}

// Subtracts one bigint from another
// Does not make any considerations for the bigints' signs, and assumes
// that a is greater than or equal to b
static void bigint_do_subtract(Bigint *a, const Bigint *b) {
  bool borrow = false;

  for (uint32_t i = 0; i < b->length || borrow; i++) {
    if (borrow) {
      borrow = (a->digits[i] == 0);
      a->digits[i]--;
    }

    if (i < b->length) {
      if (b->digits[i] > a->digits[i]) {
        borrow = true;
      }
      a->digits[i] -= b->digits[i];
    }
  }

  bigint_remove_leading_zeros(a);
}

// Returns the result of adding a and b, taking into account the signs of
// the numbers
void bigint_add(Bigint *a, const Bigint *b) {
  if (a->is_negative == b->is_negative) {
    bigint_do_add(a, b);
  }
  else {
    int comp_result = bigint_compare_absolute(a, b);
    if (comp_result > 0) {
      bigint_do_subtract(a, b);
    }
    else if (comp_result < 0) {
      Bigint result = copy_bigint(b);
      bigint_do_subtract(&result, a);
      free_bigint(a);
      *a = result;
    }
    else {
      // They're equal, just with different signs. They cancel each other out,
      // so we set a to zero
      a->length = 1;
      a->digits[0] = 0;
      a->is_negative = false;
    }
  }
}

// Returns the result of subtracting b from a, taking into account the signs
// of the numbers
void bigint_subtract(Bigint *a, const Bigint *b) {
  if (a->is_negative != b->is_negative) {
    bigint_do_add(a, b);
  }
  else {
    int comp_result = bigint_compare_absolute(a, b);
    if (comp_result > 0) {
      bigint_do_subtract(a, b);
    }
    else if (comp_result < 0) {
      Bigint result = copy_bigint(b);
      bigint_do_subtract(&result, a);
      result.is_negative = !a->is_negative;
      free_bigint(a);
      *a = result;
    }
    else {
      // They're equal to each other, so they cancel each other out. In this
      // case, we set a to 0.
      a->length = 1;
      a->digits[0] = 0;
      a->is_negative = false;
    }
  }
}

// Adds b to a, and returns whether the addition caused a to overflow
static bool add_check_overflow(uint64_t *a, uint64_t b) {
  uint64_t original_a = *a;
  *a += b;
  return *a < original_a;
}

// Returns the result of multiplying a and b, but doesn't adjust the
// sign of the number
static Bigint bigint_do_multiply(const Bigint *a, const Bigint *b) {
  Bigint result = new_bigint();

  for (uint32_t i = 0; i < a->length; i++) {
    uint64_t overflow = 0;

    for (uint32_t j = 0; j < b->length; j++) {
      uint32_t cur_digit = i + j;

      if (result.length <= cur_digit) {
        bigint_add_digit(&result);
      }

      // In order to multiply the two digits of the numbers together, but
      // protect against overflow, we break up this multiplication into four
      // Where the first 32 bits and last 32 bits of the digits are each
      // multiplied by each other and are shifted and added together to get the
      // correct result in regards to overflow
      uint64_t lower_a = a->digits[i] & UINT32_MAX;
      uint64_t lower_b = b->digits[j] & UINT32_MAX;
      uint64_t upper_a = a->digits[i] >> 32;
      uint64_t upper_b = b->digits[j] >> 32;

      uint64_t res1 = lower_a * lower_b;
      uint64_t res2 = lower_a * upper_b;
      uint64_t res3 = upper_a * lower_b;
      uint64_t res4 = upper_a * upper_b;

      if (add_check_overflow(&result.digits[cur_digit], overflow)) {
        res4++;
      }

      overflow = res4;

      if (add_check_overflow(&result.digits[cur_digit], res1)) {
        overflow++;
      }

      if (add_check_overflow(&result.digits[cur_digit], res2 << 32)) {
        overflow++;
      }

      if (add_check_overflow(&result.digits[cur_digit], res3 << 32)) {
        overflow++;
      }

      overflow += res2 >> 32;
      overflow += res3 >> 32;
  }

    if (overflow) {
      bigint_add_digit(&result);
      result.digits[i + b->length] = overflow;
    }
  }

  return result;
}

// Returns the result of multiplying a and b, but making the result
// negative or positive appropriately
Bigint bigint_multiply(const Bigint *a, const Bigint *b) {
  Bigint result = bigint_do_multiply(a, b);
  result.is_negative = (a->is_negative != b->is_negative);
  bigint_remove_leading_zeros(&result);
  return result;
}

// Divides a by b, and returns the quotient and remainder through the passed
// in pointers
void bigint_divmod(const Bigint *a, const Bigint *b, Bigint *quotient_result,
                   Bigint *remainder_result)
{
  assert(!bigint_is_zero(b));

  Bigint quotient = new_bigint();
  Bigint remainder = new_bigint();

  for (int64_t i = a->length - 1; i >= 0; i--) {
    for (int32_t j = 63; j >= 0; j--) {
      bigint_bitshift_left(&quotient);
      bigint_bitshift_left(&remainder);

      remainder.digits[0] |= ((a->digits[i] & (1ULL << j)) >> j);
      if (bigint_compare_absolute(b, &remainder) <= 0) {
        bigint_do_subtract(&remainder, b);
        quotient.digits[0] |= 1;
      }
    }
  }

  if (remainder_result == NULL) {
    free_bigint(&remainder);
  }
  else {
    *remainder_result = remainder;
    if (!bigint_is_zero(remainder_result)) {
        remainder_result->is_negative = b->is_negative;
    }
  }

  if (quotient_result == NULL) {
    free_bigint(&quotient);
  }
  else {
    *quotient_result = quotient;
    quotient_result->is_negative = (a->is_negative != b->is_negative);
  }
}

Bigint bigint_exponent(const Bigint *base, const Bigint *exponent) {
  Bigint result = bigint_from_int64(1);
  Bigint digit_val = copy_bigint(base);
  for (uint32_t i = 0; i < exponent->length; i++) {
    for (uint64_t j = 1;
         j > 0 && (i + 1 < exponent->length || j <= exponent->digits[i]);
         j <<= 1)
    {
      if (j & exponent->digits[i]) {
        Bigint temp_result = bigint_multiply(&result, &digit_val);
        free_bigint(&result);
        result = temp_result;
      }
      Bigint squared_digit = bigint_multiply(&digit_val, &digit_val);
      free_bigint(&digit_val);
      digit_val = squared_digit;
    }
  }
  free_bigint(&digit_val);
  return result;
}
