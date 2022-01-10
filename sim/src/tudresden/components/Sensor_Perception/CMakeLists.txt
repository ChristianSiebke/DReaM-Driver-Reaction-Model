set(COMPONENT_NAME Sensor_Perception)
 
add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module
 
#   HEADERS
#     sensor_perception_global.h
#     sensor_perception_implementation.h
#     sensor_perception_logic.h
#     sensor_perception.h
#     AABBTree/aabb.h
#     AABBTree/aabbInterface.h
#     AABBTree/aabbtree.h
#     AABBTree/ray.h
#     Objects/observeddynamicobject.h
#     Objects/observedstaticobject.h
#     Objects/observedtrafficlight.h
#     Objects/observedtrafficsign.h
#     Objects/observedworldobject.h
#     Sensors/aabbtreehandler.h
#     Sensors/basicvisualsensor.h
#     Sensors/driverperception.h
#     Sensors/roadnetworksensor.h
#     Sensors/simplesensorinterface.h
#     Sensors/trafficsignvisualsensor.h
#     Sensors/visualsensorinterface.h
 
#   SOURCES
#     sensor_perception_implementation.cpp
#     sensor_perception_logic.cpp
#     sensor_perception.cpp
#     AABBTree/aabbtree.cpp
#     Sensors/aabbtreehandler.cpp
#     Sensors/basicvisualsensor.cpp
#     Sensors/driverperception.cpp
#     Sensors/roadnetworksensor.cpp
#     Sensors/trafficsignvisualsensor.cpp
 
  INCDIRS
    ${OPENPASS_SIMCORE_DIR}/core
    ${OPENPASS_SIMCORE_DIR}/common
 
  LIBRARIES
    Qt5::Core
    Common
    TUDresdenCommon
)