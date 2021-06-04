#include <stdio.h>
#include <emscripten.h>
#include <emscripten/html5.h>

class GLobalStatic
{
public:
    GLobalStatic() {
        puts("GlobalStatic constructor");
    }
    ~GLobalStatic() {
        puts("GlobalStatic destructor");
    }
};
static GLobalStatic globalStatic;

int main(int argc, char  **argv) {

    puts("main");

    emscripten_set_timeout(
        [](void *) {
            puts("force exit");
            emscripten_force_exit(0);
        },
        2000,
        nullptr
    );

    emscripten_set_main_loop(
      [](){
          puts("loop");

          //  emscripten_cancel_main_loop();
          //  emscripten_pause_main_loop();
          
          // Note that both of the above calls will cancel/pause the loop
          // callback but also run global destructors, in effect shutting down
          // parts of the app. Using emscripten_set_main_loop() with "simulate infinite loop"
          // togethewr with EXIT_RUNTIME=1 does not work well.
          
      },
      1, // 1 fps
      true // simulate infinite loop
    );

    puts("main exit");
}