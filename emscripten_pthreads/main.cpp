#include <stdio.h>
#include <pthread.h>

#include <emscripten.h>
#include <emscripten/html5.h>

using namespace std;

int value;

int main() {

    printf("hello, world from main()!\n");
    fprintf(stderr, "anyone?");
    fflush(stdout); // ### stdout goes nowhere?

    value = 10;

    EM_ASM_({
        console.log("Value before starting thread (expecting 10) " + $0);
    }, value);

    // start a thread which updates the value
    pthread_t thread;
    bool threadCreated = pthread_create(&thread, NULL, [](void *) -> void * {
        value = 20;
        return nullptr;
    }, nullptr);

    bool threadJoined = pthread_join(thread, NULL);

    EM_ASM_({
        console.log("Value after joining thread (expecting 20) " + $0);
    }, value);

    return 0;
}
