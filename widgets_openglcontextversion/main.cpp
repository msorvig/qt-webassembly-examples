#include <QSurfaceFormat>
#include <QtWidgets>
#include <QOpenGLWindow>

class GLWindow: public QOpenGLWindow
{
public:
    GLWindow(QWindow *parent = nullptr)
        : QOpenGLWindow(QOpenGLWindow::NoPartialUpdate, parent)
    {

    }

protected:
    void paintGL() override {
        qDebug() << "paintGL";
        glClearColor(0.3, 0.3, 0.7, 1);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void mousePressEvent(QMouseEvent *event) override {
        qDebug() << "mousePressEvent" << event->pos();
        event->setAccepted(false);
        this->hide();
        delete this;
    }
};

class Controller : public QWidget
{
public:
    Controller() {
        QVBoxLayout *layout = new QVBoxLayout();
        setLayout(layout);

        QLineEdit *glVersion = new QLineEdit("3");
        layout->addWidget(glVersion);

        QPushButton *newWindow = new QPushButton("Create Window");
        connect(newWindow, &QPushButton::clicked, [this, glVersion](){
            qDebug() << "new window";
            qDebug() << "Setting default GL major version to" << glVersion->text();

            QSurfaceFormat format;
            format.setMajorVersion(glVersion->text().toInt());
            QSurfaceFormat::setDefaultFormat(format);
            if (m_window)
                delete m_window;
            m_window = new GLWindow(this->windowHandle());
            m_window->setGeometry(20, 150, 300, 200);
            m_window->show();
        });
        layout->addWidget(newWindow);

        QPushButton *newContext = new QPushButton("Create Extra Context");
        connect(newContext, &QPushButton::clicked, [this, glVersion]() {
            qDebug() << "new context with GL major version" << glVersion->text();

            QSurfaceFormat format;
            format.setMajorVersion(glVersion->text().toInt());

            QOpenGLContext *context = new QOpenGLContext();
            context->setFormat(format);
            context->create();
            qDebug() << "context is valid?" << context->isValid();

            if (m_window) {
                bool ok = context->makeCurrent(m_window);
                qDebug() << "makeCurrent" << ok;
                qDebug() << "context is valid?" << context->isValid();
            }
                
        });
        layout->addWidget(newContext);

        layout->addStretch();
    }
private:
    QPointer<QWindow> m_window = nullptr;
};

int main( int argc, char ** argv )
{
    QApplication app( argc, argv );

    Controller controller;
    controller.resize(640, 480);
    controller.show();

    return app.exec();
}
