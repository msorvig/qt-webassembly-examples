#include <iostream>
#include <map>

#include <emscripten.h>
#include <emscripten/val.h>
#include <emscripten/html5.h>

using namespace std;
using emscripten::val;

// This example suspends and resumes on a requestAnimationFrame callback, using
// window.requestAnimationFrame and JS resume code. The frame callback itself
// is a 

static bool isSuspended = false;
std::map<int, std::function<void(val)>> callbacks;

// Suspend the calling thread. The thread can be resumed by calling Module.resume
EM_ASYNC_JS(void, suspend_js, (), {
    return new Promise(resolve => {
        Module.resume = resolve;
    });
});

void suspend() {

    cout << "suspend" << endl;
    isSuspended = true;
    suspend_js();
    isSuspended = false;
    cout << "resumed" << endl;

    // Call callback if needed
    val currentIndex = val::module_property("currentIndex");
    val currentArg = val::module_property("currentArg");
    if (!currentIndex.isNull()) {
        auto it = callbacks.find(currentIndex.as<int>());
        auto callback = it->second;
        callbacks.erase(it);
        callback(currentArg);
    }
}

EM_JS(void, resume, (), {
    if (Module.resume) {
        const resume = Module.resume;
        Module.resume = null;
        resume();
    }
});

EM_JS(void, registerCallbackFunctionJs, (int i), {
    if (Module.callbackFunctions === undefined)
        Module.callbackFunctions = [];
    Module.callbackFunctions[i] = (arg) => {
        Module.currentIndex = i;
        Module.currentArg = arg;
        resume();
    };
});

int registerCallbackFunction(std::function<void(val)> callback) 
{
    static unsigned int i = 0;
    ++i;
    callbacks[i] = callback;
    registerCallbackFunctionJs(i);
    return i;
}

EM_JS(void, requestAnimationFrameJs, (int i), {
    window.requestAnimationFrame((frameTime) => {
        Module.callbackFunctions[i](frameTime);
    })
})

void requestAnimationFrame(std::function<void(double)> callback)
{
    int i = registerCallbackFunction([callback](val arg){
        double frameTime = arg.as<double>();
        callback(frameTime);
    });
    requestAnimationFrameJs(i);
}

void requestFrame()
{
    requestAnimationFrame([](double frameTime) {
        cout << "frame callback 0 time " << frameTime << " suspended " << isSuspended << endl;
    });

    requestAnimationFrame([](double frameTime) {
        cout << "frame callback 1 time " << frameTime << " suspended " << isSuspended << endl;
    });
}

int main(int argc, char **argv)
{
    requestFrame();

    while (true) {
        suspend();
    }
}
