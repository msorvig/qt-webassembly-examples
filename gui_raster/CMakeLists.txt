project(gui_raster)
cmake_minimum_required(VERSION 3.19)

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)

qt_add_executable(gui_raster
    main.cpp rasterwindow.cpp
)

target_link_libraries(gui_raster PUBLIC
    Qt::Core
    Qt::Gui
)
