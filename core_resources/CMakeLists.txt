project(core_resources)
cmake_minimum_required(VERSION 3.19)
find_package(Qt6 COMPONENTS Core)

qt_add_executable(core_minimal
    main.cpp
)

qt_add_resources(core_minimal "core-minimal"
    PREFIX "/core_minimal"
    FILES core-minimal.dat)

target_link_libraries(core_minimal PUBLIC
    Qt::Core
)
