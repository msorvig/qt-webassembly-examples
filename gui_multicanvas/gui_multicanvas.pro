TEMPLATE = app
TARGET = multicanvas

HEADERS += \
    hellowindow.h
SOURCES += \
    hellowindow.cpp \
    main.cpp

# copy custom html shell
QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$PWD/multicanvas.html) $$quote($$OUT_PWD) $$escape_expand(\n\t)
