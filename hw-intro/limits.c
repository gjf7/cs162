#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/resource.h>

void print_rlimit(const char* name, int resource) {
  struct rlimit lim;
  if (getrlimit(resource, &lim) == 0) {
    printf("%s: %lu\n", name, (unsigned long)lim.rlim_cur);
  } else {
    fprintf(stderr, "Failed to getrlimit for %s: %s\n", name, strerror(errno));
  }
}

int main() {
  print_rlimit("stack size", RLIMIT_STACK);

  print_rlimit("process limit", RLIMIT_NPROC);

  print_rlimit("max file descriptors", RLIMIT_NOFILE);

  return 0;
}
