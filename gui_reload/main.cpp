
#include <QtCore>
#include <QtGui>

#include <emscripten/val.h>
#include <emscripten/bind.h>

class TestWindow;

QGuiApplication *g_app = nullptr;
TestWindow *g_window = nullptr;
int g_argc;
char **g_argv;

char** copyArgv(int argc, char* argv[])
{
    char** argvCopy = new char*[argc];
    for (int i = 0; i < argc; ++i) {
        argvCopy[i] = new char[strlen(argv[i]) + 1];
        strcpy(argvCopy[i], argv[i]);
    }
    return argvCopy;
}

class TestWindow : public QRasterWindow {
public:
    TestWindow() {
        qDebug() << "Create Window" << this;
    }

    ~TestWindow() {
        qDebug() << "Destroy Window" << this;
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        QPainter painter(this);
        painter.fillRect(event->rect(), QColor(0, 0, 255));
    }

    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton)
            qDebug() << "Window" << this << "Mouse Clicked at Coordinates:" << event->position();
    }

    void mouseDoubleClickEvent(QMouseEvent *event) override
    {
        qDebug() << "Window" << this << "double click";

        // Shut down: delete app and instance. However, Qt is in the middle of
        // processing events so we can't delete the applicaiton object.
        // (we could call deleteLater() on this window object, but that
        // still leaves the application object). Call out to JavaScript
        // and ask it to delete the app:
        emscripten::val::global("window").call<void>("timerDestroyAppAndInstance");
    }
};

void createApp()
{
    qDebug() << "C++ createApp()";

    QGuiApplication *g_app = new QGuiApplication(g_argc, g_argv);
    g_window = new TestWindow();
    g_window->show();
}

void destroyApp()
{
    qDebug() << "C++ destroyApp()";

    delete g_window;
    g_window = nullptr;
    delete g_app;
    g_app = nullptr;
}

EMSCRIPTEN_BINDINGS(gui_reload)
{
    emscripten::function("createApp", &createApp);
    emscripten::function("destroyApp", &destroyApp);
}

int main(int argc, char **argv)
{
    qDebug() << "C++ main()";

    // Emscripten frees argv when main() returns; a make copy for
    // the QGuiApplication constructor.
    g_argc = argc;
    g_argv = copyArgv(argc, argv);
}

