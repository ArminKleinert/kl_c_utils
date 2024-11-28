
#ifndef KL_UTF8_H
#define KL_UTF8_H

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#ifdef _MSC_VER
#define utf8_inline __forceinline
#elif defined(__GNUC__)
#define utf8_inline inline __attribute__((__always_inline__))
#elif defined(__CLANG__)
#if __has_attribute(__always_inline__)
#define utf8_inline inline __attribute__((__always_inline__))
#else
#define utf8_inline inline
#endif
#else
#define utf8_inline inline
#endif

#ifndef WHERE_AM_I
#define WHERE_AM_I()                                                           \
  do {                                                                         \
    fprintf(stderr, "You are in function %s at line %d.\n", __FUNCTION__,      \
            __LINE__);                                                         \
  } while (0)
#endif

typedef char utf8_chr;
typedef utf8_chr *utf8_str;
typedef uint32_t utf8_code_pt;

// 1: Invalid unicode codepoint
// 2: Invalid utf-8 symbol
#define INVALID_UNICODE_CODEPOINT 1
#define INVALID_UTF8_SYMBOL 2

#define UNICODE_MAX_CODEPT 0x10FFFF

int set_utf8_lib_error(const int);
int get_utf8_lib_error(void);

utf8_chr *utf8_reinterpret_string(const char *const);

char *utf8_reinterpret_as_string(const utf8_chr *const);

// Take a unicode code point and stuff it into a byte-array of 4 bytes.
// Also, does some conversion into the correct format.
// Returns the number of bytes written or -1 on error.
int utf8_from_codepoint(const utf8_code_pt, utf8_chr *const);

// Converts an array of unicode codepoints into an array of utf-8 characters.
// Returns the number of codepoints converted.
ssize_t utf8_from_codepoints(const size_t src_cnt,
                             const utf8_code_pt *const src,
                             const size_t dest_cnt, utf8_chr *const dest);

// Convert UTF-8 byte array into a unicode codepoint.
// Returns MAX_INT (~0) on error.
utf8_code_pt utf8_to_codepoint(const utf8_chr *const);

// Checks whether a UTF-8 symbol (as byte pointer) is valid.
bool utf8_char_valid(const utf8_chr *const);

size_t utf8_strlen(const utf8_chr *const);

int utf8_str_cmp(const utf8_chr *const, const utf8_chr *const);

// Checks whether the string includes a given codepoint. Returns NULL if not.
// On error, sets utf8_lib_error and returns NULL.
const utf8_chr *utf8_strchr(const utf8_chr *const, utf8_code_pt);

// Checks whether a UTF-8 symbol (as byte pointer) is valid.
bool utf8_string_valid(const utf8_chr *const);

int utf8_codepoint_bytes(utf8_code_pt c);

// Get number of bytes in utf8 symbol
int utf8_num_bytes_in_next_symbol(utf8_chr b, bool set_errno);

#endif // KL_UTF8_H
