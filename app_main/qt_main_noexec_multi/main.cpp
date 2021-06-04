#include <QtGui>

// This example demonstrates how to create QGuiApplication
// without calling exec(), and then exiting main() without
// shutting down Qt's event loop.

class ClickWindow: public QRasterWindow 
{
public:
	void mousePressEvent(QMouseEvent *ev) override {
		qDebug() << "mousePressEvent(): at" << ev->pos();
	}
};

int main(int argc, char **argv)
{

    int i = 4;
    while (--i > 0) {
        qDebug() << "main(): Creating app and window ";
        QGuiApplication *app = new QGuiApplication(argc, argv);
        QObject::connect(app, &QCoreApplication::aboutToQuit, [](){
            qDebug() << "QCoreApplication::aboutToQuit";
        });
        
        ClickWindow *window = new ClickWindow();
        qDebug() << "main(): Destroying window and app";
        delete window;
        delete app;
    }

    qDebug() << "main(): Creating final app";
	QGuiApplication *app = new QGuiApplication(argc, argv);
	ClickWindow *window = new ClickWindow();
	window->show();
    qDebug() << "main(): exit";
}


