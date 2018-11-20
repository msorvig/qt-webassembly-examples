#include <stdio.h>
#include <pthread.h>

using namespace std;

int value = 0;

int main() {

    printf("hello world from main()!\n");

    const int threadCount =2;

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

    return 0;
}
