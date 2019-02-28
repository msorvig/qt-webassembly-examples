#include <QtCore>
#include <QtNetwork>
#include <QGuiApplication>

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv); // for the event loop

    QUrl url("http://localhost:8000/core_network.wasm");
    QNetworkRequest request(url);
    QNetworkAccessManager manager;
    QObject::connect(&manager, &QNetworkAccessManager::finished, [](QNetworkReply *reply) {
        qDebug() << "download finished";
        if (reply->error()) {
            qDebug() << "error" << reply->errorString();
        } else {
            QByteArray payload = reply->readAll();
            qDebug() << "size" << payload.size();
            
            QByteArray hash = QCryptographicHash::hash(payload, QCryptographicHash::Sha256);
            qDebug() << "sha256" << hash.toHex();
        }
    });
    
    qDebug() << "download begin";
    manager.get(request);
        
    return app.exec();
     
 }
