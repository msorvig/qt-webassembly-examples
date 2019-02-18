#include <QtGui>

#include "hellowindow.h"

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    qDebug() << "Screen count" << QGuiApplication::screens().size();

    int i = 0;
    for (QScreen *screen : QGuiApplication::screens()) {
        qDebug() << "Screen  " << i << screen->name();
        qDebug() << "Geometry" << screen->geometry();
        ++i;
    }

    for (QScreen *screen : QGuiApplication::screens()) {
        HelloWindow *window = new HelloWindow();
        window->setScreen(screen);
        window->showFullScreen();
    }

    return app.exec();
}
