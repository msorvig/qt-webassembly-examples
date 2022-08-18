project(widgets_wiggly)
cmake_minimum_required(VERSION 3.19)

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)

qt_add_executable(widgets_wiggly
    main.cpp
    wigglywidget.cpp wigglywidget.h
    dialog.cpp dialog.h
)

target_link_libraries(widgets_wiggly PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
)
