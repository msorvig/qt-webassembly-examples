project(widgets_openglcontextversion)
cmake_minimum_required(VERSION 3.19)

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS OpenGL)
find_package(Qt6 COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)

qt_add_executable(widgets_openglcontextversion
    main.cpp
)

target_link_libraries(widgets_openglcontextversion PUBLIC
    Qt::Core
    Qt::Gui
    Qt::OpenGL
    Qt::Widgets
)
