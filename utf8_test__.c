#include "utf8.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef uint64_t test_result;

#define NUM_TOTAL_CASES(x) (((x) >> 32) & 0xFFFFFFFF)
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
  RUN_ASSERT(buff[0], (char)0xE1, "%X");
  RUN_ASSERT(buff[1], (char)0xAE, "%X");
  RUN_ASSERT(buff[2], (char)0x98, "%X");
  RUN_ASSERT(buff[3], (char)0x0, "%X");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);

  clear_buff(buff, 4);
  n = utf8_from_codepoint(0x0, &(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, 1, "%u");
  RUN_ASSERT(buff[0], (char)0, "%X");
  RUN_ASSERT(buff[1], (char)0x0, "%X");
  RUN_ASSERT(buff[2], (char)0x0, "%X");
  RUN_ASSERT(buff[3], (char)0x0, "%X");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);

  clear_buff(buff, 4);
  n = utf8_from_codepoint(0x7F, &(buff[0]));
  err = get_utf8_lib_error();
  err = get_utf8_lib_error();
  RUN_ASSERT(n, 1, "%u");
  RUN_ASSERT(buff[0], (char)0x7F, "%X");
  RUN_ASSERT(buff[1], (char)0x0, "%X");
  RUN_ASSERT(buff[2], (char)0x0, "%X");
  RUN_ASSERT(buff[3], (char)0x0, "%X");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);

  // Lambda symbol
  clear_buff(buff, 4);
  n = utf8_from_codepoint(0x03BB, &(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, 2, "%u");
  RUN_ASSERT(buff[0], (char)0xCE, "%X");
  RUN_ASSERT(buff[1], (char)0xBB, "%X");
  RUN_ASSERT(buff[2], (char)0x0, "%X");
  RUN_ASSERT(buff[3], (char)0x0, "%X");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);

  // Dagger symbol (†)
  clear_buff(buff, 4);
  n = utf8_from_codepoint(0x2020, &(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, 3, "%u");
  RUN_ASSERT(buff[0], (char)0xE2, "%X");
  RUN_ASSERT(buff[1], (char)0x80, "%X");
  RUN_ASSERT(buff[2], (char)0xA0, "%X");
  RUN_ASSERT(buff[3], (char)0x0, "%X");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);

  // Summation symbol (∑)
  clear_buff(buff, 4);
  n = utf8_from_codepoint(0x2211, &(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, 3, "%u");
  RUN_ASSERT(buff[0], (char)0xE2, "%X");
  RUN_ASSERT(buff[1], (char)0x88, "%X");
  RUN_ASSERT(buff[2], (char)0x91, "%X");
  RUN_ASSERT(buff[3], (char)0x0, "%X");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);

  // "CUNEIFORM SIGN LAK-490" (𒓻)
  clear_buff(buff, 4);
  n = utf8_from_codepoint(0x124FB, &(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, 4, "%u");
  RUN_ASSERT(buff[0], (char)0xF0, "%X");
  RUN_ASSERT(buff[1], (char)0x92, "%X");
  RUN_ASSERT(buff[2], (char)0x93, "%X");
  RUN_ASSERT(buff[3], (char)0xBB, "%X");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);

  // "EGYPTIAN HIEROGLYPH I007" (𓆏)
  clear_buff(buff, 4);
  n = utf8_from_codepoint(0x1318F, &(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, 4, "%u");
  RUN_ASSERT(buff[0], (char)0xF0, "%X");
  RUN_ASSERT(buff[1], (char)0x93, "%X");
  RUN_ASSERT(buff[2], (char)0x86, "%X");
  RUN_ASSERT(buff[3], (char)0x8F, "%X");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);

  // Largest possible codepoint
  clear_buff(buff, 4);
  n = utf8_from_codepoint(0x10FFFF, &(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, 4, "%u");
  RUN_ASSERT(buff[0], (char)0xF4, "%X");
  RUN_ASSERT(buff[1], (char)0x8F, "%X");
  RUN_ASSERT(buff[2], (char)0xBF, "%X");
  RUN_ASSERT(buff[3], (char)0xBF, "%X");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);

  // Error case: Larger than possible range.
  clear_buff(buff, 4);
  n = utf8_from_codepoint(0x110000, &(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, UINT32_MAX, "%u");
  RUN_ASSERT(buff[0], (char)0x00, "%X");
  RUN_ASSERT(buff[1], (char)0x00, "%X");
  RUN_ASSERT(buff[2], (char)0x00, "%X");
  RUN_ASSERT(buff[3], (char)0x00, "%X");
  RUN_ASSERT(err, INVALID_UNICODE_CODEPOINT, "%d");
  set_utf8_lib_error(0);

  // Error case: Larger than possible range. (Maximum 32-bit integer)
  clear_buff(buff, 4);
  n = utf8_from_codepoint(0xFFFFFFFF, &(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, UINT32_MAX, "%u");
  RUN_ASSERT(buff[0], (char)0x00, "%X");
  RUN_ASSERT(buff[1], (char)0x00, "%X");
  RUN_ASSERT(buff[2], (char)0x00, "%X");
  RUN_ASSERT(buff[3], (char)0x00, "%X");
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
  RUN_ASSERT(buff[0], (char)0, "%X");
  RUN_ASSERT(buff[1], (char)0, "%X");
  RUN_ASSERT(buff[2], (char)0, "%X");
  RUN_ASSERT(buff[3], (char)0, "%X");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);

  // Single byte (ASCII) utf8 char
  clear_buff(buff, 4);
  buff[0] = 'A';
  n = utf8_to_codepoint(&(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, (int)'A', "%u");
  RUN_ASSERT(buff[0], 'A', "%X");
  RUN_ASSERT(buff[1], (char)0, "%X");
  RUN_ASSERT(buff[2], (char)0, "%X");
  RUN_ASSERT(buff[3], (char)0, "%X");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);

  // Dagger symbol (3 bytes)
  clear_buff(buff, 4);
  buff[0] = 0xE2;
  buff[1] = 0x80;
  buff[2] = 0xA0;
  n = utf8_to_codepoint(&(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, 0x2020, "%u");
  RUN_ASSERT(buff[0], (char)0xE2, "%X");
  RUN_ASSERT(buff[1], (char)0x80, "%X");
  RUN_ASSERT(buff[2], (char)0xA0, "%X");
  RUN_ASSERT(buff[3], (char)0x0, "%X");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);

  // Largest possible codepoint
  clear_buff(buff, 4);
  buff[0] = 0xF4;
  buff[1] = 0x8F;
  buff[2] = 0xBF;
  buff[3] = 0xBF;
  n = utf8_to_codepoint(&(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, 0x10FFFF, "%u");
  RUN_ASSERT(buff[0], (char)0xF4, "%X");
  RUN_ASSERT(buff[1], (char)0x8F, "%X");
  RUN_ASSERT(buff[2], (char)0xBF, "%X");
  RUN_ASSERT(buff[3], (char)0xBF, "%X");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);

  // Error case: Larger than possible range. (first byte illegal)
  clear_buff(buff, 4);
  buff[0] = 0xFF;
  buff[1] = 0x00;
  buff[2] = 0x00;
  buff[3] = 0x00;
  n = utf8_to_codepoint(&(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, ~((uint32_t)0), "%u");
  RUN_ASSERT(buff[0], (char)0xFF, "%X");
  RUN_ASSERT(buff[1], (char)0x00, "%X");
  RUN_ASSERT(buff[2], (char)0x00, "%X");
  RUN_ASSERT(buff[3], (char)0x00, "%X");
  RUN_ASSERT(err, INVALID_UTF8_SYMBOL, "%d");
  set_utf8_lib_error(0);

  // Error case: Illegal codepoint. (first byte is legal, but second byte is
  // not)
  clear_buff(buff, 4);
  buff[0] = 0xF0;
  buff[1] = 0xF0;
  buff[2] = 0x00;
  buff[3] = 0x00;
  n = utf8_to_codepoint(&(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, UINT32_MAX, "%u");
  RUN_ASSERT(buff[0], (char)0xF0, "%X");
  RUN_ASSERT(buff[1], (char)0xF0, "%X");
  RUN_ASSERT(buff[2], (char)0x00, "%X");
  RUN_ASSERT(buff[3], (char)0x00, "%X");
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
  buff[0] = 0xE2;
  buff[1] = 0x80;
  buff[2] = 0xA0;
  n = utf8_strlen(&(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, (size_t)1, "%lu");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);

  // Single utf8 symbol (3 bytes) followed by 3 ASCII chars followed by utf8
  // symbol (2 bytes) followed by 0 Expected length is 5.
  clear_buff(buff, 17);
  buff[0] = 0xE2;
  buff[1] = 0x80;
  buff[2] = 0xA0;
  buff[3] = 'A';
  buff[4] = 'B';
  buff[5] = 'C';
  buff[6] = 0xCE;
  buff[7] = 0xBB;
  n = utf8_strlen(&(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, (size_t)5, "%lu");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);

  clear_buff(buff, 17);
  buff[0] = 0xCE;
  buff[1] = 0xBB;
  n = utf8_strlen(&(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, (size_t)1, "%lu");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);

  // Largest possible codepoint (still length 1)
  clear_buff(buff, 4);
  buff[0] = 0xF4;
  buff[1] = 0x8F;
  buff[2] = 0xBF;
  buff[3] = 0xBF;
  n = utf8_strlen(&(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, (size_t)1, "%zu");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);

  // Illegal symbol (maximum
  clear_buff(buff, 4);
  buff[0] = 0xFF;
  buff[1] = 0xFF;
  buff[2] = 0xFF;
  buff[3] = 0xFF;
  n = utf8_strlen(&(buff[0]));
  err = get_utf8_lib_error();
  RUN_ASSERT(n, SIZE_MAX, "%zu");
  RUN_ASSERT(err, INVALID_UTF8_SYMBOL, "%d");
  set_utf8_lib_error(0);

  return TEST_RESULT(total_cases, successes);
}



test_result test_utf8_strchr(void) {
  set_utf8_lib_error(0);
  utf8_chr buff[17];
  utf8_chr *buff_ptr = &(buff[0]);
  utf8_chr *pos;
  uint32_t total_cases = 0;
  uint32_t successes = 0;
  int err;

  // Empty utf8 string does not contain 'A'.
  clear_buff(buff, 17);
  pos = utf8_strchr(buff_ptr, 'A');
  err = get_utf8_lib_error();
  RUN_ASSERT((void*)pos, NULL, "%p");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);

  // Found at first byte
  clear_buff(buff, 17);
  buff[0] = 'A';
  pos = utf8_strchr(buff_ptr, 'A');
  err = get_utf8_lib_error();
  RUN_ASSERT((void*)pos, (void*)buff_ptr, "%p");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);

  // Found later
  clear_buff(buff, 17);
  buff[0] = 'Y'; buff[1] = 'e';buff[2] = 'a';buff[3] = 'h';
  pos = utf8_strchr(buff_ptr, 'a');
  err = get_utf8_lib_error();
  RUN_ASSERT((void*)pos, (void*)(buff_ptr+2), "%p");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);

  // Not found, but length>0
  clear_buff(buff, 17);
  buff[0] = 'Y'; buff[1] = 'e';buff[2] = 'a';buff[3] = 'h';
  pos = utf8_strchr(buff_ptr, 'Z');
  err = get_utf8_lib_error();
  RUN_ASSERT((void*)pos, NULL, "%p");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);

  // Found, but not ASCII
  clear_buff(buff, 17);
  buff[0] = 'Y'; buff[1] = 'e';
  buff[2] = 0xE2;
  buff[3] = 0x80;
  buff[4] = 0xA0;
  pos = utf8_strchr(buff_ptr, 0x2020);
  err = get_utf8_lib_error();
  RUN_ASSERT((void*)pos, (void*)(buff_ptr+2), "%p");
  RUN_ASSERT(err, 0, "%d");
  set_utf8_lib_error(0);

  // Invalid codepoint
  clear_buff(buff, 17);
  buff[0] = 'Y'; buff[1] = 'e';
  pos = utf8_strchr(buff_ptr, UINT32_MAX);
  err = get_utf8_lib_error();
  RUN_ASSERT((void*)pos, NULL, "%p");
  RUN_ASSERT(err, INVALID_UNICODE_CODEPOINT, "%d");
  set_utf8_lib_error(0);

  // Invalid string
  clear_buff(buff, 17);
  buff[0] = 0xFF;
  buff[1] = 0x8F;
  buff[2] = 0xBF;
  buff[3] = 0xBF;
  pos = utf8_strchr(buff_ptr, 'A');
  err = get_utf8_lib_error();
  RUN_ASSERT((void*)pos, NULL, "%p");
  RUN_ASSERT(err, INVALID_UTF8_SYMBOL, "%d");
  set_utf8_lib_error(0);

  return TEST_RESULT(total_cases, successes);
}

