#include <stdio.h>
#include <pthread.h>
#include <emscripten.h>
#include <emscripten/html5.h>

using namespace std;

int value = 0;

void create_and_join()
{
    const int threadCount = 2;

    pthread_t threads[threadCount];
    for (int i = 0; i < threadCount; ++i) {
        pthread_create(&(threads[i]) , NULL, [](void *) -> void * {
            printf("hello world from thread \n");
            value += 1;
            return nullptr;
        }, nullptr);
    }

    for (int i = 0; i < threadCount; ++i) {
        pthread_join(threads[i], NULL);
    }

    printf("Value after joining thread (expecting %d) %d \n", threadCount, value);
}


void timer_callback(void *userData) {
    pthread_t thread_id = pthread_self();
    printf("Timer triggered on thread %lu!\n", (unsigned long)thread_id);
}

void create_and_timer()
{
    emscripten_set_timeout(timer_callback, 100, NULL);

    const int threadCount = 1;

    pthread_t threads[threadCount];
    for (int i = 0; i < threadCount; ++i) {
        pthread_create(&(threads[i]) , NULL, [](void *) -> void * {
            printf("hello world from thread \n");
            emscripten_set_timeout(timer_callback, 1000, NULL);
            emscripten_set_timeout(timer_callback, 2000, NULL);
            emscripten_set_timeout(timer_callback, 3000, NULL);
            value += 1;
            return nullptr;
        }, nullptr);
    }
}

int main() {

    printf("hello world from main()!\n");

    create_and_join();
    create_and_timer();

    return 0;
}
