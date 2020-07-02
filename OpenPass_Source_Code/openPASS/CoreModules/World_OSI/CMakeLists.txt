set(COMPONENT_NAME World_OSI)

add_compile_definitions(WORLD_OSI_LIBRARY WORLD_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module

  HEADERS
    AgentAdapter.h
    AgentNetwork.h
    GeometryConverter.h
    JointsBuilder.h
    Localization.h
    LocalizationElement.h
    PointQuery.h
    SceneryConverter.h
    TrafficObjectAdapter.h
    World.h
    WorldData.h
    WorldDataException.h
    WorldDataQuery.h
    WorldGlobal.h
    WorldImplementation.h
    WorldObjectAdapter.h
    WorldToRoadCoordinateConverter.h
    OWL/DataTypes.h
    OWL/LaneGeometryElement.h
    OWL/LaneGeometryJoint.h
    OWL/OpenDriveTypeMapper.h
    OWL/Primitives.h
    RoutePlanning/RouteCalculation.h
    egoAgent.h

  SOURCES
    AgentAdapter.cpp
    AgentNetwork.cpp
    GeometryConverter.cpp
    JointsBuilder.cpp
    Localization.cpp
    PointQuery.cpp
    SceneryConverter.cpp
    TrafficObjectAdapter.cpp
    World.cpp
    WorldData.cpp
    WorldDataException.cpp
    WorldDataQuery.cpp
    WorldImplementation.cpp
    WorldObjectAdapter.cpp
    WorldToRoadCoordinateConverter.cpp
    OWL/DataTypes.cpp
    OWL/OpenDriveTypeMapper.cpp
    egoAgent.cpp

  INCDIRS
    ${CMAKE_CURRENT_LIST_DIR}

  LIBRARIES
    Qt5::Core
    Boost::headers
    Common
    CoreShare

  LINKOSI
)
