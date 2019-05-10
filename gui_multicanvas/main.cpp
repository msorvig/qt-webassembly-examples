#include <QtGui>
#ifdef HAVE_WIDGETS
#include <QtWidgets>
#endif

#include "hellowindow.h"
#include "rasterwindow.h"

QWindow *createHelloOpenGLWindow()
{
    return new HelloWindow();
}

QWindow *createRasterWindow()
{
    return new RasterWindow();
}

#ifdef HAVE_WIDGETS
QWindow *createQWidgetMainWindow()
{
    QWidget* mainWindow = new QMainWindow();
    mainWindow->winId();
    return mainWindow->windowHandle();
}
#endif

// select window content type
QWindow *createWindow(int screenAddress)
{
    QWindow *window = (screenAddress% 16 == 0 ) ? createHelloOpenGLWindow() : createRasterWindow();
    //QWindow *window = createHelloOpenGLWindow();
    //QWindow *window = createMaindow();
    //QWindow *window = createRasterWindow();
    return window;
}

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

    // Verify that QWidnows can be constructed, even if there are
    // no screens.
    if (QGuiApplication::screens().size() == 0) {
        QWindow *dummy = createHelloOpenGLWindow();
        dummy->setScreen(nullptr);
    }

    // One window per screen
    QHash<QScreen *, QWindow *> windows;
    auto addScreenWindow = [&windows](QScreen *screen) {
        QWindow *window = createWindow(int(screen));
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
