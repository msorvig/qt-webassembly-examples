#include <qmui.h>
#include <emscripten/val.h>

using namespace emscripten;

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    
    QMinimalUIWindow window;
    QMinimalUIWindow *popup = nullptr;
    val popupWindow = val::undefined();
    
    // Create UI with open/close buttons
    window.addLabel("Warning: non-functional");
    window.addButton("Open Popup Window", [&]() {
        qDebug() << "C++ open popup window";
        std::string source = "popup.html";
        std::string title = "Popup Window";
        std::string features = "height=500,width=500,resizable=yes,scrollbars=yes,toolbar=no,menubar=no,location=no,directories=no,status=no";
        popupWindow = val::global("window").call<val>("open", source, title, features);
    });
    window.addButton("Close Popup Window", [&]() {
        qDebug() << "C++ close popup window";
        if (popupWindow == val::undefined())
            return;
        popupWindow.call<void>("close");
        popupWindow = val::undefined();
    });
    
    // Connect to screenAdded/screenRemoved functions
    QObject::connect(&app, &QGuiApplication::screenAdded, [=, &popup](QScreen *screen){
        qDebug() << "C++ screen added" << screen->name();
        popup = new QMinimalUIWindow();
        popup->addLabel("This is the popup window");
        popup->setScreen(screen);
        popup->showFullScreen();
    });
    QObject::connect(&app, &QGuiApplication::screenRemoved, [=, &popup](QScreen *screen){
        qDebug() << "C++ screen removed" << screen->name();
        delete popup;
        popup = nullptr;
    });

    window.showFullScreen();
    
    return app.exec();
}
