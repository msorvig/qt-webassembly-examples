#include "trianglerenderer.h"

#include <QApplication>
#include <QtWidgets>

int main(int argc, char *argv[])
{
    QSurfaceFormat format;
    format.setOption(QSurfaceFormat::DebugContext);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(24);
    format.setSamples(4);  //enabled MSAA
    format.setStencilBufferSize(8);

    // This example requires OpenGL ES 3 (shaders are hardcoded for that version)
    // TODO: maybe support ES 2  as well.
    format.setVersion(3, 0);

    QSurfaceFormat::setDefaultFormat(format);
    QApplication a(argc, argv);

    // Test cases (enable one)

#if 0
    // QOpenGLWindow subclass
    TriangleWindow w1;
    w1.show();
#endif

#if 1
    // QOpenGLWidget subclass
    TriangleWidget w1;
    w1.show();
#endif
    
#if 0
    // QOpenGLWidget child widget
    QMainWindow mainwindow;
    mainwindow.addToolBar(new QToolBar());
    mainwindow.setCentralWidget(new TriangleWidget());
    mainwindow.show();
#endif
    return a.exec();
}
