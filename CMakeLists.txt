cmake_minimum_required(VERSION 3.15)
project(kalashnikov)

set(CMAKE_CXX_STANDARD 14)


set(CHIP_OPENCV_PATH "" CACHE PATH "Путь к библиотекам OpenCV")
set(CHIP_QT5_PATH "" CACHE PATH "Путь к библиотекам Qt5")


find_package(OpenCV REQUIRED PATHS ${CHIP_OPENCV_PATH})
find_package(Qt5 REQUIRED COMPONENTS Core Widgets PATHS ${CHIP_QT5_PATH})


set(app_cv_path src/app_cv)
add_executable(app_cv
        ${app_cv_path}/main.cpp
        ${app_cv_path}/CmdLineHelper.cpp)
target_include_directories(app_cv
        PRIVATE ${app_cv_path} ${OpenCV_INCLUDE_DIRS})
target_link_libraries(app_cv PRIVATE ${OpenCV_LIBS})