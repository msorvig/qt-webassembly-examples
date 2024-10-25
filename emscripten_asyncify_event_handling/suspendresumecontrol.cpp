#include <iostream>
#include <map>

#include <emscripten.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>

using namespace std;
using emscripten::val;

/*
    SuspendResumeControl controls asyncify suspend and resume when handling native events.

    The class supports adding C++ event handlers to native (element, event) pairs, and
    transparently resumes the wasm instance if it is suspended at the time of the event.
    The wasm instance suspends itself by calling the provided suspend() function.

    Event handlers are std::function<void(emscripten::val)> instances, where the val argument
    is the native event.

    SuspendResumeControl also supports the case where threre is an event while the wasm instance
    is not suspended (but has returned control normally), and whill then call the event handler
    directly.
*/
class SuspendResumeControl
{
public:
    SuspendResumeControl();
    ~SuspendResumeControl();

    void suspend();

    uint32_t addEventListener(val element, std::string event, std::function<void(val)> handler);
    void removeEventListener(val element, std::string event, uint32_t handlerIndex);

private:
    uint32_t registerEventHandler(std::function<void(val)> handler);
    void removeEventHandler(uint32_t index);
    val jsEventHandlerAt(uint32_t index);

    void callCurrentEventHandler();
    friend void callCurrentEventHandler();

    friend class AnimationFrameHandler;
    friend class SingleShotTimer;

    std::map<int, std::function<void(val)>> m_eventHandlers;
    val m_suspendResumeControlJs;
};

__thread SuspendResumeControl *t_suspendResumeControl;

// Setup/constructor function for Module.suspendResumeControl.
// (if assigning to Module.suspendResumeControl is possible from C++)
// then this does not need to be a separate JS function)
EM_JS(void, suspendResumeControlSetupJs, (), {
    Module.suspendResumeControl = {
        resume: null,
        eventHandlers: {},
        currentEventHandlerIndex: null,
        currentEventHandlerArg: null
    };
});

// Suspends the calling thread
EM_ASYNC_JS(void, suspendJs, (), {
    return new Promise(resolve => {
        Module.suspendResumeControl.resume = resolve;
    });
});

// Registers a JS event handler which when called registers itself as the
// "current" event handler, and then resumes the wasm instance.
EM_JS(void, registerEventHandlerJs, (int index), {
    let control = Module.suspendResumeControl;
    let handler = (arg) => {
        control.currentEventHandlerIndex = index;
        control.currentEventHandlerArg = arg;
        if (control.resume) {
            const resume = control.resume;
            control.resume = null;
            resume();
        } else {
            Module.callCurrentEventHandler(); // not suspended, call the handler directly
        }
    };  
    control.eventHandlers[index] = handler;
});

SuspendResumeControl::SuspendResumeControl()
{
    suspendResumeControlSetupJs();
    m_suspendResumeControlJs = val::module_property("suspendResumeControl");
    t_suspendResumeControl = this;
}

SuspendResumeControl::~SuspendResumeControl()
{

}

void SuspendResumeControl::suspend()
{
    cout << "suspend" << endl;
    suspendJs();
    cout << "\nresume" << endl;
    callCurrentEventHandler();
}

// Registers a C++ event handler
uint32_t SuspendResumeControl::registerEventHandler(std::function<void(val)> handler) 
{
    static uint32_t i = 0;
    ++i;
    m_eventHandlers[i] = handler;
    registerEventHandlerJs(i);
    return i;
}

// Removes a C++ event handler
void SuspendResumeControl::removeEventHandler(uint32_t index)
{
    m_eventHandlers.erase(m_eventHandlers.find(index));
    m_suspendResumeControlJs["eventHandlers"].set(index, val::null());
}

// 
val SuspendResumeControl::jsEventHandlerAt(uint32_t index)
{
    return m_suspendResumeControlJs["eventHandlers"][index];
}

// Adds an event handler for the given elment and event. Returns the event handler
// index, which is used for removing the event handler.
uint32_t SuspendResumeControl::addEventListener(val element, std::string event, std::function<void(val)> handler)
{
    uint32_t handlerIndex = registerEventHandler(handler);
    element.call<void>("addEventListener", event, jsEventHandlerAt(handlerIndex));
    return handlerIndex;
}
// Removes an event handler for the given elment and event.
void SuspendResumeControl::removeEventListener(val element, std::string event, uint32_t handlerIndex)
{
    element.call<void>("removeEventListener", event, jsEventHandlerAt(handlerIndex));
    removeEventHandler(handlerIndex);
}

