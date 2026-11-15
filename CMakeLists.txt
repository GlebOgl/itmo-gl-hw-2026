cmake_minimum_required(VERSION 3.16)
project(itmo-gl-hw-2026 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

if(QT_VERSION_MAJOR EQUAL 6)
    find_package(Qt6 REQUIRED COMPONENTS OpenGLWidgets)
endif()

set(PROJECT_SOURCES
        main.cpp
        window.cpp
        window.h
        textures/cat.jpg
)

add_executable(itmo-gl-hw-2026
    ${PROJECT_SOURCES}
    resources.qrc
    shaders/template.fs
    shaders/template.vs
    openglwidget.h openglwidget.cpp
)

set(QT_LIBS Qt${QT_VERSION_MAJOR}::Widgets)

if(QT_VERSION_MAJOR EQUAL 6)
    list(APPEND QT_LIBS Qt6::OpenGLWidgets)
else()
    find_package(Qt5 REQUIRED COMPONENTS OpenGL)
    list(APPEND QT_LIBS Qt5::OpenGL)
endif()

target_link_libraries(itmo-gl-hw-2026 PRIVATE ${QT_LIBS})
