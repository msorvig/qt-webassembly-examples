#include <QtCore>
#include <stdio.h>
#include <pthread.h>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    qDebug() << "Main thread running";
    qDebug() << "QThread::idealThreadCount()" << QThread::idealThreadCount();

    QThread *thread = QThread::create([] {
        qDebug() << "QThread::create thread running";
        qDebug() << "QThread::idealThreadCount() from thread" << QThread::idealThreadCount();
    });
    thread->start();

    pthread_t thread2;
    pthread_create(&thread2 , NULL, [](void *) -> void * {
        printf("pthread_create thread running \n");
        return nullptr;
    }, nullptr);

    // NOTE: The follwing wait() and join() calls will block the main thread,
    // which will (on Q_OS_WASM) prevent the browser from creating the web
    // workers for the treads, resulting in a deadlock. UNLESSS the web wokers
    // have already been created by building with -s PTHREAD_POOL_SIZE=n.
    thread->wait();
    pthread_join(thread2, NULL);

    qDebug() << "Main thread exit";
}
