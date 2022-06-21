#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>

#include <emscripten/proxying.h>
#include <emscripten.h>

using namespace std;
using namespace std::chrono;
using namespace emscripten;

// Thread create/join tester. Threads are created and joined on the thread which
// creates ThreadCreateJoinTester object, via emscripten::ProxyingQueue.
class ThreadCreateJoinTester
{
public:
    ThreadCreateJoinTester() {

        createThread = [this]() -> void *{
            thread *thread = new std::thread();
            *thread = std::thread([this, thread](){ threadFunction(thread); });
            return thread;
        };

        joinThread = [](void *t) {
            std::thread *thread = static_cast<std::thread *>(t);
            thread->join();
            delete thread;
        };

        done = [](int, int) {

        };

        m_t1 = high_resolution_clock::now();
    }

    void run(int maxLiveThreads, int maxCreatedThreads) {

        m_maxLiveThreads = maxLiveThreads;
        m_maxCreatedThreads = maxCreatedThreads;
        m_liveThreadCount = 0;
        m_createdThreadCount = 0;
        createThreads();
    }

    bool tryIncrementTreadCount() {
        lock_guard guard(m_mutex);
        if (m_timedout)
            return false;
        if (m_createdThreadCount == m_maxCreatedThreads)
            return false;
        if (m_liveThreadCount == m_maxLiveThreads)
            return false;
        ++m_createdThreadCount;
        ++m_liveThreadCount;
        return true;
    }

    void decrementThreadCount() {
        lock_guard guard(m_mutex);
        --m_liveThreadCount;
    }

    void checkDone() {
        auto t2 = high_resolution_clock::now();
        int elapsed = duration_cast<milliseconds>(t2 - m_t1).count();
        m_timedout = elapsed > timeout;

        bool done = false;
        {
            lock_guard guard(m_mutex);
            done = (m_timedout || m_createdThreadCount == m_maxCreatedThreads) && m_liveThreadCount == 0;
        }
        if (done) {
            this->done(m_createdThreadCount, elapsed);
            delete this;
        }
    }

    void threadFunction(void *thread) {

       // std::this_thread::sleep_for(2ms);

        // Proxy back to the main thread and join this thread.
        m_queue.proxyAsync(m_mainThread, [this, thread]() {
            joinThread(thread);
            decrementThreadCount();
            createThreads();
            checkDone();
        });
    }

    void createThreads() {
        while (tryIncrementTreadCount())
            createThread();
    }

    std::function<void *(void)> createThread;
    std::function<void(void *)> joinThread;
    function<void(int, int)> done;

private:
    int m_maxLiveThreads = 1;
    int m_maxCreatedThreads = 1;
    int m_liveThreadCount = 0;
    int m_createdThreadCount = 0;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_t1;
    const int timeout = 500; // ms
    bool m_timedout = false;

    mutex m_mutex;
    ProxyingQueue m_queue;
    pthread_t m_mainThread = pthread_self();
};

// Calls the given iterator function n times
void asyncLoop(int n, std::function<void(int, const std::function<bool(void)> &)> iteration) {

    struct State {
        int i = 0;
        std::function<bool(void)> runit;
    };

    State *state = new State;
    state->runit = [n, state, iteration](){
        if (state->i == n) {
            delete state;
            return true; // done
        }
        ++(state->i);
        iteration(state->i, state->runit);
        return false;
    };

    // Call first iteration
    state->runit();
}

int main(int argc, char **argv)
{
    cout << "Starting Thread Create/Join Test" << endl;

    asyncLoop(5, [](int, const std::function<bool (void)> &iterationDone) {

        ThreadCreateJoinTester *tester = new ThreadCreateJoinTester();
        tester->done = [iterationDone](int finalThreadCount, int elapsed){
            cout << "DONE: completed " << finalThreadCount << " std::threads in " << elapsed  << " ms "
                 << (finalThreadCount / elapsed) <<  " threads/ms" << endl;
            if (iterationDone()) {
                cout << "Test Completed" << endl;
            }
        };
        
        const int maxConcurrentThreads = 20;
        const int threadCount = 500;
        tester->run(maxConcurrentThreads, threadCount);

    });

    return 0;

    // workaround for https://github.com/emscripten-core/emscripten/issues/16654 , along with EXIT_RUNTIME=1 in makefile
  //  emscripten_set_main_loop([](){}, 0, 0);
}
