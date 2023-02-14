#include <QtCore>

#include <emscripten/bind.h>
#include <emscripten/val.h>

int main(int argc, char **argv)
{
    QCoreApplication *app = new QCoreApplication(argc, argv);

    qDebug() << "hello";
}