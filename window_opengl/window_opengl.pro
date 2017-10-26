TEMPLATE = app
TARGET = window_opengl

HEADERS += hellowindow.h
SOURCES += hellowindow.cpp main.cpp

OBJECTS_DIR += .obj
MOC_DIR += .moc

# widgets required by the html5 platform plugin.
QT += widgets
