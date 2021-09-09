project(gui_opengl)
cmake_minimum_required(VERSION 3.19)

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)

qt_add_executable(gui_opengl
    main.cpp hellowindow.cpp
)

target_link_libraries(gui_opengl PUBLIC
    Qt::Core
    Qt::Gui
)
