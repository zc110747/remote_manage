cmake_minimum_required(VERSION 3.16)

project(main_process)

# add subdirectory
add_subdirectory(mb)
add_subdirectory(../common ${CMAKE_CURRENT_BINARY_DIR}/common)

# add executable
add_executable(main_process)

# add compiler
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER arm-none-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER arm-none-linux-gnueabihf-g++)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# add source
file(GLOB GENERATED_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/module/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/mqtt/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/remote/*.cpp")

target_sources(main_process PRIVATE 
        ${GENERATED_SOURCES}
        ${LIB_SOURCES}
        ${COMMON_LIB_SOURCES})

target_include_directories(main_process PRIVATE 
        ${INCLUDE_HEADERS}
        ${COMMON_INCLUDE_HEADERS}
        "${CMAKE_CURRENT_SOURCE_DIR}/module/"
        "${CMAKE_CURRENT_SOURCE_DIR}/mqtt/"
        "${CMAKE_CURRENT_SOURCE_DIR}/remote/"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/"
        "$ENV{ENV_PATH_ROOT}/thirdparts/asio/include/"
        "$ENV{ENV_PATH_ROOT}/thirdparts/jsoncpp/include/"
        "$ENV{ENV_PATH_ROOT}/thirdparts/mosquitto/include/"
        "$ENV{ENV_PATH_ROOT}/thirdparts/fmt/include/")

# 添加链接库
target_link_libraries(main_process PRIVATE pthread m mosquitto mosquittopp ssl crypto jsoncpp)

target_link_directories(main_process PRIVATE "$ENV{SUPPORT_ENV_INSTALL}/lib")
