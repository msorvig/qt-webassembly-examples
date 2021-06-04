#include <QtGui>

// This example demonstrates how to create QGuiApplication
// without calling exec(), and then exiting main() without
// shutting down Qt's event loop.

class GLobalStatic
{
public:
    GLobalStatic() {
        qDebug() << "GLobalStatic constructor";
    }
    ~GLobalStatic() {
        qDebug() << "GLobalStatic destructor";
    }
};
static GLobalStatic globalStatic;

class ClickWindow: public QRasterWindow 
{
public:
    
    ClickWindow() {
        qDebug() << "ClickWindow constructor";
    }
    ~ClickWindow() {
        qDebug() << "ClickWindow destructor";
    }
    
	void mousePressEvent(QMouseEvent *ev) override {
		qDebug() << "mousePressEvent(): calling QGuiApplication::quit()";
		QGuiApplication::quit();
	}
};

int main(int argc, char **argv)
{
    qDebug() << "main(): Creating QGuiApplication object";
	QGuiApplication *app = new QGuiApplication(argc, argv);
    
    QObject::connect(app, &QCoreApplication::aboutToQuit, [](){
        qDebug() << "QCoreApplication::aboutToQuit";
    });
	
	qDebug() << "main(): Creating ClickWindow object";
	ClickWindow *window = new ClickWindow();
	window->show();

    // We can exit main; the Qt event loop and the emscripten runtime 
    // will keep running. (Provided Emscriptens EXIT_RUNTIME is not set
    // to 1)

	qDebug() << "main(): exit";
}


