#include "utest/utest.h"
#include "utf8.h"

#define clear_buff(buffer)                                                     \
  do {                                                                         \
    memset(&(buffer[0]), 0, sizeof(buffer));                                   \
  } while (0)

#define TEST_SETUP()                                                           \
  set_utf8_lib_error(0);                                                       \
  utf8_chr buff[255];                                                          \
  utf8_chr *buff_ptr = &(buff[0]);                                             \
  int err;

//////////////////////////////////////////////////////////////////////
// SECTION: utf8_from_codepoint                                     //
//////////////////////////////////////////////////////////////////////

UTEST(utf8_from_codepoint, single_byte) {
  TEST_SETUP();
  uint32_t n;

  clear_buff(buff);
  n = utf8_from_codepoint(0x1B98, buff_ptr);
  err = get_utf8_lib_error();
  ASSERT_EQ(n, (utf8_code_pt)3);
  ASSERT_EQ(buff[0], (char)0xE1);
  ASSERT_EQ(buff[1], (char)0xAE);
  ASSERT_EQ(buff[2], (char)0x98);
  ASSERT_EQ(buff[3], (char)0x0);
  ASSERT_EQ(err, 0);
  set_utf8_lib_error(0);

  clear_buff(buff);
  n = utf8_from_codepoint(0x0, buff_ptr);
  err = get_utf8_lib_error();
  ASSERT_EQ(n, (utf8_code_pt)1);
  ASSERT_EQ(buff[0], (char)0x0);
  ASSERT_EQ(buff[1], (char)0x0);
  ASSERT_EQ(buff[2], (char)0x0);
  ASSERT_EQ(buff[3], (char)0x0);
  ASSERT_EQ(err, 0);
  set_utf8_lib_error(0);
}

UTEST(utf8_from_codepoint, multi_byte) {
  TEST_SETUP();
  uint32_t n;

  clear_buff(buff);
  n = utf8_from_codepoint(0x7F, buff_ptr);
  err = get_utf8_lib_error();
  err = get_utf8_lib_error();
  ASSERT_EQ(n, (utf8_code_pt)1);
  ASSERT_EQ(buff[0], (char)0x7F);
  ASSERT_EQ(buff[1], (char)0x0);
  ASSERT_EQ(buff[2], (char)0x0);
  ASSERT_EQ(buff[3], (char)0x0);
  ASSERT_EQ(err, 0);
  set_utf8_lib_error(0);

  // Lambda symbol
  clear_buff(buff);
  n = utf8_from_codepoint(0x03BB, buff_ptr);
  err = get_utf8_lib_error();
  ASSERT_EQ(n, (utf8_code_pt)2);
  ASSERT_EQ(buff[0], (char)0xCE);
  ASSERT_EQ(buff[1], (char)0xBB);
  ASSERT_EQ(buff[2], (char)0x0);
  ASSERT_EQ(buff[3], (char)0x0);
  ASSERT_EQ(err, 0);
  set_utf8_lib_error(0);

  // Dagger symbol (†)
  clear_buff(buff);
  n = utf8_from_codepoint(0x2020, buff_ptr);
  err = get_utf8_lib_error();
  ASSERT_EQ(n, (utf8_code_pt)3);
  ASSERT_EQ(buff[0], (char)0xE2);
  ASSERT_EQ(buff[1], (char)0x80);
  ASSERT_EQ(buff[2], (char)0xA0);
  ASSERT_EQ(buff[3], (char)0x0);
  ASSERT_EQ(err, 0);
  set_utf8_lib_error(0);

  // Summation symbol (∑)
  clear_buff(buff);
  n = utf8_from_codepoint(0x2211, buff_ptr);
  err = get_utf8_lib_error();
  ASSERT_EQ(n, (utf8_code_pt)3);
  ASSERT_EQ(buff[0], (char)0xE2);
  ASSERT_EQ(buff[1], (char)0x88);
  ASSERT_EQ(buff[2], (char)0x91);
  ASSERT_EQ(buff[3], (char)0x0);
  ASSERT_EQ(err, 0);
  set_utf8_lib_error(0);

  // "CUNEIFORM SIGN LAK-490" (𒓻)
  clear_buff(buff);
  n = utf8_from_codepoint(0x124FB, buff_ptr);
  err = get_utf8_lib_error();
  ASSERT_EQ(n, (utf8_code_pt)4);
  ASSERT_EQ(buff[0], (char)0xF0);
  ASSERT_EQ(buff[1], (char)0x92);
  ASSERT_EQ(buff[2], (char)0x93);
  ASSERT_EQ(buff[3], (char)0xBB);
  ASSERT_EQ(err, 0);
  set_utf8_lib_error(0);

  // "EGYPTIAN HIEROGLYPH I007" (𓆏)
  clear_buff(buff);
  n = utf8_from_codepoint(0x1318F, buff_ptr);
  err = get_utf8_lib_error();
  ASSERT_EQ(n, (utf8_code_pt)4);
  ASSERT_EQ(buff[0], (char)0xF0);
  ASSERT_EQ(buff[1], (char)0x93);
  ASSERT_EQ(buff[2], (char)0x86);
  ASSERT_EQ(buff[3], (char)0x8F);
  ASSERT_EQ(err, 0);
  set_utf8_lib_error(0);

  // Largest possible codepoint
  clear_buff(buff);
  n = utf8_from_codepoint(0x10FFFF, buff_ptr);
  err = get_utf8_lib_error();
  ASSERT_EQ(n, (utf8_code_pt)4);
  ASSERT_EQ(buff[0], (char)0xF4);
  ASSERT_EQ(buff[1], (char)0x8F);
  ASSERT_EQ(buff[2], (char)0xBF);
  ASSERT_EQ(buff[3], (char)0xBF);
  ASSERT_EQ(err, 0);
  set_utf8_lib_error(0);
}

