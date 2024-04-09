add_subdirectory(overlap)

target_sources(wfc PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/image_factory.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/image.cc
)
