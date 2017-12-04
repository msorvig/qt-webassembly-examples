#include <QtGui>

#include "rasterwindow.h"

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    
    RasterWindow window;
    window.showFullScreen();
    
    return app.exec();
}
