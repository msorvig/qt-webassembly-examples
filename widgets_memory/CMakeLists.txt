project(widgets_memory)
cmake_minimum_required(VERSION 3.19)

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)

qt_add_executable(widgets_memory
    main.cpp
)

target_link_libraries(widgets_memory PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
)
