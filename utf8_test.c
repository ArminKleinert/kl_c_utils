#include "utf8.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int fromStdin(void) {
  // Set up variables
  utf8_chr buff[5];

  buff[0] = 0xE1;
  buff[1] = 0xAE;
  buff[2] = 0x98;
  write(1, buff, 4);
  printf("\n");

  int n = utf8_from_codepoint(0x1B98, &(buff[0]));
  write(1, buff, n);
  printf("\n");

  buff[0] = 0xE1;
  buff[1] = 0xAE;
  buff[2] = 0x98;
  int codeP = utf8_to_codepoint(&(buff[0]));
  n = utf8_from_codepoint(codeP, &(buff[0]));
  write(1, buff, 4);
  printf("\n");

  utf8_chr buff1[8];
  buff1[0] = 'A';
  buff1[1] = 0xE1;
  buff1[2] = 0xAE;
  buff1[3] = 0x98;
  buff1[4] = '9';
  buff1[5] = '\0';
  buff1[6] = '\0';
  write(1, buff1, 6);
  printf("\n");
  printf("%zu\n", utf8_strlen(buff1));

  utf8_chr *nine_chars = utf8_reinterpret_string("abcdefghi");
  printf("%zu\n", utf8_strlen(nine_chars));

  printf("Expected %d, got %d\n", 0, utf8_str_cmp(nine_chars, nine_chars));
  printf("Expected %d, got %d\n", -1, utf8_str_cmp(nine_chars, buff));
  printf("Expected %d, got %d\n", 1, utf8_str_cmp(buff, nine_chars));
  printf("Expected %d, got %d\n", 0, utf8_str_cmp(buff, buff));

  utf8_chr _buff2[4];
  utf8_chr *buff2 = &(_buff2[0]);
  buff2[0] = 0xE1;
  buff2[1] = 0xAE;
  buff2[2] = 0x98;
  printf("utf8_char_valid: Expected %d, got %d\n", 1, utf8_char_valid(buff2));
  buff2[0] = 0xFF;
  printf("utf8_char_valid: Expected %d, got %d\n", 0, utf8_char_valid(buff2));
  buff2[0] = 0xE1;
  buff2[1] = 0x7E;
  printf("utf8_char_valid: Expected %d, got %d\n", 0, utf8_char_valid(buff2));

  return get_utf8_lib_error();
}

int main(int argc, char **args) {
  (void)argc;
  (void)args;
  // printf("%u\n", cl1(0x80000000));
  return fromStdin();
}
