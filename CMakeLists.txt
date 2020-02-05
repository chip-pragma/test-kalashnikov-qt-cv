cmake_minimum_required(VERSION 3.15)
project(kalashnikov)

set(CMAKE_CXX_STANDARD 14)


set(CHIP_OPENCV_PATH "" CACHE PATH "Путь к библиотекам OpenCV")
set(CHIP_QT5_PATH "" CACHE PATH "Путь к библиотекам Qt5")


find_package(OpenCV REQUIRED PATHS ${CHIP_OPENCV_PATH})
find_package(Qt5 REQUIRED COMPONENTS Widgets PATHS ${CHIP_QT5_PATH})

set(LINUX_LIB_RUNTIME "rt")

# lib_api
set(lib_api_path src/lib_api)
file(GLOB_RECURSE lib_api_sources "${lib_api_path}/src/**.cpp")

add_library(lib_api STATIC ${lib_api_sources})

target_include_directories(lib_api
        PRIVATE ${lib_api_path}/include/chip
        PUBLIC ${OpenCV_INCLUDE_DIRS} ${lib_api_path}/include/)

target_link_libraries(lib_api
        PUBLIC ${LINUX_LIB_RUNTIME})

# app_cv
set(app_cv_path src/app_cv)
file(GLOB_RECURSE app_cv_sources "${app_cv_path}/**.cpp")

add_executable(app_cv ${app_cv_sources})

target_include_directories(app_cv
        PRIVATE ${app_cv_path})
target_link_libraries(app_cv
        PRIVATE ${OpenCV_LIBS} lib_api)

# app_qt
set(app_qt_path src/app_qt)
file(GLOB_RECURSE app_qt_sources "${app_qt_path}/**.cpp")

add_executable(app_qt ${app_qt_sources})

target_include_directories(app_qt
        PRIVATE ${app_qt_path})
target_link_libraries(app_qt
        PRIVATE lib_api Qt5::Widgets)