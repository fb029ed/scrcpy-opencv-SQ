cmake_minimum_required(VERSION 3.5)
project(scrcpy-gamepro)
add_compile_options(-std=c++11)
add_compile_options(-fPIC)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

include_directories(code/server)
#qt头文件路径
include_directories(${QT_INCLUDES})
include_directories(${Qt5Widgets_INCLUDE_DIRS})

#生成server库
AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/code/server DIR_SRCS)
add_library(server  ${DIR_SRCS})
target_link_libraries(server
    PRIVATE Qt5::Widgets
)

add_executable(scrcpy-gamepro
  code/main.cpp
)

target_link_libraries(scrcpy-gamepro
    server
)