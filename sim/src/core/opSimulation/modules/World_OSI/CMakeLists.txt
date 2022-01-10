################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_NAME World_OSI)

add_compile_definitions(WORLD_OSI_LIBRARY WORLD_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    AgentAdapter.h
    AgentNetwork.h
    GeometryConverter.h
    EntityInfo.h
    EntityInfoPublisher.h
    EntityRepository.h
    JointsBuilder.h
    LaneStream.h
    Localization.h
    LocalizationElement.h
    RamerDouglasPeucker.h
    RoadStream.h
    SceneryConverter.h
    SceneryEntities.h
    TrafficObjectAdapter.h
    TrafficLightNetwork.h
    World.h
    WorldData.h
    WorldDataException.h
    WorldDataQuery.h
    WorldGlobal.h
    WorldEntities.h
    WorldImplementation.h
    WorldObjectAdapter.h
    WorldToRoadCoordinateConverter.h
    OWL/DataTypes.h
    OWL/LaneGeometryElement.h
    OWL/LaneGeometryJoint.h
    OWL/OpenDriveTypeMapper.h
    OWL/Primitives.h
    ../../../../common/RoutePlanning/RouteCalculation.h
    egoAgent.h

  SOURCES
    AgentAdapter.cpp
    AgentNetwork.cpp
    GeometryConverter.cpp
    EntityInfoPublisher.cpp
    EntityRepository.cpp
    JointsBuilder.cpp
    LaneStream.cpp
    Localization.cpp
    RoadStream.cpp
    SceneryConverter.cpp
    TrafficObjectAdapter.cpp
    TrafficLightNetwork.cpp
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
    ../../../../common

  LIBRARIES
    Qt5::Core
    Boost::headers
    Common
    CoreCommon

  LINKOSI
)
