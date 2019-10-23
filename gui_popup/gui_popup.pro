TEMPLATE = app
TARGET = gui_popup

OBJECTS_DIR = .obj
MOC_DIR = .moc

HEADERS += qmui.h
SOURCES += qmui.cpp
SOURCES += main.cpp
    
QT = core gui

QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$PWD/popup.html) $$quote($$OUT_PWD) $$escape_expand(\n\t)