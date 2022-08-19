#include <QtCore/QCoreApplication>
#include <QtCore/QString>
#include <QtCore/QDebug>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    qDebug() << QString("Hello from qDebug()");
}

