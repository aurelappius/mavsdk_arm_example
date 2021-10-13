cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(arm)

add_executable(arm
    arm.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(arm
    MAVSDK::mavsdk_action
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(arm PRIVATE -Wall -Wextra)
else()
    add_compile_options(arm PRIVATE -WX -W2)
endif()
