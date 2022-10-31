# Building: set QT_BUILD_PATH to point to qtbase while running cmake, where qtbase/lib contains libQt6Core.so:
#    QT_BUILD_PATH=/path/to/qtbase path/to/qt-cmake .
#
project(qtguiloader)
cmake_minimum_required(VERSION 3.23)

# Set paths to Qt build
set(QT_BUILD_PATH $ENV{QT_BUILD_PATH})
include_directories("${QT_BUILD_PATH}/include")
include_directories("${QT_BUILD_PATH}/include/QtCore")
include_directories("${QT_BUILD_PATH}/include/QtGui")
link_directories("${QT_BUILD_PATH}/lib")

# Build app using Emscripten's default html loader
add_executable(qtguiloader main.cpp)
set_target_properties(qtguiloader PROPERTIES OUTPUT_NAME qtguiloader SUFFIX .html)
set_target_properties(qtguiloader PROPERTIES COMPILE_FLAGS "-g -std=c++17 -sMAIN_MODULE")
set_target_properties(qtguiloader PROPERTIES LINK_FLAGS "-g -std=c++17 -sMAIN_MODULE --bind -sWASM_BIGINT")
target_link_libraries(qtguiloader Qt6Core)
target_link_libraries(qtguiloader Qt6Gui)
target_link_libraries(qtguiloader Qt6OpenGL)

# Create symlinks to QtCore and QtGui next to qtguiloader.wasm
add_custom_command(
    TARGET qtguiloader POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${QT_BUILD_PATH}/lib/libQt6Core.so
            ${CMAKE_CURRENT_BINARY_DIR}/libQt6Core.so)
add_custom_command(
    TARGET qtguiloader POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${QT_BUILD_PATH}/lib/libQt6Gui.so
            ${CMAKE_CURRENT_BINARY_DIR}/libQt6Gui.so)
add_custom_command(
    TARGET qtguiloader POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${QT_BUILD_PATH}/lib/libQt6Opengl.so
            ${CMAKE_CURRENT_BINARY_DIR}/libQt6Opengl.so)
