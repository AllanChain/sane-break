include_directories(${CMAKE_CURRENT_SOURCE_DIR})

configure_file(config.h.cmake config.h)

add_subdirectory(lib)
add_subdirectory(gui)
add_subdirectory(app)
