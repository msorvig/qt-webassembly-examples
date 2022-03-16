
#include <QtWidgets>
#include <emscripten/val.h>

class VideoController : public QWidget
{
public:
    VideoController()
    {
        createVideoElement();
        createVideoControlUi();
        updateVideoElementGeometry();
    }

    void createVideoControlUi()
    {
        QHBoxLayout *layout = new QHBoxLayout();

        QPushButton *playPauseButton = new QPushButton("Play/pause");
        connect(playPauseButton, &QPushButton::clicked, [this](){

            qDebug() << "Play/pause";

            // Toggle play state
            if (m_video["paused"].as<bool>()) {
                m_video.call<void>("play");
            } else {
                m_video.call<void>("pause");
            }

        });
        layout->addWidget(playPauseButton);

        QCheckBox *nativeControls = new QCheckBox("Native Controls");
        connect(nativeControls, &QCheckBox::stateChanged, [this](int i) {

            qDebug() << "Toggle native controls" << i;

            if (i > 0) 
                m_video.call<void>("setAttribute", std::string("controls"), std::string(""));
            else
                m_video.call<void>("removeAttribute", std::string("controls"));

        });
        layout->addWidget(nativeControls);
        layout->addStretch();

        QVBoxLayout *vlayout = new QVBoxLayout();
        vlayout->addLayout(layout);
        vlayout->addStretch();
        setLayout(vlayout);
    }

    void createVideoElement()
    {
        // Create <video> element and add it to the page body
        emscripten::val document = emscripten::val::global("document");
        emscripten::val body = document["body"];
        m_video = document.call<emscripten::val>("createElement", std::string("video"));
        body.call<void>("appendChild", m_video);
        
        // Create/add video source
        videoElementGeometry = document.call<emscripten::val>("createElement", std::string("source"));
        videoElementGeometry.set("src", m_source.toStdString());
        m_video.call<void>("appendChild", videoElementGeometry);
        
        // Set position:absolute, which makes it possible to position the video element
        // using x,y. coordinates, relative to its parent (the page's <body> element)
        emscripten::val style = m_video["style"];
        style.set("position", "absolute");
    }
    
    void updateVideoElementGeometry()
    {

        QRect windowGeometry = geometry();

        // Resolve the geometry against Qt canvas geometry, in case the canvas
        // is offset (not at (0,0)) relative to the body element. The id for the
        // canvas is set in the .html file, in this case "qtcanvas" when using the
        // default generated html.
        emscripten::val document = emscripten::val::global("document");
        emscripten::val canvas = document.call<emscripten::val>("getElementById", std::string("qtcanvas"));
        if (canvas.isUndefined())
            qFatal("Could not find canvas element");
        emscripten::val rect = canvas.call<emscripten::val>("getBoundingClientRect");
        QPoint canvasPosition(rect["left"].as<int>(), rect["top"].as<int>());

        QRect videoElementGeometry(windowGeometry.topLeft() + canvasPosition, windowGeometry.size());
        videoElementGeometry.adjust(40, 60, -40, -40); // add margins

        emscripten::val style = m_video["style"];
        style.set("left", QString("%1px").arg(videoElementGeometry.left()).toStdString());
        style.set("top", QString("%1px").arg(videoElementGeometry.top()).toStdString());
        style.set("width", QString("%1px").arg(videoElementGeometry.width()).toStdString());
        style.set("height", QString("%1px").arg(videoElementGeometry.height()).toStdString());
    }

    void resizeEvent(QResizeEvent *) override {
        updateVideoElementGeometry();
    }
    void moveEvent(QMoveEvent *) override {
        updateVideoElementGeometry();
    }

private:

    // Test video, from e.g. https://test-videos.co.uk/bigbuckbunny/mp4-h264
    // Download and place in the build directory.
    QString m_source = "Big_Buck_Bunny_1080_10s_1MB.mp4";
    emscripten::val m_video = emscripten::val::undefined();
    emscripten::val videoElementGeometry = emscripten::val::undefined();
};

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    VideoController controller;
    controller.show();

    return app.exec();
}
