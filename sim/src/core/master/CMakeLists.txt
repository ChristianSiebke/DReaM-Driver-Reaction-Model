set(COMPONENT_NAME OpenPassMaster)

set(CMAKE_AUTOMOC ON)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE executable COMPONENT bin

  HEADERS
    framework/processManager.h
    framework/masterConfig.h
    framework/slaveConfig.h
    importer/masterConfigImporter.h
    openPassMaster.h

  SOURCES
    framework/main.cpp
    framework/processManager.cpp
    importer/masterConfigImporter.cpp

  INCDIRS
    framework
    importer
    ../../../..

  LIBRARIES
    Qt5::Concurrent
    Qt5::Core
    Qt5::Widgets
    Qt5::Xml
    CoreCommon
)

set_target_properties(${COMPONENT_NAME} PROPERTIES FOLDER "core")
