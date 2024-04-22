cmake_minimum_required(VERSION 3.20)
project(wfc)

include(cmake/options.cmake)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -fno-omit-frame-pointer")

add_library(wfc)
wfc_set_defaults(wfc)

add_executable(wfc_app)
wfc_set_defaults(wfc_app)

add_executable(wfc_unit_tests)
wfc_set_defaults(wfc_unit_tests)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    wfc_set_sanitiser_options(wfc)
    wfc_set_sanitiser_options(wfc_app)
    wfc_set_sanitiser_options(wfc_unit_tests)
endif()

include_directories(include)
include_directories(external/lodepng)

add_subdirectory(app)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(external/lodepng)
add_subdirectory(external/catch2 EXCLUDE_FROM_ALL)
add_subdirectory(external/SDL EXCLUDE_FROM_ALL)

target_link_libraries(wfc PRIVATE lodepng)
target_link_libraries(wfc_app PRIVATE wfc SDL3::SDL3)
target_link_libraries(wfc_unit_tests PRIVATE wfc Catch2WithMain)
