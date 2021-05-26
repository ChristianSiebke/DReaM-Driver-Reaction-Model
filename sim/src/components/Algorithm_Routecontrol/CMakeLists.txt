set(COMPONENT_NAME Algorithm_RouteControl)

add_compile_definitions(ALGORITHM_ROUTECONTROL_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    algorithm_RouteControl.h
    algorithm_RouteControl_implementation.h
    algorithm_RouteControl_global.h
	routeControl.h

  SOURCES
    algorithm_RouteControl.cpp
    algorithm_RouteControl_implementation.cpp
	routeControl.cpp

  LIBRARIES
    Qt5::Core
)
