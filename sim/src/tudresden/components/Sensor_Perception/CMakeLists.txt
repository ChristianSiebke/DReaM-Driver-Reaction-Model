set(COMPONENT_NAME Sensor_Perception)

add_compile_definitions(SENSOR_PERCEPTION_LIBRARY)
 
add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module
 
  HEADERS
    # MAIN
    sensor_perception_global.h
    sensor_perception.h
    sensor_perception_implementation.h
    sensor_perception_logic.h
    # AABBTREE
    AABBTree/aabb.h
    AABBTree/aabbInterface.h
    AABBTree/aabbtree.h
    AABBTree/ray.h
    # OBJECTS
    Objects/observeddynamicobject.h
    Objects/observedstaticobject.h
    Objects/observedtrafficlight.h
    Objects/observedtrafficsign.h
    Objects/observedworldobject.h
    # SENSORS
    Sensors/aabbtreehandler.h
    Sensors/basicvisualsensor.h
    Sensors/driverperception.h
    Sensors/roadnetworksensor.h
    Sensors/simplesensorinterface.h
    Sensors/trafficsignvisualsensor.h
    Sensors/visualsensorinterface.h
    Sensors/stoppingpointcalculation.h
    Sensors/agentperceptioninterface.h
    ../../../core/opSimulation/modules/World_OSI/OWL/DataTypes.h
 
  SOURCES
    # MAIN
    sensor_perception.cpp
    sensor_perception_implementation.cpp
    sensor_perception_logic.cpp
    # AABBTREE
     AABBTree/aabbtree.cpp
    # SENSORS
    Sensors/aabbtreehandler.cpp
    Sensors/basicvisualsensor.cpp
    Sensors/driverperception.cpp
    Sensors/roadnetworksensor.cpp
    Sensors/trafficsignvisualsensor.cpp
    Sensors/stoppingpointcalculation.cpp
 
  INCDIRS
    ${OPENPASS_SIMCORE_DIR}/core
    ${OPENPASS_SIMCORE_DIR}/common
    .
    ../../../core/opSimulation/modules/World_OSI
 
  LIBRARIES
    Qt5::Core
    Common
    TUDresdenCommon
    World_OSI

  LINKOSI
)