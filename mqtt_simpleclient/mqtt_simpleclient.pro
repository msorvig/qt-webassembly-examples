QT += core gui network widgets websockets mqtt

TARGET = mqtt_simpleclient
TEMPLATE = app

SOURCES +=\
    main.cpp\
    mainwindow.cpp\
    websocketiodevice.cpp

HEADERS +=\
    mainwindow.h\
    websocketiodevice.h

FORMS    += mainwindow.ui
