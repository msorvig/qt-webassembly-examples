#include <iostream>

#include <emscripten.h>
#include <emscripten/html5.h>

using namespace std;

// This example suspends and resumes on a requestAnimationFrame callback, using
// emscripten_request_animation_frame() and C++/wasm callback code.

static bool isSuspended = false;
std::function<void()> resumeFunction;

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

    // Call resumeFunction if any has been registered.
    if (resumeFunction) {
        auto fn = resumeFunction;
        resumeFunction = std::function<void()>();
        fn();
    }
}

EM_JS(void, resume, (), {
    if (Module.resume) {
        const resume = Module.resume;
        Module.resume = null;
        resume();
    }
});

void requestFrame()
{
    static auto frame_callback = [](double frameTime, void *context) -> bool {
        cout << "frame callback context:" << context << "  " << "suspended: " << isSuspended << endl;
        return false;
    };

    static auto frame_callback_intermediate_resume = [](double frameTime, void *context) -> bool {

        // Set the resume function with captured data for this call, and then resume
        resumeFunction = [frameTime, context](){
            frame_callback(frameTime, context);
        };

        resume();

        cout << "event handler return, context: " << context << endl;

        // ### At this point we should return true or false, where true
        // signals that we want a new frame. However, the actual frame
        // call back has not run yet, and won't until after we return.
        return false;
    };

    emscripten_request_animation_frame(frame_callback_intermediate_resume, (void*)1);
    emscripten_request_animation_frame(frame_callback_intermediate_resume, (void*)2);
}

int main(int argc, char **argv)
{
    requestFrame();

    while (true){
        suspend();
    }
}
