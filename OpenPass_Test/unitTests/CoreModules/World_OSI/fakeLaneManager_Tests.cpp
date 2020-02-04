/*********************************************************************
* Copyright (c) 2018, 2019 in-tech
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "fakeLaneManager.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace OWL;

using ::testing::IsEmpty;
using ::testing::Ref;
using ::testing::SizeIs;
using ::testing::Eq;
using ::testing::Ne;
using ::testing::ElementsAre;

TEST(FakeLaneManager, SingleLaneTest)
{
    FakeLaneManager flm(1, 1, 3.0, {1000}, "TestRoadId");

    EXPECT_THAT(flm.lanes[0][0]->Exists(), Eq(true));
    EXPECT_THAT(flm.lanes[0][0]->GetWidth(0), Eq(3.0));
    EXPECT_THAT(flm.lanes[0][0]->GetLeftLane(), Ref(flm.invalidLane));
    EXPECT_THAT(flm.lanes[0][0]->GetRightLane(), Ref(flm.invalidLane));
    EXPECT_THAT(flm.lanes[0][0]->GetPrevious(), IsEmpty());
    EXPECT_THAT(flm.lanes[0][0]->GetNext(), IsEmpty());
}

TEST(FakeLaneManager, GetLaneTest)
{
    FakeLaneManager flm(1, 1, 3.0, {1000}, "TestRoadId");

    auto& lane = flm.GetLane(0, 0);
    EXPECT_THAT(lane.Exists(), Eq(true));
    EXPECT_THAT(lane.GetWidth(0), Eq(3.0));
    EXPECT_THAT(lane.GetLeftLane(), Ref(flm.invalidLane));
    EXPECT_THAT(lane.GetRightLane(), Ref(flm.invalidLane));
    EXPECT_THAT(flm.lanes[0][0]->GetPrevious(), IsEmpty());
    EXPECT_THAT(flm.lanes[0][0]->GetNext(), IsEmpty());
}

TEST(FakeLaneManager, SingleColumnTest)
{
    FakeLaneManager flm(1, 2, 3.0, {1000}, "TestRoadId");

    EXPECT_THAT(flm.lanes[0][0]->Exists(), Eq(true));
    EXPECT_THAT(flm.lanes[0][0]->GetWidth(0), Eq(3.0));
    EXPECT_THAT(flm.lanes[0][0]->GetLeftLane(), Ref(flm.invalidLane));
    EXPECT_THAT(&flm.lanes[0][0]->GetRightLane(), Eq(flm.lanes[0][1]));
    EXPECT_THAT(flm.lanes[0][0]->GetPrevious(), IsEmpty());
    EXPECT_THAT(flm.lanes[0][0]->GetNext(), IsEmpty());

    EXPECT_THAT(flm.lanes[0][1]->Exists(), Eq(true));
    EXPECT_THAT(flm.lanes[0][1]->GetWidth(0), Eq(3.0));
    EXPECT_THAT(&flm.lanes[0][1]->GetLeftLane(), Eq(flm.lanes[0][0]));
    EXPECT_THAT(flm.lanes[0][1]->GetRightLane(), Ref(flm.invalidLane));
    EXPECT_THAT(flm.lanes[0][0]->GetPrevious(), IsEmpty());
    EXPECT_THAT(flm.lanes[0][0]->GetNext(), IsEmpty());
}

TEST(FakeLaneManager, SingleRowTest)
{
    FakeLaneManager flm(2, 1, 3.0, {1000, 1000}, "TestRoadId");

    EXPECT_THAT(flm.lanes[0][0]->Exists(), Eq(true));
    EXPECT_THAT(flm.lanes[0][0]->GetWidth(0), Eq(3.0));
    EXPECT_THAT(flm.lanes[0][0]->GetLeftLane(), Ref(flm.invalidLane));
    EXPECT_THAT(flm.lanes[0][0]->GetRightLane(), Ref(flm.invalidLane));
    EXPECT_THAT(flm.lanes[0][0]->GetPrevious(), IsEmpty());
    EXPECT_THAT(flm.lanes[0][0]->GetNext(), ElementsAre(flm.lanes[1][0]->GetId()));

    EXPECT_THAT(flm.lanes[1][0]->Exists(), Eq(true));
    EXPECT_THAT(flm.lanes[1][0]->GetWidth(0), Eq(3.0));
    EXPECT_THAT(flm.lanes[1][0]->GetLeftLane(), Ref(flm.invalidLane));
    EXPECT_THAT(flm.lanes[1][0]->GetRightLane(), Ref(flm.invalidLane));
    EXPECT_THAT(flm.lanes[1][0]->GetNext(), IsEmpty());
    EXPECT_THAT(flm.lanes[1][0]->GetPrevious(), ElementsAre(flm.lanes[0][0]->GetId()));
}

TEST(FakeLaneManager, MatrixTest)
{
    FakeLaneManager flm(2, 3, 3.0, {1000, 1000}, "TestRoadId");

    EXPECT_THAT(flm.lanes[0][0]->Exists(), Eq(true));
    EXPECT_THAT(flm.lanes[0][0]->GetWidth(0), Eq(3.0));
    EXPECT_THAT(flm.lanes[0][0]->GetLeftLane(), Ref(flm.invalidLane));
    EXPECT_THAT(&flm.lanes[0][0]->GetRightLane(), Eq(flm.lanes[0][1]));
    EXPECT_THAT(flm.lanes[0][0]->GetPrevious(), IsEmpty());
    EXPECT_THAT(flm.lanes[0][0]->GetNext(), ElementsAre(flm.lanes[1][0]->GetId()));

    EXPECT_THAT(flm.lanes[1][2]->Exists(), Eq(true));
    EXPECT_THAT(flm.lanes[1][2]->GetWidth(0), Eq(3.0));
    EXPECT_THAT(&flm.lanes[1][2]->GetLeftLane(), Eq(flm.lanes[1][1]));
    EXPECT_THAT(flm.lanes[1][2]->GetRightLane(), Ref(flm.invalidLane));
    EXPECT_THAT(flm.lanes[1][2]->GetNext(), IsEmpty());
    EXPECT_THAT(flm.lanes[1][2]->GetPrevious(), ElementsAre(flm.lanes[0][2]->GetId()));
}

TEST(FakeLaneManager, SectionConnectionTest)
{
    FakeLaneManager flm(2, 2, 3.0, {1000, 1000}, "TestRoadId");

    EXPECT_THAT(&flm.lanes[0][0]->GetSection(), &flm.lanes[0][1]->GetSection());
    EXPECT_THAT(&flm.lanes[1][0]->GetSection(), &flm.lanes[1][1]->GetSection());
    EXPECT_THAT(&flm.lanes[0][0]->GetSection(), Ne(&flm.lanes[1][1]->GetSection()));

    // test const access
    const OWL::Lane* nextLane = flm.lanes[0][0];
    EXPECT_THAT(&nextLane->GetSection(), &flm.lanes[0][0]->GetSection());
}

TEST(FakeLaneManager, ApplyMovingObjectTest)
{
    FakeLaneManager flm(2, 2, 3.0, {1000, 1000}, "TestRoadId");
    Fakes::MovingObject fmo00;
    Fakes::MovingObject fmo01;
    Fakes::MovingObject fmo11;

    flm.AddWorldObject(0, 0, fmo00);
    flm.AddWorldObject(0, 0, fmo01);
    flm.AddWorldObject(1, 1, fmo11);

    auto gmo00 = flm.lanes[0][0]->GetWorldObjects();
    auto gmo01 = flm.lanes[0][1]->GetWorldObjects();
    auto gmo10 = flm.lanes[1][0]->GetWorldObjects();
    auto gmo11 = flm.lanes[1][1]->GetWorldObjects();

    ASSERT_THAT(gmo00, SizeIs(2));
    ASSERT_THAT(gmo01, SizeIs(0));
    ASSERT_THAT(gmo10, SizeIs(0));
    ASSERT_THAT(gmo11, SizeIs(1));

    EXPECT_THAT(gmo00.front(), Eq(&fmo00));
    EXPECT_THAT(gmo00.back(), Eq(&fmo01));

    EXPECT_THAT(gmo11.front(), Eq(&fmo11));
}

TEST(FakeLaneManager, SetWidth)
{
    FakeLaneManager flm(2, 2, 3.0, {1000, 1000}, "TestRoadId");

    flm.SetWidth(0, 0, 1.0, 10.0);
    flm.SetWidth(0, 1, 2.0, 20.0);
    flm.SetWidth(1, 0, 3.0, 30.0);
    flm.SetWidth(1, 1, 4.0, 40.0);

    // default values (set by constructor)
    EXPECT_THAT(flm.lanes[0][0]->GetWidth(0.0), Eq(3.0));
    EXPECT_THAT(flm.lanes[0][1]->GetWidth(0.0), Eq(3.0));
    EXPECT_THAT(flm.lanes[1][0]->GetWidth(0.0), Eq(3.0));
    EXPECT_THAT(flm.lanes[1][1]->GetWidth(0.0), Eq(3.0));

    // special values at distance x
    EXPECT_THAT(flm.lanes[0][0]->GetWidth(10.0), Eq(1.0));
    EXPECT_THAT(flm.lanes[0][1]->GetWidth(20.0), Eq(2.0));
    EXPECT_THAT(flm.lanes[1][0]->GetWidth(30.0), Eq(3.0));
    EXPECT_THAT(flm.lanes[1][1]->GetWidth(40.0), Eq(4.0));
}

TEST(FakeLaneManager, SetLength)
{
    FakeLaneManager flm(1, 2, 3.0, {1000, 1000}, "TestRoadId");

    flm.SetLength(0, 0, 1.0);
    EXPECT_THAT(flm.lanes[0][0]->GetLength(), Eq(1.0));
    EXPECT_THAT(flm.lanes[0][0]->GetDistance(OWL::MeasurementPoint::RoadStart), Eq(0.0));
    EXPECT_THAT(flm.lanes[0][0]->GetDistance(OWL::MeasurementPoint::RoadEnd),   Eq(1.0));

    flm.SetLength(0, 1, 2.0, 20.0);

    EXPECT_THAT(flm.lanes[0][1]->GetLength(), Eq(2.0));
    EXPECT_THAT(flm.lanes[0][1]->GetDistance(OWL::MeasurementPoint::RoadStart), Eq(20.0));
    EXPECT_THAT(flm.lanes[0][1]->GetDistance(OWL::MeasurementPoint::RoadEnd),   Eq(22.0));
}

////////////////////////////////////////////////////////////////////////////
TEST(FakeLaneManager, SectionCoversValidPosition_ReturnsTrue)
{
    FakeLaneManager flm(1, 1, 3.0, {200}, "TestRoadId");

    const auto& sections = flm.GetSections();
    const std::list<double> distances = {0.0, 50.0, 200.0};

    ASSERT_EQ(sections.size(), 1);
    const auto& section = sections.begin()->second;

    for (auto& distance : distances)
    {
        EXPECT_THAT(section->Covers(distance), Eq(true));
    }

}

TEST(FakeLaneManager, SectionCoversInValidPosition_ReturnsFalse)
{
    FakeLaneManager flm(1, 1, 3.0, {200}, "TestRoadId");

    const auto& sections = flm.GetSections();
    const std::list<double> distances = {-1.0, 201.0};

    ASSERT_EQ(sections.size(), 1);
    const auto& section = sections.begin()->second;

    for (auto& distance : distances)
    {
        EXPECT_THAT(section->Covers(distance), Eq(false));
    }
}

TEST(FakeLaneManager, SectionCoversIntervalValidPositions_ReturnsTrue)
{
    FakeLaneManager flm(1, 1, 3.0, {200}, "TestRoadId");

    const auto& sections = flm.GetSections();
    std::vector<double> startDistances = {0.0, 30.0, 190.0};
    std::vector<double> endDistances = {10.0, 40.0, 200.0};

    ASSERT_EQ(sections.size(), 1);
    const auto& section = sections.begin()->second;

    for (unsigned cnt = 0; cnt < 3; cnt++)
    {
        bool result = section->CoversInterval(startDistances[cnt], endDistances[cnt]);
        EXPECT_THAT(result, Eq(true));
    }
}

TEST(FakeLaneManager,  SectionCoversIntervalOneDistanceOutsideSectionRange_ReturnsTrue)
{
    FakeLaneManager flm(1, 1, 3.0, {200}, "TestRoadId");

    const auto& sections = flm.GetSections();
    std::vector<double> startDistances = {-10.0, 190.0};
    std::vector<double> endDistances = {10.0, 210.0};

    ASSERT_EQ(sections.size(), 1);
    const auto& section = sections.begin()->second;

    for (unsigned cnt = 0; cnt < 2; cnt++)
    {
        EXPECT_THAT(section->CoversInterval(startDistances[cnt], endDistances[cnt]), Eq(true));
    }
}

TEST(FakeLaneManager,  SectionCoversIntervalBothDistancesOutsideSectionRange_ReturnsFalse)
{
    FakeLaneManager flm(1, 1, 3.0, {200}, "TestRoadId");

    const auto& sections = flm.GetSections();
    std::vector<double> startDistances = {-50.0, 210.0};
    std::vector<double> endDistances = {-10.0, 300.0};

    ASSERT_EQ(sections.size(), 1);
    const auto& section = sections.begin()->second;

    for (unsigned cnt = 0; cnt < 2; cnt++)
    {
        EXPECT_THAT(section->CoversInterval(startDistances[cnt], endDistances[cnt]), Eq(false));
    }
}

