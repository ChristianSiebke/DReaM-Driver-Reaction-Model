set(COMPONENT_NAME Manipulator)

add_compile_definitions(MANIPULATOR_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module

  HEADERS
    CollisionManipulator.h
    ComponentStateChangeManipulator.h
    LaneChangeManipulator.h
    ManipulatorCommonBase.h
    ManipulatorExport.h
    ManipulatorGlobal.h
    NoOperationManipulator.h
    RemoveAgentsManipulator.h
    SpeedActionManipulator.h
    TrajectoryManipulator.h
    ../../../../common/vector2d.h
    srcCollisionPostCrash/polygon.h
    srcCollisionPostCrash/collisionDetection_Impact_implementation.h
    ../../../../core/common/coreDataPublisher.h

  SOURCES
    CollisionManipulator.cpp
    ComponentStateChangeManipulator.cpp
    LaneChangeManipulator.cpp
    ManipulatorCommonBase.cpp
    ManipulatorExport.cpp
    NoOperationManipulator.cpp
    RemoveAgentsManipulator.cpp
    SpeedActionManipulator.cpp
    TrajectoryManipulator.cpp
    srcCollisionPostCrash/polygon.cpp
    srcCollisionPostCrash/collisionDetection_Impact_implementation.cpp

  LIBRARIES
    Qt5::Core
    CoreCommon

  INCDIRS
    ../../../../common
    srcCollisionPostCrash
    core/common
)
