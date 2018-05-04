#include <stdio.h>

#include <emscripten.h>

extern "C" {

void main_func(void *context);

void start_main_loop()
{
    int infinite_loop = 1; // simulate no-return
    int fps = 2;
    void *context = nullptr;
    emscripten_set_main_loop_arg(main_func, context, fps, infinite_loop);
}

void main_func(void *context)
{
    puts("C++: main_func main loop callback");
    emscripten_set_main_loop_timing(EM_TIMING_RAF, 1);
}

// set timing and raf usage with emscripten_set_main_loop_timing
void update_loop_timing1(int delay, int useRaf)
{
    printf("C++: update_loop_timing1 %d %d\n", delay, useRaf);

    // Update main loop timing based on whether we are unimating with requestAnimationFrame
    // or waiting for a timer fire.
    // ### this has the bug that emscripten_set_main_loop_timing won't immediately apply
    //     new timings - we need to way until the original delay has elapsed.
    //     See https://github.com/kripken/emscripten/issues/6488
    if (useRaf) {
        emscripten_set_main_loop_timing(EM_TIMING_RAF, 1); // 1: full frame rate
    } else {
        emscripten_set_main_loop_timing(EM_TIMING_SETTIMEOUT, delay);
    }
}

// set timing with emscripten_set_main_loop_timing
void update_loop_timing2(int delay, int useRaf)
{
    printf("C++: update_loop_timing2 %d %d\n", delay, useRaf);
    
    // main loop timing is always "use requestAnimationFrame". We enable/disable
    // main loop according to if animations are enabled.
    emscripten_set_main_loop_timing(EM_TIMING_RAF, 1);
    if (useRaf) {
        emscripten_resume_main_loop();
    } else {
        emscripten_pause_main_loop();
    }
    
    // setup timed callback if we are not going to wake up on the next frame using RAF
    if (!useRaf)
        emscripten_async_call(main_func, nullptr, delay);
}

int main()
{
    puts("C++: hello from main");
    
    start_main_loop();
    
    puts("this will not print - start_main_loop never returns");
    
    return 0;
}

}
