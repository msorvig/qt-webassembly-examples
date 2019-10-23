#include <QtCore/QtCore>
#include <QtGui/QtGui>

#ifndef QMUI_H
#define QMUI_H

class QMinimalUIWindow : public QRasterWindow
{
Q_OBJECT

struct Item {
    virtual ~Item() = default;
    virtual void paint(QPainter *p) = 0;

    QRect geometry;
};

public:
    void addItem(Item *item);
    void addButton(QString text, std::function<void(void)> onActivate);
    std::function<void(const QString &)> addLabel(const QString &text);
protected:
    
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    
    struct Button : public Item {
        QString text;
        std::function<void(void)> onActivate;
        bool pressed;
        
        void paint(QPainter *p) override;
    };

    struct Label : public Item {
        QString text;
        
        void paint(QPainter *p) override;
    };
    
    QVector<Item *> m_items;
    QVector<Button *> m_buttons;
    Button *m_pressedButton = nullptr;
};

#endif