UTEST(utf8_from_codepoint, error_cases) {
  TEST_SETUP();
  uint32_t n;

  // Error case: Larger than possible range.
  clear_buff(buff);
  n = utf8_from_codepoint(0x110000, buff_ptr);
  err = get_utf8_lib_error();
  ASSERT_EQ(n, UINT32_MAX);
  ASSERT_EQ(buff[0], (char)0x00);
  ASSERT_EQ(buff[1], (char)0x00);
  ASSERT_EQ(buff[2], (char)0x00);
  ASSERT_EQ(buff[3], (char)0x00);
  ASSERT_EQ(err, INVALID_UNICODE_CODEPOINT);
  set_utf8_lib_error(0);

  // Error case: Larger than possible range. (Maximum 32-bit integer)
  clear_buff(buff);
  n = utf8_from_codepoint(0xFFFFFFFF, buff_ptr);
  err = get_utf8_lib_error();
  ASSERT_EQ(n, UINT32_MAX);
  ASSERT_EQ(buff[0], (char)0x00);
  ASSERT_EQ(buff[1], (char)0x00);
  ASSERT_EQ(buff[2], (char)0x00);
  ASSERT_EQ(buff[3], (char)0x00);
  ASSERT_EQ(err, INVALID_UNICODE_CODEPOINT);
}

//////////////////////////////////////////////////////////////////////
// SECTION: utf8_to_codepoint                                       //
//////////////////////////////////////////////////////////////////////

UTEST(utf8_to_codepoint, single_byte) {
  TEST_SETUP();
  (void)buff_ptr;
  uint32_t n;

  // Empty utf8 char
  clear_buff(buff);
  n = utf8_to_codepoint(&(buff[0]));
  err = get_utf8_lib_error();
  ASSERT_EQ(n, 0U);
  ASSERT_EQ(buff[0], 0);
  ASSERT_EQ(buff[1], 0);
  ASSERT_EQ(buff[2], 0);
  ASSERT_EQ(buff[3], 0);
  ASSERT_EQ(err, 0);
  set_utf8_lib_error(0);

  // Single byte (ASCII) utf8 char
  clear_buff(buff);
  buff[0] = 'A';
  n = utf8_to_codepoint(&(buff[0]));
  err = get_utf8_lib_error();
  ASSERT_EQ(n, (uint32_t)'A');
  ASSERT_EQ(buff[0], 'A');
  ASSERT_EQ(buff[1], 0);
  ASSERT_EQ(buff[2], 0);
  ASSERT_EQ(buff[3], 0);
  ASSERT_EQ(err, 0);
  set_utf8_lib_error(0);
}

