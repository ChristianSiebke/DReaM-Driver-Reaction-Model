set(COMPONENT_NAME EventDetector)

add_compile_definitions(EVENT_DETECTOR_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module

  HEADERS
    CollisionDetector.h
    ConditionalEventDetector.h
    EventDetectorCommonBase.h
    EventDetectorExport.h
    EventDetectorGlobal.h
    Conditions/ConditionCommonBase.h
    Conditions/ReachPositionCondition.h
    Conditions/SimulationTimeCondition.h

  SOURCES
    CollisionDetector.cpp
    ConditionalEventDetector.cpp
    EventDetectorCommonBase.cpp
    EventDetectorExport.cpp

  LIBRARIES
    Qt5::Core
    Common
)
