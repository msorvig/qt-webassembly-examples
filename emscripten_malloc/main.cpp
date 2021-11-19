#include <stdio.h>
#include "emscripten.h"
#include <emscripten/bind.h>

size_t k = 1024;
size_t m = k * k;
size_t min_malloc_size = 64 * m;
size_t max_malloc_size = 512 * 4 * m;
size_t malloc_size = min_malloc_size / 2;
bool malloc_has_failed = false;
size_t total_malloc;
std::vector<char *> pointers;
std::vector<size_t> alloc_sizes;

int memset_buffer_index = 0;

void malloc_again()
{
    if (malloc_has_failed) {
        if (malloc_size / 2>= min_malloc_size)
            malloc_size /= 2;
    } else {
        if (malloc_size * 2<= max_malloc_size)
            malloc_size *=2;
    }

    printf("malloc %zu\n", malloc_size);
    void *ptr = malloc(malloc_size);
    printf("malloc %zu %x\n", malloc_size, (unsigned int)ptr);
    
    bool malloc_failed = (ptr == 0);

    if (malloc_failed) {
        malloc_has_failed = true;
        puts("malloc failed");
        return;
    }

    pointers.push_back((char *)ptr);
    alloc_sizes.push_back(malloc_size);

    total_malloc += malloc_size; 
    printf("total_malloc %zu\n", total_malloc);
}

void memset_again()
{
    if (memset_buffer_index >= pointers.size())
        return;
    
    printf("memset_again %d %zu\n", memset_buffer_index, alloc_sizes[memset_buffer_index]);
    memset(pointers[memset_buffer_index], 128, alloc_sizes[memset_buffer_index]);
    ++memset_buffer_index;
}

EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::function("malloc_again", &malloc_again);
    emscripten::function("memset_again", &memset_again);
}

int main()
{
    pointers.reserve(128);
    alloc_sizes.reserve(128);
    return 0;
}
