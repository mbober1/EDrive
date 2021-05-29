cmake_minimum_required(VERSION 3.5)

project(EDrive LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS uic)
set(AUTORCC_EXECUTABLE resources/resources.qrc)

find_package(Qt5 COMPONENTS Widgets Charts Mqtt REQUIRED)



set(SOURCES
    source/main.cpp
    source/mainwindow.cpp
    source/chart.cpp
    source/mqtt.cpp
    source/connectiondialog.cpp
    source/engine.cpp
)

set(HEADERS
    include/mainwindow.hpp
    include/chart.hpp
    include/mqtt.hpp
    include/secrets.hpp
    include/connectiondialog.hpp
    include/engine.hpp
)

set(UIS
    uic/mainwindow.ui
    uic/connectiondialog.ui
)

qt5_add_resources(SOURCES resources/resources.qrc)
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${UIS})
target_include_directories(${PROJECT_NAME} PUBLIC include ui)

target_link_libraries(EDrive 
Qt5::Widgets 
Qt5::Charts 
Qt5::Mqtt 
)
