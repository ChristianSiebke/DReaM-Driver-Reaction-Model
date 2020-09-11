set(COMPONENT_NAME ComponentController)

add_compile_definitions(COMPONENT_CONTROLLER_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    componentController.h
    src/componentControllerCommon.h
    src/componentControllerImpl.h
    src/componentStateInformation.h
    src/condition.h
    src/stateManager.h

  SOURCES
    componentController.cpp
    src/componentControllerImpl.cpp
    src/componentStateInformation.cpp
    src/condition.cpp
    src/stateManager.cpp

  LIBRARIES
    Qt5::Core
)
