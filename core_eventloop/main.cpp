#include <QtCore/QtCore>

#include <emscripten/bind.h>
#include <emscripten/val.h>

#include <thread>

using namespace emscripten;

class EventTarget;

QCoreApplication *g_app = nullptr;
QThread *g_eventLoopThread = nullptr;

std::thread *g_eventPosterThread = nullptr;
std::condition_variable g_condition;
std::mutex g_mutex;
bool g_ready = false;
bool g_exit = false;
enum ThreadTask { NoOp, PostEvent, CreateTimer };
ThreadTask g_threadTask = NoOp;

int g_timeout;
std::vector<QTimer *> g_timers;

int g_eventType = -1;
int g_selfPostEventType = -1;
int g_registerTimerEventType = -1;


// Event targets: QObject on main thread and on worker thread
EventTarget *g_mainEventTarget = nullptr;
EventTarget *g_workerEventTarget = nullptr;

// Event posting behavior
//bool g_postFromMainThread = true;
//bool g_postFromServiceThread = false;
bool g_postToMainThread = true;
bool g_postToSecondaryThread = false;
bool g_selfPostExtraEvent = false;

bool g_eventCount = 1;

void createTimer(EventTarget *target, int timeout);

class EventLoopThread : public QThread
{
    void run() {
        qDebug() << "EventLoopThread thread" << QThread::currentThreadId();
        exec();
    }
};

class EventTarget : public QObject
{
public:
    EventTarget(const QString &name)
        :m_name(name) { }

    bool event(QEvent *event) override {
        if (event->type() == g_eventType) {
            qDebug() << "Test Harness: Event delivered" << m_name << "on thread" << QThread::currentThreadId();
            if (g_selfPostExtraEvent)
                g_app->postEvent(this, new QEvent(QEvent::Type(g_selfPostEventType)));
        } else if (event->type() == g_selfPostEventType) {
            qDebug() << "Test Harness: Event (self-posted) delivered" << m_name << "on thread" << QThread::currentThreadId();
        } else if (event->type() == g_registerTimerEventType) {
            createTimer(this, g_timeout);

        } else {
            qDebug() << "Test Harness: Event of unknown type";
        }
        return QObject::event(event);
    };
    QString m_name;
};

extern int qGlobalPostedEventsCount(); // from qapplication.cpp

void postEvents()
{
    qDebug() << "Test Harness: postEvents from thread" << QThread::currentThreadId();

//    qDebug() << "Test Harness: qGlobalPostedEventsCount" << qGlobalPostedEventsCount();

    if (g_postToMainThread)
        g_app->postEvent(g_mainEventTarget, new QEvent(QEvent::Type(g_eventType)));
    if (g_postToSecondaryThread)
        g_app->postEvent(g_workerEventTarget, new QEvent(QEvent::Type(g_eventType)));
    if (!g_postToMainThread && !g_postToSecondaryThread)
        qDebug() << "Test Harness Warning: No event target selected. Did not post event";

//    qDebug() << "Test Harness: qGlobalPostedEventsCount" << qGlobalPostedEventsCount();
}

void createTimer(EventTarget *target, int timeout)
{
    qDebug() << "Test Harness: createTimer on thread" << QThread::currentThreadId();
    QTimer *timer = new QTimer;
    g_timers.push_back(timer);
    timer->moveToThread(target->thread());
    timer->callOnTimeout(target, [](){ qDebug() << "timeout" ; });
    timer->start(timeout);
}

void thread_function()
{
    qDebug() << "Test Harness: Starting service thread" << QThread::currentThreadId();
    do {
        std::unique_lock<std::mutex> lock(g_mutex);
        g_condition.wait(lock, []{ return g_ready; });
        g_ready = false;
        if (g_threadTask == PostEvent)
            postEvents();
        else if (g_threadTask == CreateTimer)
            createTimer(g_workerEventTarget, g_timeout);
        else {
           qDebug() << "Test Harness: service thread started with NoOp task";
        }
        g_threadTask = NoOp;
    } while (!g_exit);
}

