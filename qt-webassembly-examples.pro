TEMPLATE = subdirs

SUBDIRS = \
    gui_opengl \
    gui_raster \
    widgets_wiggly \
    quick_clocks \


# Add Sensortag if its submodule has been synced
exists($$PWD/SensorTagDemo/SensorTagDemo.pro) {
    SUBDIRS += SensorTagDemo
}
