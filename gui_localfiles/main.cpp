#include <qmui.h>

#include <QtCore/qhtml5file.h>
#include <QtCore/qhtml5file.h>

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    
    QMinimalUIWindow window;
    
    QByteArray fileContent;
    std::function<void(const QString&)> updateFileSisze;
    std::function<void(const QString&)> updateFileSha1;

    window.addButton("Load File", [&]() {
        QHtml5File::load("*", [&](const QByteArray &content, const QString &fileName){
            qDebug() << "load file" << fileName << "size" << content.size();
            fileContent = content;
            
            updateFileSisze(QString("size: ")  + QString::number(content.size()));

            QTimer::singleShot(60, [&](){
                auto sha1 = QCryptographicHash::hash(content, QCryptographicHash::Sha1).toHex();
                updateFileSha1(QString(sha1));
            });
        });
    });

    window.addButton("Save File", [&](){
        QString fileName = "qttestfile.dat";
        qDebug() << "save file" << fileName << "size" << fileContent.size();
        QHtml5File::save(fileContent, fileName);
    });
    
    updateFileSisze = window.addLabel("size:");
    updateFileSha1 = window.addLabel("sha1: ");

    window.showFullScreen();
    
    return app.exec();
}
