add_subdirectory(data)
add_subdirectory(heuristic)
add_subdirectory(io)
add_subdirectory(model)

target_sources(wfc PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/domain.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/id.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/wfc.cc
)
