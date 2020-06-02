set(COMPONENT_NAME Manipulator)

add_compile_definitions(MANIPULATOR_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module

  HEADERS
    CollisionManipulator.h
    ComponentStateChangeManipulator.h
    LaneChangeManipulator.h
    ManipulatorCommonBase.h
    ManipulatorGlobal.h
    ManipulatorExport.h
    NoOperationManipulator.h
    RemoveAgentsManipulator.h
    TrajectoryManipulator.h

  SOURCES
    CollisionManipulator.cpp
    ComponentStateChangeManipulator.cpp
    LaneChangeManipulator.cpp
    ManipulatorCommonBase.cpp
    ManipulatorExport.cpp
    NoOperationManipulator.cpp
    RemoveAgentsManipulator.cpp
    TrajectoryManipulator.cpp

  LIBRARIES
    Qt5::Core
)
