cmake_minimum_required(VERSION 3.16)

project(UdpClientGUI VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    Client.cpp
    Client.h
    Server.cpp
    Server.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(UdpClientGUI
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(UdpClientGUI ${PROJECT_SOURCES})
endif()

# Link
target_link_libraries(UdpClientGUI PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ws2_32)

# Properties
set_target_properties(UdpClientGUI PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(UdpClientGUI)
endif()
