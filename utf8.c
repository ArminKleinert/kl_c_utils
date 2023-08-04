#include "utf8.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define utf8_check_byte(b) ((b & (3 << 6)) == (1 << 7))

int _utf8_lib_error = 0;

inline int set_utf8_lib_error(int code) {
  _utf8_lib_error = code;
  return _utf8_lib_error;
}

inline int get_utf8_lib_error(void) { return _utf8_lib_error; }

inline utf8_chr *utf8_reinterpret_string(const char *s) {
  union {
    utf8_chr *u8;
    const char *c;
  } p = {.c = s};
  return p.u8;
}

inline char *utf8_reinterpret_as_string(const utf8_chr *s) {
  union {
    const utf8_chr *u8;
    char *c;
  } p = {.u8 = s};
  return p.c;
}

int utf8_codepoint_bytes(const uint32_t c) {
  if (c < 0x80) {
    return 1;
  } else if (c < 0x0800) {
    return 2;
  } else if (c < 0x010000) {
    return 3;
  } else if (c < 0x110000) {
    return 4;
  }
  return 0;
}

// Take a unicode code point and stuff it into a byte-array of 4 bytes.
// Also, does some conversion into the correct format.
int utf8_from_codepoint(const uint32_t c, utf8_chr *buff) {
  if (c < 0x80) {
    buff[0] = ((c >> 0) & 0x7F) | 0x00;
    return 1;
  } else if (c < 0x0800) {
    buff[0] = ((c >> 6) & 0x1F) | 0xC0;
    buff[1] = ((c >> 0) & 0x3F) | 0x80;
    return 2;
  } else if (c < 0x010000) {
    buff[0] = ((c >> 12) & 0x0F) | 0xE0;
    buff[1] = ((c >> 6) & 0x3F) | 0x80;
    buff[2] = ((c >> 0) & 0x3F) | 0x80;
    return 3;
  } else if (c < 0x110000) {
    buff[0] = ((c >> 18) & 0x07) | 0xF0;
    buff[1] = ((c >> 12) & 0x3F) | 0x80;
    buff[2] = ((c >> 6) & 0x3F) | 0x80;
    buff[3] = ((c >> 0) & 0x3F) | 0x80;
    return 4;
  }
  set_utf8_lib_error(INVALID_UNICODE_CODEPOINT);
  return 0;
}

// Get the number of leading 1s. These indicate the number of bytes used:
// 0 (b[0] == 0XXX_XXXX) => 1 byte
// 2 (b[0] == 110X_XXXX) => 2 bytes
// 3 (b[0] == 1110_XXXX) => 3 bytes
// 4 (b[0] == 1111_0XXX) => 4 bytes
// Returns a number above 4 on error.
static int utf8_num_bytes_in_next_symbol(const utf8_chr b,
                                         const bool set_errno) {
  uint_fast8_t bitIndex;
  // Find first position of a 0-bit in number, going from left to right.
  for (bitIndex = 7; bitIndex > 0 && (b & (1 << bitIndex)) != 0; bitIndex--) {
  }

  // Pattern 0XXX_XXXX => single byte (ASCII)
  if (bitIndex == 7)
    return 1;

  // Error-case: invalid bit-pattern
  if (bitIndex == 6 || bitIndex < 3) {
    if (set_errno) {
      set_utf8_lib_error(INVALID_UTF8_SYMBOL);
    }
    return 0xFF;
  }

  return 7 - bitIndex;
}

// Checks whether a UTF-8 symbol (as byte pointer) is valid.
bool utf8_char_valid(utf8_chr *b) {
  int len = utf8_num_bytes_in_next_symbol(*b, false);
  if (len > 4)
    return false;
  if (len == 1)
    return (b[0] & (1 << 7)) == 0;
  for (int i = 1; i < len; i++) {
    if (!utf8_check_byte(b[i]))
      return false;
  }
  return true;
}