UTEST(utf8_to_codepoint, multi_byte) {
  TEST_SETUP();
  (void)buff_ptr;
  uint32_t n;

  // Dagger symbol (3 bytes)
  clear_buff(buff);
  buff[0] = (utf8_chr)0xE2;
  buff[1] = (utf8_chr)0x80;
  buff[2] = (utf8_chr)0xA0;
  n = utf8_to_codepoint(&(buff[0]));
  err = get_utf8_lib_error();
  ASSERT_EQ(n, (uint32_t)0x2020);
  ASSERT_EQ(buff[0], (char)0xE2);
  ASSERT_EQ(buff[1], (char)0x80);
  ASSERT_EQ(buff[2], (char)0xA0);
  ASSERT_EQ(buff[3], (char)0x0);
  ASSERT_EQ(err, 0);
  set_utf8_lib_error(0);

  // Largest possible codepoint
  clear_buff(buff);
  buff[0] = (utf8_chr)0xF4;
  buff[1] = (utf8_chr)0x8F;
  buff[2] = (utf8_chr)0xBF;
  buff[3] = (utf8_chr)0xBF;
  n = utf8_to_codepoint(&(buff[0]));
  err = get_utf8_lib_error();
  ASSERT_EQ(n, (uint32_t)0x10FFFF);
  ASSERT_EQ(buff[0], (char)0xF4);
  ASSERT_EQ(buff[1], (char)0x8F);
  ASSERT_EQ(buff[2], (char)0xBF);
  ASSERT_EQ(buff[3], (char)0xBF);
  ASSERT_EQ(err, 0);
  set_utf8_lib_error(0);
}

UTEST(utf8_to_codepoint, error_cases) {
  TEST_SETUP();
  (void)buff_ptr;
  uint32_t n;

  // Error case: Larger than possible range. (first byte illegal)
  clear_buff(buff);
  buff[0] = (utf8_chr)0xFF;
  buff[1] = (utf8_chr)0x00;
  buff[2] = (utf8_chr)0x00;
  buff[3] = (utf8_chr)0x00;
  n = utf8_to_codepoint(&(buff[0]));
  err = get_utf8_lib_error();
  ASSERT_EQ(n, ~((uint32_t)0));
  ASSERT_EQ(buff[0], (char)0xFF);
  ASSERT_EQ(buff[1], (char)0x00);
  ASSERT_EQ(buff[2], (char)0x00);
  ASSERT_EQ(buff[3], (char)0x00);
  ASSERT_EQ(err, INVALID_UTF8_SYMBOL);
  set_utf8_lib_error(0);

  // Error case: Illegal codepoint. (first byte is legal, but second byte is
  // not)
  clear_buff(buff);
  buff[0] = (utf8_chr)0xF0;
  buff[1] = (utf8_chr)0xF0;
  buff[2] = (utf8_chr)0x00;
  buff[3] = (utf8_chr)0x00;
  n = utf8_to_codepoint(&(buff[0]));
  err = get_utf8_lib_error();
  ASSERT_EQ(n, UINT32_MAX);
  ASSERT_EQ(buff[0], (char)0xF0);
  ASSERT_EQ(buff[1], (char)0xF0);
  ASSERT_EQ(buff[2], (char)0x00);
  ASSERT_EQ(buff[3], (char)0x00);
  ASSERT_EQ(err, INVALID_UTF8_SYMBOL);
  set_utf8_lib_error(0);
}

//////////////////////////////////////////////////////////////////////
// SECTION: utf8_char_valid                                         //
//////////////////////////////////////////////////////////////////////

