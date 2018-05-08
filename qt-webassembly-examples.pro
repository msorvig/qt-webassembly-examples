TEMPLATE = subdirs

SUBDIRS = \
    gui_opengl \
    gui_raster \
    gui_lifecycle \
    gui_localfiles \
    widgets_wiggly \
    quick_hellosquare \
    quick_clocks \
    quick_controls2_gallery \
    quick_controls2_testbench \


# Add Sensortag if its submodule has been synced
exists($$PWD/SensorTagDemo/SensorTagDemo.pro) {
#    SUBDIRS += SensorTagDemo
}

# Add Slate if its submodule has been synced
exists($$PWD/slate/slate.pro) {
    SUBDIRS += slate
}
