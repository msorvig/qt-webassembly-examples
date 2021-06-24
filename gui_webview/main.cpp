#include <QtGui>
#include <emscripten/val.h>

//
// QViebView-like web view for Qt for WebAssembly. Applications
// can display html content by overlaying an iframe on top of
// Qt app content.
//
// In this example, the web view is an iframe which is
// controlled by by a QWindow subclass. This is specific
// to the example, any html element can be used, and the
// controller can be any component which can provide geometry
// for the element.
//
class WebViewControllerWindow : public QRasterWindow
{
public:
    WebViewControllerWindow() {
        createIframe();
        updateIframeGeometry();
    }

    void resizeEvent(QResizeEvent *ev) override {
        updateIframeGeometry();
    }
    void moveEvent(QMoveEvent *ev) override {
        updateIframeGeometry();
    }

    void createIframe() {

        // Add a new html <iframe> element as a child of the pages's <body> element
        emscripten::val document = emscripten::val::global("document");
        emscripten::val body = document["body"];
        m_iframe = document.call<emscripten::val>("createElement", std::string("iframe"));
        body.call<void>("appendChild", m_iframe);

        // Set frame source. I've had mixed results with displaying
        // content from other domains; maybe there's a way to enable
        // it but it looks like it might be blocked on modern browsers.
        m_iframe.set("src", "iframe.html");

        emscripten::val style = m_iframe["style"];
        style.set("border", "none");
        style.set("background-color", "White");

        // Set position:absolute, which makes it possible to position the iframe
        // using x,y. coordinates, relative to its parent (the page's <body> element)
        style.set("position", "absolute");

        updateIframeGeometry();
    }

    void updateIframeGeometry() {

        // This functions set geometry on the iframe such that it covers the
        // window content area.

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
        QPoint canvasPoition(rect["left"].as<int>(), rect["top"].as<int>());

        QRect iframeGeometry(windowGeometry.topLeft() + canvasPoition, windowGeometry.size());

        // Apply geometry to the iframe
        emscripten::val style = m_iframe["style"];
        style.set("left", QString("%1px").arg(iframeGeometry.left()).toStdString());
        style.set("top", QString("%1px").arg(iframeGeometry.top()).toStdString());
        style.set("width", QString("%1px").arg(iframeGeometry.width()).toStdString());
        style.set("height", QString("%1px").arg(iframeGeometry.height()).toStdString());
    }

private:
    emscripten::val m_iframe = emscripten::val::undefined();
};

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    WebViewControllerWindow window;
    window.setGeometry(QRect(40, 40, 640, 480));
    window.showNormal();

    return app.exec();
}
