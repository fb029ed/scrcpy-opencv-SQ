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
find_package(OpenCV 4.2 REQUIRED)

include_directories(code/server)
include_directories(code/stream)
include_directories(code/recipe)
#qt头文件路径
include_directories(${QT_INCLUDES})
include_directories(${Qt5Widgets_INCLUDE_DIRS})

#生成server库
AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/code/server DIR_SRCS)
add_library(server  ${DIR_SRCS})
target_link_libraries(server
    PRIVATE Qt5::Widgets
    pthread
)

#生成stream库
AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/code/stream DIR_SRCS)
add_library(stream  ${DIR_SRCS})
target_link_libraries(stream
    ${OpenCV_LIBS}
    avcodec   
    avformat 
    avutil
    swscale
)

#生成recipe库
AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/code/recipe DIR_SRCS)
add_library(recipe  ${DIR_SRCS})
target_link_libraries(recipe
    ${OpenCV_LIBS}
)

add_executable(scrcpy-gamepro
  code/main.cpp
)

target_link_libraries(scrcpy-gamepro
    recipe
    server
    stream
    ${OpenCV_LIBS}
)