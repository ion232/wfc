cmake_minimum_required(VERSION 3.10)
project(wfc)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -fno-omit-frame-pointer")

add_executable(wfc)
target_compile_options(wfc PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    # -fsanitize=address,undefined,float-divide-by-zero,unsigned-integer-overflow,implicit-conversion,local-bounds
)
target_link_options(wfc PRIVATE
    # -fsanitize=address,undefined,float-divide-by-zero,unsigned-integer-overflow,implicit-conversion,local-bounds
)

include_directories(include)
include_directories(external/lodepng)

add_subdirectory(src)
add_subdirectory(external/lodepng)
add_subdirectory(external/SDL EXCLUDE_FROM_ALL)

target_link_libraries(wfc PRIVATE SDL3::SDL3 lodepng)
