#include <iostream>
#include <emscripten.h>
#include <emscripten/val.h>

int main(int argc, char **argv) {
    std::cout << "hello from main()" << std::endl;
    std::cout << "FOO envoironment variable: " << getenv("foo") << std::endl;

    EM_ASM(
        console.log("hello from EM_ASM");

        // We can still access the Emscripten module as "Module"
        // here, even though we are building with MODULARIZE=1, and
        // have set a custom EXPORT_NAME.
        console.log(Module);
        Module.foo1 = "bar";
    );

    emscripten::val foo1 = emscripten::val::module_property("foo1");
    std::cout << "value of foo1 is " << foo1.as<std::string>() << std::endl;
}
