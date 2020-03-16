#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

set(SOURCES AgentAdapter.cpp
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
			RoutePlanning/StochasticNavigation.cpp)

set(HEADERS AgentAdapter.h
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
			 OWL/fakes/fakeLane.h
			OWL/fakes/fakeLaneBoundary.h
			OWL/fakes/fakeLaneManager.h
			OWL/fakes/fakeMovingObject.h
			OWL/fakes/fakeOWLJunction.h
			OWL/fakes/fakeRoad.h
			OWL/fakes/fakeSection.h
			OWL/fakes/fakeTrafficSign.h
			OWL/fakes/fakeWorldData.h
			RoutePlanning/NavigationInterface.h
			RoutePlanning/StochasticNavigation.h)
			

add_definitions(-DWORLD_OSI_LIBRARY -DWORLD_LIBRARY)
include_directories(. ${OSI_INCLUDE_DIR} ${PROTOBUF_INCLUDE_DIRS})

OPENPASS_ADD_MODULE(World_OSI OSI)

target_link_libraries(World_OSI Common  ${OSI_LIBRARIES} ${PROTOBUF_LIBRARIES})
