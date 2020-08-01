#include <QtWidgets/QtWidgets>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTextEdit textEdit;

    QString pez;
    for (int i = 0; i < 100; ++i) {
        pez += QString("<p> Text line number %1").arg(i);
    }
    textEdit.setHtml(pez);
    textEdit.show();

    return app.exec();
}
