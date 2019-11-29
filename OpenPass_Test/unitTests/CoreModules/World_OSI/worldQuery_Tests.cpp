/*******************************************************************************
* Copyright (c) 2018, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "agentManager_Testhelper.h"
#include "AgentAdapter.h"
#include "WorldDataQuery.h"
#include "Common/globalDefinitions.h"

#include "fakeWorld.h"
#include "fakeSection.h"
#include "fakeLane.h"
#include "fakeLaneBoundary.h"
#include "fakeWorldData.h"
#include "fakeLaneManager.h"
#include "fakeMovingObject.h"
#include "fakeAgent.h"
#include "fakeTrafficSign.h"
#include "fakeOWLJunction.h"
#include "dontCare.h"

#include "Generators/laneGeometryElementGenerator.h"

using namespace OWL;

using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::IsNull;
using ::testing::_;
using ::testing::Eq;
using ::testing::Ne;
using ::testing::DoubleEq;
using ::testing::Ge;
using ::testing::Le;
using ::testing::Lt;
using ::testing::AllOf;
using ::testing::ElementsAre;
using ::testing::ElementsAreArray;
using ::testing::UnorderedElementsAre;
using ::testing::IsEmpty;
using ::testing::SizeIs;
using ::testing::Const;
using ::testing::DontCare;
using ::testing::NiceMock;

class FakeLaneStream
{
public:
    FakeLaneStream()
    {
        ON_CALL(road, GetId()).WillByDefault(Return(100));
    }

    ~FakeLaneStream()
    {
        for (auto lane : lanes)
        {
            delete lane.element;
        }
    }

    LaneStream Get()
    {
        return LaneStream{lanes};
    }

    std::unordered_map<Id, std::string> GetRoadIdMapping()
    {
        return { {100, "Road"} };
    }

    Fakes::Lane* AddLane(double length, bool inStreamDirection)
    {
        LaneStreamInfo laneInfo;
        Fakes::Lane* lane = new Fakes::Lane;
        ON_CALL(*lane, GetLength()).WillByDefault(Return(length));
        ON_CALL(*lane, GetRoad()).WillByDefault(ReturnRef(road));
        laneInfo.element= lane;
        laneInfo.sOffset = accumulatedLength + (inStreamDirection ? 0 : length);
        laneInfo.inStreamDirection = inStreamDirection;
        lanes.push_back(laneInfo);
        accumulatedLength += length;
        return lane;
    }

private:
    double accumulatedLength = 0;
    std::vector<LaneStreamInfo> lanes;
    Fakes::Road road;
};

class FakeRoadStream
{
public:
    FakeRoadStream()
    {
    }

    ~FakeRoadStream()
    {
        for (auto road : roads)
        {
            delete road.element;
        }
    }

    RoadStream Get()
    {
        return RoadStream{roads};
    }

    Fakes::Road* AddRoad(double length, bool inStreamDirection)
    {
        RoadStreamInfo roadInfo;
        Fakes::Road* road = new Fakes::Road;
        ON_CALL(*road, GetLength()).WillByDefault(Return(length));
        roadInfo.element= road;
        roadInfo.sOffset = accumulatedLength + (inStreamDirection ? 0 : length);
        roadInfo.inStreamDirection = inStreamDirection;
        roads.push_back(roadInfo);
        accumulatedLength += length;
        return road;
    }

private:
    double accumulatedLength = 0;
    std::vector<RoadStreamInfo> roads;
};

TEST(GetDistanceToEndOfLane, EndWithinSightDistance_ReturnsRemainingDistance)
{
    FakeLaneStream laneStream;
    auto lane = laneStream.AddLane(100.0, true);
    ON_CALL(*lane, GetLaneType()).WillByDefault(Return(LaneType::Driving));

    Fakes::WorldData worldData;

    WorldDataQuery wdQuery(worldData);
    auto result = wdQuery.GetDistanceToEndOfLane(laneStream.Get(), 5.0, OWL::EVENTHORIZON, {LaneType::Driving});

    ASSERT_THAT(result, Eq(95.0));
}

TEST(GetDistanceToEndOfLane, LaneShorterThanInitialSearchDistance_ReturnsZero)
{
    FakeLaneStream laneStream;
    auto lane = laneStream.AddLane(100.0, true);
    ON_CALL(*lane, GetLaneType()).WillByDefault(Return(LaneType::Driving));

    Fakes::WorldData worldData;

    WorldDataQuery wdQuery(worldData);
    auto result = wdQuery.GetDistanceToEndOfLane(laneStream.Get(), 1000.0, OWL::EVENTHORIZON, {LaneType::Driving});

    ASSERT_THAT(result, Eq(0.0));
}

TEST(GetDistanceToEndOfLane, SearchDistanceInBetweenLaneStream_ReturnsRemainingDistance)
{
    FakeLaneStream laneStream;
    auto lane1 = laneStream.AddLane(100.0, true);
    ON_CALL(*lane1, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    auto lane2 = laneStream.AddLane(100.0, true);
    ON_CALL(*lane2, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    auto lane3 = laneStream.AddLane(100.0, true);
    ON_CALL(*lane3, GetLaneType()).WillByDefault(Return(LaneType::Driving));

    Fakes::WorldData worldData;

    WorldDataQuery wdQuery(worldData);
    auto result = wdQuery.GetDistanceToEndOfLane(laneStream.Get(), 150.1, OWL::EVENTHORIZON, {LaneType::Driving});

    ASSERT_THAT(result, Eq(149.9));
}

TEST(GetDistanceToEndOfLane, LaneLongerThanMaxSearchLength_ReturnsInfinite)
{
    FakeLaneStream laneStream;
    auto lane = laneStream.AddLane(1000.0, true);
    ON_CALL(*lane, GetLaneType()).WillByDefault(Return(LaneType::Driving));

    Fakes::WorldData worldData;

    WorldDataQuery wdQuery(worldData);
    auto result = wdQuery.GetDistanceToEndOfLane(laneStream.Get(), 0.0, 100.0, {LaneType::Driving});

    ASSERT_THAT(result, Eq(INFINITY));
}

TEST(GetDistanceToEndOfLane, OneLaneHasWrongType)
{
    FakeLaneStream laneStream;
    auto lane1 = laneStream.AddLane(100.0, true);
    ON_CALL(*lane1, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    auto lane2 = laneStream.AddLane(100.0, true);
    ON_CALL(*lane2, GetLaneType()).WillByDefault(Return(LaneType::Exit));
    auto lane3 = laneStream.AddLane(100.0, true);
    ON_CALL(*lane3, GetLaneType()).WillByDefault(Return(LaneType::Driving));

    Fakes::WorldData worldData;

    WorldDataQuery wdQuery(worldData);
    auto result = wdQuery.GetDistanceToEndOfLane(laneStream.Get(), 50.1, OWL::EVENTHORIZON, {LaneType::Driving});

    ASSERT_THAT(result, Eq(49.9));
}

///////////////////////////////////////////////////////////////////////////////

TEST(GetNextObjectInLane, NoObjects_ReturnsDefault)
{
    FakeLaneStream laneStream;
    auto lane = laneStream.AddLane(100.0, true);
    OWL::Interfaces::WorldObjects emptyObjectsList{};
    ON_CALL(*lane, GetWorldObjects()).WillByDefault(ReturnRef(emptyObjectsList));

    Fakes::WorldData worldData;
    const auto& roadIdMapping = laneStream.GetRoadIdMapping();
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetNextObjectInLane<OWL::Interfaces::MovingObject>(laneStream.Get(), 0, 1000);
    ASSERT_THAT(result, IsNull());
}

TEST(GetNextObjectInLane, TwoObjectsOnFirstLane_ReturnsCloserObject)
{
    FakeLaneStream laneStream;
    auto lane1 = laneStream.AddLane(200.0, true);
    auto lane2 = laneStream.AddLane(200.0, true);

    Fakes::MovingObject closeObject;
    Fakes::MovingObject distantObject;
    ON_CALL(closeObject, GetDistance(_,_)).WillByDefault(Return(10));
    ON_CALL(distantObject, GetDistance(_,_)).WillByDefault(Return(100));
    OWL::Interfaces::WorldObjects objectsLane1{ {&closeObject, &distantObject} };
    ON_CALL(*lane1, GetWorldObjects()).WillByDefault(ReturnRef(objectsLane1));
    OWL::Interfaces::WorldObjects emptyObjectsList{};
    ON_CALL(*lane2, GetWorldObjects()).WillByDefault(ReturnRef(emptyObjectsList));

    Fakes::WorldData worldData;
    const auto& roadIdMapping = laneStream.GetRoadIdMapping();
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    WorldDataQuery wdQuery(worldData);
    auto result = wdQuery.GetNextObjectInLane<OWL::Interfaces::MovingObject>(laneStream.Get(), 0, 1000);
    ASSERT_THAT(result, Eq(&closeObject));
}

TEST(GetNextObjectInLane, TwoObjectsOnLaneStream_ReturnsCloserObject)
{
    FakeLaneStream laneStream;
    auto lane1 = laneStream.AddLane(200.0, true);
    auto lane2 = laneStream.AddLane(200.0, true);

    Fakes::MovingObject closeObject;
    Fakes::MovingObject distantObject;
    ON_CALL(closeObject, GetDistance(_,_)).WillByDefault(Return(10));
    ON_CALL(distantObject, GetDistance(_,_)).WillByDefault(Return(100));
    OWL::Interfaces::WorldObjects objectsLane1{ {&closeObject} };
    ON_CALL(*lane1, GetWorldObjects()).WillByDefault(ReturnRef(objectsLane1));
    OWL::Interfaces::WorldObjects objectsLane2{ {&distantObject} };
    ON_CALL(*lane2, GetWorldObjects()).WillByDefault(ReturnRef(objectsLane2));

    Fakes::WorldData worldData;
    const auto& roadIdMapping = laneStream.GetRoadIdMapping();
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    WorldDataQuery wdQuery(worldData);
    auto result = wdQuery.GetNextObjectInLane<OWL::Interfaces::MovingObject>(laneStream.Get(), 0, 1000);
    ASSERT_THAT(result, Eq(&closeObject));
}

TEST(GetNextObjectInLane, TwoObjectsOnSecondLane_ReturnsCloserObject)
{
    FakeLaneStream laneStream;
    auto lane1 = laneStream.AddLane(200.0, true);
    auto lane2 = laneStream.AddLane(200.0, true);

    Fakes::MovingObject closeObject;
    Fakes::MovingObject distantObject;
    ON_CALL(closeObject, GetDistance(_,_)).WillByDefault(Return(10));
    ON_CALL(distantObject, GetDistance(_,_)).WillByDefault(Return(100));
    OWL::Interfaces::WorldObjects emptyObjectsList{};
    ON_CALL(*lane1, GetWorldObjects()).WillByDefault(ReturnRef(emptyObjectsList));
    OWL::Interfaces::WorldObjects objectsLane2{ {&closeObject, &distantObject} };
    ON_CALL(*lane2, GetWorldObjects()).WillByDefault(ReturnRef(objectsLane2));

    Fakes::WorldData worldData;
    const auto& roadIdMapping = laneStream.GetRoadIdMapping();
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    WorldDataQuery wdQuery(worldData);
    auto result = wdQuery.GetNextObjectInLane<OWL::Interfaces::MovingObject>(laneStream.Get(), 0, 1000);
    ASSERT_THAT(result, Eq(&closeObject));
}

TEST(GetNextObjectInLane, ObjectsOutOfSearchDistance_ReturnsDefault)
{
    FakeLaneStream laneStream;
    auto lane = laneStream.AddLane(100.0, true);
    Fakes::MovingObject objectTooFarAway;
    ON_CALL(objectTooFarAway, GetDistance(_,_)).WillByDefault(Return(150));
    Fakes::MovingObject objectTooNear;
    ON_CALL(objectTooNear, GetDistance(_,_)).WillByDefault(Return(10));
    OWL::Interfaces::WorldObjects objects{ {&objectTooNear, &objectTooFarAway} };
    ON_CALL(*lane, GetWorldObjects()).WillByDefault(ReturnRef(objects));

    Fakes::WorldData worldData;
    const auto& roadIdMapping = laneStream.GetRoadIdMapping();
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetNextObjectInLane<OWL::Interfaces::MovingObject>(laneStream.Get(),20, 100);
    ASSERT_THAT(result, IsNull());
}

TEST(GetNextObjectInLane, ObjectsOnRangeBorder_ReturnsDefault)
{
    FakeLaneStream laneStream;
    auto lane = laneStream.AddLane(100.0, true);
    Fakes::MovingObject objectTooFarAway;
    ON_CALL(objectTooFarAway, GetDistance(_,_)).WillByDefault(Return(120));
    Fakes::MovingObject objectTooNear;
    ON_CALL(objectTooNear, GetDistance(_,_)).WillByDefault(Return(20));
    OWL::Interfaces::WorldObjects objects{ {&objectTooNear, &objectTooFarAway} };
    ON_CALL(*lane, GetWorldObjects()).WillByDefault(ReturnRef(objects));

    Fakes::WorldData worldData;
    const auto& roadIdMapping = laneStream.GetRoadIdMapping();
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetNextObjectInLane<OWL::Interfaces::MovingObject>(laneStream.Get(),20, 100);
    ASSERT_THAT(result, IsNull());
}

TEST(GetNextObjectInLane, TwoObjectsOnLaneInReverseDirection_ReturnsCloserObject)
{
    FakeLaneStream laneStream;
    auto lane1 = laneStream.AddLane(200.0, true);
    auto lane2 = laneStream.AddLane(200.0, false);

    Fakes::MovingObject closeObject;
    Fakes::MovingObject distantObject;
    ON_CALL(closeObject, GetDistance(_,_)).WillByDefault(Return(100));
    ON_CALL(distantObject, GetDistance(_,_)).WillByDefault(Return(10));
    OWL::Interfaces::WorldObjects emptyObjectsList{};
    ON_CALL(*lane1, GetWorldObjects()).WillByDefault(ReturnRef(emptyObjectsList));
    OWL::Interfaces::WorldObjects objectsLane2{ {&closeObject, &distantObject} };
    ON_CALL(*lane2, GetWorldObjects()).WillByDefault(ReturnRef(objectsLane2));

    Fakes::WorldData worldData;
    const auto& roadIdMapping = laneStream.GetRoadIdMapping();
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    WorldDataQuery wdQuery(worldData);
    auto result = wdQuery.GetNextObjectInLane<OWL::Interfaces::MovingObject>(laneStream.Get(), 0, 1000);
    ASSERT_THAT(result, Eq(&closeObject));
}
///////////////////////////////////////////////////////////////////////////////

TEST(GetLastObjectInLane, NoObjects_ReturnsDefault)
{
    FakeLaneStream laneStream;
    auto lane = laneStream.AddLane(100.0, true);
    OWL::Interfaces::WorldObjects emptyObjectsList{};
    ON_CALL(*lane, GetWorldObjects()).WillByDefault(ReturnRef(emptyObjectsList));

    Fakes::WorldData worldData;
    const auto& roadIdMapping = laneStream.GetRoadIdMapping();
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetLastObjectInLane<OWL::Interfaces::MovingObject>(laneStream.Get(), 0, 1000);
    ASSERT_THAT(result, IsNull());
}

TEST(GetLastObjectInLane, TwoObjectsOnFirstLane_ReturnsDistantObject)
{
    FakeLaneStream laneStream;
    auto lane1 = laneStream.AddLane(200.0, true);
    auto lane2 = laneStream.AddLane(200.0, true);

    Fakes::MovingObject closeObject;
    Fakes::MovingObject distantObject;
    ON_CALL(closeObject, GetDistance(_,_)).WillByDefault(Return(10));
    ON_CALL(distantObject, GetDistance(_,_)).WillByDefault(Return(100));
    OWL::Interfaces::WorldObjects objectsLane1{ {&closeObject, &distantObject} };
    ON_CALL(*lane1, GetWorldObjects()).WillByDefault(ReturnRef(objectsLane1));
    OWL::Interfaces::WorldObjects emptyObjectsList{};
    ON_CALL(*lane2, GetWorldObjects()).WillByDefault(ReturnRef(emptyObjectsList));

    Fakes::WorldData worldData;
    const auto& roadIdMapping = laneStream.GetRoadIdMapping();
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    WorldDataQuery wdQuery(worldData);
    auto result = wdQuery.GetLastObjectInLane<OWL::Interfaces::MovingObject>(laneStream.Get(), 0, 1000);
    ASSERT_THAT(result, Eq(&distantObject));
}

TEST(GetLastObjectInLane, TwoObjectsOnLaneStream_ReturnsDistantObject)
{
    FakeLaneStream laneStream;
    auto lane1 = laneStream.AddLane(200.0, true);
    auto lane2 = laneStream.AddLane(200.0, true);

    Fakes::MovingObject closeObject;
    Fakes::MovingObject distantObject;
    ON_CALL(closeObject, GetDistance(_,_)).WillByDefault(Return(10));
    ON_CALL(distantObject, GetDistance(_,_)).WillByDefault(Return(100));
    OWL::Interfaces::WorldObjects objectsLane1{ {&closeObject} };
    ON_CALL(*lane1, GetWorldObjects()).WillByDefault(ReturnRef(objectsLane1));
    OWL::Interfaces::WorldObjects objectsLane2{ {&distantObject} };
    ON_CALL(*lane2, GetWorldObjects()).WillByDefault(ReturnRef(objectsLane2));

    Fakes::WorldData worldData;
    const auto& roadIdMapping = laneStream.GetRoadIdMapping();
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    WorldDataQuery wdQuery(worldData);
    auto result = wdQuery.GetLastObjectInLane<OWL::Interfaces::MovingObject>(laneStream.Get(), 0, 1000);
    ASSERT_THAT(result, Eq(&distantObject));
}

TEST(GetLastObjectInLane, TwoObjectsOnSecondLane_ReturnsDistantObject)
{
    FakeLaneStream laneStream;
    auto lane1 = laneStream.AddLane(200.0, true);
    auto lane2 = laneStream.AddLane(200.0, true);

    Fakes::MovingObject closeObject;
    Fakes::MovingObject distantObject;
    ON_CALL(closeObject, GetDistance(_,_)).WillByDefault(Return(10));
    ON_CALL(distantObject, GetDistance(_,_)).WillByDefault(Return(100));
    OWL::Interfaces::WorldObjects emptyObjectsList{};
    ON_CALL(*lane1, GetWorldObjects()).WillByDefault(ReturnRef(emptyObjectsList));
    OWL::Interfaces::WorldObjects objectsLane2{ {&closeObject, &distantObject} };
    ON_CALL(*lane2, GetWorldObjects()).WillByDefault(ReturnRef(objectsLane2));

    Fakes::WorldData worldData;
    const auto& roadIdMapping = laneStream.GetRoadIdMapping();
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    WorldDataQuery wdQuery(worldData);
    auto result = wdQuery.GetLastObjectInLane<OWL::Interfaces::MovingObject>(laneStream.Get(), 0, 1000);
    ASSERT_THAT(result, Eq(&distantObject));
}

TEST(GetLastObjectInLane, ObjectsOutOfSearchDistance_ReturnsDefault)
{
    FakeLaneStream laneStream;
    auto lane = laneStream.AddLane(100.0, true);
    Fakes::MovingObject objectTooFarAway;
    ON_CALL(objectTooFarAway, GetDistance(_,_)).WillByDefault(Return(150));
    Fakes::MovingObject objectTooNear;
    ON_CALL(objectTooNear, GetDistance(_,_)).WillByDefault(Return(10));
    OWL::Interfaces::WorldObjects objects{ {&objectTooNear, &objectTooFarAway} };
    ON_CALL(*lane, GetWorldObjects()).WillByDefault(ReturnRef(objects));

    Fakes::WorldData worldData;
    const auto& roadIdMapping = laneStream.GetRoadIdMapping();
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetLastObjectInLane<OWL::Interfaces::MovingObject>(laneStream.Get(),20, 100);
    ASSERT_THAT(result, IsNull());
}

TEST(GetLastObjectInLane, ObjectsOnRangeBorder_ReturnsDefault)
{
    FakeLaneStream laneStream;
    auto lane = laneStream.AddLane(100.0, true);
    Fakes::MovingObject objectTooFarAway;
    ON_CALL(objectTooFarAway, GetDistance(_,_)).WillByDefault(Return(120));
    Fakes::MovingObject objectTooNear;
    ON_CALL(objectTooNear, GetDistance(_,_)).WillByDefault(Return(20));
    OWL::Interfaces::WorldObjects objects{ {&objectTooNear, &objectTooFarAway} };
    ON_CALL(*lane, GetWorldObjects()).WillByDefault(ReturnRef(objects));

    Fakes::WorldData worldData;
    const auto& roadIdMapping = laneStream.GetRoadIdMapping();
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetLastObjectInLane<OWL::Interfaces::MovingObject>(laneStream.Get(),20, 100);
    ASSERT_THAT(result, IsNull());
}

TEST(GetLastObjectInLane, TwoObjectsOnLaneInReverseDirection_ReturnsDistantObject)
{
    FakeLaneStream laneStream;
    auto lane1 = laneStream.AddLane(200.0, true);
    auto lane2 = laneStream.AddLane(200.0, false);

    Fakes::MovingObject closeObject;
    Fakes::MovingObject distantObject;
    ON_CALL(closeObject, GetDistance(_,_)).WillByDefault(Return(100));
    ON_CALL(distantObject, GetDistance(_,_)).WillByDefault(Return(10));
    OWL::Interfaces::WorldObjects emptyObjectsList{};
    ON_CALL(*lane1, GetWorldObjects()).WillByDefault(ReturnRef(emptyObjectsList));
    OWL::Interfaces::WorldObjects objectsLane2{ {&closeObject, &distantObject} };
    ON_CALL(*lane2, GetWorldObjects()).WillByDefault(ReturnRef(objectsLane2));

    Fakes::WorldData worldData;
    const auto& roadIdMapping = laneStream.GetRoadIdMapping();
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    WorldDataQuery wdQuery(worldData);
    auto result = wdQuery.GetLastObjectInLane<OWL::Interfaces::MovingObject>(laneStream.Get(), 0, 1000);
    ASSERT_THAT(result, Eq(&distantObject));
}


//////////////////////////////////////////////////////////////////////////////

TEST(GetObjectsOfTypeInRange, NoObjectstInRange_ReturnsEmptyVector)
{
    FakeLaneStream laneStream;
    auto lane = laneStream.AddLane(100.0, true);
    OWL::Interfaces::WorldObjects emptyObjectsList{};
    ON_CALL(*lane, GetWorldObjects()).WillByDefault(ReturnRef(emptyObjectsList));

    Fakes::WorldData worldData;
    const auto& roadIdMapping = laneStream.GetRoadIdMapping();
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetObjectsOfTypeInRange<OWL::Interfaces::WorldObject>(laneStream.Get(), 0, 1000);
    ASSERT_THAT(result.size(), Eq(0));
}

TEST(GetObjectsOfTypeInRange, TwoObjectsInRange_ReturnsBothInCorrectOrder)
{
    FakeLaneStream laneStream;
    auto lane1 = laneStream.AddLane(200.0, true);
    auto lane2 = laneStream.AddLane(200.0, true);

    Fakes::MovingObject closeObject;
    Fakes::MovingObject distantObject;
    ON_CALL(closeObject, GetDistance(_,_)).WillByDefault(Return(10));
    ON_CALL(distantObject, GetDistance(_,_)).WillByDefault(Return(100));
    OWL::Interfaces::WorldObjects objectsLane1{ {&distantObject, &closeObject} };
    ON_CALL(*lane1, GetWorldObjects()).WillByDefault(ReturnRef(objectsLane1));
    OWL::Interfaces::WorldObjects emptyObjectsList{};
    ON_CALL(*lane2, GetWorldObjects()).WillByDefault(ReturnRef(emptyObjectsList));

    Fakes::WorldData worldData;
    const auto& roadIdMapping = laneStream.GetRoadIdMapping();
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    WorldDataQuery wdQuery(worldData);
    auto result = wdQuery.GetObjectsOfTypeInRange<OWL::Interfaces::MovingObject>(laneStream.Get(), 0, 1000);
    ASSERT_THAT(result.size(), Eq(2));
    ASSERT_THAT(result.at(0), Eq(&closeObject));
    ASSERT_THAT(result.at(1), Eq(&distantObject));
}

TEST(GetObjectsOfTypeInRange, TwoObjectsOnDifferntLane_ReturnsBothInCorrectOrder)
{
    FakeLaneStream laneStream;
    auto lane1 = laneStream.AddLane(200.0, true);
    auto lane2 = laneStream.AddLane(200.0, true);

    Fakes::MovingObject closeObject;
    Fakes::MovingObject distantObject;
    ON_CALL(closeObject, GetDistance(_,_)).WillByDefault(Return(10));
    ON_CALL(distantObject, GetDistance(_,_)).WillByDefault(Return(100));
    OWL::Interfaces::WorldObjects objectsLane1{ {&closeObject} };
    ON_CALL(*lane1, GetWorldObjects()).WillByDefault(ReturnRef(objectsLane1));
    OWL::Interfaces::WorldObjects objectsLane2{ {&distantObject} };
    ON_CALL(*lane2, GetWorldObjects()).WillByDefault(ReturnRef(objectsLane2));

    Fakes::WorldData worldData;
    const auto& roadIdMapping = laneStream.GetRoadIdMapping();
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    WorldDataQuery wdQuery(worldData);
    auto result = wdQuery.GetObjectsOfTypeInRange<OWL::Interfaces::MovingObject>(laneStream.Get(), 0, 1000);
    ASSERT_THAT(result.size(), Eq(2));
    ASSERT_THAT(result.at(0), Eq(&closeObject));
    ASSERT_THAT(result.at(1), Eq(&distantObject));
}

TEST(GetObjectsOfTypeInRange, TwoObjectsOneInRangeOneOutside_ReturnsOneObject)
{
    FakeLaneStream laneStream;
    auto lane1 = laneStream.AddLane(200.0, true);
    auto lane2 = laneStream.AddLane(200.0, true);

    Fakes::MovingObject closeObject;
    Fakes::MovingObject distantObject;
    ON_CALL(closeObject, GetDistance(_,_)).WillByDefault(Return(50));
    ON_CALL(distantObject, GetDistance(_,_)).WillByDefault(Return(60));
    OWL::Interfaces::WorldObjects objectsLane1{ {&closeObject} };
    ON_CALL(*lane1, GetWorldObjects()).WillByDefault(ReturnRef(objectsLane1));
    OWL::Interfaces::WorldObjects objectsLane2{ {&distantObject} };
    ON_CALL(*lane2, GetWorldObjects()).WillByDefault(ReturnRef(objectsLane2));

    Fakes::WorldData worldData;
    const auto& roadIdMapping = laneStream.GetRoadIdMapping();
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    WorldDataQuery wdQuery(worldData);
    auto result = wdQuery.GetObjectsOfTypeInRange<OWL::Interfaces::MovingObject>(laneStream.Get(), 0, 200);
    ASSERT_THAT(result.size(), Eq(1));
    ASSERT_THAT(result.at(0), Eq(&closeObject));
}

/////////////////////////////////////////////////////////////////////////////

TEST(GetLanesOfLaneTypeAtDistance, NoLanesInWorld_ReturnsNoLanes)
{
    std::list<const OWL::Interfaces::Section*> sections;
    std::unordered_map<OWL::Id, OWL::OdId> laneIdMapping;

    Fakes::Road fakeRoad;
    ON_CALL(fakeRoad, GetSections()).WillByDefault(ReturnRef(sections));
    ON_CALL(fakeRoad, GetId()).WillByDefault(Return(1));

    std::unordered_map<OWL::Id, std::string> roadIdMapping = { {1, "TestRoadId"} };
    std::unordered_map<OWL::Id, OWL::Interfaces::Road*> roads = { {1, &fakeRoad } };

    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(laneIdMapping));

    WorldDataQuery wdQuery(worldData);

    OWL::CLanes lanes = wdQuery.GetLanesOfLaneTypeAtDistance("TestRoadId", 0.0, {LaneType::Driving});

    ASSERT_EQ(lanes.size(), 0);
}

TEST(GetLanesOfLaneTypeAtDistance, ThreeLanesOneLongerThanTwo_ReturnsCorrectListForFirstSection)
{
    FakeLaneManager flm(3, 3, 3.0, {100, 100, 100}, "TestRoadId");
    Fakes::WorldData worldData;

    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(flm.GetRoads()));
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(flm.GetRoadIdMapping()));
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(flm.GetLaneIdMapping()));

    ON_CALL(*flm.lanes[0][0], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[0][1], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[0][2], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[1][0], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[1][1], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[1][2], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[2][0], GetLaneType()).WillByDefault(Return(LaneType::None));
    ON_CALL(*flm.lanes[2][1], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[2][2], GetLaneType()).WillByDefault(Return(LaneType::None));

    WorldDataQuery wdQuery(worldData);

    OWL::CLanes lanes = wdQuery.GetLanesOfLaneTypeAtDistance("TestRoadId", 50.0, {LaneType::Driving});

    ASSERT_EQ(lanes.size(), 3);

    unsigned int i = 0;
    for (const auto& lane : lanes)
    {
        ASSERT_EQ(lane, flm.lanes[0][i++]);
    }
}

TEST(GetLanesOfLaneTypeAtDistance, ThreeLanesOneLongerThanTwo_ReturnsCorrectListForSecondSection)
{
    FakeLaneManager flm(3, 3, 3.0, {100, 100, 100}, "TestRoadId");
    Fakes::WorldData worldData;

    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(flm.GetRoads()));
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(flm.GetRoadIdMapping()));
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(flm.GetLaneIdMapping()));

    ON_CALL(*flm.lanes[0][0], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[0][1], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[0][2], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[1][0], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[1][1], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[1][2], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[2][0], GetLaneType()).WillByDefault(Return(LaneType::None));
    ON_CALL(*flm.lanes[2][1], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[2][2], GetLaneType()).WillByDefault(Return(LaneType::None));

    WorldDataQuery wdQuery(worldData);

    OWL::CLanes lanes = wdQuery.GetLanesOfLaneTypeAtDistance("TestRoadId", 150.0, {LaneType::Driving});

    ASSERT_EQ(lanes.size(), 3);

    unsigned int i = 0;
    for (const auto& lane : lanes)
    {
        ASSERT_EQ(lane, flm.lanes[1][i++]);
    }
}

TEST(GetLanesOfLaneTypeAtDistance, ThreeLanesOneLongerThanTwo_ReturnsCorrectListForThirdSection)
{
    FakeLaneManager flm(3, 3, 3.0, {100, 100, 100}, "TestRoadId");
    Fakes::WorldData worldData;

    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(flm.GetRoads()));
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(flm.GetRoadIdMapping()));
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(flm.GetLaneIdMapping()));

    ON_CALL(*flm.lanes[0][0], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[0][1], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[0][2], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[1][0], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[1][1], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[1][2], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[2][0], GetLaneType()).WillByDefault(Return(LaneType::None));
    ON_CALL(*flm.lanes[2][1], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[2][2], GetLaneType()).WillByDefault(Return(LaneType::None));

    WorldDataQuery wdQuery(worldData);

    OWL::CLanes lanes = wdQuery.GetLanesOfLaneTypeAtDistance("TestRoadId", 250.0, {LaneType::Driving});

    ASSERT_EQ(lanes.size(), 1);
    ASSERT_EQ(lanes.front(), flm.lanes[2][1]);
}

/////////////////////////////////////////////////////////////////////////////

TEST(GetLaneByOdId, CheckFakeLaneManagerImplementation)
{
    FakeLaneManager flm(2, 2, 3.0, {100, 100}, "TestRoadId");

    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(flm.GetRoads()));
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(flm.GetRoadIdMapping()));
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(flm.GetLaneIdMapping()));

    WorldDataQuery wdQuery(worldData);

    EXPECT_THAT(flm.lanes[0][0]->Exists(), Eq(true));
    ASSERT_EQ(flm.lanes[0][0], &(wdQuery.GetLaneByOdId("TestRoadId", -1, 50)));

    EXPECT_THAT(flm.lanes[1][0]->Exists(), Eq(true));
    ASSERT_EQ(flm.lanes[1][0], &(wdQuery.GetLaneByOdId("TestRoadId", -1, 150)));

    EXPECT_THAT(flm.lanes[0][1]->Exists(), Eq(true));
    ASSERT_EQ(flm.lanes[0][1], &(wdQuery.GetLaneByOdId("TestRoadId", -2, 50)));

    EXPECT_THAT(flm.lanes[1][1]->Exists(), Eq(true));
    ASSERT_EQ(flm.lanes[1][1], &(wdQuery.GetLaneByOdId("TestRoadId", -2, 150)));
}

TEST(GetLaneByOdId, TwoSectionsWithVariableLanes_ReturnsCorrectLanes)
{ 
    std::list<const OWL::Interfaces::Section*> sections;
    std::unordered_map<OWL::Id, OWL::OdId> laneIdMapping = {{1, -1}, {2, -2}, {3, -2}};
    std::unordered_map<OWL::Id, std::string> roadIdMapping = { {1, "TestRoadId"} };

    Fakes::Road fakeRoad;
    std::unordered_map<OWL::Id, OWL::Interfaces::Road*> roads = { {1, &fakeRoad } };
    ON_CALL(fakeRoad, GetSections()).WillByDefault(ReturnRef(sections));
    ON_CALL(fakeRoad, GetId()).WillByDefault(Return(1));

    Fakes::WorldData worldData;
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(laneIdMapping));
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));

    WorldDataQuery wdQuery(worldData);
    Fakes::Section section1;
    Fakes::Section section2;

    Fakes::Lane lane1_minus1; //Id 1
    Fakes::Lane lane1_minus2; //Id 2
    Fakes::Lane lane2_minus2; //Id 3

    ON_CALL(lane1_minus1, GetId()).WillByDefault(Return(1));
    ON_CALL(lane1_minus2, GetId()).WillByDefault(Return(2));
    ON_CALL(lane2_minus2, GetId()).WillByDefault(Return(3));

    const std::list<const OWL::Interfaces::Lane*> lanesSection1 = {{&lane1_minus1, &lane1_minus2}} ;
    ON_CALL(section1, GetLanes()).WillByDefault(ReturnRef(lanesSection1));
    ON_CALL(section1, Covers(_)).WillByDefault(Return(false));
    ON_CALL(section1, Covers(50)).WillByDefault(Return(true));

    const std::list<const OWL::Interfaces::Lane*> lanesSection2 = {{&lane2_minus2}};
    ON_CALL(section2, GetLanes()).WillByDefault(ReturnRef(lanesSection2));
    ON_CALL(section2, Covers(_)).WillByDefault(Return(false));
    ON_CALL(section2, Covers(150)).WillByDefault(Return(true));

    sections = {{&section1, &section2}};

    ASSERT_EQ(wdQuery.GetLaneByOdId("TestRoadId", -1, 50).GetId(), 1);
    ASSERT_EQ(wdQuery.GetLaneByOdId("TestRoadId", -2, 50).GetId(), 2);
    ASSERT_FALSE(wdQuery.GetLaneByOdId("TestRoadId", -1, 150).Exists());
    ASSERT_EQ(wdQuery.GetLaneByOdId("TestRoadId", -2, 150).GetId(), 3);
}

/////////////////////////////////////////////////////////////////////////////

TEST(GetValidSOnLane, CheckIfSIsValid_ReturnsTrue)
{
    FakeLaneManager laneManager(3, 1, 3.0, {100, 100, 100}, "TestRoadId");

    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(laneManager.GetRoads()));
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(laneManager.GetRoadIdMapping()));
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(laneManager.GetLaneIdMapping()));

    WorldDataQuery wdQuery(worldData);

    bool result = wdQuery.IsSValidOnLane("TestRoadId", -1, 50.0);
    ASSERT_TRUE(result);

    result = wdQuery.IsSValidOnLane("TestRoadId", -1, 150.0);
    ASSERT_TRUE(result);

    result = wdQuery.IsSValidOnLane("TestRoadId", -1, 250.0);
    ASSERT_TRUE(result);

    result = wdQuery.IsSValidOnLane("TestRoadId", -1, 0.0);
    ASSERT_TRUE(result);

    result = wdQuery.IsSValidOnLane("TestRoadId", -1, 300.0);
    ASSERT_TRUE(result);
}

TEST(GetValidSOnLane, CheckIfSIsValid_ReturnsFalse)
{
    FakeLaneManager laneManager(1, 2, 3.0, {100}, "TestRoadId");

    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(laneManager.GetRoads()));
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(laneManager.GetRoadIdMapping()));
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(laneManager.GetLaneIdMapping()));

    WorldDataQuery wdQuery(worldData);
    bool result = wdQuery.IsSValidOnLane("TestRoadId", -1, 100.1);
    ASSERT_FALSE(result);
}

TEST(GetValidSOnLane, GetNextValidSOnLaneInDownstream_ReturnsNextValidSInDownstream)
{
    FakeLaneManager laneManager(3, 1, 3.0, {100, 100, 100}, "TestRoadId");

    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(laneManager.GetRoads()));
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(laneManager.GetRoadIdMapping()));
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(laneManager.GetLaneIdMapping()));

    WorldDataQuery wdQuery(worldData);
    double result = wdQuery.GetNextValidSOnLaneInDownstream("TestRoadId", -1, 5.0, 50.0);
    ASSERT_EQ(result, 55.0);

    result = wdQuery.GetNextValidSOnLaneInDownstream("TestRoadId", -1, 150, 100.0);
    ASSERT_EQ(result, 250.0);

    result = wdQuery.GetNextValidSOnLaneInDownstream("TestRoadId", -1, 0.0, 250.0);
    ASSERT_EQ(result, 250.0);

    result = wdQuery.GetNextValidSOnLaneInDownstream("TestRoadId", -1, 0.0, 5.0);
    ASSERT_EQ(result, 5.0);

    result = wdQuery.GetNextValidSOnLaneInDownstream("TestRoadId", -1, 295.0, 5.0);
    ASSERT_EQ(result, 300.0);

    result = wdQuery.GetNextValidSOnLaneInDownstream("TestRoadId", -1, 295.0, 10.0);
    ASSERT_EQ(result, INFINITY);
}

TEST(GetValidSOnLane, GetLastValidSInUpstream_ReturnsLastValidSInUpstream)
{
    FakeLaneManager laneManager(3, 1, 3.0, {100, 100, 100}, "TestRoadId");

    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(laneManager.GetRoads()));
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(laneManager.GetRoadIdMapping()));
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(laneManager.GetLaneIdMapping()));

    WorldDataQuery wdQuery(worldData);
    double result = wdQuery.GetLastValidSInUpstream("TestRoadId", -1, 200.0, 50.0);
    ASSERT_EQ(result, 150.0);

    result = wdQuery.GetLastValidSInUpstream("TestRoadId", -1, 300.0, 250.0);
    ASSERT_EQ(result, 50.0);

    result = wdQuery.GetLastValidSInUpstream("TestRoadId", -1, 50.0, 5.0);
    ASSERT_EQ(result, 45.0);

    result = wdQuery.GetLastValidSInUpstream("TestRoadId", -1, 5.0, 5.0);
    ASSERT_EQ(result, 0.0);

    result = wdQuery.GetLastValidSInUpstream("TestRoadId", -1, 0.0, 5.0);
    ASSERT_EQ(result, -INFINITY);
}


/////////////////////////////////////////////////////////////////////////////

TEST(SideLanesExist, LeftLanesExist_ReturnsTrue)
{
    FakeLaneManager laneManager(1, 3, 3.0, {100}, "TestRoadId");

    ON_CALL(*laneManager.lanes[0][0], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*laneManager.lanes[0][1], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*laneManager.lanes[0][2], GetLaneType()).WillByDefault(Return(LaneType::Driving));

    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(laneManager.GetRoads()));
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(laneManager.GetRoadIdMapping()));
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(laneManager.GetLaneIdMapping()));


    WorldDataQuery wdQuery(worldData);
    bool result = wdQuery.ExistsDrivingLaneOnSide("TestRoadId", -3, 50, Side::Left);
    ASSERT_TRUE(result);

    result = wdQuery.ExistsDrivingLaneOnSide("TestRoadId", -2, 50.0, Side::Left);
    ASSERT_TRUE(result);

    result = wdQuery.ExistsDrivingLaneOnSide("TestRoadId", -1, 50.0, Side::Left);
    ASSERT_FALSE(result);

}

TEST(SideLanesExist, RightLanesExist_ReturnsTrue)
{
    FakeLaneManager laneManager(1, 3, 3.0, {100}, "TestRoadId");

    ON_CALL(*laneManager.lanes[0][0], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*laneManager.lanes[0][1], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*laneManager.lanes[0][2], GetLaneType()).WillByDefault(Return(LaneType::Driving));

    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(laneManager.GetRoads()));
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(laneManager.GetRoadIdMapping()));
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(laneManager.GetLaneIdMapping()));

    WorldDataQuery wdQuery(worldData);
    bool result = wdQuery.ExistsDrivingLaneOnSide("TestRoadId", -1, 50, Side::Right);
    ASSERT_TRUE(result);

    result = wdQuery.ExistsDrivingLaneOnSide("TestRoadId", -2, 50, Side::Right);
    ASSERT_TRUE(result);

    result = wdQuery.ExistsDrivingLaneOnSide("TestRoadId", -3, 50, Side::Right);
    ASSERT_FALSE(result);
}

TEST(SideLanesExist, RightLanesExistButInvalidLaneType_ReturnsFalse)
{
    FakeLaneManager laneManager(1, 3, 3.0, {100}, "TestRoadId");

    ON_CALL(*laneManager.lanes[0][1], GetLaneType()).WillByDefault(Return(LaneType::Undefined));
    ON_CALL(*laneManager.lanes[0][2], GetLaneType()).WillByDefault(Return(LaneType::Biking));

    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(laneManager.GetRoads()));
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(laneManager.GetRoadIdMapping()));
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(laneManager.GetLaneIdMapping()));

    WorldDataQuery wdQuery(worldData);
    bool result = wdQuery.ExistsDrivingLaneOnSide("TestRoadId", -1, 50, Side::Right);
    ASSERT_FALSE(result);

    result = wdQuery.ExistsDrivingLaneOnSide("TestRoadId", -2, 50, Side::Right);
    ASSERT_FALSE(result);
}

TEST(SideLanesExist, LeftLanesExistButInvalidLaneType_ReturnsFalse)
{
    FakeLaneManager laneManager(1, 3, 3.0, {100}, "TestRoadId");

    ON_CALL(*laneManager.lanes[0][0], GetLaneType()).WillByDefault(Return(LaneType::Exit));
    ON_CALL(*laneManager.lanes[0][1], GetLaneType()).WillByDefault(Return(LaneType::Biking));

    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(laneManager.GetRoads()));
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(laneManager.GetRoadIdMapping()));
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(laneManager.GetLaneIdMapping()));


    WorldDataQuery wdQuery(worldData);
    bool result = wdQuery.ExistsDrivingLaneOnSide("TestRoadId", -3, 50, Side::Left);
    ASSERT_FALSE(result);

    result = wdQuery.ExistsDrivingLaneOnSide("TestRoadId", -2, 50.0, Side::Left);
    ASSERT_FALSE(result);
}

//////////////////////////////////////////////////////////////////

TEST(GetTrafficSignsInRange, ReturnsCorrectTrafficSigns)
{
    FakeLaneStream laneStream;
    auto lane1 =laneStream.AddLane(400, true);
    auto lane2 = laneStream.AddLane(400, true);

    Fakes::TrafficSign fakeSign1;
    ON_CALL(fakeSign1, GetS()).WillByDefault(Return(100.0));

    Fakes::TrafficSign fakeSign2;
    ON_CALL(fakeSign2, GetS()).WillByDefault(Return(200.0));

    Fakes::TrafficSign fakeSign3;
    ON_CALL(fakeSign3, GetS()).WillByDefault(Return(50.0));

    Fakes::TrafficSign fakeSign4;
    ON_CALL(fakeSign4, GetS()).WillByDefault(Return(250.0));

    OWL::Interfaces::TrafficSigns signsLane1{ {&fakeSign1, &fakeSign2} };
    ON_CALL(*lane1, GetTrafficSigns()).WillByDefault(ReturnRef(signsLane1));
    OWL::Interfaces::TrafficSigns signsLane2{ {&fakeSign3, &fakeSign4} };
    ON_CALL(*lane2, GetTrafficSigns()).WillByDefault(ReturnRef(signsLane2));

    Fakes::WorldData worldData;
    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetTrafficSignsInRange(laneStream.Get(), 100.0, 1000.0);
    EXPECT_EQ(result.size(), 4);
    ASSERT_THAT(result.at(0).first, DoubleEq(0.0));
    ASSERT_THAT(result.at(0).second, Eq(&fakeSign1));
    ASSERT_THAT(result.at(1).first, DoubleEq(100.0));
    ASSERT_THAT(result.at(1).second, Eq(&fakeSign2));
    ASSERT_THAT(result.at(2).first, DoubleEq(350.0));
    ASSERT_THAT(result.at(2).second, Eq(&fakeSign3));
    ASSERT_THAT(result.at(3).first, DoubleEq(550.0));
    ASSERT_THAT(result.at(3).second, Eq(&fakeSign4));

    result = wdQuery.GetTrafficSignsInRange(laneStream.Get(), 100.0, 500.0);
    EXPECT_EQ(result.size(), 3);
    ASSERT_THAT(result.at(0).first, DoubleEq(0.0));
    ASSERT_THAT(result.at(0).second, Eq(&fakeSign1));
    ASSERT_THAT(result.at(1).first, DoubleEq(100.0));
    ASSERT_THAT(result.at(1).second, Eq(&fakeSign2));
    ASSERT_THAT(result.at(2).first, DoubleEq(350.0));
    ASSERT_THAT(result.at(2).second, Eq(&fakeSign3));

    result = wdQuery.GetTrafficSignsInRange(laneStream.Get(), 150.0, 400.0);
    EXPECT_EQ(result.size(), 2);
    ASSERT_THAT(result.at(0).first, DoubleEq(50.0));
    ASSERT_THAT(result.at(0).second, Eq(&fakeSign2));
    ASSERT_THAT(result.at(1).first, DoubleEq(300.0));
    ASSERT_THAT(result.at(1).second, Eq(&fakeSign3));

    result = wdQuery.GetTrafficSignsInRange(laneStream.Get(), 1000.0, 1000.0);
    EXPECT_EQ(result.size(), 0);
}

TEST(DISABLED_QueryLanes, ThreeLanesInTwoSections)
{
    std::list<const OWL::Interfaces::Section*> sections;
    std::unordered_map<OWL::Id, OWL::OdId> laneIdMapping = {{1, -1}, {2, -2}, {3, -2}};
    std::unordered_map<OWL::Id, std::string> roadIdMapping = { {1, "TestRoadId"} };

    Fakes::Road fakeRoad;
    std::unordered_map<OWL::Id, OWL::Interfaces::Road*> roads = { {1, &fakeRoad} };
    ON_CALL(fakeRoad, GetSections()).WillByDefault(ReturnRef(sections));
    ON_CALL(fakeRoad, GetId()).WillByDefault(Return(1));

    Fakes::WorldData worldData;
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(laneIdMapping));
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));

    WorldDataQuery wdQuery(worldData);
    Fakes::Section section1;
    Fakes::Section section2;

    Fakes::Lane lane1_minus1; //Id 1
    Fakes::Lane lane1_minus2; //Id 2
    Fakes::Lane lane2_minus2; //Id 3

    ON_CALL(lane1_minus1, GetId()).WillByDefault(Return(1));
    ON_CALL(lane1_minus2, GetId()).WillByDefault(Return(2));
    ON_CALL(lane2_minus2, GetId()).WillByDefault(Return(3));

    ON_CALL(lane1_minus1, Exists()).WillByDefault(Return(true));
    ON_CALL(lane1_minus2, Exists()).WillByDefault(Return(true));
    ON_CALL(lane2_minus2, Exists()).WillByDefault(Return(true));

    std::vector<OWL::Id> previous{2};
    ON_CALL(lane2_minus2, GetPrevious()).WillByDefault(Return(previous));

    std::vector<OWL::Id> next{3};
    ON_CALL(lane1_minus2, GetNext()).WillByDefault(Return(next));


    ON_CALL(lane1_minus1, GetRightLaneCount()).WillByDefault(Return(1));

    ON_CALL(lane1_minus1, GetLaneType()).WillByDefault(Return(LaneType::Stop));
    ON_CALL(lane1_minus2, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(lane2_minus2, GetLaneType()).WillByDefault(Return(LaneType::Driving));

    ON_CALL(lane1_minus1, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(50));
    ON_CALL(lane1_minus1, GetDistance(OWL::MeasurementPoint::RoadEnd)).WillByDefault(Return(100));
    ON_CALL(lane1_minus2, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(50));
    ON_CALL(lane1_minus2, GetDistance(OWL::MeasurementPoint::RoadEnd)).WillByDefault(Return(100));
    ON_CALL(lane2_minus2, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(100));
    ON_CALL(lane2_minus2, GetDistance(OWL::MeasurementPoint::RoadEnd)).WillByDefault(Return(150));

    const std::list<const OWL::Interfaces::Lane*> lanesSection1 = {{&lane1_minus1, &lane1_minus2}} ;
    ON_CALL(section1, GetLanes()).WillByDefault(ReturnRef(lanesSection1));
    ON_CALL(section1, Covers(_)).WillByDefault(Return(false));
    ON_CALL(section1, CoversInterval(0.0, 100.0)).WillByDefault(Return(true));

    const std::list<const OWL::Interfaces::Lane*> lanesSection2 = {{&lane2_minus2}} ;
    ON_CALL(section2, GetLanes()).WillByDefault(ReturnRef(lanesSection2));
    ON_CALL(section2, Covers(_)).WillByDefault(Return(false));
    ON_CALL(section2, CoversInterval(0.0, 100.0)).WillByDefault(Return(true));

    sections = {{&section1, &section2}};

    auto queryResult = wdQuery.QueryLanes("TestRoadId", 0.0, 100.0);

    ASSERT_EQ(queryResult.size(), 2);
    queryResult.sort([](LaneQueryResult result1, LaneQueryResult result2){return result1.streamId < result2.streamId;});
    LaneQueryResult result = queryResult.front();
    ASSERT_TRUE(result.valid);
    ASSERT_EQ(result.streamId, 1);
    ASSERT_EQ(result.laneCategory, LaneCategory::RegularLane);
    ASSERT_FALSE(result.isDrivingLane);
    ASSERT_EQ(result.startDistance, 50);
    ASSERT_EQ(result.endDistance, 100);
    queryResult.pop_front();
    result = queryResult.front();
    ASSERT_TRUE(result.valid);
    ASSERT_EQ(result.streamId, 2);
    ASSERT_EQ(result.laneCategory, LaneCategory::RightMostLane);
    ASSERT_TRUE(result.isDrivingLane);
    ASSERT_EQ(result.startDistance, 50);
    ASSERT_EQ(result.endDistance, 150);
}

TEST(GetLaneMarkings, OneLaneWithOnlyOneBoundaryPerSide)
{
    FakeLaneStream laneStream;
    auto lane = laneStream.AddLane(100.0, true);

    Fakes::WorldData worldData;

    std::vector<OWL::Id> leftLaneBoundaries { {4} };
    std::vector<OWL::Id> rightLaneBoundaries { {5} };
    ON_CALL(*lane, GetLeftLaneBoundaries()).WillByDefault(Return(leftLaneBoundaries));
    ON_CALL(*lane, GetRightLaneBoundaries()).WillByDefault(Return(rightLaneBoundaries));

    Fakes::LaneBoundary fakeLeftLaneBoundary;
    ON_CALL(fakeLeftLaneBoundary, GetWidth()).WillByDefault(Return(0.5));
    ON_CALL(fakeLeftLaneBoundary, GetSStart()).WillByDefault(Return(0));
    ON_CALL(fakeLeftLaneBoundary, GetSEnd()).WillByDefault(Return(100));
    ON_CALL(fakeLeftLaneBoundary, GetType()).WillByDefault(Return(LaneMarking::Type::Solid));
    ON_CALL(fakeLeftLaneBoundary, GetColor()).WillByDefault(Return(LaneMarking::Color::Yellow));
    ON_CALL(fakeLeftLaneBoundary, GetSide()).WillByDefault(Return(OWL::LaneMarkingSide::Single));

    Fakes::LaneBoundary fakeRightLaneBoundary;
    ON_CALL(fakeRightLaneBoundary, GetWidth()).WillByDefault(Return(0.6));
    ON_CALL(fakeRightLaneBoundary, GetSStart()).WillByDefault(Return(0));
    ON_CALL(fakeRightLaneBoundary, GetSEnd()).WillByDefault(Return(100));
    ON_CALL(fakeRightLaneBoundary, GetType()).WillByDefault(Return(LaneMarking::Type::Broken));
    ON_CALL(fakeRightLaneBoundary, GetColor()).WillByDefault(Return(LaneMarking::Color::Blue));
    ON_CALL(fakeRightLaneBoundary, GetSide()).WillByDefault(Return(OWL::LaneMarkingSide::Single));

    std::unordered_map<OWL::Id, OWL::Interfaces::LaneBoundary *> laneBoundaries { {4, &fakeLeftLaneBoundary} , {5, &fakeRightLaneBoundary}};
    ON_CALL(worldData, GetLaneBoundaries()).WillByDefault(ReturnRef(laneBoundaries));

    WorldDataQuery wdQuery(worldData);

    auto resultLeft = wdQuery.GetLaneMarkings(laneStream.Get(), 0, 100, Side::Left);
    ASSERT_THAT(resultLeft.size(), Eq(1));
    auto& resultLeftLaneMarking = resultLeft.at(0);
    ASSERT_THAT(resultLeftLaneMarking.type, Eq(LaneMarking::Type::Solid));
    ASSERT_THAT(resultLeftLaneMarking.color, Eq(LaneMarking::Color::Yellow));
    ASSERT_THAT(resultLeftLaneMarking.relativeStartDistance, DoubleEq(0.0));
    ASSERT_THAT(resultLeftLaneMarking.width, DoubleEq(0.5));

    auto resultRight = wdQuery.GetLaneMarkings(laneStream.Get(), 0, 100, Side::Right);
    ASSERT_THAT(resultRight.size(), Eq(1));
    auto& resultRightLaneMarking = resultRight.at(0);
    ASSERT_THAT(resultRightLaneMarking.type, Eq(LaneMarking::Type::Broken));
    ASSERT_THAT(resultRightLaneMarking.color, Eq(LaneMarking::Color::Blue));
    ASSERT_THAT(resultRightLaneMarking.relativeStartDistance, DoubleEq(0.0));
    ASSERT_THAT(resultRightLaneMarking.width, DoubleEq(0.6));
}

TEST(GetLaneMarkings, OneLaneWithSolidBrokenBoundary)
{
    FakeLaneStream laneStream;
    auto lane = laneStream.AddLane(100.0, true);

    Fakes::WorldData worldData;

    std::vector<OWL::Id> leftLaneBoundaries { {4, 5} };
    ON_CALL(*lane, GetLeftLaneBoundaries()).WillByDefault(Return(leftLaneBoundaries));

    Fakes::LaneBoundary leftLine;
    ON_CALL(leftLine, GetWidth()).WillByDefault(Return(0.5));
    ON_CALL(leftLine, GetSStart()).WillByDefault(Return(0));
    ON_CALL(leftLine, GetSEnd()).WillByDefault(Return(100));
    ON_CALL(leftLine, GetType()).WillByDefault(Return(LaneMarking::Type::Solid));
    ON_CALL(leftLine, GetColor()).WillByDefault(Return(LaneMarking::Color::Yellow));
    ON_CALL(leftLine, GetSide()).WillByDefault(Return(OWL::LaneMarkingSide::Left));

    Fakes::LaneBoundary rightLine;
    ON_CALL(rightLine, GetWidth()).WillByDefault(Return(0.5));
    ON_CALL(rightLine, GetSStart()).WillByDefault(Return(0));
    ON_CALL(rightLine, GetSEnd()).WillByDefault(Return(100));
    ON_CALL(rightLine, GetType()).WillByDefault(Return(LaneMarking::Type::Broken));
    ON_CALL(rightLine, GetColor()).WillByDefault(Return(LaneMarking::Color::Yellow));
    ON_CALL(rightLine, GetSide()).WillByDefault(Return(OWL::LaneMarkingSide::Right));

    std::unordered_map<OWL::Id, OWL::Interfaces::LaneBoundary *> laneBoundaries { {4, &leftLine} , {5, &rightLine}};
    ON_CALL(worldData, GetLaneBoundaries()).WillByDefault(ReturnRef(laneBoundaries));

    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetLaneMarkings(laneStream.Get(), 0, 100, Side::Left);
    ASSERT_THAT(result.size(), Eq(1));
    auto& resultLaneMarking = result.at(0);
    ASSERT_THAT(resultLaneMarking.type, Eq(LaneMarking::Type::Solid_Broken));
    ASSERT_THAT(resultLaneMarking.color, Eq(LaneMarking::Color::Yellow));
    ASSERT_THAT(resultLaneMarking.relativeStartDistance, DoubleEq(0.0));
    ASSERT_THAT(resultLaneMarking.width, DoubleEq(0.5));
}

TEST(GetLaneMarkings, OneLaneWithBrokenSolidBoundary)
{
    FakeLaneStream laneStream;
    auto lane = laneStream.AddLane(100.0, true);

    Fakes::WorldData worldData;

    std::vector<OWL::Id> leftLaneBoundaries { {4, 5} };
    ON_CALL(*lane, GetLeftLaneBoundaries()).WillByDefault(Return(leftLaneBoundaries));

    Fakes::LaneBoundary leftLine;
    ON_CALL(leftLine, GetWidth()).WillByDefault(Return(0.5));
    ON_CALL(leftLine, GetSStart()).WillByDefault(Return(0));
    ON_CALL(leftLine, GetSEnd()).WillByDefault(Return(100));
    ON_CALL(leftLine, GetType()).WillByDefault(Return(LaneMarking::Type::Broken));
    ON_CALL(leftLine, GetColor()).WillByDefault(Return(LaneMarking::Color::Yellow));
    ON_CALL(leftLine, GetSide()).WillByDefault(Return(OWL::LaneMarkingSide::Left));

    Fakes::LaneBoundary rightLine;
    ON_CALL(rightLine, GetWidth()).WillByDefault(Return(0.5));
    ON_CALL(rightLine, GetSStart()).WillByDefault(Return(0));
    ON_CALL(rightLine, GetSEnd()).WillByDefault(Return(100));
    ON_CALL(rightLine, GetType()).WillByDefault(Return(LaneMarking::Type::Solid));
    ON_CALL(rightLine, GetColor()).WillByDefault(Return(LaneMarking::Color::Yellow));
    ON_CALL(rightLine, GetSide()).WillByDefault(Return(OWL::LaneMarkingSide::Right));

    std::unordered_map<OWL::Id, OWL::Interfaces::LaneBoundary *> laneBoundaries { {4, &leftLine} , {5, &rightLine}};
    ON_CALL(worldData, GetLaneBoundaries()).WillByDefault(ReturnRef(laneBoundaries));

    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetLaneMarkings(laneStream.Get(), 0, 100, Side::Left);
    ASSERT_THAT(result.size(), Eq(1));
    auto& resultLaneMarking = result.at(0);
    ASSERT_THAT(resultLaneMarking.type, Eq(LaneMarking::Type::Broken_Solid));
    ASSERT_THAT(resultLaneMarking.color, Eq(LaneMarking::Color::Yellow));
    ASSERT_THAT(resultLaneMarking.relativeStartDistance, DoubleEq(0.0));
    ASSERT_THAT(resultLaneMarking.width, DoubleEq(0.5));
}

TEST(GetLaneMarkings, OneLaneWithSolidSolidBoundary)
{
    FakeLaneStream laneStream;
    auto lane = laneStream.AddLane(100.0, true);

    Fakes::WorldData worldData;

    std::vector<OWL::Id> leftLaneBoundaries { {4, 5} };
    ON_CALL(*lane, GetLeftLaneBoundaries()).WillByDefault(Return(leftLaneBoundaries));

    Fakes::LaneBoundary leftLine;
    ON_CALL(leftLine, GetWidth()).WillByDefault(Return(0.5));
    ON_CALL(leftLine, GetSStart()).WillByDefault(Return(0));
    ON_CALL(leftLine, GetSEnd()).WillByDefault(Return(100));
    ON_CALL(leftLine, GetType()).WillByDefault(Return(LaneMarking::Type::Solid));
    ON_CALL(leftLine, GetColor()).WillByDefault(Return(LaneMarking::Color::Yellow));
    ON_CALL(leftLine, GetSide()).WillByDefault(Return(OWL::LaneMarkingSide::Left));

    Fakes::LaneBoundary rightLine;
    ON_CALL(rightLine, GetWidth()).WillByDefault(Return(0.5));
    ON_CALL(rightLine, GetSStart()).WillByDefault(Return(0));
    ON_CALL(rightLine, GetSEnd()).WillByDefault(Return(100));
    ON_CALL(rightLine, GetType()).WillByDefault(Return(LaneMarking::Type::Solid));
    ON_CALL(rightLine, GetColor()).WillByDefault(Return(LaneMarking::Color::Yellow));
    ON_CALL(rightLine, GetSide()).WillByDefault(Return(OWL::LaneMarkingSide::Right));

    std::unordered_map<OWL::Id, OWL::Interfaces::LaneBoundary *> laneBoundaries { {4, &leftLine} , {5, &rightLine}};
    ON_CALL(worldData, GetLaneBoundaries()).WillByDefault(ReturnRef(laneBoundaries));

    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetLaneMarkings(laneStream.Get(), 0, 100, Side::Left);
    ASSERT_THAT(result.size(), Eq(1));
    auto& resultLaneMarking = result.at(0);
    ASSERT_THAT(resultLaneMarking.type, Eq(LaneMarking::Type::Solid_Solid));
    ASSERT_THAT(resultLaneMarking.color, Eq(LaneMarking::Color::Yellow));
    ASSERT_THAT(resultLaneMarking.relativeStartDistance, DoubleEq(0.0));
    ASSERT_THAT(resultLaneMarking.width, DoubleEq(0.5));
}

TEST(GetLaneMarkings, OneLaneWithBrokenBrokenBoundary)
{
    FakeLaneStream laneStream;
    auto lane = laneStream.AddLane(100.0, true);

    Fakes::WorldData worldData;

    std::vector<OWL::Id> leftLaneBoundaries { {4, 5} };
    ON_CALL(*lane, GetLeftLaneBoundaries()).WillByDefault(Return(leftLaneBoundaries));

    Fakes::LaneBoundary leftLine;
    ON_CALL(leftLine, GetWidth()).WillByDefault(Return(0.5));
    ON_CALL(leftLine, GetSStart()).WillByDefault(Return(0));
    ON_CALL(leftLine, GetSEnd()).WillByDefault(Return(100));
    ON_CALL(leftLine, GetType()).WillByDefault(Return(LaneMarking::Type::Broken));
    ON_CALL(leftLine, GetColor()).WillByDefault(Return(LaneMarking::Color::Yellow));
    ON_CALL(leftLine, GetSide()).WillByDefault(Return(OWL::LaneMarkingSide::Left));

    Fakes::LaneBoundary rightLine;
    ON_CALL(rightLine, GetWidth()).WillByDefault(Return(0.5));
    ON_CALL(rightLine, GetSStart()).WillByDefault(Return(0));
    ON_CALL(rightLine, GetSEnd()).WillByDefault(Return(100));
    ON_CALL(rightLine, GetType()).WillByDefault(Return(LaneMarking::Type::Broken));
    ON_CALL(rightLine, GetColor()).WillByDefault(Return(LaneMarking::Color::Yellow));
    ON_CALL(rightLine, GetSide()).WillByDefault(Return(OWL::LaneMarkingSide::Right));

    std::unordered_map<OWL::Id, OWL::Interfaces::LaneBoundary *> laneBoundaries { {4, &leftLine} , {5, &rightLine}};
    ON_CALL(worldData, GetLaneBoundaries()).WillByDefault(ReturnRef(laneBoundaries));

    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetLaneMarkings(laneStream.Get(), 0, 100, Side::Left);
    ASSERT_THAT(result.size(), Eq(1));
    auto& resultLaneMarking = result.at(0);
    ASSERT_THAT(resultLaneMarking.type, Eq(LaneMarking::Type::Broken_Broken));
    ASSERT_THAT(resultLaneMarking.color, Eq(LaneMarking::Color::Yellow));
    ASSERT_THAT(resultLaneMarking.relativeStartDistance, DoubleEq(0.0));
    ASSERT_THAT(resultLaneMarking.width, DoubleEq(0.5));
}

TEST(GetLaneMarkings, TwoLanesWithMultipleBoundaries)
{
    FakeLaneStream laneStream;
    auto lane1 = laneStream.AddLane(100.0, true);
    auto lane2 = laneStream.AddLane(100.0, true);

    Fakes::WorldData worldData;

    std::vector<OWL::Id> leftLane1Boundaries { {4, 5, 6} };
    ON_CALL(*lane1, GetLeftLaneBoundaries()).WillByDefault(Return(leftLane1Boundaries));
    ON_CALL(*lane1, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(0.0));
    std::vector<OWL::Id> leftLane2Boundaries { {10, 11} };
    ON_CALL(*lane2, GetLeftLaneBoundaries()).WillByDefault(Return(leftLane2Boundaries));
    ON_CALL(*lane2, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(100.0));

    Fakes::LaneBoundary fakeLaneBoundary4;
    ON_CALL(fakeLaneBoundary4, GetWidth()).WillByDefault(Return(0.5));
    ON_CALL(fakeLaneBoundary4, GetSStart()).WillByDefault(Return(0));
    ON_CALL(fakeLaneBoundary4, GetSEnd()).WillByDefault(Return(20));
    ON_CALL(fakeLaneBoundary4, GetType()).WillByDefault(Return(LaneMarking::Type::Solid));
    ON_CALL(fakeLaneBoundary4, GetColor()).WillByDefault(Return(LaneMarking::Color::Yellow));
    ON_CALL(fakeLaneBoundary4, GetSide()).WillByDefault(Return(OWL::LaneMarkingSide::Single));

    Fakes::LaneBoundary fakeLaneBoundary5;
    ON_CALL(fakeLaneBoundary5, GetWidth()).WillByDefault(Return(0.6));
    ON_CALL(fakeLaneBoundary5, GetSStart()).WillByDefault(Return(20));
    ON_CALL(fakeLaneBoundary5, GetSEnd()).WillByDefault(Return(60));
    ON_CALL(fakeLaneBoundary5, GetType()).WillByDefault(Return(LaneMarking::Type::Broken));
    ON_CALL(fakeLaneBoundary5, GetColor()).WillByDefault(Return(LaneMarking::Color::Blue));
    ON_CALL(fakeLaneBoundary5, GetSide()).WillByDefault(Return(OWL::LaneMarkingSide::Single));

    Fakes::LaneBoundary fakeLaneBoundary6;
    ON_CALL(fakeLaneBoundary6, GetWidth()).WillByDefault(Return(0.7));
    ON_CALL(fakeLaneBoundary6, GetSStart()).WillByDefault(Return(60));
    ON_CALL(fakeLaneBoundary6, GetSEnd()).WillByDefault(Return(100));
    ON_CALL(fakeLaneBoundary6, GetType()).WillByDefault(Return(LaneMarking::Type::Solid));
    ON_CALL(fakeLaneBoundary6, GetColor()).WillByDefault(Return(LaneMarking::Color::White));
    ON_CALL(fakeLaneBoundary6, GetSide()).WillByDefault(Return(OWL::LaneMarkingSide::Single));

    Fakes::LaneBoundary fakeLaneBoundary10;
    ON_CALL(fakeLaneBoundary10, GetWidth()).WillByDefault(Return(0.8));
    ON_CALL(fakeLaneBoundary10, GetSStart()).WillByDefault(Return(100));
    ON_CALL(fakeLaneBoundary10, GetSEnd()).WillByDefault(Return(150));
    ON_CALL(fakeLaneBoundary10, GetType()).WillByDefault(Return(LaneMarking::Type::None));
    ON_CALL(fakeLaneBoundary10, GetColor()).WillByDefault(Return(LaneMarking::Color::White));
    ON_CALL(fakeLaneBoundary10, GetSide()).WillByDefault(Return(OWL::LaneMarkingSide::Single));

    Fakes::LaneBoundary fakeLaneBoundary11;
    ON_CALL(fakeLaneBoundary11, GetWidth()).WillByDefault(Return(0.9));
    ON_CALL(fakeLaneBoundary11, GetSStart()).WillByDefault(Return(150));
    ON_CALL(fakeLaneBoundary11, GetSEnd()).WillByDefault(Return(200));
    ON_CALL(fakeLaneBoundary11, GetType()).WillByDefault(Return(LaneMarking::Type::Solid));
    ON_CALL(fakeLaneBoundary11, GetColor()).WillByDefault(Return(LaneMarking::Color::Green));
    ON_CALL(fakeLaneBoundary11, GetSide()).WillByDefault(Return(OWL::LaneMarkingSide::Single));

    std::unordered_map<OWL::Id, OWL::Interfaces::LaneBoundary *> laneBoundaries { {4, &fakeLaneBoundary4},
                                                                                  {5, &fakeLaneBoundary5},
                                                                                  {6, &fakeLaneBoundary6},
                                                                                  {10, &fakeLaneBoundary10},
                                                                                  {11, &fakeLaneBoundary11} };
    ON_CALL(worldData, GetLaneBoundaries()).WillByDefault(ReturnRef(laneBoundaries));

    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetLaneMarkings(laneStream.Get(), 50, 90, Side::Left);
    ASSERT_THAT(result.size(), Eq(3));
    auto& resultLaneMarking0 = result.at(0);
    ASSERT_THAT(resultLaneMarking0.type, Eq(LaneMarking::Type::Broken));
    ASSERT_THAT(resultLaneMarking0.color, Eq(LaneMarking::Color::Blue));
    ASSERT_THAT(resultLaneMarking0.relativeStartDistance, DoubleEq(-30.0));
    ASSERT_THAT(resultLaneMarking0.width, DoubleEq(0.6));
    auto& resultLaneMarking1 = result.at(1);
    ASSERT_THAT(resultLaneMarking1.type, Eq(LaneMarking::Type::Solid));
    ASSERT_THAT(resultLaneMarking1.color, Eq(LaneMarking::Color::White));
    ASSERT_THAT(resultLaneMarking1.relativeStartDistance, DoubleEq(10.0));
    ASSERT_THAT(resultLaneMarking1.width, DoubleEq(0.7));
    auto& resultLaneMarking2 = result.at(2);
    ASSERT_THAT(resultLaneMarking2.type, Eq(LaneMarking::Type::None));
    ASSERT_THAT(resultLaneMarking2.color, Eq(LaneMarking::Color::White));
    ASSERT_THAT(resultLaneMarking2.relativeStartDistance, DoubleEq(50.0));
    ASSERT_THAT(resultLaneMarking2.width, DoubleEq(0.8));
}

TEST(CreateLaneStream, OnlyUniqueConnectionsInStreamDirection)
{
    FakeLaneManager fakelm1{2, 2, 3.0, {10.0, 11.0}, ""};
    Fakes::Lane& startLane = fakelm1.GetLane(0, 0);
    auto road1 = fakelm1.GetRoads().cbegin()->second;
    auto road1Id = fakelm1.GetRoads().cbegin()->first;
    FakeLaneManager fakelm2{2, 2, 3.0, {12.0, 13.0}, ""};
    auto road2 = fakelm2.GetRoads().cbegin()->second;
    auto road2Id = fakelm2.GetRoads().cbegin()->first;
    std::vector<OWL::Id> successorsLanes1_1 {fakelm1.GetLane(1, 0).GetId()};
    ON_CALL(fakelm1.GetLane(0, 0), GetNext()).WillByDefault(Return(successorsLanes1_1));
    std::vector<OWL::Id> successorsLanes1_2 {fakelm2.GetLane(0, 0).GetId()};
    std::vector<OWL::Id> predecessorsLanes1_2 {fakelm1.GetLane(0, 0).GetId()};
    ON_CALL(fakelm1.GetLane(1, 0), GetNext()).WillByDefault(Return(successorsLanes1_2));
    ON_CALL(fakelm1.GetLane(1, 0), GetPrevious()).WillByDefault(Return(predecessorsLanes1_2));
    std::vector<OWL::Id> successorsLanes2_1 {fakelm2.GetLane(1, 0).GetId()};
    std::vector<OWL::Id> predecessorsLanes2_1 {fakelm1.GetLane(1, 0).GetId()};
    ON_CALL(fakelm2.GetLane(0, 0), GetNext()).WillByDefault(Return(successorsLanes2_1));
    ON_CALL(fakelm2.GetLane(0, 0), GetPrevious()).WillByDefault(Return(predecessorsLanes2_1));
    std::vector<OWL::Id> predecessorsLanes2_2 {fakelm2.GetLane(0, 0).GetId()};
    ON_CALL(fakelm2.GetLane(1, 0), GetPrevious()).WillByDefault(Return(predecessorsLanes2_2));

    std::vector<RouteElement> route {{"Road1", true}, {"Road2", true}};
    std::unordered_map<OWL::Id, OWL::Road*> roads {{road1Id, road1}, {road2Id, road2}};
    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    std::unordered_map<OWL::Id, std::string> roadIdMapping { {road1Id, "Road1"}, {road2Id, "Road2"} };
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(fakelm1.GetLaneIdMapping()));
    WorldDataQuery wdQuery(worldData);

    auto laneStream = wdQuery.CreateLaneStream(route, "Road1", -1, 0.0);
    auto lanesInStream = laneStream->GetElements();
    ASSERT_THAT(lanesInStream.size(), Eq(4));
    ASSERT_THAT(lanesInStream.at(0).element, Eq(&startLane));
    ASSERT_THAT(lanesInStream.at(0).sOffset, DoubleEq(0.0));
    ASSERT_THAT(lanesInStream.at(0).inStreamDirection, Eq(true));
    ASSERT_THAT(lanesInStream.at(1).element, Eq(&fakelm1.GetLane(1,0)));
    ASSERT_THAT(lanesInStream.at(1).sOffset, DoubleEq(10.0));
    ASSERT_THAT(lanesInStream.at(1).inStreamDirection, Eq(true));
    ASSERT_THAT(lanesInStream.at(2).element, Eq(&fakelm2.GetLane(0,0)));
    ASSERT_THAT(lanesInStream.at(2).sOffset, DoubleEq(21.0));
    ASSERT_THAT(lanesInStream.at(2).inStreamDirection, Eq(true));
    ASSERT_THAT(lanesInStream.at(3).element, Eq(&fakelm2.GetLane(1,0)));
    ASSERT_THAT(lanesInStream.at(3).sOffset, DoubleEq(33.0));
    ASSERT_THAT(lanesInStream.at(3).inStreamDirection, Eq(true));
}

TEST(CreateLaneStream, SecondRoadAgainstStreamDirection)
{
    FakeLaneManager fakelm1{2, 2, 3.0, {10.0, 11.0}, ""};
    Fakes::Lane& startLane = fakelm1.GetLane(0, 0);
    auto road1 = fakelm1.GetRoads().cbegin()->second;
    auto road1Id = fakelm1.GetRoads().cbegin()->first;
    FakeLaneManager fakelm2{2, 2, 3.0, {12.0, 13.0}, ""};
    auto road2 = fakelm2.GetRoads().cbegin()->second;
    auto road2Id = fakelm2.GetRoads().cbegin()->first;
    std::vector<OWL::Id> successorsLanes1_1 {fakelm1.GetLane(1, 0).GetId()};
    ON_CALL(fakelm1.GetLane(0, 0), GetNext()).WillByDefault(Return(successorsLanes1_1));
    std::vector<OWL::Id> successorsLanes1_2 {fakelm2.GetLane(1, 0).GetId()};
    std::vector<OWL::Id> predecessorsLanes1_2 {fakelm1.GetLane(0, 0).GetId()};
    ON_CALL(fakelm1.GetLane(1, 0), GetNext()).WillByDefault(Return(successorsLanes1_2));
    ON_CALL(fakelm1.GetLane(1, 0), GetPrevious()).WillByDefault(Return(predecessorsLanes1_2));
    std::vector<OWL::Id> successorsLanes2_2 {fakelm1.GetLane(1, 0).GetId()};
    std::vector<OWL::Id> predecessorsLanes2_2 {fakelm2.GetLane(0, 0).GetId()};
    ON_CALL(fakelm2.GetLane(1, 0), GetNext()).WillByDefault(Return(successorsLanes2_2));
    ON_CALL(fakelm2.GetLane(1, 0), GetPrevious()).WillByDefault(Return(predecessorsLanes2_2));
    std::vector<OWL::Id> successorsLanes2_1 {fakelm2.GetLane(1, 0).GetId()};
    ON_CALL(fakelm2.GetLane(0, 0), GetNext()).WillByDefault(Return(successorsLanes2_1));

    std::vector<RouteElement> route {{"Road1", true}, {"Road2", false}};
    std::unordered_map<OWL::Id, OWL::Road*> roads {{road1Id, road1}, {road2Id, road2}};
    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    std::unordered_map<OWL::Id, std::string> roadIdMapping { {road1Id, "Road1"}, {road2Id, "Road2"} };
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(fakelm1.GetLaneIdMapping()));
    WorldDataQuery wdQuery(worldData);

    auto laneStream = wdQuery.CreateLaneStream(route, "Road1", -1, 0.0);
    auto lanesInStream = laneStream->GetElements();
    ASSERT_THAT(lanesInStream.size(), Eq(4));
    ASSERT_THAT(lanesInStream.at(0).element, Eq(&startLane));
    ASSERT_THAT(lanesInStream.at(0).sOffset, DoubleEq(0.0));
    ASSERT_THAT(lanesInStream.at(0).inStreamDirection, Eq(true));
    ASSERT_THAT(lanesInStream.at(1).element, Eq(&fakelm1.GetLane(1,0)));
    ASSERT_THAT(lanesInStream.at(1).sOffset, DoubleEq(10.0));
    ASSERT_THAT(lanesInStream.at(1).inStreamDirection, Eq(true));
    ASSERT_THAT(lanesInStream.at(2).element, Eq(&fakelm2.GetLane(1,0)));
    ASSERT_THAT(lanesInStream.at(2).sOffset, DoubleEq(34.0));
    ASSERT_THAT(lanesInStream.at(2).inStreamDirection, Eq(false));
    ASSERT_THAT(lanesInStream.at(3).element, Eq(&fakelm2.GetLane(0,0)));
    ASSERT_THAT(lanesInStream.at(3).sOffset, DoubleEq(46.0));
    ASSERT_THAT(lanesInStream.at(3).inStreamDirection, Eq(false));
}

TEST(CreateLaneStream, MultipleConnections)
{
    FakeLaneManager fakelm1{1, 2, 3.0, {10.0}, ""};
    Fakes::Lane& startLane = fakelm1.GetLane(0, 0);
    auto road1 = fakelm1.GetRoads().cbegin()->second;
    auto road1Id = fakelm1.GetRoads().cbegin()->first;
    FakeLaneManager fakelm2{1, 2, 3.0, {11.0}, ""};
    auto road2 = fakelm2.GetRoads().cbegin()->second;
    auto road2Id = fakelm2.GetRoads().cbegin()->first;
    FakeLaneManager fakelm3{1, 2, 3.0, {12.0}, ""};
    auto road3 = fakelm3.GetRoads().cbegin()->second;
    auto road3Id = fakelm3.GetRoads().cbegin()->first;
    std::vector<OWL::Id> successorsLanes1_1 {fakelm2.GetLane(0, 0).GetId(), fakelm3.GetLane(0, 0).GetId()};
    ON_CALL(fakelm1.GetLane(0, 0), GetNext()).WillByDefault(Return(successorsLanes1_1));
    std::vector<OWL::Id> predecessorsLanes2_1 {fakelm1.GetLane(0, 0).GetId()};
    ON_CALL(fakelm2.GetLane(0, 0), GetPrevious()).WillByDefault(Return(predecessorsLanes2_1));
    std::vector<OWL::Id> predecessorsLanes3_1 {fakelm1.GetLane(0, 0).GetId()};
    ON_CALL(fakelm3.GetLane(0, 0), GetPrevious()).WillByDefault(Return(predecessorsLanes3_1));

    std::vector<RouteElement> route {{"Road1", true}, {"Road3", true}};
    std::unordered_map<OWL::Id, OWL::Road*> roads {{road1Id, road1}, {road2Id, road2}, {road3Id, road3}};
    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    std::unordered_map<OWL::Id, std::string> roadIdMapping { {road1Id, "Road1"}, {road2Id, "Road2"}, {road3Id, "Road3"} };
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(fakelm1.GetLaneIdMapping()));
    WorldDataQuery wdQuery(worldData);

    auto laneStream = wdQuery.CreateLaneStream(route, "Road1", -1, 0.0);
    auto lanesInStream = laneStream->GetElements();
    ASSERT_THAT(lanesInStream.size(), Eq(2));
    ASSERT_THAT(lanesInStream.at(0).element, Eq(&startLane));
    ASSERT_THAT(lanesInStream.at(0).sOffset, DoubleEq(0.0));
    ASSERT_THAT(lanesInStream.at(0).inStreamDirection, Eq(true));
    ASSERT_THAT(lanesInStream.at(1).element, Eq(&fakelm3.GetLane(0,0)));
    ASSERT_THAT(lanesInStream.at(1).sOffset, DoubleEq(10.0));
    ASSERT_THAT(lanesInStream.at(1).inStreamDirection, Eq(true));
}

TEST(CreateLaneStream, StartOnSecondLane)
{
    FakeLaneManager fakelm1{2, 2, 3.0, {10.0, 11.0}, ""};
    auto road1 = fakelm1.GetRoads().cbegin()->second;
    auto road1Id = fakelm1.GetRoads().cbegin()->first;
    FakeLaneManager fakelm2{2, 2, 3.0, {12.0, 13.0}, ""};
    auto road2 = fakelm2.GetRoads().cbegin()->second;
    auto road2Id = fakelm2.GetRoads().cbegin()->first;
    std::vector<OWL::Id> successorsLanes1_1 {fakelm1.GetLane(1, 0).GetId()};
    ON_CALL(fakelm1.GetLane(0, 0), GetNext()).WillByDefault(Return(successorsLanes1_1));
    std::vector<OWL::Id> successorsLanes1_2 {fakelm2.GetLane(0, 0).GetId()};
    std::vector<OWL::Id> predecessorsLanes1_2 {fakelm1.GetLane(0, 0).GetId()};
    ON_CALL(fakelm1.GetLane(1, 0), GetNext()).WillByDefault(Return(successorsLanes1_2));
    ON_CALL(fakelm1.GetLane(1, 0), GetPrevious()).WillByDefault(Return(predecessorsLanes1_2));
    std::vector<OWL::Id> successorsLanes2_1 {fakelm2.GetLane(1, 0).GetId()};
    std::vector<OWL::Id> predecessorsLanes2_1 {fakelm1.GetLane(1, 0).GetId()};
    ON_CALL(fakelm2.GetLane(0, 0), GetNext()).WillByDefault(Return(successorsLanes2_1));
    ON_CALL(fakelm2.GetLane(0, 0), GetPrevious()).WillByDefault(Return(predecessorsLanes2_1));
    std::vector<OWL::Id> predecessorsLanes2_2 {fakelm2.GetLane(0, 0).GetId()};
    ON_CALL(fakelm2.GetLane(1, 0), GetPrevious()).WillByDefault(Return(predecessorsLanes2_2));

    std::vector<RouteElement> route {{"Road1", true}, {"Road2", true}};
    std::unordered_map<OWL::Id, OWL::Road*> roads {{road1Id, road1}, {road2Id, road2}};
    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    std::unordered_map<OWL::Id, std::string> roadIdMapping { {road1Id, "Road1"}, {road2Id, "Road2"} };
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(fakelm1.GetLaneIdMapping()));
    std::unordered_map<OWL::Id, OWL::Interfaces::Lane*> lanes{ {fakelm1.GetLane(0, 0).GetId(), &fakelm1.GetLane(0, 0)} };
    ON_CALL(worldData, GetLanes()).WillByDefault(ReturnRef(lanes));
    WorldDataQuery wdQuery(worldData);

    auto laneStream = wdQuery.CreateLaneStream(route, "Road1", -1, 15.0);
    auto lanesInStream = laneStream->GetElements();
    ASSERT_THAT(lanesInStream.size(), Eq(4));
    ASSERT_THAT(lanesInStream.at(0).element, Eq(&fakelm1.GetLane(0,0)));
    ASSERT_THAT(lanesInStream.at(0).sOffset, DoubleEq(0.0));
    ASSERT_THAT(lanesInStream.at(0).inStreamDirection, Eq(true));
    ASSERT_THAT(lanesInStream.at(1).element, Eq(&fakelm1.GetLane(1,0)));
    ASSERT_THAT(lanesInStream.at(1).sOffset, DoubleEq(10.0));
    ASSERT_THAT(lanesInStream.at(1).inStreamDirection, Eq(true));
    ASSERT_THAT(lanesInStream.at(2).element, Eq(&fakelm2.GetLane(0,0)));
    ASSERT_THAT(lanesInStream.at(2).sOffset, DoubleEq(21.0));
    ASSERT_THAT(lanesInStream.at(2).inStreamDirection, Eq(true));
    ASSERT_THAT(lanesInStream.at(3).element, Eq(&fakelm2.GetLane(1,0)));
    ASSERT_THAT(lanesInStream.at(3).sOffset, DoubleEq(33.0));
    ASSERT_THAT(lanesInStream.at(3).inStreamDirection, Eq(true));
}

TEST(CreateLaneStream, StartOnSecondRoad)
{
    FakeLaneManager fakelm1{2, 2, 3.0, {10.0, 11.0}, ""};
    auto road1 = fakelm1.GetRoads().cbegin()->second;
    auto road1Id = fakelm1.GetRoads().cbegin()->first;
    FakeLaneManager fakelm2{2, 2, 3.0, {12.0, 13.0}, ""};
    auto road2 = fakelm2.GetRoads().cbegin()->second;
    auto road2Id = fakelm2.GetRoads().cbegin()->first;
    FakeLaneManager fakelm3{2, 2, 3.0, {14.0, 15.0}, ""};
    auto road3 = fakelm2.GetRoads().cbegin()->second;
    auto road3Id = fakelm2.GetRoads().cbegin()->first;
    std::vector<OWL::Id> successorsLanes1_1 {fakelm1.GetLane(1, 0).GetId()};
    ON_CALL(fakelm1.GetLane(0, 0), GetNext()).WillByDefault(Return(successorsLanes1_1));
    std::vector<OWL::Id> successorsLanes1_2 {fakelm2.GetLane(0, 0).GetId()};
    std::vector<OWL::Id> predecessorsLanes1_2 {fakelm1.GetLane(0, 0).GetId()};
    ON_CALL(fakelm1.GetLane(1, 0), GetNext()).WillByDefault(Return(successorsLanes1_2));
    ON_CALL(fakelm1.GetLane(1, 0), GetPrevious()).WillByDefault(Return(predecessorsLanes1_2));
    std::vector<OWL::Id> successorsLanes2_1 {fakelm2.GetLane(1, 0).GetId()};
    std::vector<OWL::Id> predecessorsLanes2_1 {fakelm1.GetLane(1, 0).GetId()};
    ON_CALL(fakelm2.GetLane(0, 0), GetNext()).WillByDefault(Return(successorsLanes2_1));
    ON_CALL(fakelm2.GetLane(0, 0), GetPrevious()).WillByDefault(Return(predecessorsLanes2_1));
    std::vector<OWL::Id> predecessorsLanes2_2 {fakelm2.GetLane(0, 0).GetId()};
    ON_CALL(fakelm2.GetLane(1, 0), GetPrevious()).WillByDefault(Return(predecessorsLanes2_2));

    std::vector<RouteElement> route {{"Road1", true}, {"Road2", true}};
    std::unordered_map<OWL::Id, OWL::Road*> roads {{road1Id, road1}, {road2Id, road2}, {road3Id, road3}};
    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    std::unordered_map<OWL::Id, std::string> roadIdMapping { {road1Id, "Road1"}, {road2Id, "Road2"}, {road3Id, "Road3"} };
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(fakelm2.GetLaneIdMapping()));
    std::unordered_map<OWL::Id, OWL::Interfaces::Lane*> lanes{ {fakelm1.GetLane(0, 0).GetId(), &fakelm1.GetLane(0, 0)},
                                                               {fakelm1.GetLane(1, 0).GetId(), &fakelm1.GetLane(1, 0)},
                                                               {fakelm3.GetLane(1, 0).GetId(), &fakelm3.GetLane(1, 0)}};
    ON_CALL(worldData, GetLanes()).WillByDefault(ReturnRef(lanes));
    WorldDataQuery wdQuery(worldData);

    auto laneStream = wdQuery.CreateLaneStream(route, "Road2", -1, 1.0);
    auto lanesInStream = laneStream->GetElements();
    ASSERT_THAT(lanesInStream.size(), Eq(4));
    ASSERT_THAT(lanesInStream.at(0).element, Eq(&fakelm1.GetLane(0,0)));
    ASSERT_THAT(lanesInStream.at(0).sOffset, DoubleEq(0.0));
    ASSERT_THAT(lanesInStream.at(0).inStreamDirection, Eq(true));
    ASSERT_THAT(lanesInStream.at(1).element, Eq(&fakelm1.GetLane(1,0)));
    ASSERT_THAT(lanesInStream.at(1).sOffset, DoubleEq(10.0));
    ASSERT_THAT(lanesInStream.at(1).inStreamDirection, Eq(true));
    ASSERT_THAT(lanesInStream.at(2).element, Eq(&fakelm2.GetLane(0,0)));
    ASSERT_THAT(lanesInStream.at(2).sOffset, DoubleEq(21.0));
    ASSERT_THAT(lanesInStream.at(2).inStreamDirection, Eq(true));
    ASSERT_THAT(lanesInStream.at(3).element, Eq(&fakelm2.GetLane(1,0)));
    ASSERT_THAT(lanesInStream.at(3).sOffset, DoubleEq(33.0));
    ASSERT_THAT(lanesInStream.at(3).inStreamDirection, Eq(true));
}

TEST(CreateLaneStream, ExcessRoudsInRoute)
{
    FakeLaneManager fakelm1{2, 2, 3.0, {10.0, 11.0}, ""};
    Fakes::Lane& startLane = fakelm1.GetLane(0, 0);
    auto road1 = fakelm1.GetRoads().cbegin()->second;
    auto road1Id = fakelm1.GetRoads().cbegin()->first;
    FakeLaneManager fakelm2{2, 2, 3.0, {12.0, 13.0}, ""};
    auto road2 = fakelm2.GetRoads().cbegin()->second;
    auto road2Id = fakelm2.GetRoads().cbegin()->first;
    std::vector<OWL::Id> successorsLanes1_1 {fakelm1.GetLane(1, 0).GetId()};
    ON_CALL(fakelm1.GetLane(0, 0), GetNext()).WillByDefault(Return(successorsLanes1_1));
    std::vector<OWL::Id> successorsLanes1_2 {fakelm2.GetLane(0, 0).GetId()};
    std::vector<OWL::Id> predecessorsLanes1_2 {fakelm1.GetLane(0, 0).GetId()};
    ON_CALL(fakelm1.GetLane(1, 0), GetNext()).WillByDefault(Return(successorsLanes1_2));
    ON_CALL(fakelm1.GetLane(1, 0), GetPrevious()).WillByDefault(Return(predecessorsLanes1_2));
    std::vector<OWL::Id> successorsLanes2_1 {fakelm2.GetLane(1, 0).GetId()};
    std::vector<OWL::Id> predecessorsLanes2_1 {fakelm1.GetLane(1, 0).GetId()};
    ON_CALL(fakelm2.GetLane(0, 0), GetNext()).WillByDefault(Return(successorsLanes2_1));
    ON_CALL(fakelm2.GetLane(0, 0), GetPrevious()).WillByDefault(Return(predecessorsLanes2_1));
    std::vector<OWL::Id> predecessorsLanes2_2 {fakelm2.GetLane(0, 0).GetId()};
    ON_CALL(fakelm2.GetLane(1, 0), GetPrevious()).WillByDefault(Return(predecessorsLanes2_2));
    Fakes::Road road0, road3;
    OWL::Id road0Id = 1000, road3Id = 1001;
    ON_CALL(road3, GetId()).WillByDefault(Return(road3Id));
    std::list<const OWL::Interfaces::Section*> emptySections{};
    ON_CALL(road3, GetSections()).WillByDefault(ReturnRef(emptySections));

    std::vector<RouteElement> route {{"Road0", true}, {"Road1", true}, {"Road2", true}, {"Road3", true}};
    std::unordered_map<OWL::Id, OWL::Road*> roads {{road0Id, &road0}, {road1Id, road1}, {road2Id, road2}, {road3Id, &road3}};
    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    std::unordered_map<OWL::Id, std::string> roadIdMapping { {road0Id, "Road0"}, {road1Id, "Road1"}, {road2Id, "Road2"}, {road3Id, "Road3"} };
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(fakelm1.GetLaneIdMapping()));
    WorldDataQuery wdQuery(worldData);

    auto laneStream = wdQuery.CreateLaneStream(route, "Road1", -1, 0.0);
    auto lanesInStream = laneStream->GetElements();
    ASSERT_THAT(lanesInStream.size(), Eq(4));
    ASSERT_THAT(lanesInStream.at(0).element, Eq(&startLane));
    ASSERT_THAT(lanesInStream.at(0).sOffset, DoubleEq(0.0));
    ASSERT_THAT(lanesInStream.at(0).inStreamDirection, Eq(true));
    ASSERT_THAT(lanesInStream.at(1).element, Eq(&fakelm1.GetLane(1,0)));
    ASSERT_THAT(lanesInStream.at(1).sOffset, DoubleEq(10.0));
    ASSERT_THAT(lanesInStream.at(1).inStreamDirection, Eq(true));
    ASSERT_THAT(lanesInStream.at(2).element, Eq(&fakelm2.GetLane(0,0)));
    ASSERT_THAT(lanesInStream.at(2).sOffset, DoubleEq(21.0));
    ASSERT_THAT(lanesInStream.at(2).inStreamDirection, Eq(true));
    ASSERT_THAT(lanesInStream.at(3).element, Eq(&fakelm2.GetLane(1,0)));
    ASSERT_THAT(lanesInStream.at(3).sOffset, DoubleEq(33.0));
    ASSERT_THAT(lanesInStream.at(3).inStreamDirection, Eq(true));
}

TEST(GetIntersectingConnections, NoIntersections_ReturnsEmptyVector)
{
    Fakes::WorldData worldData;
    Fakes::Junction fakeJunction;
    Fakes::Road connectionA;
    OWL::Interfaces::Roads connections{&connectionA};
    ON_CALL(fakeJunction, GetConnectingRoads()).WillByDefault(ReturnRef(connections));
    Id idJunction{1}, idConnectionA{2};
    ON_CALL(connectionA, GetId()).WillByDefault(Return(idConnectionA));
    std::map<std::string, std::vector<IntersectionInfo>> intersections{ {"ConnectionA", {} } };
    ON_CALL(fakeJunction, GetIntersections()).WillByDefault(ReturnRef(intersections));
    std::unordered_map<Id, Junction*> junctions{ {idJunction, &fakeJunction} };
    ON_CALL(worldData, GetJunctions()).WillByDefault(ReturnRef(junctions));
    std::unordered_map<Id, std::string> junctionIdMapping{ {idJunction, "Junction"}};
    ON_CALL(worldData, GetJunctionIdMapping()).WillByDefault(ReturnRef(junctionIdMapping));
    std::unordered_map<Id, Road*> roads{ {idConnectionA, &connectionA}};
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    std::unordered_map<Id, std::string> roadIdMapping{ {idConnectionA, "ConnectionA"}};
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));

    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetIntersectingConnections("ConnectionA");
    ASSERT_THAT(result.empty(), Eq(true));
}

TEST(GetIntersectingConnections, OneIntersectionWithLowerPriority_ReturnsIntersectionWithCorrectPriority)
{
    Fakes::WorldData worldData;
    Fakes::Junction fakeJunction;
    Fakes::Road connectionA, connectionB;
    OWL::Interfaces::Roads connections{&connectionA, &connectionB};
    ON_CALL(fakeJunction, GetConnectingRoads()).WillByDefault(ReturnRef(connections));
    Id idJunction{1}, idConnectionA{2}, idConnectionB{3};
    ON_CALL(connectionA, GetId()).WillByDefault(Return(idConnectionA));
    ON_CALL(connectionB, GetId()).WillByDefault(Return(idConnectionB));
    IntersectionInfo intersection{idConnectionB, IntersectingConnectionRank::Lower, {} };
    std::map<std::string, std::vector<IntersectionInfo>> intersections{ {"ConnectionA", {intersection} } };
    ON_CALL(fakeJunction, GetIntersections()).WillByDefault(ReturnRef(intersections));
    std::unordered_map<Id, Junction*> junctions{ {idJunction, &fakeJunction} };
    ON_CALL(worldData, GetJunctions()).WillByDefault(ReturnRef(junctions));
    std::unordered_map<Id, std::string> junctionIdMapping{ {idJunction, "Junction"}};
    ON_CALL(worldData, GetJunctionIdMapping()).WillByDefault(ReturnRef(junctionIdMapping));
    std::unordered_map<Id, Road*> roads{ {idConnectionA, &connectionA}, {idConnectionB, &connectionB} };
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    std::unordered_map<Id, std::string> roadIdMapping{ {idConnectionA, "ConnectionA"}, {idConnectionB, "ConnectionB"} };
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));

    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetIntersectingConnections("ConnectionA");
    ASSERT_THAT(result.size(), Eq(1));
    ASSERT_THAT(result.at(0).id, Eq("ConnectionB"));
    ASSERT_THAT(result.at(0).rank, Eq(IntersectingConnectionRank::Lower));
}

TEST(GetIntersectingConnections, OneIntersectionWithHigherPriority_ReturnsIntersectionWithCorrectPriority)
{
    Fakes::WorldData worldData;
    Fakes::Junction fakeJunction;
    Fakes::Road connectionA, connectionB;
    OWL::Interfaces::Roads connections{&connectionA, &connectionB};
    ON_CALL(fakeJunction, GetConnectingRoads()).WillByDefault(ReturnRef(connections));
    Id idJunction{1}, idConnectionA{2}, idConnectionB{3};
    ON_CALL(connectionA, GetId()).WillByDefault(Return(idConnectionA));
    ON_CALL(connectionB, GetId()).WillByDefault(Return(idConnectionB));
    IntersectionInfo intersection{idConnectionB, IntersectingConnectionRank::Higher, {} };
    std::map<std::string, std::vector<IntersectionInfo>> intersections{ {"ConnectionA", {intersection} } };
    ON_CALL(fakeJunction, GetIntersections()).WillByDefault(ReturnRef(intersections));
    std::unordered_map<Id, Junction*> junctions{ {idJunction, &fakeJunction} };
    ON_CALL(worldData, GetJunctions()).WillByDefault(ReturnRef(junctions));
    std::unordered_map<Id, std::string> junctionIdMapping{ {idJunction, "Junction"}};
    ON_CALL(worldData, GetJunctionIdMapping()).WillByDefault(ReturnRef(junctionIdMapping));
    std::unordered_map<Id, Road*> roads{ {idConnectionA, &connectionA}, {idConnectionB, &connectionB} };
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    std::unordered_map<Id, std::string> roadIdMapping{ {idConnectionA, "ConnectionA"}, {idConnectionB, "ConnectionB"} };
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));

    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetIntersectingConnections("ConnectionA");
    ASSERT_THAT(result, SizeIs(1));
    ASSERT_THAT(result.at(0).id, Eq("ConnectionB"));
    ASSERT_THAT(result.at(0).rank, Eq(IntersectingConnectionRank::Higher));
}

TEST(GetIntersectingConnections, TwoIntersections_ReturnsBothIntersections)
{
    Fakes::WorldData worldData;
    Fakes::Junction fakeJunction;
    Fakes::Road connectionA, connectionB, connectionC;
    OWL::Interfaces::Roads connections{&connectionA, &connectionB, &connectionC};
    ON_CALL(fakeJunction, GetConnectingRoads()).WillByDefault(ReturnRef(connections));
    Id idJunction{1}, idConnectionA{2}, idConnectionB{3}, idConnectionC{4};
    ON_CALL(connectionA, GetId()).WillByDefault(Return(idConnectionA));
    ON_CALL(connectionB, GetId()).WillByDefault(Return(idConnectionB));
    ON_CALL(connectionC, GetId()).WillByDefault(Return(idConnectionC));
    IntersectionInfo intersection1{idConnectionB, IntersectingConnectionRank::Lower, {} };
    IntersectionInfo intersection2{idConnectionC, IntersectingConnectionRank::Higher, {} };
    std::map<std::string, std::vector<IntersectionInfo>> intersections{ {"ConnectionA", {intersection1, intersection2} } };
    ON_CALL(fakeJunction, GetIntersections()).WillByDefault(ReturnRef(intersections));
    std::unordered_map<Id, Junction*> junctions{ {idJunction, &fakeJunction} };
    ON_CALL(worldData, GetJunctions()).WillByDefault(ReturnRef(junctions));
    std::unordered_map<Id, std::string> junctionIdMapping{ {idJunction, "Junction"}};
    ON_CALL(worldData, GetJunctionIdMapping()).WillByDefault(ReturnRef(junctionIdMapping));
    std::unordered_map<Id, Road*> roads{ {idConnectionA, &connectionA}, {idConnectionB, &connectionB}, {idConnectionC, &connectionC} };
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    std::unordered_map<Id, std::string> roadIdMapping{ {idConnectionA, "ConnectionA"}, {idConnectionB, "ConnectionB"}, {idConnectionC, "ConnectionC"} };
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));

    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetIntersectingConnections("ConnectionA");
    ASSERT_THAT(result, SizeIs(2));
    ASSERT_THAT(result, UnorderedElementsAre(IntersectingConnection{"ConnectionB", IntersectingConnectionRank::Lower},
                                             IntersectingConnection{"ConnectionC", IntersectingConnectionRank::Higher}));
}

class GetMovingObjectsInRangeOfJunctionConnection : public ::testing::Test
{
public:
    GetMovingObjectsInRangeOfJunctionConnection()
    {
        ON_CALL(junction, GetId()).WillByDefault(Return(idJunction));
        ON_CALL(junction, GetConnectingRoads()).WillByDefault(ReturnRef(connectingRoads));
        ON_CALL(connectingRoad, GetId()).WillByDefault(Return(idConnectingRoad));
        ON_CALL(incomingRoad, GetId()).WillByDefault(Return(idIncomingRoad));
        ON_CALL(connectingRoad, IsInStreamDirection()).WillByDefault(Return(true));
        ON_CALL(connectingRoad, GetPredecessor()).WillByDefault(Return(idIncomingRoad));
        ON_CALL(connectingRoad, GetSections()).WillByDefault(ReturnRef(connectingSections));
        ON_CALL(connectingSection, GetLanes()).WillByDefault(ReturnRef(connectingLanes));
        ON_CALL(connectingSection, Covers(_)).WillByDefault(Return(true));
        ON_CALL(connectingLane1, GetId()).WillByDefault(Return(idConnectingLane1));
        ON_CALL(connectingLane1, GetLaneType()).WillByDefault(Return(LaneType::Driving));
        ON_CALL(connectingLane1, GetRoad()).WillByDefault(ReturnRef(connectingRoad));
        ON_CALL(connectingLane1, GetLength()).WillByDefault(Return(10.0));
        ON_CALL(connectingLane2, GetId()).WillByDefault(Return(idConnectingLane2));
        ON_CALL(connectingLane2, GetLaneType()).WillByDefault(Return(LaneType::Driving));
        ON_CALL(connectingLane2, GetRoad()).WillByDefault(ReturnRef(connectingRoad));
        ON_CALL(connectingLane2, GetLength()).WillByDefault(Return(10.0));
        ON_CALL(incomingRoad, GetSections()).WillByDefault(ReturnRef(incomingSections));
        ON_CALL(incomingRoad, IsInStreamDirection()).WillByDefault(Return(true));
        ON_CALL(incomingRoad, GetSuccessor()).WillByDefault(Return(idJunction));
        ON_CALL(incomingSection, GetLanes()).WillByDefault(ReturnRef(incomingLanes));
        ON_CALL(incomingLane1, GetId()).WillByDefault(Return(idIncomingLane1));
        ON_CALL(connectingLane1, GetPrevious()).WillByDefault(Return(std::vector<OWL::Id>{idIncomingLane1}));
        ON_CALL(incomingLane1, GetNext()).WillByDefault(Return(std::vector<OWL::Id>{idConnectingLane1}));
        ON_CALL(incomingLane1, GetRoad()).WillByDefault(ReturnRef(incomingRoad));
        ON_CALL(incomingLane1, GetLength()).WillByDefault(Return(1000.0));
        ON_CALL(incomingLane2, GetId()).WillByDefault(Return(idIncomingLane2));
        ON_CALL(connectingLane2, GetPrevious()).WillByDefault(Return(std::vector<OWL::Id>{idIncomingLane2}));
        ON_CALL(incomingLane2, GetNext()).WillByDefault(Return(std::vector<OWL::Id>{idConnectingLane2}));
        ON_CALL(incomingLane2, GetRoad()).WillByDefault(ReturnRef(incomingRoad));
        ON_CALL(incomingLane2, GetLength()).WillByDefault(Return(1000.0));

        ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
        ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
        ON_CALL(worldData, GetJunctions()).WillByDefault(ReturnRef(junctions));
        ON_CALL(worldData, GetJunctionIdMapping()).WillByDefault(ReturnRef(junctionIdMapping));
        ON_CALL(worldData, GetLanes()).WillByDefault(ReturnRef(lanes));
        ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(laneIdMapping));
    }

    Fakes::WorldData worldData;
    Fakes::Road connectingRoad, incomingRoad;
    Fakes::Section connectingSection, incomingSection;
    Fakes::Lane connectingLane1, connectingLane2, incomingLane1, incomingLane2;
    Fakes::Junction junction;
    OWL::Id idConnectingRoad = 1, idIncomingRoad = 2, idJunction = 3, idConnectingLane1 = 11, idConnectingLane2 = 12, idIncomingLane1 = 13, idIncomingLane2 = 14;
    std::list<const OWL::Interfaces::Road*> connectingRoads {&connectingRoad};
    std::list<const OWL::Interfaces::Section*> connectingSections {&connectingSection};
    std::list<const OWL::Interfaces::Lane*> connectingLanes { {&connectingLane1, &connectingLane2} };
    std::list<const OWL::Interfaces::Section*> incomingSections {&incomingSection};
    std::list<const OWL::Interfaces::Lane*> incomingLanes { {&incomingLane1, &incomingLane2} };
    std::unordered_map<Id, Road*> roads{ {idConnectingRoad, &connectingRoad}, {idIncomingRoad, &incomingRoad} };
    std::unordered_map<Id, std::string> roadIdMapping{ {idConnectingRoad, "Connection"}, {idIncomingRoad, "IncomingRoad"} };
    std::unordered_map<Id, Junction*> junctions { {idJunction, &junction} };
    std::unordered_map<Id, std::string> junctionIdMapping{ {idJunction, "Junction"}};
    std::unordered_map<Id, Lane*> lanes{ {idConnectingLane1, &connectingLane1}, {idConnectingLane2, &connectingLane2}, {idIncomingLane1, &incomingLane1}, {idIncomingLane2, &incomingLane2} };
    std::unordered_map<Id, OdId> laneIdMapping{ {idConnectingLane1, -1}, {idConnectingLane2, -2} };
};

TEST_F(GetMovingObjectsInRangeOfJunctionConnection, NoAgents_ReturnsEmptyVector)
{
    OWL::Interfaces::WorldObjects emptyWorldObjects;
    ON_CALL(connectingLane1, GetWorldObjects()).WillByDefault(ReturnRef(emptyWorldObjects));
    ON_CALL(connectingLane2, GetWorldObjects()).WillByDefault(ReturnRef(emptyWorldObjects));
    ON_CALL(incomingLane1, GetWorldObjects()).WillByDefault(ReturnRef(emptyWorldObjects));
    ON_CALL(incomingLane2, GetWorldObjects()).WillByDefault(ReturnRef(emptyWorldObjects));

    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetMovingObjectsInRangeOfJunctionConnection("Connection", 100.0);

    ASSERT_THAT(result.empty(), Eq(true));
}

TEST_F(GetMovingObjectsInRangeOfJunctionConnection, OneAgentsOnConnector_ReturnsThisAgent)
{
    OWL::Fakes::MovingObject object;
    ON_CALL(object, GetDistance(_,"Connection")).WillByDefault(Return(5.0));
    OWL::Interfaces::WorldObjects worldObjectsOnConnector{&object};
    OWL::Interfaces::WorldObjects emptyWorldObjects;
    ON_CALL(connectingLane1, GetWorldObjects()).WillByDefault(ReturnRef(emptyWorldObjects));
    ON_CALL(connectingLane2, GetWorldObjects()).WillByDefault(ReturnRef(worldObjectsOnConnector));
    ON_CALL(incomingLane1, GetWorldObjects()).WillByDefault(ReturnRef(emptyWorldObjects));
    ON_CALL(incomingLane2, GetWorldObjects()).WillByDefault(ReturnRef(emptyWorldObjects));

    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetMovingObjectsInRangeOfJunctionConnection("Connection", 100.0);

    ASSERT_THAT(result, SizeIs(1));
    ASSERT_THAT(result.at(0), Eq(&object));
}

TEST_F(GetMovingObjectsInRangeOfJunctionConnection, TwoAgentsOnInComingRoad_ReturnsAgentInRange)
{
    OWL::Fakes::MovingObject objectInRange, objectOutsideRange;
    ON_CALL(objectInRange, GetDistance(_,"IncomingRoad")).WillByDefault(Return(950.0));
    ON_CALL(objectOutsideRange, GetDistance(_,"IncomingRoad")).WillByDefault(Return(850.0));
    OWL::Interfaces::WorldObjects worldObjectsOnInComingRoad{&objectInRange, &objectOutsideRange};
    OWL::Interfaces::WorldObjects emptyWorldObjects;
    ON_CALL(connectingLane1, GetWorldObjects()).WillByDefault(ReturnRef(emptyWorldObjects));
    ON_CALL(connectingLane2, GetWorldObjects()).WillByDefault(ReturnRef(emptyWorldObjects));
    ON_CALL(incomingLane1, GetWorldObjects()).WillByDefault(ReturnRef(worldObjectsOnInComingRoad));
    ON_CALL(incomingLane2, GetWorldObjects()).WillByDefault(ReturnRef(emptyWorldObjects));

    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetMovingObjectsInRangeOfJunctionConnection("Connection", 100.0);

    ASSERT_THAT(result, SizeIs(1));
    ASSERT_THAT(result.at(0), Eq(&objectInRange));
}

TEST_F(GetMovingObjectsInRangeOfJunctionConnection, OneAgentsOnBothConnectors_ReturnsThisAgentOnlyOnce)
{
    OWL::Fakes::MovingObject object;
    ON_CALL(object, GetDistance(_,"Connection")).WillByDefault(Return(5.0));
    OWL::Interfaces::WorldObjects worldObjectsOnConnector{&object};
    OWL::Interfaces::WorldObjects emptyWorldObjects;
    ON_CALL(connectingLane1, GetWorldObjects()).WillByDefault(ReturnRef(worldObjectsOnConnector));
    ON_CALL(connectingLane2, GetWorldObjects()).WillByDefault(ReturnRef(worldObjectsOnConnector));
    ON_CALL(incomingLane1, GetWorldObjects()).WillByDefault(ReturnRef(emptyWorldObjects));
    ON_CALL(incomingLane2, GetWorldObjects()).WillByDefault(ReturnRef(emptyWorldObjects));

    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetMovingObjectsInRangeOfJunctionConnection("Connection", 100.0);

    ASSERT_THAT(result, SizeIs(1));
    ASSERT_THAT(result.at(0), Eq(&object));
}

TEST_F(GetMovingObjectsInRangeOfJunctionConnection, OneAgentsOnBothIncomingRoads_ReturnsThisAgentOnlyOnce)
{
    OWL::Fakes::MovingObject object;
    ON_CALL(object, GetDistance(_,"IncomingRoad")).WillByDefault(Return(950.0));
    OWL::Interfaces::WorldObjects worldObjectsOnInComingRoad{&object};
    OWL::Interfaces::WorldObjects emptyWorldObjects;
    ON_CALL(connectingLane1, GetWorldObjects()).WillByDefault(ReturnRef(emptyWorldObjects));
    ON_CALL(connectingLane2, GetWorldObjects()).WillByDefault(ReturnRef(emptyWorldObjects));
    ON_CALL(incomingLane1, GetWorldObjects()).WillByDefault(ReturnRef(worldObjectsOnInComingRoad));
    ON_CALL(incomingLane2, GetWorldObjects()).WillByDefault(ReturnRef(worldObjectsOnInComingRoad));

    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetMovingObjectsInRangeOfJunctionConnection("Connection", 100.0);

    ASSERT_THAT(result, SizeIs(1));
    ASSERT_THAT(result.at(0), Eq(&object));
}

TEST_F(GetMovingObjectsInRangeOfJunctionConnection, OneAgentsOnConnectorAndIncomingRoads_ReturnsThisAgentOnlyOnce)
{
    OWL::Fakes::MovingObject object;
    ON_CALL(object, GetDistance(_,_)).WillByDefault(Return(5.0));
    OWL::Interfaces::WorldObjects worldObjectsOnConnector{&object};
    OWL::Interfaces::WorldObjects emptyWorldObjects;
    ON_CALL(connectingLane1, GetWorldObjects()).WillByDefault(ReturnRef(worldObjectsOnConnector));
    ON_CALL(connectingLane2, GetWorldObjects()).WillByDefault(ReturnRef(emptyWorldObjects));
    ON_CALL(incomingLane1, GetWorldObjects()).WillByDefault(ReturnRef(worldObjectsOnConnector));
    ON_CALL(incomingLane2, GetWorldObjects()).WillByDefault(ReturnRef(emptyWorldObjects));

    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetMovingObjectsInRangeOfJunctionConnection("Connection", 100.0);

    ASSERT_THAT(result, SizeIs(1));
    ASSERT_THAT(result.at(0), Eq(&object));
}

TEST_F(GetMovingObjectsInRangeOfJunctionConnection, OneAgentsOnEachConnector_ReturnsBoth)
{
    OWL::Fakes::MovingObject object1, object2;
    ON_CALL(object1, GetDistance(_,"Connection")).WillByDefault(Return(5.0));
    ON_CALL(object2, GetDistance(_,"Connection")).WillByDefault(Return(5.0));
    OWL::Interfaces::WorldObjects worldObjectsOnConnector1{&object1};
    OWL::Interfaces::WorldObjects worldObjectsOnConnector2{&object2};
    OWL::Interfaces::WorldObjects emptyWorldObjects;
    ON_CALL(connectingLane1, GetWorldObjects()).WillByDefault(ReturnRef(worldObjectsOnConnector1));
    ON_CALL(connectingLane2, GetWorldObjects()).WillByDefault(ReturnRef(worldObjectsOnConnector2));
    ON_CALL(incomingLane1, GetWorldObjects()).WillByDefault(ReturnRef(emptyWorldObjects));
    ON_CALL(incomingLane2, GetWorldObjects()).WillByDefault(ReturnRef(emptyWorldObjects));

    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetMovingObjectsInRangeOfJunctionConnection("Connection", 100.0);

    ASSERT_THAT(result, SizeIs(2));
    ASSERT_THAT(result, UnorderedElementsAre(&object1, &object2));
}

TEST(GetDistanceUntilMovingObjectEntersConnector, ObjectOnConnector_ReturnsCorrectDistance)
{
    OWL::Fakes::WorldData worldData;
    OWL::Fakes::Junction junction;
    OWL::Fakes::Road connectorA, connectorB, incomingRoad;
    OWL::Fakes::Section sectionA, sectionB;
    OWL::Fakes::Lane laneA, laneB;
    OWL::Id idJunction = 1, idConnectorA = 2, idConnectorB = 3, idIncomingRoad = 4, idLaneA = 11, idLaneB = 12;
    ON_CALL(junction, GetId()).WillByDefault(Return(idJunction));
    OWL::Interfaces::Roads connectingRoads{&connectorA, &connectorB};
    ON_CALL(junction, GetConnectingRoads()).WillByDefault(ReturnRef(connectingRoads));
    ON_CALL(connectorA, GetId()).WillByDefault(Return(idConnectorA));
    OWL::Interfaces::Sections sectionsA{&sectionA};
    ON_CALL(connectorA, GetSections()).WillByDefault(ReturnRef(sectionsA));
    OWL::Interfaces::Lanes lanesA{&laneA};
    ON_CALL(sectionA, GetLanes()).WillByDefault(ReturnRef(lanesA));
    ON_CALL(sectionA, Covers(_)).WillByDefault(Return(true));
    ON_CALL(laneA, GetId()).WillByDefault(Return(idLaneA));
    ON_CALL(laneA, GetRoad()).WillByDefault(ReturnRef(connectorA));
    ON_CALL(connectorB, IsInStreamDirection()).WillByDefault(Return(true));
    ON_CALL(connectorB, GetPredecessor()).WillByDefault(Return(idIncomingRoad));
    ON_CALL(connectorB, GetId()).WillByDefault(Return(idConnectorB));
    OWL::Interfaces::Sections sectionsB{&sectionB};
    ON_CALL(connectorB, GetSections()).WillByDefault(ReturnRef(sectionsB));
    OWL::Interfaces::Lanes lanesB{&laneB};
    ON_CALL(sectionB, GetLanes()).WillByDefault(ReturnRef(lanesB));
    ON_CALL(sectionB, Covers(_)).WillByDefault(Return(true));
    ON_CALL(laneB, GetId()).WillByDefault(Return(idLaneB));
    ON_CALL(laneB, GetRoad()).WillByDefault(ReturnRef(connectorB));
    OWL::Interfaces::Roads connections{&connectorA, &connectorB};
    ON_CALL(junction, GetConnectingRoads()).WillByDefault(ReturnRef(connections));
    std::unordered_map<OWL::Id, OWL::Interfaces::Road*> roads{ {idConnectorA, &connectorA}, {idConnectorB, &connectorB}, {idIncomingRoad, &incomingRoad} };
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    std::unordered_map<OWL::Id, std::string> roadIdMapping{ {idConnectorA, "ConnectorA"}, {idConnectorB, "ConnectorB"}, {idIncomingRoad, "IncomingRoad"}};
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    std::unordered_map<OWL::Id, OWL::Interfaces::Junction*> junctions{ {idJunction, &junction} };
    ON_CALL(worldData, GetJunctions()).WillByDefault(ReturnRef(junctions));
    std::unordered_map<OWL::Id, std::string> junctionIdMapping{ {idJunction, "Junction"}};
    ON_CALL(worldData, GetJunctionIdMapping()).WillByDefault(ReturnRef(junctionIdMapping));
    std::unordered_map<OWL::Id, OWL::Interfaces::Lane*> lanes{ {idLaneA, &laneA}, {idLaneB, &laneB} };
    ON_CALL(worldData, GetLanes()).WillByDefault(ReturnRef(lanes));
    std::unordered_map<OWL::Id, OWL::OdId> laneIdMapping{ {idLaneA, -1}, {idLaneB, -1} };
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(laneIdMapping));
    IntersectionInfo intersection{idConnectorB, IntersectingConnectionRank::Undefined, { { {idLaneA, idLaneB}, {10.0, 15.0} } } };
    std::map<std::string, std::vector<IntersectionInfo>> intersections{ {"ConnectorA", {intersection} } };
    ON_CALL(junction, GetIntersections()).WillByDefault(ReturnRef(intersections));

    ObjectPosition objectPosition;
    objectPosition.mainLocatePoint.roadId = "ConnectorB";
    objectPosition.mainLocatePoint.laneId = -1;
    objectPosition.mainLocatePoint.roadPosition.s = 5.0;
    objectPosition.touchedRoads["ConnectorB"].sStart = 4.0;
    objectPosition.touchedRoads["ConnectorB"].sEnd = 7.0;

    WorldDataQuery wdQuery(worldData);

    auto distance = wdQuery.GetDistanceUntilObjectEntersConnector(objectPosition, "ConnectorA", -1, "ConnectorB");
    ASSERT_THAT(distance, Eq(3.0));
}

TEST(GetDistanceUntilMovingObjectEntersConnector, ObjectOnIncomingRoad_ReturnsCorrectDistance)
{
    OWL::Fakes::WorldData worldData;
    OWL::Fakes::Junction junction;
    OWL::Fakes::Road connectorA, connectorB, incomingRoad;
    OWL::Fakes::Section sectionA, sectionB, incomingSection;
    OWL::Fakes::Lane laneA, laneB, incomingLane;
    OWL::Id idJunction = 1, idConnectorA = 2, idConnectorB = 3, idIncomingRoad = 4, idLaneA = 11, idLaneB = 12, idIncomingLane = 13;
    ON_CALL(junction, GetId()).WillByDefault(Return(idJunction));
    OWL::Interfaces::Roads connectingRoads{&connectorA, &connectorB};
    ON_CALL(junction, GetConnectingRoads()).WillByDefault(ReturnRef(connectingRoads));
    ON_CALL(connectorA, GetId()).WillByDefault(Return(idConnectorA));
    OWL::Interfaces::Sections sectionsA{&sectionA};
    ON_CALL(connectorA, GetSections()).WillByDefault(ReturnRef(sectionsA));
    OWL::Interfaces::Lanes lanesA{&laneA};
    ON_CALL(sectionA, GetLanes()).WillByDefault(ReturnRef(lanesA));
    ON_CALL(sectionA, Covers(_)).WillByDefault(Return(true));
    ON_CALL(laneA, GetId()).WillByDefault(Return(idLaneA));
    ON_CALL(laneA, GetRoad()).WillByDefault(ReturnRef(connectorA));
    ON_CALL(connectorB, IsInStreamDirection()).WillByDefault(Return(true));
    ON_CALL(connectorB, GetPredecessor()).WillByDefault(Return(idIncomingRoad));
    ON_CALL(connectorB, GetId()).WillByDefault(Return(idConnectorB));
    OWL::Interfaces::Sections sectionsB{&sectionB};
    ON_CALL(connectorB, GetSections()).WillByDefault(ReturnRef(sectionsB));
    OWL::Interfaces::Lanes lanesB{&laneB};
    ON_CALL(sectionB, GetLanes()).WillByDefault(ReturnRef(lanesB));
    ON_CALL(sectionB, Covers(_)).WillByDefault(Return(true));
    ON_CALL(laneB, GetId()).WillByDefault(Return(idLaneB));
    ON_CALL(laneB, GetRoad()).WillByDefault(ReturnRef(connectorB));
    ON_CALL(laneB, GetPrevious()).WillByDefault(Return(std::vector<OWL::Id>{idIncomingLane}));
    ON_CALL(incomingRoad, GetId()).WillByDefault(Return(idIncomingRoad));
    OWL::Interfaces::Sections incomingSections{&incomingSection};
    ON_CALL(incomingRoad, GetSections()).WillByDefault(ReturnRef(incomingSections));
    OWL::Interfaces::Lanes incomingLanes{&incomingLane};
    ON_CALL(incomingSection, GetLanes()).WillByDefault(ReturnRef(incomingLanes));
    ON_CALL(incomingSection, Covers(_)).WillByDefault(Return(true));
    ON_CALL(incomingLane, GetId()).WillByDefault(Return(idIncomingLane));
    ON_CALL(incomingLane, GetLength()).WillByDefault(Return(100.0));
    ON_CALL(incomingLane, GetNext()).WillByDefault(Return(std::vector<OWL::Id>{idLaneB}));
    ON_CALL(incomingLane, GetRoad()).WillByDefault(ReturnRef(incomingRoad));
    ON_CALL(incomingRoad, IsInStreamDirection()).WillByDefault(Return(true));
    ON_CALL(incomingRoad, GetSuccessor()).WillByDefault(Return(idJunction));
    OWL::Interfaces::Roads connections{&connectorA, &connectorB};
    ON_CALL(junction, GetConnectingRoads()).WillByDefault(ReturnRef(connections));
    std::unordered_map<OWL::Id, OWL::Interfaces::Road*> roads{ {idConnectorA, &connectorA}, {idConnectorB, &connectorB}, {idIncomingRoad, &incomingRoad} };
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    std::unordered_map<OWL::Id, std::string> roadIdMapping{ {idConnectorA, "ConnectorA"}, {idConnectorB, "ConnectorB"}, {idIncomingRoad, "IncomingRoad"} };
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    std::unordered_map<OWL::Id, OWL::Interfaces::Junction*> junctions{ {idJunction, &junction} };
    ON_CALL(worldData, GetJunctions()).WillByDefault(ReturnRef(junctions));
    std::unordered_map<OWL::Id, std::string> junctionIdMapping{ {idJunction, "Junction"}};
    ON_CALL(worldData, GetJunctionIdMapping()).WillByDefault(ReturnRef(junctionIdMapping));
    std::unordered_map<OWL::Id, OWL::Interfaces::Lane*> lanes{ {idLaneA, &laneA}, {idLaneB, &laneB} };
    ON_CALL(worldData, GetLanes()).WillByDefault(ReturnRef(lanes));
    std::unordered_map<OWL::Id, OWL::OdId> laneIdMapping{ {idLaneA, -1}, {idLaneB, -1}, {idIncomingLane, -1} };
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(laneIdMapping));
    IntersectionInfo intersection{idConnectorB, IntersectingConnectionRank::Undefined, { { {idLaneA, idLaneB}, {10.0, 15.0} } } };
    std::map<std::string, std::vector<IntersectionInfo>> intersections{ {"ConnectorA", {intersection} } };
    ON_CALL(junction, GetIntersections()).WillByDefault(ReturnRef(intersections));

    ObjectPosition objectPosition;
    objectPosition.mainLocatePoint.roadId = "IncomingRoad";
    objectPosition.mainLocatePoint.laneId = -1;
    objectPosition.mainLocatePoint.roadPosition.s = 5.0;
    objectPosition.touchedRoads["IncomingRoad"].sStart = 4.0;
    objectPosition.touchedRoads["IncomingRoad"].sEnd = 7.0;

    WorldDataQuery wdQuery(worldData);

    auto distance = wdQuery.GetDistanceUntilObjectEntersConnector(objectPosition, "ConnectorA", -1, "ConnectorB");
    ASSERT_THAT(distance, Eq(103.0));
}

TEST(GetDistanceUntilMovingObjectLeavesConnector, ObjectOnConnector_ReturnsCorrectDistance)
{
    OWL::Fakes::WorldData worldData;
    OWL::Fakes::Junction junction;
    OWL::Fakes::Road connectorA, connectorB, incomingRoad;
    OWL::Fakes::Section sectionA, sectionB;
    OWL::Fakes::Lane laneA, laneB;
    OWL::Id idJunction = 1, idConnectorA = 2, idConnectorB = 3, idIncomingRoad = 4, idLaneA = 11, idLaneB = 12;
    ON_CALL(junction, GetId()).WillByDefault(Return(idJunction));
    OWL::Interfaces::Roads connectingRoads{&connectorA, &connectorB};
    ON_CALL(junction, GetConnectingRoads()).WillByDefault(ReturnRef(connectingRoads));
    ON_CALL(connectorA, GetId()).WillByDefault(Return(idConnectorA));
    OWL::Interfaces::Sections sectionsA{&sectionA};
    ON_CALL(connectorA, GetSections()).WillByDefault(ReturnRef(sectionsA));
    OWL::Interfaces::Lanes lanesA{&laneA};
    ON_CALL(sectionA, GetLanes()).WillByDefault(ReturnRef(lanesA));
    ON_CALL(sectionA, Covers(_)).WillByDefault(Return(true));
    ON_CALL(laneA, GetId()).WillByDefault(Return(idLaneA));
    ON_CALL(laneA, GetRoad()).WillByDefault(ReturnRef(connectorA));
    ON_CALL(connectorB, IsInStreamDirection()).WillByDefault(Return(true));
    ON_CALL(connectorB, GetPredecessor()).WillByDefault(Return(idIncomingRoad));
    ON_CALL(connectorB, GetId()).WillByDefault(Return(idConnectorB));
    OWL::Interfaces::Sections sectionsB{&sectionB};
    ON_CALL(connectorB, GetSections()).WillByDefault(ReturnRef(sectionsB));
    OWL::Interfaces::Lanes lanesB{&laneB};
    ON_CALL(sectionB, GetLanes()).WillByDefault(ReturnRef(lanesB));
    ON_CALL(sectionB, Covers(_)).WillByDefault(Return(true));
    ON_CALL(laneB, GetId()).WillByDefault(Return(idLaneB));
    ON_CALL(laneB, GetRoad()).WillByDefault(ReturnRef(connectorB));
    OWL::Interfaces::Roads connections{&connectorA, &connectorB};
    ON_CALL(junction, GetConnectingRoads()).WillByDefault(ReturnRef(connections));
    std::unordered_map<OWL::Id, OWL::Interfaces::Road*> roads{ {idConnectorA, &connectorA}, {idConnectorB, &connectorB}, {idIncomingRoad, &incomingRoad} };
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    std::unordered_map<OWL::Id, std::string> roadIdMapping{ {idConnectorA, "ConnectorA"}, {idConnectorB, "ConnectorB"}, {idIncomingRoad, "IncomingRoad"}};
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    std::unordered_map<OWL::Id, OWL::Interfaces::Junction*> junctions{ {idJunction, &junction} };
    ON_CALL(worldData, GetJunctions()).WillByDefault(ReturnRef(junctions));
    std::unordered_map<OWL::Id, std::string> junctionIdMapping{ {idJunction, "Junction"}};
    ON_CALL(worldData, GetJunctionIdMapping()).WillByDefault(ReturnRef(junctionIdMapping));
    std::unordered_map<OWL::Id, OWL::Interfaces::Lane*> lanes{ {idLaneA, &laneA}, {idLaneB, &laneB} };
    ON_CALL(worldData, GetLanes()).WillByDefault(ReturnRef(lanes));
    std::unordered_map<OWL::Id, OWL::OdId> laneIdMapping{ {idLaneA, -1}, {idLaneB, -1} };
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(laneIdMapping));
    IntersectionInfo intersection{idConnectorB, IntersectingConnectionRank::Undefined, { { {idLaneA, idLaneB}, {10.0, 15.0} } } };
    std::map<std::string, std::vector<IntersectionInfo>> intersections{ {"ConnectorA", {intersection} } };
    ON_CALL(junction, GetIntersections()).WillByDefault(ReturnRef(intersections));

    ObjectPosition objectPosition;
    objectPosition.mainLocatePoint.roadId = "ConnectorB";
    objectPosition.mainLocatePoint.laneId = -1;
    objectPosition.mainLocatePoint.roadPosition.s = 5.0;
    objectPosition.touchedRoads["ConnectorB"].sStart = 4.0;
    objectPosition.touchedRoads["ConnectorB"].sEnd = 7.0;

    WorldDataQuery wdQuery(worldData);

    auto distance = wdQuery.GetDistanceUntilObjectLeavesConnector(objectPosition, "ConnectorA", -1, "ConnectorB");
    ASSERT_THAT(distance, Eq(11.0));
}

TEST(GetDistanceUntilMovingObjectLeavesConnector, ObjectOnIncomingRoad_ReturnsCorrectDistance)
{
    OWL::Fakes::WorldData worldData;
    OWL::Fakes::Junction junction;
    OWL::Fakes::Road connectorA, connectorB, incomingRoad;
    OWL::Fakes::Section sectionA, sectionB, incomingSection;
    OWL::Fakes::Lane laneA, laneB, incomingLane;
    OWL::Id idJunction = 1, idConnectorA = 2, idConnectorB = 3, idIncomingRoad = 4, idLaneA = 11, idLaneB = 12, idIncomingLane = 13;
    ON_CALL(junction, GetId()).WillByDefault(Return(idJunction));
    OWL::Interfaces::Roads connectingRoads{&connectorA, &connectorB};
    ON_CALL(junction, GetConnectingRoads()).WillByDefault(ReturnRef(connectingRoads));
    ON_CALL(connectorA, GetId()).WillByDefault(Return(idConnectorA));
    OWL::Interfaces::Sections sectionsA{&sectionA};
    ON_CALL(connectorA, GetSections()).WillByDefault(ReturnRef(sectionsA));
    OWL::Interfaces::Lanes lanesA{&laneA};
    ON_CALL(sectionA, GetLanes()).WillByDefault(ReturnRef(lanesA));
    ON_CALL(sectionA, Covers(_)).WillByDefault(Return(true));
    ON_CALL(laneA, GetId()).WillByDefault(Return(idLaneA));
    ON_CALL(laneA, GetRoad()).WillByDefault(ReturnRef(connectorA));
    ON_CALL(connectorB, IsInStreamDirection()).WillByDefault(Return(true));
    ON_CALL(connectorB, GetPredecessor()).WillByDefault(Return(idIncomingRoad));
    ON_CALL(connectorB, GetId()).WillByDefault(Return(idConnectorB));
    OWL::Interfaces::Sections sectionsB{&sectionB};
    ON_CALL(connectorB, GetSections()).WillByDefault(ReturnRef(sectionsB));
    OWL::Interfaces::Lanes lanesB{&laneB};
    ON_CALL(sectionB, GetLanes()).WillByDefault(ReturnRef(lanesB));
    ON_CALL(sectionB, Covers(_)).WillByDefault(Return(true));
    ON_CALL(laneB, GetId()).WillByDefault(Return(idLaneB));
    ON_CALL(laneB, GetRoad()).WillByDefault(ReturnRef(connectorB));
    ON_CALL(laneB, GetPrevious()).WillByDefault(Return(std::vector<OWL::Id>{idIncomingLane}));
    ON_CALL(incomingRoad, GetId()).WillByDefault(Return(idIncomingRoad));
    OWL::Interfaces::Sections incomingSections{&incomingSection};
    ON_CALL(incomingRoad, GetSections()).WillByDefault(ReturnRef(incomingSections));
    OWL::Interfaces::Lanes incomingLanes{&incomingLane};
    ON_CALL(incomingSection, GetLanes()).WillByDefault(ReturnRef(incomingLanes));
    ON_CALL(incomingSection, Covers(_)).WillByDefault(Return(true));
    ON_CALL(incomingLane, GetId()).WillByDefault(Return(idIncomingLane));
    ON_CALL(incomingLane, GetLength()).WillByDefault(Return(100.0));
    ON_CALL(incomingLane, GetNext()).WillByDefault(Return(std::vector<OWL::Id>{idLaneB}));
    ON_CALL(incomingLane, GetRoad()).WillByDefault(ReturnRef(incomingRoad));
    ON_CALL(incomingRoad, IsInStreamDirection()).WillByDefault(Return(true));
    ON_CALL(incomingRoad, GetSuccessor()).WillByDefault(Return(idJunction));
    OWL::Interfaces::Roads connections{&connectorA, &connectorB};
    ON_CALL(junction, GetConnectingRoads()).WillByDefault(ReturnRef(connections));
    std::unordered_map<OWL::Id, OWL::Interfaces::Road*> roads{ {idConnectorA, &connectorA}, {idConnectorB, &connectorB}, {idIncomingRoad, &incomingRoad} };
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    std::unordered_map<OWL::Id, std::string> roadIdMapping{ {idConnectorA, "ConnectorA"}, {idConnectorB, "ConnectorB"}, {idIncomingRoad, "IncomingRoad"} };
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    std::unordered_map<OWL::Id, OWL::Interfaces::Junction*> junctions{ {idJunction, &junction} };
    ON_CALL(worldData, GetJunctions()).WillByDefault(ReturnRef(junctions));
    std::unordered_map<OWL::Id, std::string> junctionIdMapping{ {idJunction, "Junction"}};
    ON_CALL(worldData, GetJunctionIdMapping()).WillByDefault(ReturnRef(junctionIdMapping));
    std::unordered_map<OWL::Id, OWL::Interfaces::Lane*> lanes{ {idLaneA, &laneA}, {idLaneB, &laneB} };
    ON_CALL(worldData, GetLanes()).WillByDefault(ReturnRef(lanes));
    std::unordered_map<OWL::Id, OWL::OdId> laneIdMapping{ {idLaneA, -1}, {idLaneB, -1}, {idIncomingLane, -1} };
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(laneIdMapping));
    IntersectionInfo intersection{idConnectorB, IntersectingConnectionRank::Undefined, { { {idLaneA, idLaneB}, {10.0, 15.0} } } };
    std::map<std::string, std::vector<IntersectionInfo>> intersections{ {"ConnectorA", {intersection} } };
    ON_CALL(junction, GetIntersections()).WillByDefault(ReturnRef(intersections));

    ObjectPosition objectPosition;
    objectPosition.mainLocatePoint.roadId = "IncomingRoad";
    objectPosition.mainLocatePoint.laneId = -1;
    objectPosition.mainLocatePoint.roadPosition.s = 5.0;
    objectPosition.touchedRoads["IncomingRoad"].sStart = 4.0;
    objectPosition.touchedRoads["IncomingRoad"].sEnd = 7.0;

    WorldDataQuery wdQuery(worldData);

    auto distance = wdQuery.GetDistanceUntilObjectLeavesConnector(objectPosition, "ConnectorA", -1, "ConnectorB");
    ASSERT_THAT(distance, Eq(111.0));
}

TEST(GetDistanceBetweenObjects, ObjectAheadOfTargetObject_ReturnsNegativeValue)
{
    Fakes::Road road1;
    OWL::Id road1OsiId = 1;
    std::string road1OdId = "road1";
    std::unordered_map<OWL::Id, std::string> roadIdMapping; //{road1OsiId, road1OdId}};
    roadIdMapping.emplace(road1OsiId, road1OdId);
    std::unordered_map<OWL::Id, OWL::Interfaces::Road*> roadsMap;// {{road1OdId, &road1}};
    roadsMap.emplace(road1OsiId, &road1);

    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoadIdMapping).WillByDefault(ReturnRef(Const(roadIdMapping)));
    ON_CALL(worldData, GetRoads).WillByDefault(ReturnRef(Const(roadsMap)));

    double objectMinS = 30;
    double objectMaxS = 35;
    RoadInterval road1ObjectInterval{{}, objectMinS, objectMaxS};
    ObjectPosition objectPos;
    objectPos.touchedRoads.emplace(road1OdId, road1ObjectInterval);

    double targetObjectMinS = 20;
    double targetObjectMaxS = 25;
    RoadInterval road1TargetObjectInterval{{}, targetObjectMinS, targetObjectMaxS};
    ObjectPosition targetObjectPos;
    targetObjectPos.touchedRoads.emplace(road1OdId, road1TargetObjectInterval);

    RoadStreamInfo rs1{&road1, 0, true};
    RoadStream roadStream({rs1});

    WorldDataQuery wdQuery(worldData);

    const auto expectedDistance = targetObjectMaxS - objectMinS;
    const auto distance = wdQuery.GetDistanceBetweenObjects(roadStream, objectPos, targetObjectPos);
    ASSERT_THAT(distance, expectedDistance);
}
struct GetDistanceBetweenObjects_Data
{
    std::string objectRoad;
    double objectRoadMinS;
    double objectRoadMaxS;

    std::string targetObjectRoad;
    double targetObjectRoadMinS;
    double targetObjectRoadMaxS;

    bool objectsShareStream;
    double expectedDistance;
};

class GetDistanceBetweenObjects : public::testing::TestWithParam<GetDistanceBetweenObjects_Data>
{
public:
    GetDistanceBetweenObjects()
    {
        OWL::Id mainRoadOsiId = 1;
        std::string mainRoadOdId = "mainRoad";

        OWL::Id roadOnMainStreamOsiId = 2;
        std::string roadOnMainStreamOdId = "roadOnMainStream";

        OWL::Id roadOnDifferentStreamOsiId = 3;
        std::string roadOnDifferentStreamOdId = "roadOnDifferentStream";

        roadIdMapping.emplace(mainRoadOsiId, mainRoadOdId);
        roadIdMapping.emplace(roadOnMainStreamOsiId, roadOnMainStreamOdId);
        roadIdMapping.emplace(roadOnDifferentStreamOsiId, roadOnDifferentStreamOdId);

        roadsMap.emplace(mainRoadOsiId, &mainRoad);
        roadsMap.emplace(roadOnMainStreamOsiId, &roadOnMainStream);
        roadsMap.emplace(roadOnDifferentStreamOsiId, &roadOnDifferentStream);

        mainRoadInfo.element = &mainRoad;
        mainRoadInfo.sOffset = 0;
        mainRoadInfo.inStreamDirection = true;

        roadOnMainStreamInfo.element = &roadOnMainStream;
        roadOnMainStreamInfo.sOffset = 0;
        roadOnMainStreamInfo.inStreamDirection = true;

        roadOnDifferentStreamInfo.element = &roadOnDifferentStream;
        roadOnDifferentStreamInfo.sOffset = 0;
        roadOnDifferentStreamInfo.inStreamDirection = true;

        mainStream = RoadStream({mainRoadInfo, roadOnMainStreamInfo});
        differentStream = RoadStream({roadOnDifferentStreamInfo});

        ON_CALL(worldData, GetRoadIdMapping).WillByDefault(ReturnRef(Const(roadIdMapping)));
        ON_CALL(worldData, GetRoads).WillByDefault(ReturnRef(Const(roadsMap)));
    }

    Fakes::Road mainRoad;
    Fakes::Road roadOnMainStream;
    Fakes::Road roadOnDifferentStream;
    RoadStreamInfo mainRoadInfo;
    RoadStreamInfo roadOnMainStreamInfo;
    RoadStreamInfo roadOnDifferentStreamInfo;
    RoadStream mainStream;
    RoadStream differentStream;
    std::unordered_map<OWL::Id, std::string> roadIdMapping;
    std::unordered_map<OWL::Id, OWL::Interfaces::Road*> roadsMap;
    Fakes::WorldData worldData;
};

TEST_P(GetDistanceBetweenObjects, CalculatesDistanceAcrossRoadStreams)
{
    auto data = GetParam();

    RoadInterval objectInterval{{}, data.objectRoadMinS, data.objectRoadMaxS};
    ObjectPosition objectPos;
    objectPos.touchedRoads.emplace(data.objectRoad, objectInterval);

    RoadInterval targetObjectInterval{{}, data.targetObjectRoadMinS, data.targetObjectRoadMaxS};
    ObjectPosition targetObjectPos;
    targetObjectPos.touchedRoads.emplace(data.targetObjectRoad, targetObjectInterval);

    WorldDataQuery wdQuery(worldData);

    const auto distance = wdQuery.GetDistanceBetweenObjects(mainStream, objectPos, targetObjectPos);
    if (data.objectsShareStream)
    {
        ASSERT_THAT(distance, data.expectedDistance);
    }
    else
    {
        ASSERT_THAT(distance, std::numeric_limits<double>::max());
    }
}

INSTANTIATE_TEST_CASE_P(DistanceIsCalculatedCorrectly, GetDistanceBetweenObjects,
    ::testing::Values(
        GetDistanceBetweenObjects_Data{"mainRoad", 30, 35, "mainRoad", 20, 25, true, -5},
        GetDistanceBetweenObjects_Data{"mainRoad", 20, 25, "mainRoad", 30, 35, true, 5},
        GetDistanceBetweenObjects_Data{"mainRoad", 20, 25, "roadOnMainStream", 30, 35, true, 5},
        GetDistanceBetweenObjects_Data{"mainRoad", 20, 25, "roadOnDifferentStream", 30, 35, false, DontCare<double>()}
    ));

struct GetNextJunctionIdOnRoute_Data
{
    std::string initialRoad;
    std::string expectedConnRoadId;
};
class GetNextJunctionIdOnRoute : public::testing::TestWithParam<GetNextJunctionIdOnRoute_Data>
{
public:
    GetNextJunctionIdOnRoute()
    {
        j1ConnectingRoads.push_back(&connectorRoad1ToRoad2);
        j2ConnectingRoads.push_back(&connectorAfterRoad2);
        roadIdMapping.emplace(1, "road1");
        roadIdMapping.emplace(2, "connectorRoad1ToRoad2");
        roadIdMapping.emplace(3, "road2");
        roadIdMapping.emplace(4, "connectorAfterRoad2");
        roads.emplace(1, &road1);
        roads.emplace(2, &connectorRoad1ToRoad2);
        roads.emplace(3, &road2);
        roads.emplace(4, &connectorAfterRoad2);
        junctionIdMapping.emplace(1, "junction1");
        junctionIdMapping.emplace(2, "junction2");
        junctions.emplace(1, &junction1);
        junctions.emplace(2, &junction2);
        route.roads.push_back({"road1", true});
        route.roads.push_back({"connectorRoad1ToRoad2", true});
        route.roads.push_back({"road2", true});
        route.roads.push_back({"connectorAfterRoad2", true});
        route.junctions.emplace_back("junction1");
        route.junctions.emplace_back("junction2");
        startPos.touchedRoads.emplace(GetParam().initialRoad, RoadInterval{});

        ON_CALL(road1, GetId()).WillByDefault(Return(1));
        ON_CALL(connectorRoad1ToRoad2, GetId()).WillByDefault(Return(2));
        ON_CALL(road2, GetId()).WillByDefault(Return(3));
        ON_CALL(connectorAfterRoad2, GetId()).WillByDefault(Return(4));
        ON_CALL(junction1, GetConnectingRoads()).WillByDefault(ReturnRef(Const(j1ConnectingRoads)));
        ON_CALL(junction2, GetConnectingRoads()).WillByDefault(ReturnRef(Const(j2ConnectingRoads)));
        ON_CALL(fakeWorldData, GetRoadIdMapping()).WillByDefault(ReturnRef(Const(roadIdMapping)));
        ON_CALL(fakeWorldData, GetRoads()).WillByDefault(ReturnRef(Const(roads)));
        ON_CALL(fakeWorldData, GetJunctionIdMapping()).WillByDefault(ReturnRef(Const(junctionIdMapping)));
        ON_CALL(fakeWorldData, GetJunctions()).WillByDefault(ReturnRef(Const(junctions)));
    }

    Fakes::Road road1;
    Fakes::Road connectorRoad1ToRoad2;
    Fakes::Road road2;
    Fakes::Road connectorAfterRoad2;
    Fakes::Junction junction1;
    Fakes::Junction junction2;
    Fakes::WorldData fakeWorldData;

    std::list<const OWL::Interfaces::Road*> j1ConnectingRoads;
    std::list<const OWL::Interfaces::Road*> j2ConnectingRoads;

    std::unordered_map<Id, std::string> roadIdMapping;
    std::unordered_map<Id, OWL::Road*> roads;
    std::unordered_map<Id, std::string> junctionIdMapping;
    std::unordered_map<Id, OWL::Junction*> junctions;

    Route route;
    ObjectPosition startPos;
};

TEST_P(GetNextJunctionIdOnRoute, GetsCorrectConnectingRoadId)
{
    WorldDataQuery wdQuery(fakeWorldData);
    const auto actualConnectingRoadId = wdQuery.GetNextJunctionIdOnRoute(route, startPos);
    ASSERT_THAT(actualConnectingRoadId, GetParam().expectedConnRoadId);
}

INSTANTIATE_TEST_CASE_P(CorrectConnectingRoadIdIsFound, GetNextJunctionIdOnRoute,
    ::testing::Values(
        GetNextJunctionIdOnRoute_Data{"road1", "junction1"},
        GetNextJunctionIdOnRoute_Data{"connectorRoad1ToRoad2", "junction1"},
        GetNextJunctionIdOnRoute_Data{"road2", "junction2"},
        GetNextJunctionIdOnRoute_Data{"connectorAfterRoad2", "junction2"}
    ));

struct GetDistanceToJunction_Data
{
    std::string objectPosRoad;
    double objectPosS;
    std::string junctionId;
    double expectedDistance; // this should match the values in GetDistanceToJunction TEST_P for returned values from GetDistanceUntilObjectEntersConnector
};

class GetDistanceToJunction : public::testing::TestWithParam<GetDistanceToJunction_Data>
{
public:
    GetDistanceToJunction()
    {
        // road1Connector insersects road2Connector on junction1
        junction1ConnectingRoads.emplace_back(&road1Connector);
        junction1ConnectingRoads.emplace_back(&road2Connector);

        // road1OutConnector intersects road3Connector on junction2
        junction2ConnectingRoads.emplace_back(&road1OutConnector);
        junction2ConnectingRoads.emplace_back(&road3Connector);

        junctions.emplace(1, &junction1);
        junctions.emplace(2, &junction2);
        junctionIdMapping.emplace(1, "junction1");
        junctionIdMapping.emplace(2, "junction2");
        // horizontal roads
        roads.emplace(1, &road1In);
        roads.emplace(2, &road1Connector);
        roads.emplace(3, &road1Out);
        roads.emplace(10, &road1OutConnector);
        roads.emplace(11, &road1OutConnectorOut);
        roadIdMapping.emplace(1, "road1In");
        roadIdMapping.emplace(2, "road1Connector");
        roadIdMapping.emplace(3, "road1Out");
        roadIdMapping.emplace(10, "road1OutConnector");
        roadIdMapping.emplace(11, "road1OutConnectorOut");
        // vertical roads
        roads.emplace(5, &road2Connector);
        roadIdMapping.emplace(5, "road2Connector");
        roads.emplace(8, &road3Connector);
        roadIdMapping.emplace(8, "road3Connector");

        route.roads = std::vector<RouteElement>{{"road1In", true},
            {"road1Connector", true},
            {"road1Out", true},
            {"road1OutConnector", true},
            {"road1OutConnectorOut", true}};
        route.junctions = std::vector<std::string>{"junction1", "junction2"};

        ON_CALL(road1In, GetPredecessor()).WillByDefault(Return(OWL::InvalidId));
        ON_CALL(road1In, GetId()).WillByDefault(Return(1));
        ON_CALL(road1In, GetLength()).WillByDefault(Return(100));
        ON_CALL(road1In, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(0));

        ON_CALL(road1Connector, GetPredecessor()).WillByDefault(Return(1));
        ON_CALL(road1Connector, GetId()).WillByDefault(Return(2));
        ON_CALL(road1Connector, GetLength()).WillByDefault(Return(100));
        ON_CALL(road1Connector, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(0));

        ON_CALL(road1Out, GetPredecessor()).WillByDefault(Return(2));
        ON_CALL(road1Out, GetId()).WillByDefault(Return(3));
        ON_CALL(road1Out, GetLength()).WillByDefault(Return(100));
        ON_CALL(road1Out, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(0));

        ON_CALL(road1OutConnector, GetPredecessor()).WillByDefault(Return(3));
        ON_CALL(road1OutConnector, GetId()).WillByDefault(Return(10));
        ON_CALL(road1OutConnector, GetLength()).WillByDefault(Return(100));
        ON_CALL(road1OutConnector, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(0));

        ON_CALL(road1OutConnectorOut, GetPredecessor()).WillByDefault(Return(10));
        ON_CALL(road1OutConnectorOut, GetId()).WillByDefault(Return(11));
        ON_CALL(road1OutConnectorOut, GetLength()).WillByDefault(Return(100));
        ON_CALL(road1OutConnectorOut, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(0));

        ON_CALL(road2Connector, GetId()).WillByDefault(Return(5));

        ON_CALL(road3Connector, GetId()).WillByDefault(Return(8));

        ON_CALL(junction1, GetConnectingRoads()).WillByDefault(ReturnRef(Const(junction1ConnectingRoads)));

        ON_CALL(junction2, GetConnectingRoads()).WillByDefault(ReturnRef(Const(junction2ConnectingRoads)));

        ON_CALL(worldData, GetJunctions()).WillByDefault(ReturnRef(Const(junctions)));
        ON_CALL(worldData, GetJunctionIdMapping()).WillByDefault(ReturnRef(Const(junctionIdMapping)));
        ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(Const(roads)));
        ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(Const(roadIdMapping)));
    }

    NiceMock<Fakes::WorldData> worldData;
    NiceMock<Fakes::Junction> junction1;
    NiceMock<Fakes::Junction> junction2;
    NiceMock<Fakes::Road> road1In;
    NiceMock<Fakes::Road> road1Connector;
    NiceMock<Fakes::Road> road1Out;
    NiceMock<Fakes::Road> road1OutConnector;
    NiceMock<Fakes::Road> road1OutConnectorOut;
    NiceMock<Fakes::Road> road2Connector;
    NiceMock<Fakes::Road> road3Connector;

    std::unordered_map<OWL::Id, OWL::Junction*> junctions;
    std::unordered_map<OWL::Id, std::string> junctionIdMapping;
    std::unordered_map<OWL::Id, OWL::Road*> roads;
    std::unordered_map<OWL::Id, std::string> roadIdMapping;
    std::list<const OWL::Interfaces::Road*> junction1ConnectingRoads;
    std::list<const OWL::Interfaces::Road*> junction2ConnectingRoads;
    Route route;
};

TEST_P(GetDistanceToJunction, CallsGetDistanceUntilObjectEntersConnectorAppropriately)
{
    WorldDataQuery wdQuery(worldData);
    ObjectPosition objectPos;
    objectPos.mainLocatePoint.roadId = GetParam().objectPosRoad;
    objectPos.mainLocatePoint.roadPosition.s = GetParam().objectPosS;

    ASSERT_THAT(wdQuery.GetDistanceToJunction(route, objectPos, GetParam().junctionId), GetParam().expectedDistance);
}

INSTANTIATE_TEST_CASE_P(CorrectlyCallsGetDistanceUntilObjectEntersConnector, GetDistanceToJunction,
    ::testing::Values(
        GetDistanceToJunction_Data{"road1In", 0, "junction1", 100},
        GetDistanceToJunction_Data{"road1In", 50, "junction1", 50},
        GetDistanceToJunction_Data{"road1In", 50, "junction2", 250},
        GetDistanceToJunction_Data{"road1In", 0, "junction2", 300},
        GetDistanceToJunction_Data{"road1Connector", 0, "junction1", 0},
        GetDistanceToJunction_Data{"road1Connector", 50, "junction1", -50},
        GetDistanceToJunction_Data{"road1Connector", 0, "junction2", 200},
        GetDistanceToJunction_Data{"road1Connector", 50, "junction2", 150},
        GetDistanceToJunction_Data{"road1Out", 0, "junction1", -100},
        GetDistanceToJunction_Data{"road1Out", 50, "junction1", -150},
        GetDistanceToJunction_Data{"road1Out", 0, "junction2", 100},
        GetDistanceToJunction_Data{"road1Out", 50, "junction2", 50},
        GetDistanceToJunction_Data{"road1OutConnector", 0, "junction1", -200},
        GetDistanceToJunction_Data{"road1OutConnector", 50, "junction1", -250},
        GetDistanceToJunction_Data{"road1OutConnector", 0, "junction2", 0},
        GetDistanceToJunction_Data{"road1OutConnector", 50, "junction2", -50},
        GetDistanceToJunction_Data{"road1OutConnectorOut", 0, "junction1", -300},
        GetDistanceToJunction_Data{"road1OutConnectorOut", 50, "junction1", -350},
        GetDistanceToJunction_Data{"road1OutConnectorOut", 0, "junction2", -100},
        GetDistanceToJunction_Data{"road1OutConnectorOut", 50, "junction2", -150},
        GetDistanceToJunction_Data{"road1In", 0, "non-existing junction", std::numeric_limits<double>::max()}
    ));

TEST(GetObstruction, ObjectOnSameLane)
{
    OWL::Fakes::WorldData worldData;
    FakeLaneStream laneStream;
    auto lane = laneStream.AddLane(1000.0, true);
    ON_CALL(*lane, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(500.0));
    ON_CALL(*lane, GetDistance(OWL::MeasurementPoint::RoadEnd)).WillByDefault(Return(1500.0));
    Primitive::LaneGeometryJoint::Points sStartPoints{{},{2000.,2002.},{}};
    ON_CALL(*lane, GetInterpolatedPointsAtDistance(510)).WillByDefault(Return(sStartPoints));
    Primitive::LaneGeometryJoint::Points sEndPoints{{},{2005.,2002.},{}};
    ON_CALL(*lane, GetInterpolatedPointsAtDistance(515)).WillByDefault(Return(sEndPoints));
    const auto& roadIdMapping = laneStream.GetRoadIdMapping();
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));

    double tCoordinate{0.0};
    ObjectPosition otherPosition;
    otherPosition.touchedRoads["Road"].sStart = 510;
    otherPosition.touchedRoads["Road"].sEnd = 515;
    std::vector<Common::Vector2d> objectCorners {{2000.,2000.,}, {2000.,2005.}, {2005.,2005.}, {2005.,2000}};

    WorldDataQuery wdQuery{worldData};
    auto result = wdQuery.GetObstruction(laneStream.Get(), tCoordinate, otherPosition, objectCorners);
    EXPECT_THAT(result.left, Eq(3));
    EXPECT_THAT(result.right, Eq(-2));
}

TEST(GetObstruction, ObjectOnNextLane)
{
    OWL::Fakes::WorldData worldData;
    FakeLaneStream laneStream;
    auto lane1 = laneStream.AddLane(1000.0, true);
    ON_CALL(*lane1, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(500.0));
    ON_CALL(*lane1, GetDistance(OWL::MeasurementPoint::RoadEnd)).WillByDefault(Return(1500.0));
    auto lane2 = laneStream.AddLane(1000.0, true);
    ON_CALL(*lane2, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(1500.0));
    ON_CALL(*lane2, GetDistance(OWL::MeasurementPoint::RoadEnd)).WillByDefault(Return(2500.0));
    Primitive::LaneGeometryJoint::Points sStartPoints{{},{2000.,2002.},{}};
    ON_CALL(*lane2, GetInterpolatedPointsAtDistance(1510)).WillByDefault(Return(sStartPoints));
    Primitive::LaneGeometryJoint::Points sEndPoints{{},{2005.,2002.},{}};
    ON_CALL(*lane2, GetInterpolatedPointsAtDistance(1515)).WillByDefault(Return(sEndPoints));
    const auto& roadIdMapping = laneStream.GetRoadIdMapping();
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));

    double tCoordinate{0.0};
    ObjectPosition otherPosition;
    otherPosition.touchedRoads["Road"].sStart = 1510;
    otherPosition.touchedRoads["Road"].sEnd = 1515;
    std::vector<Common::Vector2d> objectCorners {{2000.,2000.,}, {2000.,2005.}, {2005.,2005.}, {2005.,2000}};

    WorldDataQuery wdQuery{worldData};
    auto result = wdQuery.GetObstruction(laneStream.Get(), tCoordinate, otherPosition, objectCorners);
    EXPECT_THAT(result.left, Eq(3));
    EXPECT_THAT(result.right, Eq(-2));
}

TEST(GetObstruction, ObjectOnTwoLanes)
{
    OWL::Fakes::WorldData worldData;
    FakeLaneStream laneStream;
    auto lane1 = laneStream.AddLane(1000.0, true);
    ON_CALL(*lane1, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(500.0));
    ON_CALL(*lane1, GetDistance(OWL::MeasurementPoint::RoadEnd)).WillByDefault(Return(1500.0));
    Primitive::LaneGeometryJoint::Points sStartPoints{{},{2000.,2000.},{}};
    ON_CALL(*lane1, GetInterpolatedPointsAtDistance(1490)).WillByDefault(Return(sStartPoints));
    auto lane2 = laneStream.AddLane(1000.0, true);
    ON_CALL(*lane2, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(1500.0));
    ON_CALL(*lane2, GetDistance(OWL::MeasurementPoint::RoadEnd)).WillByDefault(Return(2500.0));
    Primitive::LaneGeometryJoint::Points sEndPoints{{},{2020.,2000.},{}};
    ON_CALL(*lane2, GetInterpolatedPointsAtDistance(1510)).WillByDefault(Return(sEndPoints));
    const auto& roadIdMapping = laneStream.GetRoadIdMapping();
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));

    double tCoordinate{0.0};
    ObjectPosition otherPosition;
    otherPosition.touchedRoads["Road"].sStart = 1490;
    otherPosition.touchedRoads["Road"].sEnd = 1510;
    std::vector<Common::Vector2d> objectCorners {{2000.,2000.,}, {2010.,2010.}, {2020.,2000.}, {2010.,1990.}};

    WorldDataQuery wdQuery{worldData};
    auto result = wdQuery.GetObstruction(laneStream.Get(), tCoordinate, otherPosition, objectCorners);
    EXPECT_THAT(result.left, Eq(10));
    EXPECT_THAT(result.right, Eq(-10));
}

TEST(GetRelativeJunctions, OnlyOneRouteNotInJunction_ReturnsEmptyVector)
{
    OWL::Fakes::WorldData worldData;
    FakeRoadStream roadStream;

    auto roadA = roadStream.AddRoad(100.0, true);
    OWL::Id idRoadA = 1;
    ON_CALL(*roadA, GetId()).WillByDefault(Return(idRoadA));
    std::unordered_map<OWL::Id, std::string> roadIdMapping {{idRoadA, "RoadA"}};
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    std::unordered_map<OWL::Id, OWL::Road*> roads{{idRoadA, roadA}};
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    std::unordered_map<OWL::Id, OWL::Junction*> junctions{};
    ON_CALL(worldData, GetJunctions()).WillByDefault(ReturnRef(junctions));

    WorldDataQuery wdQuery{worldData};
    auto result = wdQuery.GetRelativeJunctions(roadStream.Get(), 0.0, 100.0);

    EXPECT_THAT(result, IsEmpty());
}

TEST(GetRelativeJunctions, OnlyOneRouteInJunction_ReturnsOneElement)
{
    OWL::Fakes::WorldData worldData;
    FakeRoadStream roadStream;

    auto roadA = roadStream.AddRoad(100.0, true);
    OWL::Id idRoadA = 1;

    OWL::Fakes::Junction junction;
    OWL::Id idJunction = 2;

    ON_CALL(*roadA, GetId()).WillByDefault(Return(idRoadA));
    std::unordered_map<OWL::Id, std::string> roadIdMapping {{idRoadA, "RoadA"}};
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    std::unordered_map<OWL::Id, OWL::Road*> roads{{idRoadA, roadA}};
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    std::unordered_map<OWL::Id, OWL::Junction*> junctions{{idJunction, &junction}};
    ON_CALL(worldData, GetJunctions()).WillByDefault(ReturnRef(junctions));
    std::list<const Road*> roadsOnJunction{roadA};
    ON_CALL(junction, GetConnectingRoads()).WillByDefault(ReturnRef(roadsOnJunction));

    WorldDataQuery wdQuery{worldData};
    auto result = wdQuery.GetRelativeJunctions(roadStream.Get(), 0.0, 100.0);

    EXPECT_THAT(result, SizeIs(1));
    EXPECT_THAT(result.at(0).startS, Eq(0));
    EXPECT_THAT(result.at(0).endS, Eq(100));
    EXPECT_THAT(result.at(0).connectingRoadId, Eq("RoadA"));
}

TEST(GetRelativeJunctions, FourRoutesInJunction_ReturnsFourElements)
{
    OWL::Fakes::WorldData worldData;
    FakeRoadStream roadStream;

    auto roadA = roadStream.AddRoad(100.0, true);
    OWL::Id idRoadA = 1;
    auto roadB = roadStream.AddRoad(150.0, true);
    OWL::Id idRoadB = 2;
    auto roadC = roadStream.AddRoad(200.0, false);
    OWL::Id idRoadC = 3;
    auto roadD = roadStream.AddRoad(250.0, true);
    OWL::Id idRoadD = 4;

    OWL::Fakes::Junction junctionA;
    OWL::Id idJunctionA = 5;
    OWL::Fakes::Junction junctionB;
    OWL::Id idJunctionB = 6;

    ON_CALL(*roadA, GetId()).WillByDefault(Return(idRoadA));
    ON_CALL(*roadB, GetId()).WillByDefault(Return(idRoadB));
    ON_CALL(*roadC, GetId()).WillByDefault(Return(idRoadC));
    ON_CALL(*roadD, GetId()).WillByDefault(Return(idRoadD));
    std::unordered_map<OWL::Id, std::string> roadIdMapping {{idRoadA, "RoadA"}, {idRoadB, "RoadB"}, {idRoadC, "RoadC"}, {idRoadD, "RoadD"}};
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    std::unordered_map<OWL::Id, OWL::Road*> roads{{idRoadA, roadA}, {idRoadB, roadB}, {idRoadC, roadC}, {idRoadD, roadD}};
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    std::unordered_map<OWL::Id, OWL::Junction*> junctions{{idJunctionA, &junctionA}, {idJunctionB, &junctionB}};
    ON_CALL(worldData, GetJunctions()).WillByDefault(ReturnRef(junctions));
    std::list<const Road*> roadsOnJunctionA{roadA, roadB};
    ON_CALL(junctionA, GetConnectingRoads()).WillByDefault(ReturnRef(roadsOnJunctionA));
    std::list<const Road*> roadsOnJunctionB{roadC, roadD};
    ON_CALL(junctionB, GetConnectingRoads()).WillByDefault(ReturnRef(roadsOnJunctionB));

    WorldDataQuery wdQuery{worldData};
    auto result = wdQuery.GetRelativeJunctions(roadStream.Get(), 110.0, 300.0);

    EXPECT_THAT(result, SizeIs(2));
    EXPECT_THAT(result.at(0).startS, Eq(-10));
    EXPECT_THAT(result.at(0).endS, Eq(140));
    EXPECT_THAT(result.at(0).connectingRoadId, Eq("RoadB"));
    EXPECT_THAT(result.at(1).startS, Eq(140));
    EXPECT_THAT(result.at(1).endS, Eq(340));
    EXPECT_THAT(result.at(1).connectingRoadId, Eq("RoadC"));
}

TEST(GetRelativeLanes, OnlySectionInDrivingDirection_ReturnsLanesOfThisSection)
{
    OWL::Fakes::WorldData worldData;
    FakeRoadStream roadStream;

    auto road = roadStream.AddRoad(100.0, true);
    OWL::Fakes::Lane lane1;
    OWL::Fakes::Lane lane2;
    OWL::Fakes::Lane lane3;
    OWL::Id idLane1 = 1, idLane2 = 2, idLane3 = 3;
    ON_CALL(lane1, GetId()).WillByDefault(Return(idLane1));
    ON_CALL(lane2, GetId()).WillByDefault(Return(idLane2));
    ON_CALL(lane3, GetId()).WillByDefault(Return(idLane3));
    std::vector<OWL::Id> emptyIds{};
    ON_CALL(lane1, GetPrevious()).WillByDefault(Return(emptyIds));
    ON_CALL(lane2, GetPrevious()).WillByDefault(Return(emptyIds));
    ON_CALL(lane3, GetPrevious()).WillByDefault(Return(emptyIds));
    ON_CALL(lane1, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(lane2, GetLaneType()).WillByDefault(Return(LaneType::Exit));
    ON_CALL(lane3, GetLaneType()).WillByDefault(Return(LaneType::Stop));
    OWL::Fakes::Section section;
    OWL::Interfaces::Lanes lanes{{&lane1, &lane2, &lane3}};
    ON_CALL(section, GetLanes()).WillByDefault(ReturnRef(lanes));
    ON_CALL(section, GetLength()).WillByDefault(Return(100.0));
    std::list<const OWL::Interfaces::Section*> sections{&section};
    ON_CALL(*road, GetSections()).WillByDefault(ReturnRef(sections));
    std::unordered_map<OWL::Id, OWL::OdId> laneIdMapping{{idLane1, -1}, {idLane2, -2}, {idLane3, 1}};
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(laneIdMapping));

    WorldDataQuery wdQuery{worldData};
    auto result = wdQuery.GetRelativeLanes(roadStream.Get(), 0.0, -1, 300.0);

    ASSERT_THAT(result, SizeIs(1));
    EXPECT_THAT(result.at(0).startS, Eq(0));
    EXPECT_THAT(result.at(0).endS, Eq(100));
    ASSERT_THAT(result.at(0).lanes, SizeIs(3));
    EXPECT_THAT(result.at(0).lanes.at(0).relativeId, Eq(0));
    EXPECT_THAT(result.at(0).lanes.at(0).inDrivingDirection, Eq(true));
    EXPECT_THAT(result.at(0).lanes.at(0).type, Eq(LaneType::Driving));
    EXPECT_THAT(result.at(0).lanes.at(0).predecessor, Eq(std::nullopt));
    EXPECT_THAT(result.at(0).lanes.at(0).successor, Eq(std::nullopt));
    EXPECT_THAT(result.at(0).lanes.at(1).relativeId, Eq(-1));
    EXPECT_THAT(result.at(0).lanes.at(1).inDrivingDirection, Eq(true));
    EXPECT_THAT(result.at(0).lanes.at(1).type, Eq(LaneType::Exit));
    EXPECT_THAT(result.at(0).lanes.at(1).predecessor, Eq(std::nullopt));
    EXPECT_THAT(result.at(0).lanes.at(1).successor, Eq(std::nullopt));
    EXPECT_THAT(result.at(0).lanes.at(2).relativeId, Eq(1));
    EXPECT_THAT(result.at(0).lanes.at(2).inDrivingDirection, Eq(false));
    EXPECT_THAT(result.at(0).lanes.at(2).type, Eq(LaneType::Stop));
    EXPECT_THAT(result.at(0).lanes.at(2).predecessor, Eq(std::nullopt));
    EXPECT_THAT(result.at(0).lanes.at(2).successor, Eq(std::nullopt));
}

TEST(GetRelativeLanes, OnlySectionAgainstDrivingDirection_ReturnsLanesOfThisSection)
{
    OWL::Fakes::WorldData worldData;
    FakeRoadStream roadStream;

    auto road = roadStream.AddRoad(100.0, false);
    OWL::Fakes::Lane lane1;
    OWL::Fakes::Lane lane2;
    OWL::Fakes::Lane lane3;
    OWL::Id idLane1 = 1, idLane2 = 2, idLane3 = 3;
    ON_CALL(lane1, GetId()).WillByDefault(Return(idLane1));
    ON_CALL(lane2, GetId()).WillByDefault(Return(idLane2));
    ON_CALL(lane3, GetId()).WillByDefault(Return(idLane3));
    std::vector<OWL::Id> emptyIds{};
    ON_CALL(lane1, GetPrevious()).WillByDefault(Return(emptyIds));
    ON_CALL(lane2, GetPrevious()).WillByDefault(Return(emptyIds));
    ON_CALL(lane3, GetPrevious()).WillByDefault(Return(emptyIds));
    ON_CALL(lane1, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(lane2, GetLaneType()).WillByDefault(Return(LaneType::Exit));
    ON_CALL(lane3, GetLaneType()).WillByDefault(Return(LaneType::Stop));
    OWL::Fakes::Section section;
    OWL::Interfaces::Lanes lanes{{&lane1, &lane2, &lane3}};
    ON_CALL(section, GetLanes()).WillByDefault(ReturnRef(lanes));
    ON_CALL(section, GetLength()).WillByDefault(Return(100.0));
    std::list<const OWL::Interfaces::Section*> sections{&section};
    ON_CALL(*road, GetSections()).WillByDefault(ReturnRef(sections));
    std::unordered_map<OWL::Id, OWL::OdId> laneIdMapping{{idLane1, -1}, {idLane2, -2}, {idLane3, 1}};
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(laneIdMapping));

    WorldDataQuery wdQuery{worldData};
    auto result = wdQuery.GetRelativeLanes(roadStream.Get(), 0.0, -1, 300.0);

    ASSERT_THAT(result, SizeIs(1));
    EXPECT_THAT(result.at(0).startS, Eq(0));
    EXPECT_THAT(result.at(0).endS, Eq(100));
    ASSERT_THAT(result.at(0).lanes, SizeIs(3));
    EXPECT_THAT(result.at(0).lanes.at(0).relativeId, Eq(0));
    EXPECT_THAT(result.at(0).lanes.at(0).inDrivingDirection, Eq(false));
    EXPECT_THAT(result.at(0).lanes.at(0).type, Eq(LaneType::Driving));
    EXPECT_THAT(result.at(0).lanes.at(0).predecessor, Eq(std::nullopt));
    EXPECT_THAT(result.at(0).lanes.at(0).successor, Eq(std::nullopt));
    EXPECT_THAT(result.at(0).lanes.at(1).relativeId, Eq(1));
    EXPECT_THAT(result.at(0).lanes.at(1).inDrivingDirection, Eq(false));
    EXPECT_THAT(result.at(0).lanes.at(1).type, Eq(LaneType::Exit));
    EXPECT_THAT(result.at(0).lanes.at(1).predecessor, Eq(std::nullopt));
    EXPECT_THAT(result.at(0).lanes.at(1).successor, Eq(std::nullopt));
    EXPECT_THAT(result.at(0).lanes.at(2).relativeId, Eq(-1));
    EXPECT_THAT(result.at(0).lanes.at(2).inDrivingDirection, Eq(true));
    EXPECT_THAT(result.at(0).lanes.at(2).type, Eq(LaneType::Stop));
    EXPECT_THAT(result.at(0).lanes.at(2).predecessor, Eq(std::nullopt));
    EXPECT_THAT(result.at(0).lanes.at(2).successor, Eq(std::nullopt));
}

TEST(GetRelativeLanes, TwoSectionsOnSameRoad_ReturnsLanesOfBothSections)
{
    OWL::Fakes::WorldData worldData;
    FakeRoadStream roadStream;

    auto road = roadStream.AddRoad(100.0, true);
    OWL::Fakes::Lane lane1_1;
    OWL::Fakes::Lane lane1_2;
    OWL::Id idLane1_1 = 1, idLane1_2 = 2;
    ON_CALL(lane1_1, GetId()).WillByDefault(Return(idLane1_1));
    ON_CALL(lane1_2, GetId()).WillByDefault(Return(idLane1_2));
    std::vector<OWL::Id> emptyIds{};
    ON_CALL(lane1_1, GetPrevious()).WillByDefault(Return(emptyIds));
    ON_CALL(lane1_2, GetPrevious()).WillByDefault(Return(emptyIds));
    ON_CALL(lane1_1, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(lane1_2, GetLaneType()).WillByDefault(Return(LaneType::Exit));
    OWL::Fakes::Section section1;
    OWL::Interfaces::Lanes lanes1{{&lane1_1, &lane1_2}};
    ON_CALL(section1, GetLanes()).WillByDefault(ReturnRef(lanes1));
    ON_CALL(section1, GetLength()).WillByDefault(Return(100.0));
    OWL::Fakes::Lane lane2_1;
    OWL::Fakes::Lane lane2_2;
    OWL::Fakes::Lane lane2_3;
    OWL::Id idLane2_1 = 3, idLane2_2 = 4, idLane2_3 = 5;
    ON_CALL(lane2_1, GetId()).WillByDefault(Return(idLane2_1));
    ON_CALL(lane2_2, GetId()).WillByDefault(Return(idLane2_2));
    ON_CALL(lane2_3, GetId()).WillByDefault(Return(idLane2_3));
    std::vector<OWL::Id> predecessors2_1{idLane1_1};
    std::vector<OWL::Id> predecessors2_2{idLane1_2};
    ON_CALL(lane2_1, GetPrevious()).WillByDefault(Return(predecessors2_1));
    ON_CALL(lane2_2, GetPrevious()).WillByDefault(Return(predecessors2_2));
    ON_CALL(lane2_3, GetPrevious()).WillByDefault(Return(emptyIds));
    ON_CALL(lane2_1, GetLaneType()).WillByDefault(Return(LaneType::Stop));
    ON_CALL(lane2_2, GetLaneType()).WillByDefault(Return(LaneType::Entry));
    ON_CALL(lane2_3, GetLaneType()).WillByDefault(Return(LaneType::Shoulder));
    OWL::Fakes::Section section2;
    OWL::Interfaces::Lanes lanes2{{&lane2_1, &lane2_2, &lane2_3}};
    ON_CALL(section2, GetLanes()).WillByDefault(ReturnRef(lanes2));
    ON_CALL(section2, GetSOffset()).WillByDefault(Return(100.0));
    ON_CALL(section2, GetLength()).WillByDefault(Return(200.0));
    std::list<const OWL::Interfaces::Section*> sections{&section1, &section2};
    ON_CALL(*road, GetSections()).WillByDefault(ReturnRef(sections));
    std::unordered_map<OWL::Id, OWL::OdId> laneIdMapping{{idLane1_1, -1}, {idLane1_2, -2}, {idLane2_1, -1}, {idLane2_2, -2}, {idLane2_3, -3}};
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(laneIdMapping));

    WorldDataQuery wdQuery{worldData};
    auto result = wdQuery.GetRelativeLanes(roadStream.Get(), 0.0, -1, 300.0);

    ASSERT_THAT(result, SizeIs(2));
    EXPECT_THAT(result.at(0).startS, Eq(0));
    EXPECT_THAT(result.at(0).endS, Eq(100));
    ASSERT_THAT(result.at(0).lanes, SizeIs(2));
    EXPECT_THAT(result.at(0).lanes.at(0).relativeId, Eq(0));
    EXPECT_THAT(result.at(0).lanes.at(0).inDrivingDirection, Eq(true));
    EXPECT_THAT(result.at(0).lanes.at(0).type, Eq(LaneType::Driving));
    EXPECT_THAT(result.at(0).lanes.at(0).predecessor, Eq(std::nullopt));
    EXPECT_THAT(result.at(0).lanes.at(0).successor.value(), Eq(0));
    EXPECT_THAT(result.at(0).lanes.at(1).relativeId, Eq(-1));
    EXPECT_THAT(result.at(0).lanes.at(1).inDrivingDirection, Eq(true));
    EXPECT_THAT(result.at(0).lanes.at(1).type, Eq(LaneType::Exit));
    EXPECT_THAT(result.at(0).lanes.at(1).predecessor, Eq(std::nullopt));
    EXPECT_THAT(result.at(0).lanes.at(1).successor.value(), Eq(-1));
    EXPECT_THAT(result.at(1).startS, Eq(100));
    EXPECT_THAT(result.at(1).endS, Eq(300));
    ASSERT_THAT(result.at(1).lanes, SizeIs(3));
    EXPECT_THAT(result.at(1).lanes.at(0).relativeId, Eq(0));
    EXPECT_THAT(result.at(1).lanes.at(0).inDrivingDirection, Eq(true));
    EXPECT_THAT(result.at(1).lanes.at(0).type, Eq(LaneType::Stop));
    EXPECT_THAT(result.at(1).lanes.at(0).predecessor.value(), Eq(0));
    EXPECT_THAT(result.at(1).lanes.at(0).successor, Eq(std::nullopt));
    EXPECT_THAT(result.at(1).lanes.at(1).relativeId, Eq(-1));
    EXPECT_THAT(result.at(1).lanes.at(1).inDrivingDirection, Eq(true));
    EXPECT_THAT(result.at(1).lanes.at(1).type, Eq(LaneType::Entry));
    EXPECT_THAT(result.at(1).lanes.at(1).predecessor.value(), Eq(-1));
    EXPECT_THAT(result.at(1).lanes.at(1).successor, Eq(std::nullopt));
    EXPECT_THAT(result.at(1).lanes.at(2).relativeId, Eq(-2));
    EXPECT_THAT(result.at(1).lanes.at(2).inDrivingDirection, Eq(true));
    EXPECT_THAT(result.at(1).lanes.at(2).type, Eq(LaneType::Shoulder));
    EXPECT_THAT(result.at(1).lanes.at(2).predecessor, Eq(std::nullopt));
    EXPECT_THAT(result.at(1).lanes.at(2).successor, Eq(std::nullopt));
}

TEST(GetRelativeLanes, IdOfEgoLaneChanges_ReturnsLanesWithCorrectRelativeId)
{
    OWL::Fakes::WorldData worldData;
    FakeRoadStream roadStream;

    auto road = roadStream.AddRoad(100.0, true);
    OWL::Fakes::Lane lane1_1;
    OWL::Fakes::Lane lane1_2;
    OWL::Id idLane1_1 = 1, idLane1_2 = 2;
    ON_CALL(lane1_1, GetId()).WillByDefault(Return(idLane1_1));
    ON_CALL(lane1_2, GetId()).WillByDefault(Return(idLane1_2));
    std::vector<OWL::Id> emptyIds{};
    ON_CALL(lane1_1, GetPrevious()).WillByDefault(Return(emptyIds));
    ON_CALL(lane1_2, GetPrevious()).WillByDefault(Return(emptyIds));
    ON_CALL(lane1_1, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(lane1_2, GetLaneType()).WillByDefault(Return(LaneType::Exit));
    OWL::Fakes::Section section1;
    OWL::Interfaces::Lanes lanes1{{&lane1_1, &lane1_2}};
    ON_CALL(section1, GetLanes()).WillByDefault(ReturnRef(lanes1));
    ON_CALL(section1, GetLength()).WillByDefault(Return(100.0));
    OWL::Fakes::Lane lane2_1;
    OWL::Fakes::Lane lane2_2;
    OWL::Fakes::Lane lane2_3;
    OWL::Id idLane2_1 = 3, idLane2_2 = 4, idLane2_3 = 5;
    ON_CALL(lane2_1, GetId()).WillByDefault(Return(idLane2_1));
    ON_CALL(lane2_2, GetId()).WillByDefault(Return(idLane2_2));
    ON_CALL(lane2_3, GetId()).WillByDefault(Return(idLane2_3));
    std::vector<OWL::Id> predecessors2_2{idLane1_1};
    std::vector<OWL::Id> predecessors2_3{idLane1_2};
    ON_CALL(lane2_1, GetPrevious()).WillByDefault(Return(emptyIds));
    ON_CALL(lane2_2, GetPrevious()).WillByDefault(Return(predecessors2_2));
    ON_CALL(lane2_3, GetPrevious()).WillByDefault(Return(predecessors2_3));
    ON_CALL(lane2_1, GetLaneType()).WillByDefault(Return(LaneType::Stop));
    ON_CALL(lane2_2, GetLaneType()).WillByDefault(Return(LaneType::Entry));
    ON_CALL(lane2_3, GetLaneType()).WillByDefault(Return(LaneType::Shoulder));
    OWL::Fakes::Section section2;
    OWL::Interfaces::Lanes lanes2{{&lane2_1, &lane2_2, &lane2_3}};
    ON_CALL(section2, GetLanes()).WillByDefault(ReturnRef(lanes2));
    ON_CALL(section2, GetSOffset()).WillByDefault(Return(100.0));
    ON_CALL(section2, GetLength()).WillByDefault(Return(200.0));
    std::list<const OWL::Interfaces::Section*> sections{&section1, &section2};
    ON_CALL(*road, GetSections()).WillByDefault(ReturnRef(sections));
    std::unordered_map<OWL::Id, OWL::OdId> laneIdMapping{{idLane1_1, -1}, {idLane1_2, -2}, {idLane2_1, -1}, {idLane2_2, -2}, {idLane2_3, -3}};
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(laneIdMapping));

    WorldDataQuery wdQuery{worldData};
    auto result = wdQuery.GetRelativeLanes(roadStream.Get(), 0.0, -1, 300.0);

    ASSERT_THAT(result, SizeIs(2));
    EXPECT_THAT(result.at(0).startS, Eq(0));
    EXPECT_THAT(result.at(0).endS, Eq(100));
    ASSERT_THAT(result.at(0).lanes, SizeIs(2));
    EXPECT_THAT(result.at(0).lanes.at(0).relativeId, Eq(0));
    EXPECT_THAT(result.at(0).lanes.at(0).inDrivingDirection, Eq(true));
    EXPECT_THAT(result.at(0).lanes.at(0).type, Eq(LaneType::Driving));
    EXPECT_THAT(result.at(0).lanes.at(0).predecessor, Eq(std::nullopt));
    EXPECT_THAT(result.at(0).lanes.at(0).successor.value(), Eq(0));
    EXPECT_THAT(result.at(0).lanes.at(1).relativeId, Eq(-1));
    EXPECT_THAT(result.at(0).lanes.at(1).inDrivingDirection, Eq(true));
    EXPECT_THAT(result.at(0).lanes.at(1).type, Eq(LaneType::Exit));
    EXPECT_THAT(result.at(0).lanes.at(1).predecessor, Eq(std::nullopt));
    EXPECT_THAT(result.at(0).lanes.at(1).successor.value(), Eq(-1));
    EXPECT_THAT(result.at(1).startS, Eq(100));
    EXPECT_THAT(result.at(1).endS, Eq(300));
    ASSERT_THAT(result.at(1).lanes, SizeIs(3));
    EXPECT_THAT(result.at(1).lanes.at(0).relativeId, Eq(1));
    EXPECT_THAT(result.at(1).lanes.at(0).inDrivingDirection, Eq(true));
    EXPECT_THAT(result.at(1).lanes.at(0).type, Eq(LaneType::Stop));
    EXPECT_THAT(result.at(1).lanes.at(0).predecessor, Eq(std::nullopt));
    EXPECT_THAT(result.at(1).lanes.at(0).successor, Eq(std::nullopt));
    EXPECT_THAT(result.at(1).lanes.at(1).relativeId, Eq(0));
    EXPECT_THAT(result.at(1).lanes.at(1).inDrivingDirection, Eq(true));
    EXPECT_THAT(result.at(1).lanes.at(1).type, Eq(LaneType::Entry));
    EXPECT_THAT(result.at(1).lanes.at(1).predecessor.value(), Eq(0));
    EXPECT_THAT(result.at(1).lanes.at(1).successor, Eq(std::nullopt));
    EXPECT_THAT(result.at(1).lanes.at(2).relativeId, Eq(-1));
    EXPECT_THAT(result.at(1).lanes.at(2).inDrivingDirection, Eq(true));
    EXPECT_THAT(result.at(1).lanes.at(2).type, Eq(LaneType::Shoulder));
    EXPECT_THAT(result.at(1).lanes.at(2).predecessor.value(), Eq(-1));
    EXPECT_THAT(result.at(1).lanes.at(2).successor, Eq(std::nullopt));
}

class LaneStreamTest_Data
{
public:
    std::vector<std::tuple<double, double, bool>> elements;
    std::vector<std::tuple<double, double, bool>> reversed_elements;
};

class LaneStreamTest : public::testing::TestWithParam<LaneStreamTest_Data>
{
public:
    LaneStreamTest()
    {
        const auto& data = GetParam();

        for (auto [sOffset, length, direction] : data.elements)
        {
            auto fakeLane = new NiceMock<OWL::Fakes::Lane>();
            ON_CALL(*fakeLane, GetLength()).WillByDefault(Return(length));
            fakeLanes.push_back(fakeLane);
            laneStreamInfos.push_back({fakeLane, sOffset, direction});
        }

        laneStream = LaneStream{laneStreamInfos};
    }

    ~LaneStreamTest()
    {
        for (auto fakeLane : fakeLanes)
        {
            delete fakeLane;
        }
    }

    LaneStream laneStream;
    std::vector<const OWL::Fakes::Lane*> fakeLanes;
    std::vector<LaneStreamInfo> laneStreamInfos;
};

TEST_P(LaneStreamTest, StreamReverse_CorrectlyReverses)
{
    const auto& data = GetParam();

    auto reversedLaneStream = laneStream.Reverse();
    auto reversedLaneElements = reversedLaneStream.GetElements();

    std::vector<std::tuple<double, double, bool>> reversedResult;
    for (const auto reversedLaneElement : reversedLaneElements)
    {
        reversedResult.push_back({reversedLaneElement.sOffset, reversedLaneElement.element->GetLength(), reversedLaneElement.inStreamDirection});
    }

    ASSERT_THAT(reversedLaneElements, SizeIs(laneStreamInfos.size()));
    EXPECT_THAT(reversedResult, ElementsAreArray(data.reversed_elements));
}

INSTANTIATE_TEST_CASE_P(SimpleLaneStreams, LaneStreamTest, ::testing::Values(
//                      | input lanes      |  expected reversed lanes |
//                      |                  |                          |
//                      |                  |                          |
    LaneStreamTest_Data{{{   0, 100, true  }}, {{ 100, 100, false }}},
    LaneStreamTest_Data{{{ 100, 100, false }}, {{   0, 100, true  }}},
//                           |    |    |
//                       sOffset  |   inStreamDirection
//                            length

    LaneStreamTest_Data{{{   0, 100, true  }, { 100, 100, true  }},     // two lanes
                        {{ 100, 100, false }, { 200, 100, false }}},

    LaneStreamTest_Data{{{   0, 100, true  }, { 100, 100, true  }, { 200, 100, true  }},      // three lanes
                        {{ 100, 100, false }, { 200, 100, false }, { 300, 100, false }}},

    LaneStreamTest_Data{{{ 100, 100, false }, { 100, 100, true  }, { 200, 100, true  }},
                        {{ 100, 100, false }, { 200, 100, false }, { 200, 100, true  }}},

    LaneStreamTest_Data{{{   0, 100, true  }, { 200, 100, false }, { 200, 100, true  }},
                        {{ 100, 100, false }, { 100, 100, true  }, { 300, 100, false }}},

    LaneStreamTest_Data{{{   0, 100, true  }, { 100, 100, true  }, { 300, 100, false }},
                        {{   0, 100, true  }, { 200, 100, false }, { 300, 100, false }}},

    LaneStreamTest_Data{{{ 100, 100, false }, { 100, 100, true  }, { 300, 100, false }},
                        {{   0, 100, true  }, { 200, 100, false }, { 200, 100, true  }}}
));