// Calls the current event handler. The current handler index and argument must be
// set up before calling this function.
void SuspendResumeControl::callCurrentEventHandler()
{
    val currentIndex = m_suspendResumeControlJs["currentEventHandlerIndex"];
    if (currentIndex.isNull())
        return;

    auto it = m_eventHandlers.find(currentIndex.as<int>());
    auto eventHandler = it->second;
    val currentArg = m_suspendResumeControlJs["currentEventHandlerArg"];
    m_suspendResumeControlJs.set("currentEventHandlerIndex", val::null());
    m_suspendResumeControlJs.set("currentEventHandlerArg", val::null());
    eventHandler(currentArg);
}

void callCurrentEventHandler() {
     t_suspendResumeControl->callCurrentEventHandler();
}

EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::function("callCurrentEventHandler", callCurrentEventHandler);
}

// RAII event handler
class EventHandler
{
public:
    EventHandler(val element, std::string event, std::function<void(val)> handler);
    ~EventHandler();

private:
    val m_element;
    std::string m_event;
    uint32_t m_index;
};

EventHandler::EventHandler(val element, std::string event, std::function<void(val)> handler)
:m_element(element)
,m_event(event)
{
    m_index = t_suspendResumeControl->addEventListener(element, event, handler);
}

EventHandler::~EventHandler()
{
    t_suspendResumeControl->removeEventListener(m_element, m_event, m_index);
}

class AnimationFrameHandler
{
public:
    AnimationFrameHandler(std::function<void(val)> handler);
    ~AnimationFrameHandler();
    int64_t requestAnimationFrame();
    void cancelAnimationFrame(int64_t id);
private:
    uint32_t m_handlerIndex;
};

AnimationFrameHandler::AnimationFrameHandler(std::function<void(val)> handler)
{
    m_handlerIndex = t_suspendResumeControl->registerEventHandler(handler);
}

AnimationFrameHandler::~AnimationFrameHandler()
{
    t_suspendResumeControl->removeEventHandler(m_handlerIndex);
}

int64_t AnimationFrameHandler::requestAnimationFrame()
{
    val handler = t_suspendResumeControl->jsEventHandlerAt(m_handlerIndex);
    using ReturnType = double; // emscripten::val::call() does not support int64_t
    return val::global("window").call<ReturnType>("requestAnimationFrame", handler);
}

void AnimationFrameHandler::cancelAnimationFrame(int64_t id)
{
    val::global("window").call<void>("cancelAnimationFrame", double(id));
}

class SingleShotTimer
{
public:
    SingleShotTimer(std::function<void()> handler);
    ~SingleShotTimer();
    uint64_t setTimeout(int timeout);
    void clearTimeout(uint64_t id);

private:
    uint32_t m_handlerIndex;
};

SingleShotTimer::SingleShotTimer(std::function<void()> handler)
{
    auto discardArgWrapper = [handler](val) { handler(); };
    m_handlerIndex = t_suspendResumeControl->registerEventHandler(discardArgWrapper);
}

SingleShotTimer::~SingleShotTimer()
{
    // cancelAllTimers();
    // ### if there are pending timers then that will now call an undefined handler
    t_suspendResumeControl->removeEventHandler(m_handlerIndex);
}

uint64_t SingleShotTimer::setTimeout(int timeout)
{
    val jsHandler = t_suspendResumeControl->jsEventHandlerAt(m_handlerIndex);
    using ReturnType = double; // emscripten::val::call() does not support int64_t
    return val::global("window").call<ReturnType>("setTimeout", jsHandler, timeout);
}

void SingleShotTimer::clearTimeout(uint64_t id)
{
    val::global("window").call<void>("clearTimeout", double(id));
}

int main(int argc, char **argv)
{
    SuspendResumeControl control;

    // Events
    val canvas = val::module_property("canvas");

    EventHandler canvasPointerDown(canvas, "pointerdown", [](val event){
        cout << "pointer down at x " << event["pageX"].as<int>() << endl;
    });

    EventHandler canvasPointerUp(canvas, "pointerup", [](val event){
        cout << "pointer up" << endl;
    });

    // Animation frame
    AnimationFrameHandler animationFrameHandler([](val frametime){
        cout << "animation frame at " << frametime.as<double>() << endl;
    });
    
    animationFrameHandler.requestAnimationFrame();
    int64_t id = animationFrameHandler.requestAnimationFrame();
    animationFrameHandler.cancelAnimationFrame(id);

    // Timer
    SingleShotTimer timer([](){ cout << "timeout" << endl; });
    timer.setTimeout(200);
    int64_t timerId = timer.setTimeout(500);
    timer.clearTimeout(timerId);

    while (true) {
        control.suspend();
    }
}