test_result test_utf8_string_valid(void) {
  set_utf8_lib_error(0);
  utf8_chr buff[17];
  const utf8_chr *buff_ptr = &(buff[0]);
  bool pos;
  uint32_t total_cases = 0;
  uint32_t successes = 0;
  int err;

  // Empty utf8 string is valid
  clear_buff(buff, 17);
  pos = utf8_string_valid(buff_ptr);
  err = get_utf8_lib_error();
  RUN_ASSERT(pos, true, "%d");
  RUN_ASSERT(err, 0, "%d");

  // ASCII string is valid
  pos = utf8_string_valid((utf8_chr*)"Hello world!");
  err = get_utf8_lib_error();
  RUN_ASSERT(pos, true, "%d");
  RUN_ASSERT(err, 0, "%d");

  // Single utf8 symbol (3 bytes) followed by 3 ASCII chars followed by utf8
  // symbol (2 bytes) followed by 0. Should be valid.
  clear_buff(buff, 17);
  buff[0] = 0xE2;
  buff[1] = 0x80;
  buff[2] = 0xA0;
  buff[3] = 'A';
  buff[4] = 'B';
  buff[5] = 'C';
  buff[6] = 0xCE;
  buff[7] = 0xBB;
  pos = utf8_string_valid(buff_ptr);
  err = get_utf8_lib_error();
  RUN_ASSERT(pos, true, "%d");
  RUN_ASSERT(err, 0, "%d");

  // 3 valid characters (utf8, 3x ASCII) and an invalid char (first byte).
  clear_buff(buff, 17);
  buff[0] = 0xE2;
  buff[1] = 0x80;
  buff[2] = 0xA0;
  buff[3] = 'A';
  buff[4] = 'B';
  buff[5] = 'C';
  buff[6] = 0xFF; // invalid
  buff[7] = 0x8F;
  buff[8] = 0x8F;
  buff[9] = 0x8F;
  pos = utf8_string_valid(buff_ptr);
  err = get_utf8_lib_error();
  RUN_ASSERT(pos, false, "%d");
  RUN_ASSERT(err, 0, "%d");
  
  // 3 valid characters (utf8, 3x ASCII) and an invalid char (2nd byte).
  clear_buff(buff, 17);
  buff[0] = 0xE2;
  buff[1] = 0x80;
  buff[2] = 0xA0;
  buff[3] = 'A';
  buff[4] = 'B';
  buff[5] = 'C';
  buff[6] = 0xE2;
  buff[7] = 0xF0; // invalid
  buff[8] = 0xA0;
  pos = utf8_string_valid(buff_ptr);
  err = get_utf8_lib_error();
  RUN_ASSERT(pos, false, "%d");
  RUN_ASSERT(err, 0, "%d");

  return TEST_RESULT(total_cases, successes);
}

