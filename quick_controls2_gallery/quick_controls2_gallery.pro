TEMPLATE = app
TARGET = gallery
QT += quick quickcontrols2

SOURCES += \
    gallery.cpp

RESOURCES += \
    gallery.qml \
    qtquickcontrols2.conf \
    icons/gallery/index.theme \
    $$files(icons/*.png, true) \
    $$files(images/*.png) \
    $$files(pages/*.qml)

target.path = $$[QT_INSTALL_EXAMPLES]/quickcontrols2/gallery
INSTALLS += target
