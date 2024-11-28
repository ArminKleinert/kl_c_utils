#include <stdio.h>

#include "dynamic_read.h"

int main(int argc, char **args) {
  (void)argc;
  (void)args;

  struct kstring str;
  struct kstring *kstr = &str;
  kstring_from_string(kstr, "abc");
  
  printf("%s %lu %lu\n", kstring_buff(kstr), kstring_size(kstr), kstring_reserved(kstr));
  
  kstring_append(kstr, 'd');
  printf("%s %lu %lu\n", kstring_buff(kstr), kstring_size(kstr), kstring_reserved(kstr));
  

  return 0;
}
