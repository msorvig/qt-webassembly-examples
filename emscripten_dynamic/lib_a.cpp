#include <emscripten.h>

extern "C" {
    void EMSCRIPTEN_KEEPALIVE sayHelloFromA() {
        printf("I say, hello from A\n");
    }
}
