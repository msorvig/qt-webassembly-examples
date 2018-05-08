#include <qmui.h>

const int g_width = 250;
const int g_height = 20;
const int g_spacing = 20;

void QMinimalUIWindow::addItem(Item *item)
{
    static int y = 10;
    const int x = 10;
    
    item->geometry.moveTo(x, y);
    qDebug() << "addItem" << y << item->geometry;
    y += (g_height + g_spacing);
    m_items.append(item);
}

void QMinimalUIWindow::addButton(QString text, std::function<void(void)> onActivate)
{
    Button *button = new Button;
    button->text = text;
    button->onActivate = onActivate;
    button->pressed = false;
    button->geometry = QRect(0, 0, g_width, g_height);
    m_buttons.append(button);
    addItem(button);
}

std::function<void(const QString &)> QMinimalUIWindow::addLabel(const QString &text)
{
    Label *label = new Label();
    label->geometry = QRect(0, 0, g_width, g_height);
    label->text = text;
    addItem(label);
    auto textSetter = [label, this](const QString &text) {
        label->text = text;
        this->update();
    };
    return textSetter;
}

void QMinimalUIWindow::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event);

    QPainter p(this);
    p.fillRect(QRect(QPoint(0, 0), this->size()), QColor(130, 40, 90));

    for (auto item : m_items)
        item->paint(&p);
}

void QMinimalUIWindow::mousePressEvent(QMouseEvent *event)
{
    // Find button under mouse and set down-presseed state
    for (auto &button : m_buttons) {
        if (button->geometry.contains(event->pos())) {
            button->pressed = true;
            m_pressedButton = button;
            update();
        }
    }
}

void QMinimalUIWindow::mouseMoveEvent(QMouseEvent *event)
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

void QMinimalUIWindow::mouseReleaseEvent(QMouseEvent *event)
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

void QMinimalUIWindow::Button::paint(QPainter *p)
{
    p->setPen(!this->pressed ? QColor(240, 240, 240) : QColor(140, 140, 140));
    p->drawRect(this->geometry);
    p->drawText(this->geometry.adjusted(10, 0, 0, 0), this->text);
}

void QMinimalUIWindow::Label::paint(QPainter *p)
{
    p->setPen(QColor(240, 240, 240));
    p->drawText(this->geometry.adjusted(10, 0, 0, 0), this->text);
}


