project(qt-webassembly-examples)
cmake_minimum_required(VERSION 3.19)

add_subdirectory(gui_raster)
add_subdirectory(gui_opengl)
if(QT_FEATURE_widgets)
    add_subdirectory(widgets_wiggly)
endif()
