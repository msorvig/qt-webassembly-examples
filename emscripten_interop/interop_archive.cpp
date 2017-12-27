#include <emscripten.h>
#include <emscripten/html5.h>

extern "C" {

// EMSCRIPTEN_KEEPALIVE is required but not sufficent to keep this
// function alive. Functions in statically linked archives must
// be included in EXPORTED_FUNCTIONS on the linker line, or be called
// from the C++ side.
void EMSCRIPTEN_KEEPALIVE sayHelloFromArchive() {
    extern bool g_keepAliveCall;
    if (g_keepAliveCall)
        return;
    
    printf("hello, world! from archive\n");    
}

}
