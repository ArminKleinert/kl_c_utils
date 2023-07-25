#include "utf8.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

typedef uint64_t test_result;

#define NUM_TOTAL_CASES(x) ((x) >> 32)
#define NUM_SUCCESSES(x) ((x) & 0xFFFFFFFF)
#define TEST_RESULT(total, success) ((((uint64_t) (total)) << 32) | (success))

/*
#define TEST_CASE(name, fn) do { printf("Running \"%s\"\n", name); test_result res = fn(); printf("%s: %lu / %lu\n", name, NUM_SUCCESSES(res), NUM_TOTAL_CASES(res)); } while(0)
*/
#define TEST_CASE(name, fn) do { printf("Running \"%s\"\n", name); test_result res = fn(); printf("%s: %lu / %lu\n", name, NUM_SUCCESSES(res), NUM_TOTAL_CASES(res)); } while(0)

#define RUN_ASSERT(x, expect, format) do { total_cases ++; if ((x) == expect) successes++; else printf("%s(), line: %d; Expected " format ", got " format ".\n", __FUNCTION__, __LINE__, expect, (x)); } while(0)

#define clear_buff(buffer, size) do{memset(&(buffer[0]), 0, size*sizeof(buffer[0]));}while(0)


test_result test_utf8_from_codepoint(void) {
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

int main(int argc, char **args) {
  (void)argc;
  (void)args;
  
  // printf("%u\n", cl1(0x80000000));
  //return fromStdin();
  TEST_CASE("Get utf-8 bytes from codepoint", test_utf8_from_codepoint);
  
  return 0;
}
