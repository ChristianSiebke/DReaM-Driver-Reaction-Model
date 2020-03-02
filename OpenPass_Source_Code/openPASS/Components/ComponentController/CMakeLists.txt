set(COMPONENT_NAME ComponentController)

add_compile_definitions(COMPONENT_CONTROLLER_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared

  HEADERS
    componentController.h
    componentControllerCommon.h
    componentControllerGlobal.h
    componentControllerImplementation.h
    componentStateInformation.h
    condition.h
    stateManager.h

  SOURCES
    componentController.cpp
    componentControllerImplementation.cpp
    componentStateInformation.cpp
    condition.cpp
    stateManager.cpp

  LIBRARIES
    Qt5::Core
)
