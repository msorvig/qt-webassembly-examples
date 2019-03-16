QT = core gui
HEADERS += hellowindow.h
SOURCES += \
    hellowindow.cpp \
    main.cpp

# copy custom html shell and ventus window manager
QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$PWD/gui_multicanvas.html) $$quote($$OUT_PWD) $$escape_expand(\n\t)
QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$PWD/simple.css) $$quote($$OUT_PWD) $$escape_expand(\n\t)
QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$PWD/ventus.css) $$quote($$OUT_PWD) $$escape_expand(\n\t)
QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$PWD/ventus.mod.js) $$quote($$OUT_PWD) $$escape_expand(\n\t)
