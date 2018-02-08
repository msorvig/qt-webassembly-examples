TEMPLATE = app
TARGET = gui_lifecycle

DEPENDPATH += .

OBJECTS_DIR = .obj
MOC_DIR = .moc

SOURCES += main.cpp
    
QT = core gui

# copy custom loader gui_lifecycle.html to build dir
QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$PWD/gui_lifecycle.html) $$quote($$OUT_PWD) $$escape_expand(\n\t)
