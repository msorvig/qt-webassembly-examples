
#include <stdio.h>

#include <emscripten.h>
#include <emscripten/bind.h>

extern "C" {

void (*g_sleep_fn)();
void (*g_wake_fn)();

// Test emscripten_sleep, also recursively
void tst_emscripten_sleep()
{
    g_sleep_fn = tst_emscripten_sleep;
    g_wake_fn = nullptr;

    puts("sleeping for 3 seconds - press sleep button to sleep again");
    emscripten_sleep(3000);
    puts("woke up from sleep");
}

// Implement suspend/wake using Asyncify.handleSleep, as suggested on
// https://emscripten.org/docs/porting/asyncify.html
EM_JS(void, qt_suspend, (), {
  out("qt_suspend");
  let sleepFn = function(wakeUp) {
      if (Module.qtWakeUps == undefined)
          Module.qtWakeUps = [];
      Module.qtWakeUps.push(wakeUp);
  };
  return Asyncify.handleSleep(sleepFn);
});

EM_JS(void, qt_resume, (), {
  out("qt_resume");
  Module.qtWakeUps.pop()();
  out("qt_resume done");
});

void tst_custom_suspend()
{
    g_sleep_fn = tst_custom_suspend;
    g_wake_fn = qt_resume;
    
    puts("suspending - press wake button to wake");
    qt_suspend();
    puts("woke up from suspending");
}

// 
void html_sleep()
{
    if (g_sleep_fn)
        g_sleep_fn();
}

void html_wake()
{
    if (g_wake_fn)
        g_wake_fn();
}

EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::function("html_sleep", &html_sleep);
    emscripten::function("html_wake", &html_wake);
}

int main()
{
    puts("running test case");

    // Comment in one test case:
    // tst_emscripten_sleep();
    tst_custom_suspend();
    
    puts("done running test case");
    return 0;
}

}
