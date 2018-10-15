#include <emscripten.h>
#include <emscripten/val.h>

extern "C" {

    EM_JS(void, sayHelloFromEMJS, (), {
        console.log("Hello from EM_JS");
    });

    void EMSCRIPTEN_KEEPALIVE sayHelloFromA() {
        printf("I say, hello from A\n");

        emscripten::val console = emscripten::val::global("console");

//exception thrown: BindingError: parameter 1 has unknown type NSt3__212basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEEE,BindingError:
#if 0
        console.call<void>("log", std::string("Hello from emscripten::val in sayHelloFromA()"));
#endif
        console.call<void>("log", 1);

        emscripten::val window = emscripten::val::global("window");
        printf("dpr: %f\n", window["devicePixelRatio"].as<double>());

// AssertionError: EM_ASM is not yet supported in shared wasm module (it cannot be stored in the wasm itself, need some solution)
#if 0
        EM_ASM(
           alert('hello world from EM_ASM');
        );
#endif

// LinkError: import object field '_sayHelloFromEMJS' is not a Function
#if 0
        sayHelloFromEMJS();
#endif

    }
}