#define utf8_check_byte(b) (((uint8_t)(b & (3 << 6))) == ((uint8_t)(1 << 7)))

#include <limits.h>

int main(int argc, char **args) {
  (void)argc;
  (void)args;

  TEST_CASE("Get utf-8 bytes from codepoint", test_utf8_from_codepoint);
  TEST_CASE("Get codepoint from utf-8 bytes", test_utf8_to_codepoint);
  TEST_CASE("Check utf-8 bytes for validity", test_utf8_char_valid);
  TEST_CASE("Check utf-8 string for validity ", test_utf8_string_valid);
  TEST_CASE("Test utf-8 string length      ", test_utf8_strlen);
  TEST_CASE("Test test_utf8_strchr         ", test_utf8_strchr);

  /*
  uint8_t n = 'a';
  printf("(3<<6)                == 0x%X\n", (3 << 6));
  printf("(1<<7)                == 0x%X\n", (1 << 7));
  printf("utf8_check_byte(0x%X) == 0x%X\n", n, utf8_check_byte(n));
  printf("(0x%X & (3 << 6))     == 0x%X\n", n, (n & (3 << 6)));
  printf("!((0x%X&(3<<6))==(1<<7)) == %X\n", n, !(((uint8_t)(n & (3 << 6))) == (1 << 7)));
  */

  return 0;
}
