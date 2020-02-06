#include <stdio.h>

#include <emscripten.h>
#include <emscripten/bind.h>

extern "C" {

void main_func(void *context);

void start_main_loop()
{
    int infinite_loop = 1; // simulate no-return
    int fps = 0;
    void *context = nullptr;
    emscripten_set_main_loop_arg(main_func, context, fps, infinite_loop);
}

// prototype implementaton of Qt envent loop functions:
//   processEvents(bool waitForEvents) : processes pending events (mouse/keboard/timers etc). Waits for 
//.                                      the next event if waitForEvents is set
//   interrupt()                       : interrupts a current processEvents() call, making it return
//                                       as early as possible
bool g_interrupt = false;

void interrupt()
{
    puts("C interrupt");
    g_interrupt = true;
}

void processEvents(bool waitForEvents)
{
    puts("  processEvents");
    
    // process events until interrupted. Implemented using emscripten_sleep, which
    // has the unfortunate side effect that we must poll for the g_interrupt state
    // at regular intervals.
    const int sleepTime = waitForEvents ? 200 : 0; // 0: process all pending events, then return
    do {
        puts("  emscripten_sleep");
        emscripten_sleep(sleepTime);
    } while (waitForEvents && !g_interrupt);
    g_interrupt = false;
    
    puts("  processEvents done");
}

EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::function("interrupt", &interrupt);
}

void main_func(void *context)
{
    puts("C++: main_func main loop callback");
    emscripten_cancel_main_loop();

    puts("sleeping");
    emscripten_sleep(100);
    puts("woke up");

    puts("processEvents once");
    processEvents(false);

    puts("processEvents wait");
    processEvents(true);

    puts("processEvents all done");
}

int main()
{
    puts("C++: hello from main");

    start_main_loop();

    puts("this will not print - start_main_loop never returns");

    return 0;
}

// Possible new emscripten API for better control over suspend/resume
extern void emscripten_yield(); // process all pending browser/JS events, then return
extern void emscripten_suspend(); // suspend until emscripten_resume is called; keep processing events
extern void emscripten_resume(); // resume at previous emscripten_suspend site.

void interrupt_2()
{
    g_interrupt = true;
    emscripten_resume(); 
}
    
void processEvents_2(bool waitForEvents)
{
    if (!waitForEvents) {
        emscripten_yield(); 
    } else {
        do {
            emscripten_suspend();
        } while (!g_interrupt);
    }
}

}
