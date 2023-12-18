#include <emscripten.h>
#include <emscripten/val.h>

#include <coroutine>
#include <iostream>

using namespace emscripten;

void trace(val item);

// Helper co-routine (main can't be a co-routine). Note that this
// is a valid co-routine since emscripten::val implements the 
// promise_type struct, which means we can co_return it.
val helper()
{
    puts("helper()");
    val window = val::global("window");

    val fonts = co_await window.call<val>("queryLocalFonts");

    int max_fonts = 5;
    int len = std::min(max_fonts, fonts["length"].as<int>());

    for (int i = 0; i < len; ++i) {

        // Get the font at i
        val font = fonts.call<val>("at", i);
        std::cout << "font " << font["fullName"].as<std::string>() << std::endl;

        // Get the font file data
        val blob = co_await font.call<val>("blob");
        val buffer = co_await blob.call<val>("arrayBuffer");
        std::cout << "font buffer size " << buffer["byteLength"].as<int>() << std::endl;

        // what do stack traces look like?
        trace(buffer);
    }

    co_return val{};
}

int main(int argc, char **argv)
{
    puts("main()");

    val _ = helper();

    // Helper returns early and we return from main(),
    // but that's OK since Emscripten keeps the runtime
    // alive after main() exits.
}

void trace(val item)
{
    val::global("window")["console"].call<void>("trace", item);
}
