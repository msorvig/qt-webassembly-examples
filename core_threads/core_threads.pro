TEMPLATE = app
DEPENDPATH += .

OBJECTS_DIR = .obj
MOC_DIR = .moc

SOURCES += main.cpp
QT = core

# Applications may override the default thread pool size:
QMAKE_LFLAGS += -s PTHREAD_POOL_SIZE=16
