project(qt-webassembly-examples)
cmake_minimum_required(VERSION 3.19)

add_subdirectory(core_minimal)
add_subdirectory(gui_raster)
add_subdirectory(gui_opengl)

if(QT_FEATURE_widgets)
    add_subdirectory(widgets_textedit)
    add_subdirectory(widgets_wiggly)
    add_subdirectory(widgets_multimedia)
    add_subdirectory(widgets_openglcontextversion)
endif()
