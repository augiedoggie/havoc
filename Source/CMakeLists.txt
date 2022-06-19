set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

add_subdirectory(popt)

include_directories(popt)

# Need HAVE_CONFIG_H for popt config definitions
add_definitions(-DHAVE_CONFIG_H -D_GNU_SOURCE -D_REENTRANT)

haiku_add_executable(mkh main.cpp VolumeControl.cpp MediaKeyHelper.rdef)

target_link_libraries(mkh media popt)

install(TARGETS mkh RUNTIME DESTINATION bin)
