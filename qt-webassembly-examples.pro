TEMPLATE = subdirs

SUBDIRS = \
    core_network \
    gui_opengl \
    gui_raster \
    gui_lifecycle \
    gui_localfiles \
    widgets_wiggly \
    quick_hellosquare \
    quick_clocks \
    quick_controls2_gallery \
    quick_controls2_testbench \

qtHaveModule(mqtt):qtHaveModule(websockets): SUBDIRS += mqtt_simpleclient

# Add submodule examples if they have been synced
exists($$PWD/SensorTagDemo/SensorTagDemo.pro) : SUBDIRS += SensorTagDemo
exists($$PWD/slate/slate.pro) : SUBDIRS += slate
exists($$PWD/colordebugger/colordebugger.pro) : SUBDIRS += colordebugger
