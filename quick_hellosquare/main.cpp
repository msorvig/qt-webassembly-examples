#include <QtGui>
#include <QtQuick>

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    
    QQuickView view;
    view.setSource(QUrl("qrc:///main.qml"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.showFullScreen();

    return app.exec();
}
