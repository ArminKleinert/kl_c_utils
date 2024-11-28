#include "utf8.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// Checks whether a byte, after the first, has the format 10XX_XXXX
#define utf8_check_byte(b) (((uint8_t)(b & (3 << 6))) == ((uint8_t)(1 << 7)))

int _utf8_lib_error = 0;

utf8_inline int set_utf8_lib_error(int code) {
  _utf8_lib_error = code;
  return _utf8_lib_error;
}

utf8_inline int get_utf8_lib_error(void) { return _utf8_lib_error; }

/**
 * Safely casts char pointer to utf8_chr pointer.
 * This function does not check for NULL or any other problems. Does not set
 * errno or utf8_lib_error. The output points to the same position in memory as
 * the input. The purpose is to make the compiler stop worrying.
 * @param s The pointer.
 * @return The same pointer, but to utf8_chr.
 * @see utf8_reinterpret_as_string
 */
utf8_inline utf8_chr *utf8_reinterpret_string(const char *const s) {
  union {
    utf8_chr *u8;
    const char *c;
  } p = {.c = s};
  return p.u8;
}

/**
 * The inverse function to utf8_reinterpret_string.
 * Also does not check for NULL or other problems. Does not set errno or
 * utf8_lib_error. The output points to the same position in memory as the
 * input.
 * @param s Pointer to utf8_chr
 * @return Pointer to char
 * @see utf8_reinterpret_string
 */
utf8_inline char *utf8_reinterpret_as_string(const utf8_chr *const s) {
  union {
    const utf8_chr *u8;
    char *c;
  } p = {.u8 = s};
  return p.c;
}

/**
 * Take a unicode codepoint and return the number of bytes needed to represent
 * it as a utf-8 char. This function does not set utf8_lib_error. The output
 * range is 1..4 for valid inputs or a number below 1 for errornous inputs.
 * This function will not set utf8_lib_error or errno.
 * @param c The codepoints.
 * @return Number of bytes required for a conversion into an utf-8 symbol.
 */
int utf8_codepoint_bytes(const utf8_code_pt c) {
  if (c < 0x80) {
    return 1;
  } else if (c < 0x0800) {
    return 2;
  } else if (c < 0x010000) {
    return 3;
  } else if (c < 0x110000) {
    return 4;
  }
  return -1;
}

// Take a unicode code point and stuff it into a byte-array of 4 bytes.
// Also, does some conversion into the correct format.
// Returns a number below 1 on error.
int utf8_from_codepoint(utf8_code_pt c, utf8_chr *const buff) {
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
  return -1;
}

// Get the number of leading 1s. These indicate the number of bytes used:
// 0 (b[0] == 0XXX_XXXX) => 1 byte
// 2 (b[0] == 110X_XXXX) => 2 bytes
// 3 (b[0] == 1110_XXXX) => 3 bytes
// 4 (b[0] == 1111_0XXX) => 4 bytes
// Returns a number above 4 on error.
int utf8_num_bytes_in_next_symbol(const utf8_chr b, bool set_errno) {
  uint_fast8_t leftmost_zero_bit_index;
  // Find first position of a 0-bit in number, going from left to right.
  for (leftmost_zero_bit_index = 7;
       leftmost_zero_bit_index > 0 && (b & (1 << leftmost_zero_bit_index)) != 0;
       leftmost_zero_bit_index--) {
  }

  // Pattern 0XXX_XXXX => single byte (ASCII)
  if (leftmost_zero_bit_index == 7)
    return 1;

  // Error-case: invalid bit-pattern
  // (leftmost_zero_bit_index == 6) means that b has the pattern 10XX_XXXX,
  // which is not possible. (leftmost_zero_bit_index < 3) means that the pattern
  // is 1111_10XX, which is also not possible (see above)
  if (leftmost_zero_bit_index == 6 || leftmost_zero_bit_index < 3) {
    if (set_errno) {
      set_utf8_lib_error(INVALID_UTF8_SYMBOL);
    }
    return 0xFF;
  }

  return 7 - leftmost_zero_bit_index;
}

// Inline version of utf8_char_valid. Not visible to the outside.
static utf8_inline bool utf8_char_valid_inline(const utf8_chr *const b) {
  int len = utf8_num_bytes_in_next_symbol(b[0], false);
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

// Checks whether a UTF-8 symbol (as byte pointer) is valid.
bool utf8_char_valid(const utf8_chr *const b) {
  return utf8_char_valid_inline(b);
}

// Convert UTF-8 byte array into a unicode codepoint.
// Returns MAX_INT (~0) on error.
utf8_code_pt utf8_to_codepoint(const utf8_chr *const b) {
  // Calculate the number of bytes the symbol has.
  const uint_fast8_t nbytes = utf8_num_bytes_in_next_symbol(b[0], true);

  // Error case: The symbol is not valid for utf-8.
  if (nbytes > 4) {
    return UINT32_MAX;
  }

  utf8_code_pt c = 0;

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

    if (!utf8_check_byte(b[i])) {
      set_utf8_lib_error(INVALID_UTF8_SYMBOL);
      return UINT32_MAX;
    }

    c += b[i] & lowest_6_bits;
  }

  return c;
}

