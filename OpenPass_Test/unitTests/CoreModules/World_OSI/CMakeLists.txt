set(COMPONENT_TEST_NAME WorldOSI_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/CoreModules/World_OSI)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT core
  DEFAULT_MAIN
  LINKOSI

  SOURCES
    agentAdapter_Tests.cpp
    datatypes_Tests.cpp
    egoAgent_Tests.cpp
    fakeLaneManager_Tests.cpp
    geometryConverter_Tests.cpp
    lane_Tests.cpp
    locator_Tests.cpp
#    objectLocator_Tests.cpp
    pointQuery_Tests.cpp
#    roadNetworkMapper_Tests.cpp
    sceneryConverter_Tests.cpp
    sensorView_Tests.cpp
    worldQuery_Tests.cpp
    worldToRoadCoordinateConverter_Tests.cpp
    Generators/laneGeometryElementGenerator_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/AgentAdapter.cpp
    ${COMPONENT_SOURCE_DIR}/AgentNetwork.cpp
    ${COMPONENT_SOURCE_DIR}/egoAgent.cpp
    ${COMPONENT_SOURCE_DIR}/GeometryConverter.cpp
    ${COMPONENT_SOURCE_DIR}/JointsBuilder.cpp
    ${COMPONENT_SOURCE_DIR}/Localization.cpp
    ${COMPONENT_SOURCE_DIR}/OWL/DataTypes.cpp
    ${COMPONENT_SOURCE_DIR}/OWL/OpenDriveTypeMapper.cpp
    ${COMPONENT_SOURCE_DIR}/PointQuery.cpp
    ${COMPONENT_SOURCE_DIR}/SceneryConverter.cpp
    ${COMPONENT_SOURCE_DIR}/TrafficObjectAdapter.cpp
    ${COMPONENT_SOURCE_DIR}/WorldData.cpp
    ${COMPONENT_SOURCE_DIR}/WorldDataException.cpp
    ${COMPONENT_SOURCE_DIR}/WorldDataQuery.cpp
    ${COMPONENT_SOURCE_DIR}/WorldImplementation.cpp
    ${COMPONENT_SOURCE_DIR}/WorldObjectAdapter.cpp
    ${COMPONENT_SOURCE_DIR}/WorldToRoadCoordinateConverter.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/AgentAdapter.h
    ${COMPONENT_SOURCE_DIR}/AgentNetwork.h
    ${COMPONENT_SOURCE_DIR}/egoAgent.h
    ${COMPONENT_SOURCE_DIR}/GeometryConverter.h
    ${COMPONENT_SOURCE_DIR}/JointsBuilder.h
    ${COMPONENT_SOURCE_DIR}/Localization.h
    ${COMPONENT_SOURCE_DIR}/OWL/DataTypes.h
    ${COMPONENT_SOURCE_DIR}/OWL/OpenDriveTypeMapper.h
    ${COMPONENT_SOURCE_DIR}/PointQuery.h
    ${COMPONENT_SOURCE_DIR}/RoutePlanning/RouteCalculation.h
    ${COMPONENT_SOURCE_DIR}/SceneryConverter.h
    ${COMPONENT_SOURCE_DIR}/TrafficObjectAdapter.h
    ${COMPONENT_SOURCE_DIR}/WorldData.h
    ${COMPONENT_SOURCE_DIR}/WorldDataException.h
    ${COMPONENT_SOURCE_DIR}/WorldDataQuery.h
    ${COMPONENT_SOURCE_DIR}/WorldImplementation.h
    ${COMPONENT_SOURCE_DIR}/WorldObjectAdapter.h
    ${COMPONENT_SOURCE_DIR}/WorldToRoadCoordinateConverter.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}
    ${COMPONENT_SOURCE_DIR}/OWL
    ${COMPONENT_SOURCE_DIR}/OWL/fakes

  LIBRARIES
    Qt5::Core
    Common
    CoreShare
)