UTEST(utf8_char_valid, ) {
  TEST_SETUP();
  (void)buff_ptr;
  bool res;

  // Empty utf8 char
  clear_buff(buff);
  res = utf8_char_valid(&(buff[0]));
  err = get_utf8_lib_error();
  ASSERT_EQ(res, true);
  ASSERT_EQ(err, 0);
  set_utf8_lib_error(0);

  // Single byte (ASCII) utf8 char
  clear_buff(buff);
  buff[0] = 'A';
  res = utf8_char_valid(&(buff[0]));
  err = get_utf8_lib_error();
  ASSERT_EQ(res, true);
  ASSERT_EQ(err, 0);
  set_utf8_lib_error(0);

  // First byte implies a single-byte char, second byte is not legal.
  // Since the second char is never checked, the char is technically
  // valid.
  clear_buff(buff);
  buff[0] = 'A';
  buff[1] = 0;
  res = utf8_char_valid(&(buff[0]));
  err = get_utf8_lib_error();
  ASSERT_EQ(res, true);
  ASSERT_EQ(err, 0);
  set_utf8_lib_error(0);

  // Completely invalid
  clear_buff(buff);
  buff[0] = (utf8_chr)0xFF;
  buff[1] = (utf8_chr)0xFF;
  buff[2] = (utf8_chr)0xFF;
  buff[3] = (utf8_chr)0xFF;
  res = utf8_char_valid(&(buff[0]));
  err = get_utf8_lib_error();
  ASSERT_EQ(res, false);
  ASSERT_EQ(err, 0);
  set_utf8_lib_error(0);
}

//////////////////////////////////////////////////////////////////////
// SECTION: utf8_strlen                                             //
//////////////////////////////////////////////////////////////////////

UTEST(utf8_strlen, ascii_only) {
  TEST_SETUP();
  (void)buff_ptr;
  size_t n;

  // Empty utf8 string has length 0
  clear_buff(buff);
  n = utf8_strlen(&(buff[0]));
  err = get_utf8_lib_error();
  ASSERT_EQ(n, (size_t)0);
  ASSERT_EQ(err, 0);
  set_utf8_lib_error(0);

  // ASCII string
  clear_buff(buff);
  utf8_chr *hello_world_str = utf8_reinterpret_string("Hello World!");
  n = utf8_strlen(hello_world_str);
  err = get_utf8_lib_error();
  ASSERT_EQ(n, (size_t)12);
  ASSERT_EQ(err, 0);
  set_utf8_lib_error(0);
}

UTEST(utf8_strlen, including_utf8) {
  TEST_SETUP();
  (void)buff_ptr;
  size_t n;

  // Single utf8 symbol followed by 0
  clear_buff(buff);
  buff[0] = (utf8_chr)0xE2;
  buff[1] = (utf8_chr)0x80;
  buff[2] = (utf8_chr)0xA0;
  n = utf8_strlen(&(buff[0]));
  err = get_utf8_lib_error();
  ASSERT_EQ(n, (size_t)1);
  ASSERT_EQ(err, 0);
  set_utf8_lib_error(0);

  // A 0-byte before an actual string. The function thinks that the string is
  // empty (correct behavior)
  clear_buff(buff);
  buff[0] = (utf8_chr)0x0;
  buff[1] = (utf8_chr)0xE2;
  buff[2] = (utf8_chr)0x80;
  buff[3] = (utf8_chr)0xA0;
  n = utf8_strlen(&(buff[0]));
  err = get_utf8_lib_error();
  ASSERT_EQ(n, (size_t)0);
  ASSERT_EQ(err, 0);
  set_utf8_lib_error(0);

  // Single utf8 symbol (3 bytes) followed by 3 ASCII chars followed by utf8
  // symbol (2 bytes) followed by 0 Expected length is 5.
  clear_buff(buff);
  buff[0] = (utf8_chr)0xE2;
  buff[1] = (utf8_chr)0x80;
  buff[2] = (utf8_chr)0xA0;
  buff[3] = 'A';
  buff[4] = 'B';
  buff[5] = 'C';
  buff[6] = (utf8_chr)0xCE;
  buff[7] = (utf8_chr)0xBB;
  n = utf8_strlen(&(buff[0]));
  err = get_utf8_lib_error();
  ASSERT_EQ(n, (size_t)5);
  ASSERT_EQ(err, 0);
  set_utf8_lib_error(0);

  // Single 2-byte symbol
  clear_buff(buff);
  buff[0] = (utf8_chr)0xCE;
  buff[1] = (utf8_chr)0xBB;
  n = utf8_strlen(&(buff[0]));
  err = get_utf8_lib_error();
  ASSERT_EQ(n, (size_t)1);
  ASSERT_EQ(err, 0);
  set_utf8_lib_error(0);

  // Largest possible codepoint (still length 1)
  clear_buff(buff);
  buff[0] = (utf8_chr)0xF4;
  buff[1] = (utf8_chr)0x8F;
  buff[2] = (utf8_chr)0xBF;
  buff[3] = (utf8_chr)0xBF;
  n = utf8_strlen(&(buff[0]));
  err = get_utf8_lib_error();
  ASSERT_EQ(n, (size_t)1);
  ASSERT_EQ(err, 0);
  set_utf8_lib_error(0);
}