// Max long on error
size_t utf8_strlen(const utf8_chr *const b) {
  size_t res = 0;
  for (size_t i = 0; b[i] != 0;) {
    int temp = utf8_num_bytes_in_next_symbol(b[i], true);
    if (temp > 4) {
      return SIZE_MAX;
    }
    i += temp;
    res++;
  }
  return res;
}

// 0x7FFF on error
int utf8_str_cmp(const utf8_chr *const xs, const utf8_chr *const ys) {
  // if the two strings are at the same memory address, we don't need any more
  // complex calculations.
  if (xs == ys)
    return 0;

  const utf8_chr *xs1 = xs;
  const utf8_chr *ys1 = ys;

  while (*xs1 != 0 && *ys1 != 0) {
    int xs_sym_len = utf8_num_bytes_in_next_symbol(xs1[0], true);
    if (xs_sym_len > 4)
      return 0x7FFF;

    int ys_sym_len = utf8_num_bytes_in_next_symbol(ys1[0], true);
    if (ys_sym_len > 4)
      return 0x7FFF;

    if (xs_sym_len > ys_sym_len) {
      return 1;
    } else if (xs_sym_len < ys_sym_len) {
      return -1;
    } else {
      utf8_code_pt xs_cp = utf8_to_codepoint(xs1);
      utf8_code_pt ys_cp = utf8_to_codepoint(ys1);
      if (xs_cp > ys_cp) {
        return 1;
      } else if (xs_cp < ys_cp) {
        return -1;
      }
    }

    xs1 += xs_sym_len;
    ys1 += ys_sym_len;
  }
  return 0;
}

// Straight memory comparison, no validity checks.
static utf8_inline bool memeq(const utf8_chr *const c1,
                              const utf8_chr *const c2, int len) {
  for (int temp = 0; temp < len; temp++) {
    if (c1[temp] != c2[temp])
      return false;
  }
  return true;
}

/**
 * Try to find codepoint in utf-8 string.
 * If the codepoint is not in the string, returns NULL.
 * If the codepoint was invalid (greater than UNICODE_MAX_CODEPT), NULL is
 * returned and utf8_lib_error is set to INVALID_UNICODE_CODEPOINT. If the
 * string is invalid before the codepoint is found, utf8_lib_error is set to
 * INVALID_UTF8_SYMBOL and NULL is returned.
 * As this function does not check the validity of the entire string in general.
 *
 * s = "Yeaye";
 * utf8_strchr(s, 'a') == s + 2;
 * utf8_strchr(s, 'Z') == NULL; // Not found
 * utf8_strchr(s, 0x110000) == NULL // Invalid codepoint. Also,
 *                                  // utf8_lib_error was set here.
 *
 * s[3] = 0xFF; // Illegal utf8 byte.
 * utf8_strchr(s, 'a') == s+2; // 'a' was found, no extra work.
 * utf8_strchr(s, 'Z') == NULL; // Also set utf8_lib_error because s is invalid.
 *
 * @return A pointer to the first position of the codepoint in the string or
 * NULL if it was not found.
 */
const utf8_chr *utf8_strchr(const utf8_chr *const s, utf8_code_pt codePt) {
  utf8_chr inp[4];
  const int codePtBytes = utf8_from_codepoint(codePt, &(inp[0]));
  if (codePtBytes < 1) {
    // Error case. INVALID_UNICODE_CODEPOINT was already set at this point.
    return NULL;
  }

  for (size_t i = 0; s[i] != 0;) {
    // Get number of bytes for next symbol
    int temp = utf8_num_bytes_in_next_symbol(s[i], true);
    if (temp > 4) {
      // Error: Invalid symbol (invalid first byte of symbol)
      // If this occurs, utf8_lib_error was already set.
      return NULL;
    }

    // Check if the number of bytes of the next symbol and the codepoint is the
    // same. If not, just progress.
    if (temp == codePtBytes && memeq(s + i, &(inp[0]), codePtBytes)) {
      return s + i;
    }

    // Jump ahead. temp can never be 0 or negative, so this is alright. (see
    // utf8_num_bytes_in_next_symbol)
    i += temp;
  }

  return NULL; // Not found.
}

// Checks whether a UTF-8 symbol (as byte pointer) is valid.
bool utf8_string_valid(const utf8_chr *const b) {
  for (size_t i = 0; b[i] != 0;) {
    int temp = utf8_num_bytes_in_next_symbol(b[i], false);
    if (!utf8_char_valid_inline(b + i)) {
      return false;
    }
    i += temp;
  }
  return true;
}

// Converts an array of unicode codepoints into an array of utf-8 characters.
// Returns the number of codepoints converted.
ssize_t utf8_from_codepoints(const size_t src_cnt,
                             const utf8_code_pt *const src,
                             const size_t dest_cnt, utf8_chr *const dest) {
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
