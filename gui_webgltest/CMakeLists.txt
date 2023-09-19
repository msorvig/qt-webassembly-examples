cmake_minimum_required(VERSION 3.16)
project(openglwindow LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Gui OpenGL)

qt_standard_project_setup()

qt_add_executable(openglwindow
    main.cpp
    trianglerenderer.cpp trianglerenderer.h
)

target_link_libraries(openglwindow PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
)
