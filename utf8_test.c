#include "utf8.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef uint64_t test_result;

#define NUM_TOTAL_CASES(x) ((x) >> 32)
#define NUM_SUCCESSES(x) ((x)&0xFFFFFFFF)
#define TEST_RESULT(total, success) ((((uint64_t)(total)) << 32) | (success))

/*
#define TEST_CASE(name, fn) do { printf("Running \"%s\"\n", name); test_result
res = fn(); printf("%s: %lu / %lu\n", name, NUM_SUCCESSES(res),
NUM_TOTAL_CASES(res)); } while(0)
*/
#define TEST_CASE(name, fn)                                                    \
  do {                                                                         \
    printf("Running \"%s\"\n", name);                                          \
    test_result res = fn();                                                    \
    printf("%s: %lu / %lu\n", name, NUM_SUCCESSES(res), NUM_TOTAL_CASES(res)); \
  } while (0)

#define RUN_ASSERT(x, expect, format)                                          \
  do {                                                                         \
    total_cases++;                                                             \
    if ((x) == expect)                                                         \
      successes++;                                                             \
    else                                                                       \
      printf("%s(), line: %d; Expected " format ", got " format ".\n",         \
             __FUNCTION__, __LINE__, expect, (x));                             \
  } while (0)

#define clear_buff(buffer, size)                                               \
  do {                                                                         \
    memset(&(buffer[0]), 0, size * sizeof(buffer[0]));                         \
  } while (0)

