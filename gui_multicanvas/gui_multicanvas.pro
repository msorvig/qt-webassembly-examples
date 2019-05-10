TEMPLATE = app
TARGET = multicanvas

HEADERS += \
    hellowindow.h
SOURCES += \
    hellowindow.cpp \
    main.cpp

HEADERS += rasterwindow.h
SOURCES += rasterwindow.cpp

# optional widgets testing
# QT += widgets
# DEFINES += HAVE_WIDGETS

# copy custom html shell
QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$PWD/multicanvas.html) $$quote($$OUT_PWD) $$escape_expand(\n\t)
QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$PWD/qwm.js) $$quote($$OUT_PWD) $$escape_expand(\n\t)
QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$PWD/qwebui.js) $$quote($$OUT_PWD) $$escape_expand(\n\t)
QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$PWD/qrafdebug.js) $$quote($$OUT_PWD) $$escape_expand(\n\t)


