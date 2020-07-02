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

#include "Interfaces/roadInterface/roadLaneInterface.h"
#include "gmock/gmock.h"

class FakeRoadLane : public RoadLaneInterface
{
  public:
    MOCK_METHOD5(AddWidth, bool (double sOffset, double a, double b, double c, double d));
    MOCK_METHOD5(AddBorder, bool (double sOffset, double a, double b, double c, double d));
    MOCK_METHOD6(AddRoadMark, bool (double sOffset,
                                    RoadLaneRoadDescriptionType descType,
                                    RoadLaneRoadMarkType roadMark,
                                    RoadLaneRoadMarkColor color,
                                    RoadLaneRoadMarkLaneChange laneChange,
                                    RoadLaneRoadMarkWeight weight));
    MOCK_METHOD1(AddSuccessor, bool (int id));
    MOCK_METHOD1(AddPredecessor, bool (int id));
    MOCK_CONST_METHOD0(GetId, int ());
    MOCK_CONST_METHOD0(GetType, RoadLaneType ());
    MOCK_CONST_METHOD0(GetWidths, const std::list<RoadLaneWidth*> &());
    MOCK_CONST_METHOD0(GetBorders, const std::list<RoadLaneWidth*> &());
    MOCK_CONST_METHOD0(GetSuccessor, const std::list<int> &());
    MOCK_CONST_METHOD0(GetPredecessor, const std::list<int> &());
    MOCK_METHOD1(SetInDirection, void (bool inDirection));
    MOCK_CONST_METHOD0(GetInDirection, bool ());
    MOCK_CONST_METHOD0(GetLaneSection, RoadLaneSectionInterface *());
    MOCK_CONST_METHOD0(GetRoadMarks, const std::list<RoadLaneRoadMark*> &());
};
