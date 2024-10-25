#include <iostream>
#include <map>

#include <emscripten.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>
#include <emscripten/html5.h>

using namespace std;
using emscripten::val;

// This example suspends and resumes on a pointerdown event, using an event
// handler registered with addEventListener

std::map<int, std::function<void(val)>> callbacks;

// Setup control variables on the Module
EM_JS(void, setup, (), {
    Module.suspendResumeControl = {
        resume: null,
        callbacks: {},
        currentCallbackIndex: null,
        currentCallbackArg: null
    };
});

// Suspend the calling thread. The thread can be resumed by calling Module.resume
EM_ASYNC_JS(void, suspend_js, (), {
    return new Promise(resolve => {
        Module.suspendResumeControl.resume = resolve;
    });
});

void invokeCurrentCallback()
{
    val control = val::module_property("suspendResumeControl");
    val currentIndex = control["currentCallbackIndex"];
    if (currentIndex.isNull())
        return;

    auto it = callbacks.find(currentIndex.as<int>());
    auto callback = it->second;
    val currentArg = control["currentCallbackArg"];
    control.set("currentCallbackIndex", val::null());
    control.set("currentCallbackArg", val::null());
    callback(currentArg);
}

EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::function("invokeCurrentCallback", &invokeCurrentCallback);
}

void suspend() {

    cout << "suspend" << endl;
    suspend_js();
    cout << "resumed" << endl;

    // Call event callback if one has been registered
    invokeCurrentCallback();
}

EM_JS(void, resume, (), {
    let control = Module.suspendResumeControl;
    const resume = control.resume;
    control.resume = null;
    resume();
});

EM_JS(void, registerCallbackFunctionJs, (int callbackIndex), {
    let control = Module.suspendResumeControl;
    control.callbacks[callbackIndex] = (arg) => {
        control.currentCallbackIndex = callbackIndex;
        control.currentCallbackArg = arg;
        if (control.resume) {
            resume() // resume and invoke callback 
        } else {
            Module.invokeCurrentCallback(); // not resumed, invoke callback directly
        }
    };  
});

uint32_t registerCallbackFunction(std::function<void(val)> callback) 
{
    static uint32_t i = 0;
    ++i;
    callbacks[i] = callback;
    registerCallbackFunctionJs(i);
    return i;
}

void removeCallbackFunction(uint32_t index)
{
    callbacks.erase(callbacks.find(index));
}

uint32_t addEventListener(val element, std::string event, std::function<void(val)> handler)
{
    uint32_t callbackIndex = registerCallbackFunction(handler);
    val jsCallbackFunction = val::module_property("suspendResumeControl")["callbacks"][callbackIndex];
    element.call<void>("addEventListener", event, jsCallbackFunction);
    return callbackIndex;
}

void removeEventListener(val element, std::string event, uint32_t callbackIndex)
{
    val jsCallbackFunction = val::module_property("suspendResumeControl")["callbacks"][callbackIndex];
    element.call<void>("removeEventListener", event, jsCallbackFunction);
    removeCallbackFunction(callbackIndex);
}

uint32_t pointerHandlerIndex = 0;
void registerPointerEventHanlder()
{
    val canvas = val::module_property("canvas");
    pointerHandlerIndex = addEventListener(canvas, "pointerdown", [](val event){
        cout << "pointer event" << endl;
    });
}

void removePointerEventHandler()
{
    val canvas = val::module_property("canvas");
    removeEventListener(canvas, "pointerdown", pointerHandlerIndex);
}

int main(int argc, char **argv)
{
    setup();
    registerPointerEventHanlder();
    //removePointerEventHandler();
    //registerPointerEventHanlder();

    while (true) {
        suspend();
    }
}
