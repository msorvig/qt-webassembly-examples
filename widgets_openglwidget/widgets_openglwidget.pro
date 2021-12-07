QT += widgets openglwidgets

SOURCES += main.cpp \
           glwidget.cpp \
           mainwindow.cpp \
           bubble.cpp

HEADERS += glwidget.h \
           mainwindow.h \
           bubble.h

RESOURCES += texture.qrc
QMAKE_LFLAGS += -g


