
#ifndef KL_UTF8_H
#define KL_UTF8_H

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#ifndef WHERE_AM_I
#define WHERE_AM_I()                                                           \
  do {                                                                         \
    fprintf(stderr, "You are in function %s at line %d.\n", __FUNCTION__,      \
            __LINE__);                                                         \
  } while (0)
#endif

typedef uint8_t utf8_chr;
typedef utf8_chr *utf8_str;

// 1: Invalid unicode codepoint
// 2: Invalid utf-8 symbol
//#define set_utf8_lib_error(n)  do { utf8_lib_error = n; } while (0)

#define INVALID_UNICODE_CODEPOINT 1
#define INVALID_UTF8_SYMBOL 2

#define UNICODE_MAX_CODEPT 0x10FFFF

 int set_utf8_lib_error(const int);
 int get_utf8_lib_error(void);

utf8_chr *utf8_reinterpret_string(const char *);

char *utf8_reinterpret_as_string(const utf8_chr *);

// Take a unicode code point and stuff it into a byte-array of 4 bytes.
// Also, does some conversion into the correct format.
// Returns the number of bytes written or -1 on error.
int utf8_from_codepoint(const uint32_t, utf8_chr *);

// Converts an array of unicode codepoints into an array of utf-8 characters.
// Returns the number of codepoints converted.
ssize_t utf8_from_codepoints(const size_t src_cnt, const uint32_t *src,
                             const size_t dest_cnt, utf8_chr *dest);

// Convert UTF-8 byte array into a unicode codepoint.
// Returns MAX_INT (~0) on error.
uint32_t utf8_to_codepoint(const utf8_chr *);

// Checks whether a UTF-8 symbol (as byte pointer) is valid.
bool utf8_char_valid(const utf8_chr *);

size_t utf8_strlen(const utf8_chr *);

int utf8_str_cmp(const utf8_chr *, const utf8_chr *);

// Checks whether the string includes a given codepoint. Returns NULL if not.
// On error, sets utf8_lib_error and returns NULL.
utf8_chr *utf8_strchr( utf8_chr *, const uint32_t);
utf8_chr *utf8_strchr2( utf8_chr *, const uint32_t);

// Checks whether a UTF-8 symbol (as byte pointer) is valid.
bool utf8_string_valid(const utf8_chr *);

 bool memeq(const utf8_chr *m1, const utf8_chr *m2, const int len);

int utf8_codepoint_bytes(const uint32_t c);

#endif // KL_UTF8_H
