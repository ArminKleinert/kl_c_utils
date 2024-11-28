#ifndef DYNAMIC_READ
#define DYNAMIC_READ

#include <stdint.h>
#include <unistd.h>

#define arraysize size_t

struct kstring {
  char *buff;
  size_t size;
  size_t reserved;
};

typedef struct kstring kstring;

char *kstring_buff(struct kstring *a);

size_t kstring_size(struct kstring *a);

size_t kstring_reserved(struct kstring *a);

ssize_t dyn_read(int fd, kstring *buff, size_t count);

struct kstring *kstring_init(struct kstring *a, arraysize initialSize);

struct kstring *kstring_from_string(struct kstring *a,
                                  const char *initial);

struct kstring *kstring_append(struct kstring *a, char element);

struct kstring *kstring_set(struct kstring *a, arraysize index, char element);

void kstring_free(struct kstring *a);

#endif // DYNAMIC_READ
