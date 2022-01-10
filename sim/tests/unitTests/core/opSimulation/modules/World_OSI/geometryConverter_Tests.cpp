/********************************************************************************
 * Copyright (c) 2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "GeometryConverter.h"
#include "JointsBuilder.h"
#include "RamerDouglasPeucker.h"
#include "fakeRoadLane.h"
#include "fakeRoadLaneSection.h"
#include "fakeRoadGeometry.h"
#include "fakeOdRoad.h"

using namespace testing;
using ::testing::_;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::Eq;
using ::testing::Le;
using ::testing::SizeIs;

TEST(GeometryConverter_UnitTests, CalculateJointOnlyRightLanes)
{
    constexpr double geometryOffset = 123.4;
    constexpr double roadOffset = 12.3;
    constexpr double roadSectionStart = 1.2;

    FakeRoadLaneSection section;
    FakeOdRoad road;
    std::vector<RoadLaneOffset *> laneOffsets;
    ON_CALL(road, GetLaneOffsets()).WillByDefault(ReturnRef(laneOffsets));

    FakeRoadLane lane0;
    FakeRoadLane laneMinus1;
    FakeRoadLane laneMinus2;
    RoadLaneWidth widthMinus1{0, 3.0, 0.0, 0.0, 0.0};
    RoadLaneWidths widthsMinus1{&widthMinus1};
    ON_CALL(laneMinus1, GetWidths()).WillByDefault(ReturnRef(widthsMinus1));
    RoadLaneWidth widthMinus2{0, 4.0, 0.0, 0.0, 0.0};
    RoadLaneWidths widthsMinus2{&widthMinus2};
    ON_CALL(laneMinus2, GetWidths()).WillByDefault(ReturnRef(widthsMinus2));
    std::map<int, RoadLaneInterface*> lanes{{0, &lane0}, {-1, &laneMinus1}, {-2, &laneMinus2}};
    ON_CALL(section, GetLanes()).WillByDefault(ReturnRef(lanes));

    FakeRoadGeometry geometry;
    ON_CALL(geometry, GetCoord(geometryOffset,_)).WillByDefault(Return(Common::Vector2d{101.0, 102.0}));

    auto result = GeometryConverter::CalculateBorderPoints(&section, &road, &geometry, geometryOffset, roadOffset, roadSectionStart);

    ASSERT_THAT(result.s, Eq(roadOffset));
    ASSERT_THAT(result.points, SizeIs(3));
    ASSERT_THAT(result.points.at(0).point, Eq(Common::Vector2d{101.0, 102.0}));
    ASSERT_THAT(result.points.at(0).lane, Eq(&lane0));
    ASSERT_THAT(result.points.at(1).point, Eq(Common::Vector2d{101.0, 99.0}));
    ASSERT_THAT(result.points.at(1).lane, Eq(&laneMinus1));
    ASSERT_THAT(result.points.at(2).point, Eq(Common::Vector2d{101.0, 95.0}));
    ASSERT_THAT(result.points.at(2).lane, Eq(&laneMinus2));
}

TEST(GeometryConverter_UnitTests, CalculateJointOnlyRightLanesWithBorders)
{
    constexpr double geometryOffset = 123.4;
    constexpr double roadOffset = 12.3;
    constexpr double roadSectionStart = 1.2;

    FakeRoadLaneSection section;
    FakeOdRoad road;
    std::vector<RoadLaneOffset *> laneOffsets;
    ON_CALL(road, GetLaneOffsets()).WillByDefault(ReturnRef(laneOffsets));

    FakeRoadLane lane0;
    FakeRoadLane laneMinus1;
    FakeRoadLane laneMinus2;

    RoadLaneWidths emptyWidths {};
    RoadLaneWidth widthMinus1{0, 3.0, 0.0, 0.0, 0.0};
    RoadLaneWidths widthsMinus1{&widthMinus1};
    ON_CALL(laneMinus1, GetWidths()).WillByDefault(ReturnRef(emptyWidths));
    ON_CALL(laneMinus1, GetBorders()).WillByDefault(ReturnRef(widthsMinus1));
    RoadLaneWidth widthMinus2{0, 7.0, 0.0, 0.0, 0.0};

    RoadLaneWidths widthsMinus2{&widthMinus2};
    ON_CALL(laneMinus2, GetWidths()).WillByDefault(ReturnRef(emptyWidths));
    ON_CALL(laneMinus2, GetBorders()).WillByDefault(ReturnRef(widthsMinus2));
    std::map<int, RoadLaneInterface*> lanes{{0, &lane0}, {-1, &laneMinus1}, {-2, &laneMinus2}};
    ON_CALL(section, GetLanes()).WillByDefault(ReturnRef(lanes));

    FakeRoadGeometry geometry;
    ON_CALL(geometry, GetCoord(geometryOffset,_)).WillByDefault(Return(Common::Vector2d{101.0, 102.0}));

    auto result = GeometryConverter::CalculateBorderPoints(&section, &road, &geometry, geometryOffset, roadOffset, roadSectionStart);

    ASSERT_THAT(result.s, Eq(roadOffset));
    ASSERT_THAT(result.points, SizeIs(3));
    ASSERT_THAT(result.points.at(0).point, Eq(Common::Vector2d{101.0, 102.0}));
    ASSERT_THAT(result.points.at(0).lane, Eq(&lane0));
    ASSERT_THAT(result.points.at(1).point, Eq(Common::Vector2d{101.0, 99.0}));
    ASSERT_THAT(result.points.at(1).lane, Eq(&laneMinus1));
    ASSERT_THAT(result.points.at(2).point, Eq(Common::Vector2d{101.0, 95.0}));
    ASSERT_THAT(result.points.at(2).lane, Eq(&laneMinus2));
}

TEST(GeometryConverter_UnitTests, CalculateJointOnlyLeftLanes)
{
    constexpr double geometryOffset = 123.4;
    constexpr double roadOffset = 12.3;
    constexpr double roadSectionStart = 1.2;

    FakeRoadLaneSection section;
    FakeOdRoad road;
    std::vector<RoadLaneOffset *> laneOffsets;
    ON_CALL(road, GetLaneOffsets()).WillByDefault(ReturnRef(laneOffsets));

    FakeRoadLane lane0;
    FakeRoadLane lanePlus1;
    FakeRoadLane lanePlus2;
    RoadLaneWidth widthPlus1{0, 3.0, 0.0, 0.0, 0.0};
    RoadLaneWidths widthsPlus1{&widthPlus1};
    ON_CALL(lanePlus1, GetWidths()).WillByDefault(ReturnRef(widthsPlus1));
    RoadLaneWidth widthPlus2{0, 4.0, 0.0, 0.0, 0.0};
    RoadLaneWidths widthsPlus2{&widthPlus2};
    ON_CALL(lanePlus2, GetWidths()).WillByDefault(ReturnRef(widthsPlus2));
    std::map<int, RoadLaneInterface*> lanes{{0, &lane0}, {1, &lanePlus1}, {2, &lanePlus2}};
    ON_CALL(section, GetLanes()).WillByDefault(ReturnRef(lanes));

    FakeRoadGeometry geometry;
    ON_CALL(geometry, GetCoord(geometryOffset,_)).WillByDefault(Return(Common::Vector2d{101.0, 102.0}));

    auto result = GeometryConverter::CalculateBorderPoints(&section, &road, &geometry, geometryOffset, roadOffset, roadSectionStart);

    ASSERT_THAT(result.s, Eq(roadOffset));
    ASSERT_THAT(result.points, SizeIs(3));
    ASSERT_THAT(result.points.at(0).point, Eq(Common::Vector2d{101.0, 109.0}));
    ASSERT_THAT(result.points.at(0).lane, Eq(&lanePlus2));
    ASSERT_THAT(result.points.at(1).point, Eq(Common::Vector2d{101.0, 105.0}));
    ASSERT_THAT(result.points.at(1).lane, Eq(&lanePlus1));
    ASSERT_THAT(result.points.at(2).point, Eq(Common::Vector2d{101.0, 102.0}));
    ASSERT_THAT(result.points.at(2).lane, Eq(&lane0));
}

TEST(GeometryConverter_UnitTests, CalculateJointLeftAndRightLanes)
{
    constexpr double geometryOffset = 123.4;
    constexpr double roadOffset = 12.3;
    constexpr double roadSectionStart = 1.2;

    FakeRoadLaneSection section;
    FakeOdRoad road;
    std::vector<RoadLaneOffset *> laneOffsets;
    ON_CALL(road, GetLaneOffsets()).WillByDefault(ReturnRef(laneOffsets));

    FakeRoadLane lane0;
    FakeRoadLane lanePlus1;
    FakeRoadLane laneMinus1;
    RoadLaneWidth widthMinus1{0, 3.0, 0.0, 0.0, 0.0};
    RoadLaneWidths widthsMinus1{&widthMinus1};
    ON_CALL(laneMinus1, GetWidths()).WillByDefault(ReturnRef(widthsMinus1));
    RoadLaneWidth widthPlus1{0, 4.0, 0.0, 0.0, 0.0};
    RoadLaneWidths widthsPlus1{&widthPlus1};
    ON_CALL(lanePlus1, GetWidths()).WillByDefault(ReturnRef(widthsPlus1));
    std::map<int, RoadLaneInterface*> lanes{{0, &lane0}, {-1, &laneMinus1}, {1, &lanePlus1}};
    ON_CALL(section, GetLanes()).WillByDefault(ReturnRef(lanes));

    FakeRoadGeometry geometry;
    ON_CALL(geometry, GetCoord(geometryOffset,_)).WillByDefault(Return(Common::Vector2d{101.0, 102.0}));

    auto result = GeometryConverter::CalculateBorderPoints(&section, &road, &geometry, geometryOffset, roadOffset, roadSectionStart);

    ASSERT_THAT(result.s, Eq(roadOffset));
    ASSERT_THAT(result.points, SizeIs(3));
    ASSERT_THAT(result.points.at(0).point, Eq(Common::Vector2d{101.0, 106.0}));
    ASSERT_THAT(result.points.at(0).lane, Eq(&lanePlus1));
    ASSERT_THAT(result.points.at(1).point, Eq(Common::Vector2d{101.0, 102.0}));
    ASSERT_THAT(result.points.at(1).lane, Eq(&lane0));
    ASSERT_THAT(result.points.at(2).point, Eq(Common::Vector2d{101.0, 99.0}));
    ASSERT_THAT(result.points.at(2).lane, Eq(&laneMinus1));
}

struct RamerDouglasPeucker_UnitTests_Data
{
    std::vector<BorderPoints> input;
};

class RamerDouglasPeucker_UnitTests: public ::TestWithParam<RamerDouglasPeucker_UnitTests_Data>
{
};

double GetErrorBetweenOriginalAndSimplified (const std::vector<BorderPoints>& original, const std::vector<BorderPoints>& simplified)
{
    double maxError{0.0};
    for (auto joint = simplified.begin(); joint < simplified.end() - 1; joint++)
    {
        for (auto originalJoint : original)
        {
            if (originalJoint.s >= joint->s && originalJoint.s <= (joint + 1)->s)
            {
                for (size_t i = 0; i < originalJoint.points.size(); i++)
                //for (const auto currentPoint : originalJoint.points)
                {
                    point_t currentPointBoost{originalJoint.points[i].point.x, originalJoint.points[i].point.y};
                    bg::model::linestring<point_t> line;
                    bg::append(line, point_t{joint->points[i].point.x, joint->points[i].point.y});
                    bg::append(line, point_t{(joint + 1)->points[i].point.x, (joint + 1)->points[i].point.y});
                    auto error = bg::distance(currentPointBoost, line);
                    maxError = std::max(maxError, error);
                }
            }
        }
    }
    return  maxError;
}

bool operator== (const LaneJoint& first, const LaneJoint& second)
{
    return first.lane == second.lane
            && first.left == second.left
            && first.center == second.center
            && first.right == second.right
            && std::abs(first.heading - second.heading) < 0.001
            && std::abs(first.curvature - second.curvature) < 0.0001;
}

std::ostream& operator<<(std::ostream& os, const LaneJoint& point)
{
    os << "left: " << point.left
       << " center: " << point.center
       << " right: " << point.right
       << " heading: " << point.heading
       << " curvature: " << point.curvature;

    return os;
}

TEST(JointsBuilder_UnitTests, CalculatePoints_CalculatesCorrectCenterAndOuterPoints)
{
    FakeRoadLane lane1;
    ON_CALL(lane1, GetId()).WillByDefault(Return(1));

    FakeRoadLane lane0;
    ON_CALL(lane0, GetId()).WillByDefault(Return(0));

    FakeRoadLane laneMinus1;
    ON_CALL(laneMinus1, GetId()).WillByDefault(Return(-1));

    FakeRoadLane laneMinus2;
    ON_CALL(laneMinus2, GetId()).WillByDefault(Return(-2));

    BorderPoints firstJoint {0.0, {BorderPoint{{100,100}, &lane1},
                                   BorderPoint{{120, 90}, &lane0},
                                    BorderPoint{{130, 80}, &laneMinus1},
                                    BorderPoint{{160, 60}, &laneMinus2}}};
    BorderPoints secondJoint {1000.0, {BorderPoint{{1100,100}, &lane1},
                                       BorderPoint{{1120, 90}, &lane0},
                                       BorderPoint{{1130, 80}, &laneMinus1},
                                       BorderPoint{{1160, 60}, &laneMinus2}}};
    JointsBuilder jointsBuilder({std::vector<BorderPoints>{{firstJoint, secondJoint}},0,0});

    jointsBuilder.CalculatePoints();

    auto result = jointsBuilder.GetJoints();

    ASSERT_THAT(result, SizeIs(2));
    auto& firstResult = result.at(0);
    ASSERT_THAT(firstResult.s, Eq(0.0));
    ASSERT_THAT(firstResult.laneJoints, ElementsAre(std::make_pair(-2, LaneJoint{&laneMinus2, {130, 80}, {145, 70}, {160, 60}, 0.0, 0.0}),
                                                    std::make_pair(-1, LaneJoint{&laneMinus1, {120, 90}, {125, 85}, {130, 80}, 0.0, 0.0}),
                                                    std::make_pair(0, LaneJoint{&lane0, {120, 90}, {120, 90}, {120, 90}, 0.0, 0.0}),
                                                    std::make_pair(1, LaneJoint{&lane1, {100, 100}, {110, 95}, {120, 90}, 0.0, 0.0})));
    auto& secondResult = result.at(1);
    ASSERT_THAT(secondResult.s, Eq(1000.0));
    ASSERT_THAT(secondResult.laneJoints, ElementsAre(std::make_pair(-2, LaneJoint{&laneMinus2, {1130, 80}, {1145, 70}, {1160, 60}, 0.0, 0.0}),
                                                     std::make_pair(-1, LaneJoint{&laneMinus1, {1120, 90}, {1125, 85}, {1130, 80}, 0.0, 0.0}),
                                                     std::make_pair(0, LaneJoint{&lane0, {1120, 90}, {1120, 90}, {1120, 90}, 0.0, 0.0}),
                                                     std::make_pair(1, LaneJoint{&lane1, {1100, 100}, {1110, 95}, {1120, 90}, 0.0, 0.0})));
}

TEST(JointsBuilder_UnitTests, CalculateHeadings)
{
    Joint firstJoint{0.0, {{-1, LaneJoint{nullptr, {}, {0,10}, {}, 0.0, 0.0}},
                           {0, LaneJoint{nullptr, {}, {0,0}, {}, 0.0, 0.0}}}};
    Joint secondJoint{10.0, {{-1, LaneJoint{nullptr, {}, {10,10}, {}, 0.0, 0.0}},
                           {0, LaneJoint{nullptr, {}, {10,0}, {}, 0.0, 0.0}}}};
    Joint thirdJoint{20.0, {{-1, LaneJoint{nullptr, {}, {20,0}, {}, 0.0, 0.0}},
                           {0, LaneJoint{nullptr, {}, {20,10}, {}, 0.0, 0.0}}}};
    Joint forthJoint{30.0, {{-1, LaneJoint{nullptr, {}, {20,-10}, {}, 0.0, 0.0}},
                           {0, LaneJoint{nullptr, {}, {10,20}, {}, 0.0, 0.0}}}};

    JointsBuilder jointsBuilder({}, Joints{firstJoint, secondJoint, thirdJoint, forthJoint});

    jointsBuilder.CalculateHeadings();

    auto result = jointsBuilder.GetJoints();

    ASSERT_THAT(result, SizeIs(4));
    auto& firstResult = result.at(0);
    ASSERT_THAT(firstResult.s, Eq(0.0));
    ASSERT_THAT(firstResult.laneJoints, ElementsAre(std::make_pair(-1, LaneJoint{nullptr, {}, {0, 10}, {}, 0.0, 0.0}),
                                                    std::make_pair(0, LaneJoint{nullptr, {}, {0, 0}, {}, 0.0, 0.0})));
    auto& secondResult = result.at(1);
    ASSERT_THAT(secondResult.s, Eq(10.0));
    ASSERT_THAT(secondResult.laneJoints, ElementsAre(std::make_pair(-1, LaneJoint{nullptr, {}, {10, 10}, {}, -M_PI_4, 0.0}),
                                                    std::make_pair(0, LaneJoint{nullptr, {}, {10, 0}, {}, M_PI_4, 0.0})));
    auto& thirdResult = result.at(2);
    ASSERT_THAT(thirdResult.s, Eq(20.0));
    ASSERT_THAT(thirdResult.laneJoints, ElementsAre(std::make_pair(-1, LaneJoint{nullptr, {}, {20, 0}, {}, -M_PI_2, 0.0}),
                                                    std::make_pair(0, LaneJoint{nullptr, {}, {20, 10}, {}, 3 * M_PI_4, 0.0})));
}

TEST(JointsBuilder_UnitTests, CalculateCurvatures)
{
    Joint firstJoint{0.0, {{-1, LaneJoint{nullptr, {}, {0,0}, {}, 0.5, 0.0}},
                           {0, LaneJoint{nullptr, {}, {0,10}, {}, 1.0, 0.0}}}};
    Joint secondJoint{10.0, {{-1, LaneJoint{nullptr, {}, {10,0}, {}, 1.0, 0.0}},
                           {0, LaneJoint{nullptr, {}, {10,10}, {}, 2.0, 0.0}}}};
    Joint thirdJoint{20.0, {{-1, LaneJoint{nullptr, {}, {40,0}, {}, 0.0, 0.0}},
                           {0, LaneJoint{nullptr, {}, {40,10}, {}, 1.0, 0.0}}}};
    Joint forthJoint{30.0, {{-1, LaneJoint{nullptr, {}, {50,0}, {}, 0.0, 0.0}},
                           {0, LaneJoint{nullptr, {}, {50,10}, {}, 0.0, 0.0}}}};

    JointsBuilder jointsBuilder({{}, 0.5, -0.5}, Joints{firstJoint, secondJoint, thirdJoint, forthJoint});

    jointsBuilder.CalculateCurvatures();

    auto result = jointsBuilder.GetJoints();

    ASSERT_THAT(result, SizeIs(4));
    auto& firstResult = result.at(0);
    ASSERT_THAT(firstResult.s, Eq(0.0));
    ASSERT_THAT(firstResult.laneJoints, ElementsAre(std::make_pair(-1, LaneJoint{nullptr, {}, {0, 0}, {}, 0.5, 0.0}),
                                                    std::make_pair(0, LaneJoint{nullptr, {}, {0, 10}, {}, 1.0, 0.1})));
    auto& secondResult = result.at(1);
    ASSERT_THAT(secondResult.s, Eq(10.0));
    ASSERT_THAT(secondResult.laneJoints, ElementsAre(std::make_pair(-1, LaneJoint{nullptr, {}, {10, 0}, {}, 1.0, 0.025}),
                                                    std::make_pair(0, LaneJoint{nullptr, {}, {10, 10}, {}, 2.0, 0.05})));
    auto& thirdResult = result.at(2);
    ASSERT_THAT(thirdResult.s, Eq(20.0));
    ASSERT_THAT(thirdResult.laneJoints, ElementsAre(std::make_pair(-1, LaneJoint{nullptr, {}, {40, 0}, {}, 0.0, -0.05}),
                                                    std::make_pair(0, LaneJoint{nullptr, {}, {40, 10}, {}, 1.0, -0.05})));
    auto& forthResult = result.at(3);
    ASSERT_THAT(forthResult.s, Eq(30.0));
    ASSERT_THAT(forthResult.laneJoints, ElementsAre(std::make_pair(-1, LaneJoint{nullptr, {}, {50, 0}, {}, 0.0, -0.1}),
                                                    std::make_pair(0, LaneJoint{nullptr, {}, {50, 10}, {}, 0.0, -0.3})));
}

TEST_P(RamerDouglasPeucker_UnitTests, Simplify)
{
    auto& input = GetParam().input;
    auto result = RamerDouglasPeucker::Simplify<BorderPoints>(input);

    ASSERT_THAT(GetErrorBetweenOriginalAndSimplified(input, result), Le(RamerDouglasPeucker::ERROR_THRESHOLD));
}

BorderPoints joint0    {   0.0, {BorderPoint{{0,0},    nullptr}, BorderPoint{   {0,1},    nullptr}, BorderPoint{{0,3},    nullptr}}};
BorderPoints joint1000 {1000.0, {BorderPoint{{1000,0}, nullptr}, BorderPoint{{1000,1},    nullptr}, BorderPoint{{1000,3}, nullptr}}};
BorderPoints joint500a { 500.0, {BorderPoint{{500,0},  nullptr}, BorderPoint{ {500,1},    nullptr}, BorderPoint{{500,3},  nullptr}}};
BorderPoints joint500b { 500.0, {BorderPoint{{500,1},  nullptr}, BorderPoint{ {500,2},    nullptr}, BorderPoint{{500,4},  nullptr}}};
BorderPoints joint600b { 600.0, {BorderPoint{{600,1},  nullptr}, BorderPoint{ {600,1.99}, nullptr}, BorderPoint{{600,4},  nullptr}}};
BorderPoints joint600c { 600.0, {BorderPoint{{600,1},  nullptr}, BorderPoint{ {600,2.5},  nullptr}, BorderPoint{{600,4},  nullptr}}};
BorderPoints joint700b { 700.0, {BorderPoint{{700,1},  nullptr}, BorderPoint{ {700,2},    nullptr}, BorderPoint{{700,4},  nullptr}}};

INSTANTIATE_TEST_CASE_P(RamerDouglasPeucker_UnitTests, RamerDouglasPeucker_UnitTests,
                        Values(
                            RamerDouglasPeucker_UnitTests_Data{{joint0, joint1000}},
                            RamerDouglasPeucker_UnitTests_Data{{joint0, joint500a, joint1000}},
                            RamerDouglasPeucker_UnitTests_Data{{joint0, joint500b, joint1000}},
                            RamerDouglasPeucker_UnitTests_Data{{joint0, joint500b, joint600b, joint700b, joint1000}},
                            RamerDouglasPeucker_UnitTests_Data{{joint0, joint500b, joint600c, joint700b, joint1000}}
                        ));
