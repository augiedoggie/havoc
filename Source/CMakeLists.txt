set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

add_subdirectory(popt)

include_directories(popt)

# Need HAVE_CONFIG_H for popt config definitions
add_definitions(-DHAVE_CONFIG_H -D_GNU_SOURCE -D_REENTRANT)

haiku_add_executable(${PROJECT_NAME} main.cpp VolumeControl.cpp VolumeApp.rdef)

target_link_libraries(${PROJECT_NAME} media popt)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
