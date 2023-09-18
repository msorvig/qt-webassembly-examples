project(helloplugin)
cmake_minimum_required(VERSION 3.19)

find_package(Qt6 COMPONENTS Core)

qt_standard_project_setup()

# app
qt_add_executable(hellomain
    main.cpp
)
target_link_libraries(hellomain PUBLIC
    Qt::Core
)

# 'hola' plugin
qt_add_plugin(holaplugin SHARED
    CLASS_NAME HolaPlugin
    main.cpp
)

target_link_libraries(holaplugin PUBLIC
    Qt::Core
)
target_compile_definitions(holaplugin PRIVATE BUILD_HOLA_PLUGIN)

# 'hello' plugin
qt_add_plugin(helloplugin SHARED
    CLASS_NAME HelloPlugin
    main.cpp
)
target_link_libraries(helloplugin PUBLIC
    Qt::Core
)
target_compile_definitions(helloplugin PRIVATE BUILD_HELLO_PLUGIN)

