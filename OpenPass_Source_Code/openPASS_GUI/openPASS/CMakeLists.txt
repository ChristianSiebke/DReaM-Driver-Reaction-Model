set(COMPONENT_NAME openPASSInterfaces)

add_compile_definitions(ACTION_BRAKELIGHT_BASIC_LIBRARY)

include_directories(.)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared LINKGUI COMPONENT bin

  HEADERS
    Interfaces/openPASS/PluginInterface.h 
    Interfaces/openPASS/PluginManagerInterface.h
    Interfaces/openPASS/RandomInterface.h
    Interfaces/openPASS/ioplugininterface.h
    Interfaces/openPASS/ServiceInterface.h
)
set_target_properties(${COMPONENT_NAME} PROPERTIES LINKER_LANGUAGE CXX)


set(COMPONENT_NAME openPASS)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE executable LINKGUI COMPONENT bin

  HEADERS
    Models/PluginManagerModel.h
    Models/ServiceManagerModel.h

  SOURCES
    openPASS.cpp
	openPASS.rc
    Models/PluginManagerModel.cpp
    Models/ServiceManagerModel.cpp

  LIBRARIES
    CoreShare
	openPASSInterfaces
)
install(DIRECTORY  ../openPASS-System/components/ DESTINATION ${SUBDIR_XML_COMPONENTS} FILES_MATCHING PATTERN "*.xml")

if(OPENPASS_ADJUST_OUTPUT)
add_custom_command(TARGET openPASS POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/OpenPass_Source_Code/openPASS_GUI/openPASS-System/components $<TARGET_FILE_DIR:openPASS>/../${SUBDIR_XML_COMPONENTS})
endif()

