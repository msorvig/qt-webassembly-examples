project(gui_reload)
cmake_minimum_required(VERSION 3.19)

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)

qt_add_executable(gui_reload
    main.cpp 
)

target_link_directories(gui_reload PUBLIC "/foo")

target_link_libraries(gui_reload PUBLIC
    Qt::Core
    Qt::Gui
)
