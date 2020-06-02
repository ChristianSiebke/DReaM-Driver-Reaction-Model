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
    RemoveAgentsManipulator.h

  SOURCES
    CollisionManipulator.cpp
    ComponentStateChangeManipulator.cpp
    LaneChangeManipulator.cpp
    ManipulatorCommonBase.cpp
    ManipulatorExport.cpp
    RemoveAgentsManipulator.cpp

  LIBRARIES
    Qt5::Core
)
