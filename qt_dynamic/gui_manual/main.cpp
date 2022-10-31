#include <QtGui/QtGui>

#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>

void foo()
{
    emscripten::val console = emscripten::val::global("console");
    emscripten::val promiseObject = console.call<emscripten::val>("log", console);
}

int main(int argc, char **argv)
{
    foo();

    QGuiApplication app(argc, argv);
    qDebug() << QString("Hello from qDebug()");
    QTimer::singleShot(2500, [](){
        qDebug() << "Hello again";
    });
    return app.exec();
}
