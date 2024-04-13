cmake_minimum_required(VERSION 3.10)
project(wfc)

include(cmake/options.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -fno-omit-frame-pointer")

add_library(wfc)
set_default_options(wfc)

add_executable(wfc_app)
set_default_options(wfc_app)

add_executable(wfc_unit_tests)
set_default_options(wfc_unit_tests)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_sanitiser_options(wfc)
    set_sanitiser_options(wfc_app)
    set_sanitiser_options(wfc_unit_tests)
endif()

include_directories(include)
include_directories(external/lodepng)

add_subdirectory(app)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(external/lodepng)
add_subdirectory(external/Catch2 EXCLUDE_FROM_ALL)
add_subdirectory(external/SDL EXCLUDE_FROM_ALL)

target_link_libraries(wfc PRIVATE lodepng)
target_link_libraries(wfc_app PRIVATE wfc SDL3::SDL3)
target_link_libraries(wfc_unit_tests PRIVATE wfc Catch2WithMain)
