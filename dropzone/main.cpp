
#include <QtWidgets>

class Dropzone : public QWidget
{
public:
    Dropzone() {
        
        setAcceptDrops(true);

        QPalette pal = QPalette();
        pal.setColor(QPalette::Window, QColorConstants::Svg::steelblue);
        setAutoFillBackground(true); 
        setPalette(pal);
        
        auto label = new QLabel("Drop Files Here");
        auto formats = new QLabel("Formats:");
        auto sizes = new QLabel("Size:");
        auto layout = new QVBoxLayout();
        layout->addWidget(label);
        layout->addWidget(formats);
        layout->addWidget(sizes);
        layout->addStretch();
        setLayout(layout);
        
        m_dropHandler = [formats, sizes](QDropEvent *event){
            auto mime = event->mimeData();

            formats->setText(QLatin1String("Formats: ") + mime->formats().join(" "));

            QString sizeText = "Sizes: ";
            for (auto format: mime->formats())
                sizeText += format + ": " + QString::number(mime->data(format).size());
            sizes->setText(sizeText);
            event->acceptProposedAction();
        };
    }
    
    void dragEnterEvent(QDragEnterEvent *event)
    {
        qDebug() << "dragEnterEvent";
        event->acceptProposedAction();
    }
    
    void dropEvent(QDropEvent *event)
    {
        qDebug() << "dropEvent";
        m_dropHandler(event);
    }
private:
    std::function<void (QDropEvent *event)> m_dropHandler;
    
};

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    
    Dropzone dropzone;
    dropzone.show();
    
    return app.exec();
}