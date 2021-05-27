/*******************************************************************************
 * Copyright (c) 2021, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include "gmock/gmock.h"

#include "include/streamInterface.h"

class FakeLaneStream : public LaneStreamInterface
{
public:
    MOCK_CONST_METHOD1(GetStreamPosition, StreamPosition (const GlobalRoadPosition& roadPosition));
    MOCK_CONST_METHOD1(GetRoadPosition, GlobalRoadPosition (const StreamPosition& streamPosition));
    MOCK_CONST_METHOD2(GetAgentsInRange, std::vector<const AgentInterface*> (const StreamPosition& start, const StreamPosition& end));
    MOCK_CONST_METHOD2(GetObjectsInRange, std::vector<const WorldObjectInterface*> (const StreamPosition& start, const StreamPosition& end));
    MOCK_CONST_METHOD2(GetStreamPosition, std::optional<StreamPosition> (const WorldObjectInterface* object, const MeasurementPoint& mp));
    MOCK_CONST_METHOD0(GetLength, double ());
    MOCK_CONST_METHOD0(GetLaneTypes, std::vector<std::pair<double, LaneType>> ());
};


class FakeRoadStream : public RoadStreamInterface
{
public:
    MOCK_CONST_METHOD1(GetStreamPosition, StreamPosition (const GlobalRoadPosition& roadPosition));
    MOCK_CONST_METHOD1(GetRoadPosition, GlobalRoadPosition (const StreamPosition& streamPosition));
    MOCK_CONST_METHOD0(GetAllLaneStreams, std::vector<std::unique_ptr<LaneStreamInterface>> ());
    MOCK_CONST_METHOD2(GetLaneStream, std::unique_ptr<LaneStreamInterface> (const StreamPosition& startPosition, const int laneId));
    MOCK_CONST_METHOD0(GetLength, double ());
};
