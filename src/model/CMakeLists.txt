add_subdirectory(overlap)

target_sources(wfc PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/image.cc
)
