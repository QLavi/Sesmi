#include "io_and_mem.h"

int main(void) {
  maintain_allocations_for_me();

  FREE_ALL();
  return 0;
}
