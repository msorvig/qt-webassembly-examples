#include <QtGui>
#include <QtQuick>

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    
    QQuickView view;
    view.setSource(QUrl("qrc:///main.qml"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.showFullScreen();

    int frames = 0;
    int fps = 0;

    QObject::connect(&view, &QQuickView::beforeRendering, [&frames](){
        ++frames;
    });

    auto timerFire = std::function<void ()>();
    timerFire = [&frames, &fps, &timerFire, &view] {
        fps = frames;
        frames = 0;
        QString fpsText = QString("FPS: %1").arg(fps);
        view.rootObject()->setProperty("fpsText", QVariant(fpsText));
        QTimer::singleShot(1000, timerFire);
    };
    timerFire();

    return app.exec();
}
