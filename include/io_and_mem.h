#pragma once

#include <stdio.h>
#include <stdlib.h>

#define ALLOC(type, count) (type*)x_alloc(NULL, sizeof(type) * (count))
#define REALLOC(type, old_ptr, count)  (type*)x_alloc(old_ptr, sizeof(type) * (count))
#define FREE(type, ptr) x_alloc(ptr, 0)
#define FREE_ALL() x_alloc(NULL, 0)

void maintain_allocations_for_me();
void* x_alloc(void* old_ptr, size_t size);
char* load_file(char* filename);

#define ALLOC_WHENEVER_FREE_AT_LAST