int main(int argc, char **argv)
{
    // This example tests QEventDispatcherWasm on the main
    // and secondary threads.

    qDebug() << "Test Harness: Main thread" << QThread::currentThreadId();

    // Crate main thread event loop
    g_app = new QCoreApplication(argc, argv);
    g_eventType = QEvent::registerEventType();
    g_selfPostEventType = QEvent::registerEventType();
    g_registerTimerEventType = QEvent::registerEventType();
    g_mainEventTarget = new EventTarget("main");

    // Create secondary thread event loop
    QThread * g_eventLoopThread = new EventLoopThread();
    g_eventLoopThread->start();
    g_workerEventTarget = new EventTarget("secondary");
    g_workerEventTarget->moveToThread(g_eventLoopThread);

    // Create service thread used by the test harness
    g_eventPosterThread = new std::thread(thread_function);

    // Return. Calling app->exec() is not required.
    qDebug() << "Test Harness: return from main";
    return 0;
}

extern "C" {

///  Events

void postEventsWithArgs(bool postFromMainThread, bool postFromServiceThread,
                        bool postToMainThread, bool postToSecondaryThread) {
    qDebug() << "Test Harness: postEventsWithArgs";

//    g_postFromMainThread = postFromMainThread;
//    g_postFromServiceThread = postFromServiceThread;
    g_postToMainThread = postToMainThread;
    g_postToSecondaryThread = postToSecondaryThread;

    if (postFromServiceThread) {
        g_threadTask = PostEvent;
        std::unique_lock<std::mutex> lock(g_mutex);
        g_ready = true;
        g_condition.notify_one();
    }

    if (postFromMainThread)
        postEvents();
    if (!postFromServiceThread && !postFromMainThread)
        qDebug() << "Test Harness Warning: No event target selected. Did not post event";
}

/// Timers

void registerMainThreadTimer(int timeout) {
    qDebug() << "Test Harness: registerMainRhreadTimer" << timeout;
    createTimer(g_mainEventTarget, timeout);
}

void registerSecondaryThreadTimer(int timeout) {
    qDebug() << "Test Harness: registerWorkerThreadTimer" << timeout;
    createTimer(g_workerEventTarget, timeout);
}

void clearTimers()
{
    for (QTimer *timer : g_timers) {
        timer->stop();
        timer->deleteLater();
    }
    g_timers.clear();
}

// Exec

void appExec() {
    qDebug() << "Test Harness: appExec";
    qApp->exec();
}

void appQuit() {
    qDebug() << "Test Harness: appQuit";
    qApp->quit();
}

// Note: Emscripten does not currently suport recursive suspend,
// which means that nesting event loops beyond the first one is
// going to fail. We stil allow testing that case here.
QStack<QEventLoop *> g_eventLoops;
void eventLoopExec() {
    qDebug() << "Test Harness: QEventLoop exec()";
    QEventLoop *eventLoop = new QEventLoop();
    g_eventLoops.push(eventLoop);
    eventLoop->exec();
}

void eventLoopQuit() {
    qDebug() << "Test Harness: QEventLoop quit()";
    if (g_eventLoops.isEmpty()) {
        qDebug() << "Test Harness: QEventLoop quit() called without correspnding exec()";
        return;
    }

    QEventLoop *eventLoop = g_eventLoops.pop();
    eventLoop->quit();
    // delete eventLoop;
}

} // extern "C"

EMSCRIPTEN_BINDINGS(my_module) {
    function("postEventsWithArgs", &postEventsWithArgs);
    function("registerMainThreadTimer", &registerMainThreadTimer);
    function("registerSecondaryThreadTimer", &registerSecondaryThreadTimer);
    function("clearTimers", &clearTimers);
    function("appExec", &appExec);
    function("appQuit", &appQuit);
    function("eventLoopExec", &eventLoopExec);
    function("eventLoopQuit", &eventLoopQuit);
}
