add_subdirectory(data)
add_subdirectory(heuristic)
add_subdirectory(image)
add_subdirectory(io)
add_subdirectory(model)

target_sources(wfc PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/variable.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/wfc.cc
)
