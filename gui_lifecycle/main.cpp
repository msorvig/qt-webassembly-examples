#include <QtGui>

#include <functional>

#ifdef Q_OS_WASM
#include <emscripten.h>
#endif

// QMUI: Qt Minimal UI

class QMUIWindow : public QRasterWindow
{
Q_OBJECT
public:
    void addButton(QString text, std::function<void(void)> onActivate);
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    struct Button {
        QString text;
        std::function<void(void)> onActivate;
        QRect geometry;
        bool pressed;
    };
    
    QVector<Button> m_buttons;
    Button *m_pressedButton = nullptr;
};


void QMUIWindow::addButton(QString text, std::function<void(void)> onActivate)
{
    static int y = 10;
    const int x = 10;
    const int hy = 20;
    const int dy = 10;

    m_buttons.append({
        text,
        onActivate,
        QRect(x, y, 250, hy),
        false
    });
    y += (hy + dy);
}

void QMUIWindow::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event);

    // Draw buttons with text and pressed state
    QPainter p(this);
    p.fillRect(QRect(QPoint(0, 0), this->size()), QColor(130, 40, 90));
    for (auto button : m_buttons) {
        p.setPen(!button.pressed ? QColor(240, 240, 240) : QColor(140, 140, 140));
        p.drawRect(button.geometry);
        p.drawText(button.geometry.adjusted(10, 0, 0, 0), button.text);
    }
}

void QMUIWindow::mousePressEvent(QMouseEvent *event)
{
    // Find button under mouse and set down-presseed state
    for (auto &button : m_buttons) {
        if (button.geometry.contains(event->pos())) {
            button.pressed = true;
            m_pressedButton = &button;
            update();
        }
    }
}

void QMUIWindow::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    // Check if the mouse cursor is still over any down-pressed button
    if (m_pressedButton) {
        bool nowPressed = m_pressedButton->geometry.contains(event->pos());
        if (nowPressed != m_pressedButton->pressed) {
            m_pressedButton->pressed = nowPressed;
            update();
        }
    }
}

void QMUIWindow::mouseReleaseEvent(QMouseEvent *event)
{
    // Activate button if released on button interior
    if (m_pressedButton) {
        if (m_pressedButton->geometry.contains(event->pos())) {
            m_pressedButton->onActivate();
        }
        m_pressedButton->pressed = false;
        m_pressedButton = nullptr;
        update();
    }
}

void stackOverflow() {
    static volatile int number = 0;
    stackOverflow();
    ++number;
    stackOverflow();
}

volatile void *ptr;
void heapOverflow() {
    for(int i = 0; i < 10000000; ++i) {
        ptr = malloc(100000);
    }
}


int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    
    QMUIWindow window;

#ifdef Q_OS_WASM
    window.addButton("exit(0)", [](){
        emscripten_force_exit(0);
    });

    window.addButton("exit(1)", [](){
        emscripten_force_exit(1);
    });

    window.addButton("Module.abort()", [](){
        EM_ASM(Module.abort("Abort button pressed"););
    });
#endif

    window.addButton("Stack Overflow", [](){
        stackOverflow();
    });

    window.addButton("Heap Owerflow", [](){
        heapOverflow();
    });

    window.addButton("*((volatile int *)(0)) = 42;", [](){
        *((volatile int *)(0)) = 42;
    });

    window.addButton("memset(100, 1, 1M)", [](){
        int *ptr = (int *)100;
        memset(ptr, 1, 1024 * 1024);
    });
    
    window.showFullScreen();
    
    return app.exec();
}

#include "main.moc"
