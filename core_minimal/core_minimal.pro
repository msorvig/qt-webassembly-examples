TEMPLATE = app
DEPENDPATH += .

OBJECTS_DIR = .obj
MOC_DIR = .moc

SOURCES += main.cpp
QT = core

# Enable non-blocking main() where returning from it does not exit the process. 
QMAKE_LFLAGS += -s EXIT_RUNTIME=0

# copy custom index.html
QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$PWD/index.html) $$quote($$OUT_PWD) $$escape_expand(\n\t)
