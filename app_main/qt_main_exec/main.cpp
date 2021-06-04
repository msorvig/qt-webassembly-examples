#include <QtGui>

// This example demonstrates how the standard Qt main()
// pattern works on Emscripten/WebAssambly, where exec()
// does not return.

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

class ClickWindow: public QWindow 
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
	QGuiApplication app(argc, argv);
    
    QObject::connect(&app, &QCoreApplication::aboutToQuit, [](){
        qDebug() << "QCoreApplication::aboutToQuit";
    });
	
	ClickWindow window;
	window.show();

	qDebug() << "main(): calling exec()";
	int code = app.exec();

	// The exec() call above never returns; instead, a JavaScript exception
	// is thrown such that control returns to the browser while preserving
	// the C++ stack.

	// This means that the window object above is not destroyed, and that
	// shutdown code after exec() does not run.

	qDebug() << "main(): after exit"; // <- will not be printed
}


