#include "dynamic_read.h"

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#ifndef max
static arraysize max(arraysize a, arraysize b) { return (a > b) ? a : b; }
#endif // max

char *kstring_buff(struct kstring *a) {
  return a->buff;
}

size_t kstring_size(struct kstring *a) {
  return a->size;
}

size_t kstring_reserved(struct kstring *a) {
  return a->reserved;
}

ssize_t dyn_read(int fd, struct kstring *a, size_t count) { return 0; }

struct kstring *kstring_init(struct kstring *a, arraysize initialSize) {
  a->buff = malloc(max(initialSize, 16) * sizeof(char));
  if (!(a->buff))
    return NULL;
  a->size = 0;
  a->reserved = initialSize;
  return a;
}

struct kstring *kstring_from_string(struct kstring *a,
                                  const char *initial) {
  arraysize initLen = strlen(initial);

  arraysize initialSize = 16;
  while (initialSize < initLen)
    initialSize *= 2;

  a->buff = malloc(initialSize * sizeof(char));
  if (!(a->buff))
    return NULL;

  strcpy(a->buff, initial);

  a->size = initLen;
  a->reserved = initialSize;
  return a;
}

struct kstring *kstring_append(struct kstring *a, char element) {
  if (a->size == a->reserved) {
    a->reserved *= 2;
    a->buff = realloc(a->buff, a->reserved * sizeof(char));
    if (!(a->buff))
      return NULL;
  }
  a->buff[a->size++] = element;
  return a;
}

struct kstring *kstring_set(struct kstring *a, arraysize index, char element) {
  if (index > a->reserved) {
    a->reserved = index + 1;
    a->buff = realloc(a->buff, a->reserved * sizeof(char));
    if (!(a->buff))
      return NULL;
  }
  a->buff[a->size++] = element;
  return a;
}

void kstring_free(struct kstring *a) {
  free(a->buff);
  a->buff = NULL;
  a->size = a->reserved = 0;
}
