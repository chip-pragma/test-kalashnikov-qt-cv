cmake_minimum_required(VERSION 3.15)
project(kalashnikov)

set(CMAKE_CXX_STANDARD 14)


set(CHIP_OPENCV_PATH "" CACHE PATH "Путь к библиотекам OpenCV")
set(CHIP_QT5_PATH "" CACHE PATH "Путь к библиотекам Qt5")


find_package(OpenCV REQUIRED PATHS ${CHIP_OPENCV_PATH})
find_package(Qt5 REQUIRED COMPONENTS Core Widgets PATHS ${CHIP_QT5_PATH})

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

# app_cv
set(app_cv_client_path src/app_cv_client)
file(GLOB_RECURSE app_cv_client_sources "${app_cv_client_path}/**.cpp")

add_executable(app_cv_client ${app_cv_client_sources})

target_include_directories(app_cv_client
        PRIVATE ${app_cv_client_path})
target_link_libraries(app_cv_client
        PRIVATE lib_api)