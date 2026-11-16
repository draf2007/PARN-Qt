cmake_minimum_required(VERSION 3.21)
project(parn VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets SerialPort)
qt_standard_project_setup()

qt_add_executable(parn
    src/main.cpp
    src/link/seriallink.h
    src/link/seriallink.cpp
)

target_link_libraries(parn PRIVATE Qt6::Widgets Qt6::SerialPort)

set_target_properties(parn PROPERTIES WIN32_EXECUTABLE ON)