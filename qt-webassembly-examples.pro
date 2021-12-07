TEMPLATE = subdirs

SUBDIRS = \
    core_minimal \
    core_network \
    core_eventloop \
    gui_opengl \
    gui_raster \
    gui_lifecycle \
    gui_localfiles \
    gui_multicanvas \
    gui_popup \
    gui_settings \
    widgets_scroll \
    widgets_openglwidget \
    widgets_wiggly \
    widgets_textedit \
    widgets_quick \
    widgets_standarddialogs \
    quick_hellosquare \
    quick_clocks \
    quick_controls2_gallery \
    quick_controls2_testbench \

qtConfig(thread): {
    SUBDIRS += \
        core_threads \
        gui_qimagereader \
        widgets_mandelbrot \
}        

qtHaveModule(mqtt):qtHaveModule(websockets): SUBDIRS += mqtt_simpleclient

#qtHaveModule(quick3d):SUBDIRS += quick3d_hellocube

# Add submodule examples if they have been synced
exists($$PWD/SensorTagDemo/SensorTagDemo.pro) : SUBDIRS += SensorTagDemo
#exists($$PWD/slate/slate.pro) : SUBDIRS += slate
#exists($$PWD/colordebugger/colordebugger.pro) : SUBDIRS += colordebugger
