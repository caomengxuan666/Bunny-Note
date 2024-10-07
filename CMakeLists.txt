cmake_minimum_required(VERSION 3.27)
project(BunnyNote)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "D:/QT_6/6.8.0/msvc2022_64")
# 开启O2优化
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

find_package(Qt6 COMPONENTS Core Widgets Gui WebEngineWidgets REQUIRED)

include_directories(include)

# 添加 cmark 的库路径
set(CMARK_LIB_DIR lib)
link_directories(${CMARK_LIB_DIR})

# 找到并链接库
find_library(CMARK_LIB cmark PATHS ${CMARK_LIB_DIR} REQUIRED)


add_executable(BunnyNote WIN32
        src/main.cpp
        src/mainwindow.h
        src/mainwindow.cpp
        src/settings.cpp
        src/settings.h
        src/HtmlConverter.hpp
        src/res.qrc
)


# 链接 Qt 和 cmark 库
target_link_libraries(BunnyNote
        Qt::Core
        Qt::Widgets
        Qt::Gui
        Qt::WebEngineWidgets
        ${CMARK_LIB}  # 链接 cmark 静态库
)