test_result test_utf8_from_codepoint(void) {
  set_utf8_lib_error(0);
  utf8_chr buff[4];
  uint32_t n;
  uint32_t total_cases = 0;
  uint32_t successes = 0;
  int err;

  clear_buff(buff, 4);
  n = utf8_from_codepoint(0x1B98, &(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, 3, "%u");
  RUN_ASSERT(buff[0], 0xE1, "%X");
  RUN_ASSERT(buff[1], 0xAE, "%X");
  RUN_ASSERT(buff[2], 0x98, "%X");
  RUN_ASSERT(buff[3], 0x0, "%X");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);

  clear_buff(buff, 4);
  n = utf8_from_codepoint(0x0, &(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, 1, "%u");
  RUN_ASSERT(buff[0], 0, "%X");
  RUN_ASSERT(buff[1], 0x0, "%X");
  RUN_ASSERT(buff[2], 0x0, "%X");
  RUN_ASSERT(buff[3], 0x0, "%X");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);

  clear_buff(buff, 4);
  n = utf8_from_codepoint(0x7F, &(buff[0]));
  err = get_utf8_lib_error();
  err = get_utf8_lib_error();
  RUN_ASSERT(n, 1, "%u");
  RUN_ASSERT(buff[0], 0x7F, "%X");
  RUN_ASSERT(buff[1], 0x0, "%X");
  RUN_ASSERT(buff[2], 0x0, "%X");
  RUN_ASSERT(buff[3], 0x0, "%X");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);

  // Lambda symbol
  clear_buff(buff, 4);
  n = utf8_from_codepoint(0x03BB, &(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, 2, "%u");
  RUN_ASSERT(buff[0], 0xCE, "%X");
  RUN_ASSERT(buff[1], 0xBB, "%X");
  RUN_ASSERT(buff[2], 0x0, "%X");
  RUN_ASSERT(buff[3], 0x0, "%X");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);

  // Dagger symbol (†)
  clear_buff(buff, 4);
  n = utf8_from_codepoint(0x2020, &(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, 3, "%u");
  RUN_ASSERT(buff[0], 0xE2, "%X");
  RUN_ASSERT(buff[1], 0x80, "%X");
  RUN_ASSERT(buff[2], 0xA0, "%X");
  RUN_ASSERT(buff[3], 0x0, "%X");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);

  // Summation symbol (∑)
  clear_buff(buff, 4);
  n = utf8_from_codepoint(0x2211, &(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, 3, "%u");
  RUN_ASSERT(buff[0], 0xE2, "%X");
  RUN_ASSERT(buff[1], 0x88, "%X");
  RUN_ASSERT(buff[2], 0x91, "%X");
  RUN_ASSERT(buff[3], 0x0, "%X");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);

  // "CUNEIFORM SIGN LAK-490" (𒓻)
  clear_buff(buff, 4);
  n = utf8_from_codepoint(0x124FB, &(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, 4, "%u");
  RUN_ASSERT(buff[0], 0xF0, "%X");
  RUN_ASSERT(buff[1], 0x92, "%X");
  RUN_ASSERT(buff[2], 0x93, "%X");
  RUN_ASSERT(buff[3], 0xBB, "%X");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);

  // "EGYPTIAN HIEROGLYPH I007" (𓆏)
  clear_buff(buff, 4);
  n = utf8_from_codepoint(0x1318F, &(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, 4, "%u");
  RUN_ASSERT(buff[0], 0xF0, "%X");
  RUN_ASSERT(buff[1], 0x93, "%X");
  RUN_ASSERT(buff[2], 0x86, "%X");
  RUN_ASSERT(buff[3], 0x8F, "%X");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);

  // Largest possible codepoint
  clear_buff(buff, 4);
  n = utf8_from_codepoint(0x10FFFF, &(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, 4, "%u");
  RUN_ASSERT(buff[0], 0xF4, "%X");
  RUN_ASSERT(buff[1], 0x8F, "%X");
  RUN_ASSERT(buff[2], 0xBF, "%X");
  RUN_ASSERT(buff[3], 0xBF, "%X");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);

  // Error case: Larger than possible range.
  clear_buff(buff, 4);
  n = utf8_from_codepoint(0x110000, &(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, 0, "%u");
  RUN_ASSERT(buff[0], 0x00, "%X");
  RUN_ASSERT(buff[1], 0x00, "%X");
  RUN_ASSERT(buff[2], 0x00, "%X");
  RUN_ASSERT(buff[3], 0x00, "%X");
  RUN_ASSERT(err, INVALID_UNICODE_CODEPOINT, "%d");
  set_utf8_lib_error(0);

  // Error case: Larger than possible range. (Maximum 32-bit integer)
  clear_buff(buff, 4);
  n = utf8_from_codepoint(0xFFFFFFFF, &(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, 0, "%u");
  RUN_ASSERT(buff[0], 0x00, "%X");
  RUN_ASSERT(buff[1], 0x00, "%X");
  RUN_ASSERT(buff[2], 0x00, "%X");
  RUN_ASSERT(buff[3], 0x00, "%X");
  RUN_ASSERT(err, INVALID_UNICODE_CODEPOINT, "%d");
  set_utf8_lib_error(0);

  return TEST_RESULT(total_cases, successes);
}

test_result test_utf8_to_codepoint(void) {
  set_utf8_lib_error(0);
  utf8_chr buff[4];
  uint32_t n;
  uint32_t total_cases = 0;
  uint32_t successes = 0;
  int err;
 
  // Empty utf8 char
  clear_buff(buff, 4);
  n = utf8_to_codepoint(&(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, 0, "%u");
  RUN_ASSERT(buff[0], 0, "%X");
  RUN_ASSERT(buff[1], 0, "%X");
  RUN_ASSERT(buff[2], 0, "%X");
  RUN_ASSERT(buff[3], 0, "%X");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);
 
  // Single byte (ASCII) utf8 char
  clear_buff(buff, 4);
  buff[0] = 'A';
  n = utf8_to_codepoint(&(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, (int)'A', "%u");
  RUN_ASSERT(buff[0], 'A', "%X");
  RUN_ASSERT(buff[1], 0, "%X");
  RUN_ASSERT(buff[2], 0, "%X");
  RUN_ASSERT(buff[3], 0, "%X");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);
 
  // Dagger symbol (3 bytes)
  clear_buff(buff, 4);
  buff[0]= 0xE2;
  buff[1]= 0x80;
  buff[2]= 0xA0;
  n = utf8_to_codepoint(&(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, 0x2020, "%u");
  RUN_ASSERT(buff[0], 0xE2, "%X");
  RUN_ASSERT(buff[1], 0x80, "%X");
  RUN_ASSERT(buff[2], 0xA0, "%X");
  RUN_ASSERT(buff[3], 0x0, "%X");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);
 
  // Largest possible codepoint
  clear_buff(buff, 4);
  buff[0]= 0xF4;
  buff[1]= 0x8F;
  buff[2]= 0xBF;
  buff[3]= 0xBF;
  n = utf8_to_codepoint(&(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n,0x10FFFF, "%u");
  RUN_ASSERT(buff[0], 0xF4, "%X");
  RUN_ASSERT(buff[1], 0x8F, "%X");
  RUN_ASSERT(buff[2], 0xBF, "%X");
  RUN_ASSERT(buff[3], 0xBF, "%X");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);
 
  // Error case: Larger than possible range. (first byte illegal)
  clear_buff(buff, 4);
  buff[0]= 0xFF;
  buff[1]= 0x00;
  buff[2]= 0x00;
  buff[3]= 0x00;
  n = utf8_to_codepoint(&(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, ~((uint32_t)0), "%u");
  RUN_ASSERT(buff[0], 0xFF, "%X");
  RUN_ASSERT(buff[1], 0x00, "%X");
  RUN_ASSERT(buff[2], 0x00, "%X");
  RUN_ASSERT(buff[3], 0x00, "%X");
  RUN_ASSERT(err, INVALID_UTF8_SYMBOL, "%d");
  set_utf8_lib_error(0);
 
  // Error case: Illegal codepoint. (first byte is legal, but second byte is not)
  clear_buff(buff, 4);
  buff[0]= 0xF0;
  buff[1]= 0xF0;
  buff[2]= 0x00;
  buff[3]= 0x00;
  n = utf8_to_codepoint(&(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, UINT32_MAX, "%u");
  RUN_ASSERT(buff[0], 0xF0, "%X");
  RUN_ASSERT(buff[1], 0xF0, "%X");
  RUN_ASSERT(buff[2], 0x00, "%X");
  RUN_ASSERT(buff[3], 0x00, "%X");
  RUN_ASSERT(err, INVALID_UTF8_SYMBOL, "%d");
  set_utf8_lib_error(0);
 
  return TEST_RESULT(total_cases, successes);
}
 
test_result test_utf8_char_valid(void) {
  set_utf8_lib_error(0);
  utf8_chr buff[4];
  uint32_t res;
  uint32_t total_cases = 0;
  uint32_t successes = 0;
  int err;
 
  // Empty utf8 char
  clear_buff(buff, 4);
  res = utf8_char_valid(&(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(res, true, "%u");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);
 
  // Single byte (ASCII) utf8 char
  clear_buff(buff, 4);
  buff[0] = 'A';
  res = utf8_char_valid(&(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(res, true, "%u");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);
 
  // First byte implies a single-byte char, second byte is not legal.
  // Since the second char is never checked, the char is technically
  // valid.
  clear_buff(buff, 4);
  buff[0] = 'A';
  buff[1] = 0;
  res = utf8_char_valid(&(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(res, true, "%u");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);
 
  // Completely invalid
  clear_buff(buff, 4);
  buff[0] = 0xFF;
  buff[1] = 0xFF;
  buff[2] = 0xFF;
  buff[3] = 0xFF;
  res = utf8_char_valid(&(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(res, false, "%u");
  RUN_ASSERT(err, 0, "%d"); // Does not set error
  set_utf8_lib_error(0);
 
  return TEST_RESULT(total_cases, successes);
}
 
test_result test_utf8_strlen(void) {
  set_utf8_lib_error(0);
  utf8_chr buff[17];
  size_t n;
  uint32_t total_cases = 0;
  uint32_t successes = 0;
  int err;
 
  // Empty utf8 string has length 0
  clear_buff(buff, 17);
  n = utf8_strlen(&(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, (size_t)0, "%lu");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);
 
  // ASCII string
  clear_buff(buff, 17);
  utf8_chr *hello_world_str = utf8_reinterpret_string("Hello World!");
  n = utf8_strlen(hello_world_str);
  err = get_utf8_lib_error();
  RUN_ASSERT(n, (size_t)12, "%lu");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);
 
  // Single utf8 symbol followed by 0
  clear_buff(buff, 17);
  buff[0]= 0xE2;
  buff[1]= 0x80;
  buff[2]= 0xA0;
  n = utf8_strlen(&(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, (size_t)1, "%lu");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);
  
  //11001110 10111011
 
  // Single utf8 symbol (3 bytes) followed by 3 ASCII chars followed by utf8 symbol (2 bytes) followed by 0
  // Expected length is 5.
  clear_buff(buff, 17);
  buff[0]= 0xE2;
  buff[1]= 0x80;
  buff[2]= 0xA0;
  buff[3]= 'A';
  buff[4]= 'B';
  buff[5]= 'C';
  buff[6]= 0xCE;
  buff[7]= 0xBB;
  n = utf8_strlen(&(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, (size_t)5, "%lu");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);
  
  clear_buff(buff, 17);
  buff[0]= 0xCE;
  buff[1]= 0xBB;
  n = utf8_strlen(&(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, (size_t)1, "%lu");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);
 
  // Largest possible codepoint (still length 1)
  clear_buff(buff, 4);
  buff[0]= 0xF4;
  buff[1]= 0x8F;
  buff[2]= 0xBF;
  buff[3]= 0xBF;
  n = utf8_strlen(&(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, (size_t)1, "%zu");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);
 
  // Illegal symbol (maximum
  clear_buff(buff, 4);
  buff[0]= 0xFF;
  buff[1]= 0xFF;
  buff[2]= 0xFF;
  buff[3]= 0xFF;
  n = utf8_strlen(&(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, SIZE_MAX, "%zu");
  RUN_ASSERT(err, INVALID_UTF8_SYMBOL, "%d");
  set_utf8_lib_error(0);
 
  return TEST_RESULT(total_cases, successes);
}

int main(int argc, char **args) {
  (void)argc;
  (void)args;

  TEST_CASE("Get utf-8 bytes from codepoint", test_utf8_from_codepoint);
  TEST_CASE("Get codepoint from utf-8 bytes", test_utf8_to_codepoint);
  TEST_CASE("Check utf-8 bytes for validity", test_utf8_char_valid);
  TEST_CASE("Test utf-8 string length      ", test_utf8_strlen);

  return 0;
}
