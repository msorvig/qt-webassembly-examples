#include <pthread.h>
#include <iostream>

#include <emscripten/stack.h>

int main(int, char**) {
    pthread_t thread = pthread_self();
    pthread_attr_t sattr;
    pthread_attr_init(&sattr);
    pthread_getattr_np(thread, &sattr);

    // pthread_attr_getstack returns the address of the memory region, which is the physical
    // base of the stack, not the logical one.
    void *pthread_stackBase;
    size_t pthread_regionSize;
    int rc = pthread_attr_getstack(&sattr, &pthread_stackBase, &pthread_regionSize);
    pthread_attr_destroy(&sattr);
    std::cout << std::hex << "pthread: virtual memory stack base " << uintptr_t(pthread_stackBase) << " region size " << pthread_regionSize << std::endl;
        
    // emscripten returns the logical stack region
    uintptr_t emscripten_base = emscripten_stack_get_base();
    uintptr_t emscripten_end = emscripten_stack_get_end();
    size_t emscripten_size = emscripten_base - emscripten_end;
    std::cout << std::hex << "emscripten: logical stack base " << emscripten_base << " end " << emscripten_end <<  " size "  << emscripten_size << std::endl;
}