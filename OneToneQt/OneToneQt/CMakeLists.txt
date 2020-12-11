cmake_minimum_required(VERSION 3.1.0)

project(helloworld VERSION 1.0.0 LANGUAGES CXX)


set ( CMAKE_CXX_FLAGS " -fpermissive -I ../ ")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 REQUIRED COMPONENTS Widgets Core PrintSupport )


add_executable(main
    OneToneQt.ui
    main.cpp
    OneToneQt.cpp
    OneToneQt.qrc
)
add_library(qcustomplot qcustomplot.cpp)
add_library(PythonThread PythonThread.cpp)

target_link_libraries(main PythonThread)
target_link_libraries(main qcustomplot)
target_link_libraries(main Qt5::Core)
target_link_libraries(main Qt5::Widgets)

target_link_libraries(PythonThread Qt5::Widgets)
target_link_libraries(qcustomplot Qt5::Widgets)
target_link_libraries(qcustomplot Qt5::Core)
target_link_libraries(qcustomplot Qt5::PrintSupport)


