#include <stdio.h>

#include <emscripten.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>

EM_ASYNC_JS(void, suspend, (), {
    return new Promise(resolve => {
        Module.resume = resolve;
    });
});

EM_JS(void, initEventHandler, (), {
    Module.resumeButton.addEventListener("click",  async (event) => {
        console.error("resume handler");
        Module.current_event = event; // save event for C++ handler
        Module.resume(); // resume C++ event loop
        console.error("resume handler done");
    });
});

EM_JS(void, cleanup, (), {
    Module.current_event = null;
});

int loop() {
    while (true) {

        // Wait for event
        fprintf(stderr, "loop(): suspending\n");
        suspend();

        // Handle event
        fprintf(stderr, "loop(): resumed\n");
        emscripten::val event = emscripten::val::module_property("current_event");
        fprintf(stderr, "loop(): event = %s\n", event["type"].as<std::string>().c_str());

        cleanup();

        // Optinally recurse, i.e. simulate a nested event loop
        loop();
    }
}

int main(int argc, char *argv[])
{
    puts("main()");

    initEventHandler();

    loop();

    return 0;
}