UTEST(utf8_strlen, error_cases) {
  TEST_SETUP();
  (void)buff_ptr;
  size_t n;

  // Illegal symbol (maximum)
  clear_buff(buff);
  buff[0] = (utf8_chr)0xFF;
  buff[1] = (utf8_chr)0xFF;
  buff[2] = (utf8_chr)0xFF;
  buff[3] = (utf8_chr)0xFF;
  n = utf8_strlen(&(buff[0]));
  err = get_utf8_lib_error();
  ASSERT_EQ(n, SIZE_MAX);
  ASSERT_EQ(err, INVALID_UTF8_SYMBOL);
  set_utf8_lib_error(0);
}

//////////////////////////////////////////////////////////////////////
// SECTION: utf8_strchr                                             //
//////////////////////////////////////////////////////////////////////

UTEST(utf8_strchr, found) {
  TEST_SETUP();
  const utf8_chr *pos;

  // Found at first byte
  clear_buff(buff);
  buff[0] = 'A';
  pos = utf8_strchr(buff_ptr, 'A');
  err = get_utf8_lib_error();
  ASSERT_EQ((char *)pos, (char *)buff_ptr);
  ASSERT_EQ(err, 0);
  set_utf8_lib_error(0);

  // Found later
  clear_buff(buff);
  buff[0] = 'Y';
  buff[1] = 'e';
  buff[2] = 'a';
  buff[3] = 'h';
  pos = utf8_strchr(buff_ptr, 'a');
  err = get_utf8_lib_error();
  ASSERT_EQ((char *)pos, (char *)(buff_ptr + 2));
  ASSERT_EQ(err, 0);
  set_utf8_lib_error(0);

  // Found, but not ASCII
  clear_buff(buff);
  buff[0] = 'Y';
  buff[1] = 'e';
  buff[2] = (utf8_chr)0xE2;
  buff[3] = (utf8_chr)0x80;
  buff[4] = (utf8_chr)0xA0;
  pos = utf8_strchr(buff_ptr, 0x2020);
  err = get_utf8_lib_error();
  ASSERT_EQ(pos, (buff_ptr + 2));
  ASSERT_EQ(err, 0);
  set_utf8_lib_error(0);
}
UTEST(utf8_strchr, not_found) {
  TEST_SETUP();
  const utf8_chr *pos;

  // Empty utf8 string does not contain 'A'.
  clear_buff(buff);
  pos = utf8_strchr(buff_ptr, 'A');
  err = get_utf8_lib_error();
  ASSERT_EQ(pos, (utf8_chr *)NULL);
  ASSERT_EQ(err, 0);
  set_utf8_lib_error(0);

  // Not found, but length>0
  clear_buff(buff);
  buff[0] = 'Y';
  buff[1] = 'e';
  buff[2] = 'a';
  buff[3] = 'h';
  pos = utf8_strchr(buff_ptr, 'Z');
  err = get_utf8_lib_error();
  ASSERT_EQ(pos, (utf8_chr *)NULL);
  ASSERT_EQ(err, 0);
  set_utf8_lib_error(0);
}
UTEST(utf8_strchr, error_cases) {
  TEST_SETUP();
  const char *pos;

  // Invalid codepoint
  clear_buff(buff);
  buff[0] = 'Y';
  buff[1] = 'e';
  pos = utf8_strchr(buff_ptr, UINT32_MAX);
  err = get_utf8_lib_error();
  ASSERT_EQ(pos, (utf8_chr *)NULL);
  ASSERT_EQ(err, INVALID_UNICODE_CODEPOINT);
  set_utf8_lib_error(0);

  // Invalid string
  clear_buff(buff);
  buff[0] = (utf8_chr)0xFF;
  buff[1] = (utf8_chr)0x8F;
  buff[2] = (utf8_chr)0xBF;
  buff[3] = (utf8_chr)0xBF;
  pos = utf8_strchr(buff_ptr, 'A');
  err = get_utf8_lib_error();
  ASSERT_EQ(pos, (char *)NULL);
  ASSERT_EQ(err, INVALID_UTF8_SYMBOL);
  set_utf8_lib_error(0);
}

