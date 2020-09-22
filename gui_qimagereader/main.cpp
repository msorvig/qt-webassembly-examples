#include <QtCore>
#include <QtGui>

int main(int argc, char **argv) {

    QGuiApplication app(argc, argv);

    // write image to the internal in-memory file system
    QImage image(32, 32, QImage::Format_ARGB32_Premultiplied);
    image.save("foo.png");

    // Check canRead() and call read() on this thread, on a worker thread,
    // and from timers.

    {
        QImageReader reader("foo.png");
        qDebug() << "main thread canRead foo.png" << reader.canRead();
        QImage image = reader.read();
    }

    QThread *thread = QThread::create([]{ 
        QImageReader reader("foo.png");
        qDebug() << "thread canRead foo.png" << reader.canRead();
        QImage image = reader.read();
    });

    thread->start();

    QTimer *timer = new QTimer();
    QObject::connect(timer, &QTimer::timeout, [](){
        QImageReader reader("foo.png");
        qDebug() << "timer canRead foo.png" << reader.canRead();
        QImage image = reader.read();
    });
    timer->start(500);

    qDebug() << "app exec";
    return app.exec(); 
}