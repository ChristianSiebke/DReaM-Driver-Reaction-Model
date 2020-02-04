/******************************************************************************
* Copyright (c) 2018 in-tech GmbH
*
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
******************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "OWL/DataTypes.h"

#include "fakeLane.h"
#include "fakeMovingObject.h"

#include "PointAggregator.h"

using ::testing::_;
using ::testing::Return;
using ::testing::SizeIs;
using ::testing::ElementsAre;
using ::testing::Args;
using ::testing::Eq;
using ::testing::Ref;


class RoadNetworkMapper
{
public:
    static void Map(OWL::Interfaces::MovingObject& movingObject,
                    PointAggregatorInterface &pointAggregator)
    {
        for(const OWL::Interfaces::Lane *lane : pointAggregator.GetLanes()) // change to reference
        {
            //TODO: make its own object
            const_cast<OWL::Interfaces::Lane*>(lane)->AddMovingObject(movingObject); //todo: change to reference
            movingObject.AddLaneAssignment(*lane);
        }
    }
};

TEST(RoadNetworkMapper, AggregatedPointsOnSingleLane_CreateOneAssignment)
{
    FakeLane mockLane;
    FakeMovingObject mockMovingObject;

    FakePointAggregator fakePointAggregator;
    std::list<const OWL::Lane*> lanes{&mockLane};

//    ON_CALL(fakePointAggregator, GetLanes()).WillByDefault(Return(lanes));

//    EXPECT_CALL(mockLane, AddMovingObject(Ref(mockMovingObject))).Times(1);
//    EXPECT_CALL(mockMovingObject, AddLaneAssignment(Ref(mockLane))).Times(1);

    RoadNetworkMapper::Map(mockMovingObject, fakePointAggregator);
}

TEST(RoadNetworkMapper, AggregatedPointsOnTwoLanes_CreateTwoAssignments)
{
    FakeLane mockLane1; // todo: move to implementation
    FakeLane mockLane2; // todo: move to implementation
    FakeMovingObject  mockMovingObject; // todo: can i do that?

    FakePointAggregator fakePointAggregator;
    std::list<const OWL::Lane*> lanes{&mockLane1, &mockLane2};

    ON_CALL(fakePointAggregator, GetLanes()).WillByDefault(Return(lanes));

//    EXPECT_CALL(mockLane1, AddMovingObject(Ref(mockMovingObject))).Times(1);
//    EXPECT_CALL(mockLane2, AddMovingObject(Ref(mockMovingObject))).Times(1);

//    EXPECT_CALL(mockMovingObject, AddLaneAssignment(Ref(mockLane1))).Times(1);
//    EXPECT_CALL(mockMovingObject, AddLaneAssignment(Ref(mockLane2))).Times(1);

    RoadNetworkMapper::Map(mockMovingObject, fakePointAggregator);
}
