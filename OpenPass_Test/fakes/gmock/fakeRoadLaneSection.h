/*********************************************************************
 * Copyright (c) 2020 in-tech
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 **********************************************************************/

#pragma once

#include "Interfaces/roadInterface/roadLaneSectionInterface.h"
#include "gmock/gmock.h"

class FakeRoadLaneSection : public RoadLaneSectionInterface
{
  public:
    MOCK_METHOD2(AddRoadLane, RoadLaneInterface *(int id, RoadLaneType type));
    MOCK_CONST_METHOD0(GetLanes, std::map<int, RoadLaneInterface *> &());
    MOCK_CONST_METHOD0(GetStart, double());
    MOCK_METHOD1(SetInDirection, void(bool inDirection));
    MOCK_METHOD1(SetLaneIndexOffset, void(int laneIndexOffset));
    MOCK_METHOD1(SetId, void(int Id));
    MOCK_CONST_METHOD0(GetInDirection, bool());
    MOCK_CONST_METHOD0(GetLaneIndexOffset, int());
    MOCK_CONST_METHOD0(GetId, int());
    MOCK_CONST_METHOD0(GetRoad, RoadInterface *());
};
