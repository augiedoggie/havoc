
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

haiku_add_executable(${PROJECT_NAME} MixerApp.cpp VolumeControl.cpp AppResources.rdef)

target_link_libraries(${PROJECT_NAME} be media popt)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
