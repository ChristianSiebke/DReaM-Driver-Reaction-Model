set(COMPONENT_NAME Manipulator)

add_compile_definitions(MANIPULATOR_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module

  HEADERS
    CollisionManipulator.h
    ComponentStateChangeManipulator.h
    CustomLaneChangeManipulator.h
    GazeFollowerManipulator.h
    LaneChangeManipulator.h
    ManipulatorCommonBase.h
    ManipulatorExport.h
    ManipulatorGlobal.h
    NoOperationManipulator.h
    RemoveAgentsManipulator.h
    SpeedActionManipulator.h
    TrajectoryManipulator.h

  SOURCES
    CollisionManipulator.cpp
    ComponentStateChangeManipulator.cpp
    CustomLaneChangeManipulator.cpp
    GazeFollowerManipulator.cpp
    LaneChangeManipulator.cpp
    ManipulatorCommonBase.cpp
    ManipulatorExport.cpp
    NoOperationManipulator.cpp
    RemoveAgentsManipulator.cpp
    SpeedActionManipulator.cpp
    TrajectoryManipulator.cpp

  LIBRARIES
    Qt5::Core
)
