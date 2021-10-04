project(widgets_wiggly)
cmake_minimum_required(VERSION 3.19)

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Widgets)

qt_add_executable(widgets_textedit
    main.cpp
)

target_link_libraries(widgets_textedit PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
)
