#include <QtGui>

class SettingsWindow : public QRasterWindow
{
public:
    SettingsWindow() {

        // The QSettings backend for Wasm is asynchronous, which makes it
        // a somewhat leaky abstraction. For example, the common pattern of
        // creating a QSettings object on the stack followed by immediate read/write
        // won't work.
        m_settings = new QSettings("MySoft", "Star Runner");

        // QSettings is not ready at this point
        Q_ASSERT(m_settings->status() == QSettings::AccessError);

        // m_settings will be ready at some later point in time - when
        // QSettings::status() returns NoError. (apologies for the somewhat
        // convoluted std::function and lambda use below)
        auto settingsReady = [=](){
            int counter = m_settings->value("counter").toInt();
            m_settings->setValue("counter", counter + 1);
            update();
        };
        std::function<void(void)> *testSettingsReady = new std::function<void(void)>();
        *testSettingsReady = [=](){
            if (m_settings->status() == QSettings::NoError) {
                delete testSettingsReady;
                settingsReady();
            } else {
                QTimer::singleShot(10, *testSettingsReady);
            }
        };
        (*testSettingsReady)();
    }

    void paintEvent(QPaintEvent *) override {
        m_counter = m_settings->value("counter").toInt();

        QPainter p(this);
        p.fillRect(QRect(0,0, 300, 300), QColorConstants::Svg::steelblue);
        p.drawText(QPoint(20, 20), QString("The counter value is %0.").arg(m_counter));
    }

    QSettings *m_settings;
    int m_counter;
};

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    SettingsWindow settingsWindow;
    settingsWindow.show();

    app.exec();
}

