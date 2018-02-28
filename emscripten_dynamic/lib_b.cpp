#include <emscripten.h>

extern "C" {
    void EMSCRIPTEN_KEEPALIVE sayHelloFromB() {
        printf("I say, hello from B\n");
    }
}
