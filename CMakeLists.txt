cmake_minimum_required(VERSION 3.10)
project(wfc)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(wfc)

include_directories(include)

add_subdirectory(src)
add_subdirectory(external/SDL EXCLUDE_FROM_ALL)

target_link_libraries(wfc PRIVATE SDL3::SDL3)