// Convert UTF-8 byte array into a unicode codepoint.
// Returns MAX_INT (~0) on error.
uint32_t utf8_to_codepoint(utf8_chr *b) {
  // Calculate the number of bytes the symbol has.
  const uint_fast8_t nbytes = utf8_num_bytes_in_next_symbol(b[0], true);

  // Error case: The symbol is not valid for utf-8.
  if (nbytes > 4) {
    return UINT32_MAX;
  }

  uint32_t c = 0;

  // Only 1 byte (a normal ASCII char). Just return it.
  if (nbytes == 1) {
    c = b[0];
    return c;
  }

  // The first byte holds the total number of bytes of the symbol as leading
  // 1-bits. These are not included in the symbol and have to be cut. 4 bytes =>
  // 1111_0XXX & 0000_0111) == 0000_0XXX 3 bytes => 1110_XXXX & 0000_1111) ==
  // 0000_XXXX 2 bytes => 110X_XXXX & 0001_1111) == 000X_XXXX We get the
  // bit-pattern by shifting 0x7F (0111_1111) by the number of bytes.
  c = b[0] & (0x7F >> nbytes);

  // Here, we know that there is more than one byte in the symbol. All bytes
  // after the first one have the format 0b10XX_XXXX. Thus, we use binary AND to
  // cut that off:
  // => (10XX_XXXX & 0x3F) == (10XX_XXXX & 0011_1111) == 00XX_XXXX
  const utf8_chr lowest_6_bits = 0x3F;
  for (uint_fast8_t i = 1; i < nbytes; i++) {
    c <<= 6;

    if (!utf8_check_byte(b[i])){
      set_utf8_lib_error(INVALID_UTF8_SYMBOL);
      return UINT32_MAX;
    }

    c += b[i] & lowest_6_bits;
  }

  return c;
}

// Max long on error
size_t utf8_strlen(utf8_chr *b) {
  size_t res = 0;
  for (size_t i = 0; b[i] != 0;) {
    int temp = utf8_num_bytes_in_next_symbol(b[i], true);
    if (temp > 4){
      return SIZE_MAX;
    }
    i += temp;
    res++;
  }
  return res;
}

// 0x7FFF on error
int utf8_str_cmp(utf8_chr *xs, utf8_chr *ys) {
  // if the two strings are at the same memory address, we don't need any more
  // complex calculations.
  if (xs == ys)
    return 0;

  while (*xs != 0 && *ys != 0) {
    int xs_sym_len = utf8_num_bytes_in_next_symbol(xs[0], true);
    if (xs_sym_len > 4)
      return 0x7FFF;

    int ys_sym_len = utf8_num_bytes_in_next_symbol(ys[0], true);
    if (ys_sym_len > 4)
      return 0x7FFF;

    if (xs_sym_len > ys_sym_len) {
      return 1;
    } else if (xs_sym_len < ys_sym_len) {
      return -1;
    } else {
      uint32_t xs_cp = utf8_to_codepoint(xs);
      uint32_t ys_cp = utf8_to_codepoint(ys);
      if (xs_cp > ys_cp) {
        return 1;
      } else if (xs_cp < ys_cp) {
        return -1;
      }
    }

    xs += xs_sym_len;
    ys += ys_sym_len;
  }
  return 0;
}

utf8_chr *utf8_strchr(utf8_chr *s, uint32_t codePt) {
  int codePtBytes = utf8_codepoint_bytes(codePt);

  for (size_t i = 0; s[i] != 0;) {
    int temp = utf8_num_bytes_in_next_symbol(s[i], true);
    if (temp > 4)
      return NULL;
    if (temp == codePtBytes && utf8_to_codepoint(s + i) == codePt) {
      return s + i;
    }
    i += temp;
  }

  return NULL;
}

// Checks whether a UTF-8 symbol (as byte pointer) is valid.
bool utf8_string_valid(utf8_chr *b) {
  for (size_t i = 0; b[i] != 0;) {
    int temp = utf8_num_bytes_in_next_symbol(b[i], false);
    if (temp > 4)
      return false;
    i += temp;
  }
  return true;
}

// Converts an array of unicode codepoints into an array of utf-8 characters.
// Returns the number of codepoints converted.
ssize_t utf8_from_codepoints(const size_t src_cnt, const uint32_t *src,
                             const size_t dest_cnt, utf8_chr *dest) {
  ssize_t bytes_written = 0;
  ssize_t codePts_written = 0;
  const ssize_t total_dest_byte_cnt = dest_cnt * sizeof(utf8_chr);

  for (size_t i = 0; i < src_cnt; i++) {
    int codePt_byte_cnt = utf8_codepoint_bytes(*src);
    if (codePt_byte_cnt > total_dest_byte_cnt - bytes_written) {
      return codePts_written;
    }
    utf8_from_codepoint(src[i], dest + bytes_written);
    bytes_written += codePt_byte_cnt;
    codePts_written++;
  }

  return codePts_written;
}
