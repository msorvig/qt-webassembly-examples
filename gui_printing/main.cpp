#include <QtGui>
#include <QPrinter>
#include <emscripten/val.h>
#include <emscripten.h>

// This example demonstrates how to use QPrinter to print to a PDF file
// stored in Emscriptens in-memory file system, and then trigger a file
// download of that file. This is similar to how online document editors
// handles printing.

class PrintWindow : public QRasterWindow
{
public:
    PrintWindow() {

    }

    void paintEvent(QPaintEvent *) override {

        QPainter p(this);
        p.fillRect(QRect(0, 0, 2000, 2000), QColor(255,255,255));
        p.drawText(QPoint(40, 40), "Press P to print");

    }

    void keyPressEvent(QKeyEvent *ev) override {
        if (ev->key() == Qt::Key_P)
            print();
    }

    void print() {

        // 1) Create a QTextDocument with content

        QTextDocument textDocument;
        textDocument.setHtml("This is QTextDocument <strong>html</strong> content");

        // 2) Create PDF printer and print to file

        QString fileName("printfile.pdf");
        QPrinter printer(QPrinter::PrinterResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setPageSize(QPageSize::A4);
        printer.setOutputFileName(fileName);
        textDocument.print(&printer);

        // 3) Download PDF file

        QFile pdfFile(fileName);
        pdfFile.open(QIODevice::ReadOnly);
        QByteArray content = pdfFile.readAll();
        size_t size = content.size();
        pdfFile.remove();

        // Note: It's possible to call QFileDialog::saveFileContent() with the content
        // QByteArray instead of implementing the code below.

        // Make a copy of the file data. The copy is owned by the JavaScript runtime,
        // and will be garbage collected when the download is completed. (Unlike
        // the QByteArray which will go out of scope by the end of this function)
        emscripten::val fileContentView = emscripten::val(emscripten::typed_memory_view(size, content.constData()));
        emscripten::val fileContentCopy = emscripten::val::global("ArrayBuffer").new_(size);
        emscripten::val fileContentCopyView = emscripten::val::global("Uint8Array").new_(fileContentCopy);
        fileContentCopyView.call<void>("set", fileContentView);

        // Create content Blob
        emscripten::val contentArray = emscripten::val::array();
        contentArray.call<void>("push", fileContentCopyView);
        emscripten::val type = emscripten::val::object();
        type.set("size", size);
        type.set("type", "application/pdf");
        emscripten::val contentBlob = emscripten::val::global("Blob").new_(contentArray, type);

        // Create content link
        emscripten::val document = emscripten::val::global("document");
        emscripten::val window = emscripten::val::global("window");
        emscripten::val contentUrl = window["URL"].call<emscripten::val>("createObjectURL", contentBlob);
        emscripten::val contentLink = document.call<emscripten::val>("createElement", std::string("a"));
        contentLink.set("href", contentUrl);
        contentLink.set("style", "display:none");

        // ### There should be some combination of contentLink options
        // which will open the pdf directly in e.g. Preview
        // on macOS instad of downloading the file, but so far
        // those options have not been found.
        contentLink.set("download", "print.pdf");

        // Click link
        emscripten::val body = document["body"];
        body.call<void>("appendChild", contentLink);
        contentLink.call<void>("click");
        body.call<void>("removeChild", contentLink);

        window["URL"].call<void>("revokeObjectURL", contentUrl);
    }
};

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    PrintWindow window;
    window.show();

    return app.exec();
}
