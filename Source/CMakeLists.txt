
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

haiku_add_executable(${PROJECT_NAME} MixerApp.cpp VolumeControl.cpp AppResources.rdef)

target_link_libraries(${PROJECT_NAME} be media popt)

if(HAIKU_ENABLE_I18N)
	set("${PROJECT_NAME}-APP_MIME_SIG" "x-vnd.cpr.VolumeControl")
	set("${PROJECT_NAME}-LOCALES" "en")
	target_link_libraries(${PROJECT_NAME} "localestub")
	haiku_add_i18n(${PROJECT_NAME})
endif()

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