//////////////////////////////////////////////////////////////////////
// SECTION: utf8_string_valid                                       //
//////////////////////////////////////////////////////////////////////

UTEST(utf8_string_valid, valid) {
  TEST_SETUP();
  bool res;

  // Empty utf8 string is valid
  clear_buff(buff);
  res = utf8_string_valid(buff_ptr);
  err = get_utf8_lib_error();
  ASSERT_EQ(res, true);
  ASSERT_EQ(err, 0);

  // ASCII string is valid
  res = utf8_string_valid((utf8_chr *)"Hello world!");
  err = get_utf8_lib_error();
  ASSERT_EQ(res, true);
  ASSERT_EQ(err, 0);

  // ASCII whitespaces are valid
  res = utf8_string_valid((utf8_chr *)" \t\r\n");
  err = get_utf8_lib_error();
  ASSERT_EQ(res, true);
  ASSERT_EQ(err, 0);

  // Single utf8 symbol (3 bytes) followed by 3 ASCII chars followed by utf8
  // symbol (2 bytes) followed by 0. Should be valid.
  clear_buff(buff);
  buff[0] = (utf8_chr)0xE2;
  buff[1] = (utf8_chr)0x80;
  buff[2] = (utf8_chr)0xA0;
  buff[3] = 'A';
  buff[4] = 'B';
  buff[5] = 'C';
  buff[6] = (utf8_chr)0xCE;
  buff[7] = (utf8_chr)0xBB;
  res = utf8_string_valid(buff_ptr);
  err = get_utf8_lib_error();
  ASSERT_EQ(res, true);
  ASSERT_EQ(err, 0);
}

UTEST(utf8_string_valid, invalid) {
  TEST_SETUP();
  bool res;

  // 3 valid characters (utf8, 3x ASCII) and an invalid char (first byte).
  clear_buff(buff);
  buff[0] = (utf8_chr)0xE2;
  buff[1] = (utf8_chr)0x80;
  buff[2] = (utf8_chr)0xA0;
  buff[3] = 'A';
  buff[4] = 'B';
  buff[5] = 'C';
  buff[6] = (utf8_chr)0xFF; // invalid
  buff[7] = (utf8_chr)0x8F;
  buff[8] = (utf8_chr)0x8F;
  buff[9] = (utf8_chr)0x8F;
  res = utf8_string_valid(buff_ptr);
  err = get_utf8_lib_error();
  ASSERT_EQ(res, false);
  ASSERT_EQ(err, 0);

  // 3 valid characters (utf8, 3x ASCII) and an invalid char (2nd byte).
  clear_buff(buff);
  buff[0] = (utf8_chr)0xE2;
  buff[1] = (utf8_chr)0x80;
  buff[2] = (utf8_chr)0xA0;
  buff[3] = 'A';
  buff[4] = 'B';
  buff[5] = 'C';
  buff[6] = (utf8_chr)0xE2;
  buff[7] = (utf8_chr)0xF0; // invalid
  buff[8] = (utf8_chr)0xA0;
  res = utf8_string_valid(buff_ptr);
  err = get_utf8_lib_error();
  ASSERT_EQ(res, false);
  ASSERT_EQ(err, 0);
}

UTEST_MAIN()
