#include <QtGui>

#include "hellowindow.h"

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    // Print info on initial screens
    qDebug() << "Screen count" << QGuiApplication::screens().size();
    int i = 0;
    for (QScreen *screen : QGuiApplication::screens()) {
        qDebug() << "Screen  " << i << screen->name();
        qDebug() << "Geometry" << screen->geometry();
        ++i;
    }

    // One window per screen
    QHash<QScreen *, QWindow *> windows;
    auto addScreenWindow = [&windows](QScreen *screen) {
        HelloWindow *window = new HelloWindow();
        window->setScreen(screen);
        window->showFullScreen();
        windows.insert(screen, window);
    };
    auto removeScreenWindow = [&windows](QScreen *screen) {
        delete windows.take(screen);
    };

    // Add windows for screens present at startup
    for (QScreen *screen : QGuiApplication::screens())
        addScreenWindow(screen);
    
    // Maintain windows as screens are added and removed
    QObject::connect(&app, &QGuiApplication::screenAdded, [=](QScreen *screen){
        addScreenWindow(screen);
    });
    QObject::connect(&app, &QGuiApplication::screenRemoved, [=](QScreen *screen){
        removeScreenWindow(screen);
    });

    return app.exec();
}
