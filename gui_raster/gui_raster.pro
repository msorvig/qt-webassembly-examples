TEMPLATE = app
TARGET = gui_raster

DEPENDPATH += .

OBJECTS_DIR = .obj
MOC_DIR = .moc

SOURCES += \
    main.cpp \
    rasterwindow.cpp
HEADERS += \
    rasterwindow.h
    
QT = core gui
