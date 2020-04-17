set(COMPONENT_NAME World_OSI)

add_compile_definitions(WORLD_OSI_LIBRARY WORLD_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module

  HEADERS
    AgentAdapter.h
    AgentNetwork.h
    GeometryConverter.h
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
    RoutePlanning/NavigationInterface.h
    RoutePlanning/StochasticNavigation.h

  SOURCES
    AgentAdapter.cpp
    AgentNetwork.cpp
    GeometryConverter.cpp
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
    RoutePlanning/StochasticNavigation.cpp

  INCDIRS
    ${CMAKE_CURRENT_LIST_DIR}

  LIBRARIES
    Qt5::Core
    Boost::headers
    Common

  LINKOSI
)
