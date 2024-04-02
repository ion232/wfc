add_subdirectory(assignment)
add_subdirectory(variable)

target_sources(wfc PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/weights.cc
)
