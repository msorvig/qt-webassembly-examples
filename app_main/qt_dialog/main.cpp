#include <QtGui>
#include <QtWidgets>

// This example show how calling QDialog::exec() shows the dialog,
// but does not return.

class ClickWindow: public QWindow 
{
public:
    ClickWindow() {
        qDebug() << "ClickWindow constructor";
    }
    ~ClickWindow() {
        qDebug() << "ClickWindow destructor";
    }

	void mousePressEvent(QMouseEvent *) override {
		qDebug() << "mousePressEvent(): calling QMessageBox::exec()";

        QMessageBox messageBox;
        messageBox.setText("Hello! This is a message box.");
        connect(&messageBox, &QMessageBox::buttonClicked, [](QAbstractButton *button) {
           qDebug() << "Button Clicked" << button;
        });
        messageBox.exec(); // <-- does not return

        qDebug() << "mousePressEvent(): done"; // <---  will not be printed
	}
};

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
    
	ClickWindow window;
	window.show();

    return app.exec();
}


