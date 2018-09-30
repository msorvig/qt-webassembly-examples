TEMPLATE = app
TARGET = gui_localfiles

OBJECTS_DIR = .obj
MOC_DIR = .moc

include(qhtml5file.pri)

HEADERS += qmui.h
SOURCES += qmui.cpp

SOURCES += main.cpp
    
QT = core gui
