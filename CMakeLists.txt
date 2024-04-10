cmake_minimum_required(VERSION 3.10)
project(wfc)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -fno-omit-frame-pointer")

add_library(wfc)
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

add_executable(wfc_app)
target_compile_options(wfc_app PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    # -fsanitize=address,undefined,float-divide-by-zero,unsigned-integer-overflow,implicit-conversion,local-bounds
)
target_link_options(wfc_app PRIVATE
    # -fsanitize=address,undefined,float-divide-by-zero,unsigned-integer-overflow,implicit-conversion,local-bounds
)

add_executable(wfc_unit_tests)
target_compile_options(wfc_unit_tests PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    # -fsanitize=address,undefined,float-divide-by-zero,unsigned-integer-overflow,implicit-conversion,local-bounds
)
target_link_options(wfc_unit_tests PRIVATE
    # -fsanitize=address,undefined,float-divide-by-zero,unsigned-integer-overflow,implicit-conversion,local-bounds
)

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
