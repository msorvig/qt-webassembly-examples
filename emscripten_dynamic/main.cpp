#include <emscripten.h>
#include <emscripten/val.h>

#include <dlfcn.h>

extern "C" void sayHelloFromA();

EMSCRIPTEN_KEEPALIVE int main() {
    printf("Hello, world! from main\n");
    
    // Call sayHelloFromA() from lib_a
    sayHelloFromA();

    // Call console.log via emscripten::val
    emscripten::val console = emscripten::val::global("console");
    console.call<void>("log", std::string("Hello from emscripten::val"));
    
    // Call sayHelloFromB() from lib_b via dlopen
    void *lib_b = dlopen("lib_b.wasm", RTLD_LAZY);
    printf("dlopen: %p\n", lib_b);
    void *sayHelloFromB = dlsym(lib_b, "sayHelloFromB");
    printf("sayHelloFromB: %p\n", sayHelloFromB);
    reinterpret_cast<void (*)()>(sayHelloFromB)();
}