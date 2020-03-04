TEMPLATE = app
DEPENDPATH += .

OBJECTS_DIR = .obj
MOC_DIR = .moc

SOURCES += main.cpp
QT = core
CONFIG += debug
CONFIG -= release

DEFINES += HAVE_THREADS

QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$PWD/core_eventloop.html) $$quote($$OUT_PWD) $$escape_expand(\n\t)
LIBS += -s DEMANGLE_SUPPORT=1 -s NO_EXIT_RUNTIME=1 -s ASSERTIONS=2 -s STACK_OVERFLOW_CHECK=2 -s SAFE_HEAP=1
