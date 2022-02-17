#include "io_and_mem.h"

#ifdef ALLOC_WHENEVER_FREE_AT_LAST
size_t** allocations;
int alloc_capacity = 0;
int alloc_count = 0;
#endif

void* x_alloc(void* old_ptr, size_t size) {
    if(old_ptr != NULL && size == 0) {
        free(old_ptr);
        return NULL;
    }

    if(size != 0) {
        void* new_ptr = realloc(old_ptr, size);

        if(new_ptr == NULL) {
            fprintf(stderr, "Out_Of_Memory_Error!\nAborted\n");
            exit(1);
        }
#ifdef ALLOC_WHENEVER_FREE_AT_LAST
        if(alloc_capacity -1 < alloc_count) {
          alloc_capacity = alloc_capacity < 8 ? 8 : alloc_capacity * 2;
          allocations = realloc(allocations, sizeof(size_t*) * alloc_capacity);
        }
        allocations[alloc_count++] = (size_t*)new_ptr;
#endif
        return new_ptr;
    }
#ifdef ALLOC_WHENEVER_FREE_AT_LAST
    /* printf("Allocation Count: %zu\n", alloc_count); */
    int x = 0;
    for(;x < alloc_count; x++) {
      free(allocations[x]);
    }
    /* printf("Freed Blocks Count: %zu\n", x); */

    free(allocations);
    alloc_count = 0;
    alloc_capacity = 0;
#endif
    return NULL;
}

char* load_file(char* filename) {
    FILE* fptr = fopen(filename, "r");
    if(fptr == NULL) {
        fprintf(stderr, "Unable to Open File: '%s'\n", filename);
        exit(1);
    }

    fseek(fptr, 0, SEEK_END);
    size_t size = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);

    char* src = ALLOC(char, size +1);
    size_t read_size = fread(src, 1, size, fptr);
    if(read_size < size) {
        fprintf(stderr, "'%s' reading failed. read_size != file_size\n", filename);
        exit(1);
    }
    src[size] = '\0';
    fclose(fptr);
    return src;
}
