/********************************************************************************
 * Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "RoadStream.h"
#include "fakeRoad.h"
#include "fakeSection.h"
#include "fakeLane.h"
#include "fakeMovingObject.h"
#include "fakeAgent.h"

using testing::Return;
using testing::ReturnRef;
using testing::Eq;
using testing::DoubleEq;
using testing::_;
using testing::SizeIs;
using testing::IsEmpty;
using testing::ElementsAre;

TEST(RoadStream_Tests, GetStreamPosition_FirstRoadInStreamDirection)
{
    constexpr double lengthRoad1 = 110;
    constexpr double lengthRoad2 = 210;
    const std::string idRoad1 = "Road1";
    const std::string idRoad2 = "Road2";

    OWL::Fakes::Road road1;
    ON_CALL(road1, GetId()).WillByDefault(ReturnRef(idRoad1));
    ON_CALL(road1, GetLength()).WillByDefault(Return(lengthRoad1));
    OWL::Fakes::Section section1;
    ON_CALL(section1, Covers(_)).WillByDefault(Return(true));
    OWL::Fakes::Lane lane1;
    ON_CALL(lane1, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(lane1, GetWidth(_)).WillByDefault(Return(4.));
    OWL::Fakes::Lane lane2;
    ON_CALL(lane2, GetOdId()).WillByDefault(Return(-2));
    ON_CALL(lane2, GetWidth(_)).WillByDefault(Return(5.));
    OWL::Interfaces::Lanes lanesSection1{&lane1, &lane2};
    ON_CALL(section1, GetLanes()).WillByDefault(ReturnRef(lanesSection1));
    OWL::Interfaces::Sections sectionsRoad1{&section1};
    ON_CALL(road1, GetSections()).WillByDefault(ReturnRef(sectionsRoad1));
    RoadStreamElement element1 {&road1, 0., true};

    OWL::Fakes::Road road2;
    ON_CALL(road2, GetId()).WillByDefault(ReturnRef(idRoad2));
    ON_CALL(road2, GetLength()).WillByDefault(Return(lengthRoad2));
    RoadStreamElement element2 {&road2, lengthRoad1, true};

    RoadStream roadStream{{element1, element2}};

    const auto result = roadStream.GetStreamPosition(GlobalRoadPosition{"Road1", -2, 30., 1., 0.});

    ASSERT_THAT(result.s, DoubleEq(30.));
    ASSERT_THAT(result.t, DoubleEq(-5.5));
}

TEST(RoadStream_Tests, GetStreamPosition_FirstRoadAgainstStreamDirection)
{
    constexpr double lengthRoad1 = 110;
    constexpr double lengthRoad2 = 210;
    const std::string idRoad1 = "Road1";
    const std::string idRoad2 = "Road2";

    OWL::Fakes::Road road1;
    ON_CALL(road1, GetId()).WillByDefault(ReturnRef(idRoad1));
    ON_CALL(road1, GetLength()).WillByDefault(Return(lengthRoad1));
    OWL::Fakes::Section section1;
    ON_CALL(section1, Covers(_)).WillByDefault(Return(true));
    OWL::Fakes::Lane lane1;
    ON_CALL(lane1, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(lane1, GetWidth(_)).WillByDefault(Return(4.));
    OWL::Fakes::Lane lane2;
    ON_CALL(lane2, GetOdId()).WillByDefault(Return(-2));
    ON_CALL(lane2, GetWidth(_)).WillByDefault(Return(5.));
    OWL::Interfaces::Lanes lanesSection1{&lane1, &lane2};
    ON_CALL(section1, GetLanes()).WillByDefault(ReturnRef(lanesSection1));
    OWL::Interfaces::Sections sectionsRoad1{&section1};
    ON_CALL(road1, GetSections()).WillByDefault(ReturnRef(sectionsRoad1));
    RoadStreamElement element1 {&road1, lengthRoad1, false};

    OWL::Fakes::Road road2;
    ON_CALL(road2, GetId()).WillByDefault(ReturnRef(idRoad2));
    ON_CALL(road2, GetLength()).WillByDefault(Return(lengthRoad2));
    RoadStreamElement element2 {&road2, lengthRoad1, true};

    RoadStream roadStream{{element1, element2}};

    const auto result = roadStream.GetStreamPosition(GlobalRoadPosition{"Road1", -2, 30., 1., 0.});

    ASSERT_THAT(result.s, DoubleEq(80.));
    ASSERT_THAT(result.t, DoubleEq(5.5));
}

TEST(RoadStream_Tests, GetStreamPosition_SecondRoadInStreamDirection)
{
    constexpr double lengthRoad1 = 110;
    constexpr double lengthRoad2 = 210;
    const std::string idRoad1 = "Road1";
    const std::string idRoad2 = "Road2";

    OWL::Fakes::Road road1;
    ON_CALL(road1, GetId()).WillByDefault(ReturnRef(idRoad1));
    ON_CALL(road1, GetLength()).WillByDefault(Return(lengthRoad1));
    RoadStreamElement element1 {&road1, 0., true};

    OWL::Fakes::Road road2;
    ON_CALL(road2, GetId()).WillByDefault(ReturnRef(idRoad2));
    ON_CALL(road2, GetLength()).WillByDefault(Return(lengthRoad2));
    OWL::Fakes::Section section1;
    ON_CALL(section1, Covers(_)).WillByDefault(Return(true));
    OWL::Fakes::Lane lane1;
    ON_CALL(lane1, GetOdId()).WillByDefault(Return(1));
    ON_CALL(lane1, GetWidth(_)).WillByDefault(Return(4.));
    OWL::Fakes::Lane lane2;
    ON_CALL(lane2, GetOdId()).WillByDefault(Return(2));
    ON_CALL(lane2, GetWidth(_)).WillByDefault(Return(5.));
    OWL::Interfaces::Lanes lanesSection1{&lane1, &lane2};
    ON_CALL(section1, GetLanes()).WillByDefault(ReturnRef(lanesSection1));
    OWL::Interfaces::Sections sectionsRoad2{&section1};
    ON_CALL(road2, GetSections()).WillByDefault(ReturnRef(sectionsRoad2));
    RoadStreamElement element2 {&road2, lengthRoad1, true};

    RoadStream roadStream{{element1, element2}};

    const auto result = roadStream.GetStreamPosition(GlobalRoadPosition{"Road2", 2, 30., -1., 0.});

    ASSERT_THAT(result.s, DoubleEq(140.));
    ASSERT_THAT(result.t, DoubleEq(5.5));
}

TEST(RoadStream_Tests, GetStreamPosition_SecondRoadAgainstStreamDirection)
{
    constexpr double lengthRoad1 = 110;
    constexpr double lengthRoad2 = 210;
    const std::string idRoad1 = "Road1";
    const std::string idRoad2 = "Road2";

    OWL::Fakes::Road road1;
    ON_CALL(road1, GetId()).WillByDefault(ReturnRef(idRoad1));
    ON_CALL(road1, GetLength()).WillByDefault(Return(lengthRoad1));
    RoadStreamElement element1 {&road1, 0., true};
    OWL::Fakes::Road road2;
    ON_CALL(road2, GetId()).WillByDefault(ReturnRef(idRoad2));
    ON_CALL(road2, GetLength()).WillByDefault(Return(lengthRoad2));
    OWL::Fakes::Section section1;
    ON_CALL(section1, Covers(_)).WillByDefault(Return(true));
    OWL::Fakes::Lane lane1;
    ON_CALL(lane1, GetOdId()).WillByDefault(Return(1));
    ON_CALL(lane1, GetWidth(_)).WillByDefault(Return(4.));
    OWL::Fakes::Lane lane2;
    ON_CALL(lane2, GetOdId()).WillByDefault(Return(2));
    ON_CALL(lane2, GetWidth(_)).WillByDefault(Return(5.));
    OWL::Interfaces::Lanes lanesSection1{&lane1, &lane2};
    ON_CALL(section1, GetLanes()).WillByDefault(ReturnRef(lanesSection1));
    OWL::Interfaces::Sections sectionsRoad2{&section1};
    ON_CALL(road2, GetSections()).WillByDefault(ReturnRef(sectionsRoad2));
    RoadStreamElement element2 {&road2, lengthRoad1 + lengthRoad2, false};

    RoadStream roadStream{{element1, element2}};

    const auto result = roadStream.GetStreamPosition(GlobalRoadPosition{"Road2", 2, 30., -1., 0.});

    ASSERT_THAT(result.s, DoubleEq(290.));
    ASSERT_THAT(result.t, DoubleEq(-5.5));
}

TEST(RoadStream_Tests, GetRoadPosition_FirstRoadInStreamDirection)
{
    constexpr double lengthRoad1 = 110;
    constexpr double lengthRoad2 = 210;
    const std::string idRoad1 = "Road1";
    const std::string idRoad2 = "Road2";

    OWL::Fakes::Road road1;
    ON_CALL(road1, GetId()).WillByDefault(ReturnRef(idRoad1));
    ON_CALL(road1, GetLength()).WillByDefault(Return(lengthRoad1));
    OWL::Fakes::Section section1;
    ON_CALL(section1, Covers(_)).WillByDefault(Return(true));
    OWL::Fakes::Lane lane1;
    ON_CALL(lane1, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(lane1, GetWidth(_)).WillByDefault(Return(4.));
    OWL::Fakes::Lane lane2;
    ON_CALL(lane2, GetOdId()).WillByDefault(Return(-2));
    ON_CALL(lane2, GetWidth(_)).WillByDefault(Return(5.));
    OWL::Interfaces::Lanes lanesSection1{&lane1, &lane2};
    ON_CALL(section1, GetLanes()).WillByDefault(ReturnRef(lanesSection1));
    OWL::Interfaces::Sections sectionsRoad1{&section1};
    ON_CALL(road1, GetSections()).WillByDefault(ReturnRef(sectionsRoad1));
    RoadStreamElement element1 {&road1, 0., true};

    OWL::Fakes::Road road2;
    ON_CALL(road2, GetId()).WillByDefault(ReturnRef(idRoad2));
    ON_CALL(road2, GetLength()).WillByDefault(Return(lengthRoad2));
    RoadStreamElement element2 {&road2, lengthRoad1, true};

    RoadStream roadStream{{element1, element2}};

    const auto result = roadStream.GetRoadPosition(StreamPosition{30., -5.5});

    ASSERT_THAT(result.roadId, Eq(idRoad1));
    ASSERT_THAT(result.laneId, Eq(-2));
    ASSERT_THAT(result.roadPosition.s, DoubleEq(30.));
    ASSERT_THAT(result.roadPosition.t, DoubleEq(1.));
}

TEST(RoadStream_Tests, GetRoadPosition_FirstRoadAgainstStreamDirection)
{
    constexpr double lengthRoad1 = 110;
    constexpr double lengthRoad2 = 210;
    const std::string idRoad1 = "Road1";
    const std::string idRoad2 = "Road2";

    OWL::Fakes::Road road1;
    ON_CALL(road1, GetId()).WillByDefault(ReturnRef(idRoad1));
    ON_CALL(road1, GetLength()).WillByDefault(Return(lengthRoad1));
    OWL::Fakes::Section section1;
    ON_CALL(section1, Covers(_)).WillByDefault(Return(true));
    OWL::Fakes::Lane lane1;
    ON_CALL(lane1, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(lane1, GetWidth(_)).WillByDefault(Return(4.));
    OWL::Fakes::Lane lane2;
    ON_CALL(lane2, GetOdId()).WillByDefault(Return(-2));
    ON_CALL(lane2, GetWidth(_)).WillByDefault(Return(5.));
    OWL::Interfaces::Lanes lanesSection1{&lane1, &lane2};
    ON_CALL(section1, GetLanes()).WillByDefault(ReturnRef(lanesSection1));
    OWL::Interfaces::Sections sectionsRoad1{&section1};
    ON_CALL(road1, GetSections()).WillByDefault(ReturnRef(sectionsRoad1));
    RoadStreamElement element1 {&road1, lengthRoad1, false};

    OWL::Fakes::Road road2;
    ON_CALL(road2, GetId()).WillByDefault(ReturnRef(idRoad2));
    ON_CALL(road2, GetLength()).WillByDefault(Return(lengthRoad2));
    RoadStreamElement element2 {&road2, lengthRoad1, true};

    RoadStream roadStream{{element1, element2}};

    const auto result = roadStream.GetRoadPosition(StreamPosition{30., 5.5});

    ASSERT_THAT(result.roadId, Eq(idRoad1));
    ASSERT_THAT(result.laneId, Eq(-2));
    ASSERT_THAT(result.roadPosition.s, DoubleEq(80.));
    ASSERT_THAT(result.roadPosition.t, DoubleEq(1.));
}

TEST(RoadStream_Tests, GetRoadPosition_SecondRoadInStreamDirection)
{
    constexpr double lengthRoad1 = 110;
    constexpr double lengthRoad2 = 210;
    const std::string idRoad1 = "Road1";
    const std::string idRoad2 = "Road2";

    OWL::Fakes::Road road1;
    ON_CALL(road1, GetId()).WillByDefault(ReturnRef(idRoad1));
    ON_CALL(road1, GetLength()).WillByDefault(Return(lengthRoad1));
    RoadStreamElement element1 {&road1, 0., true};

    OWL::Fakes::Road road2;
    ON_CALL(road2, GetId()).WillByDefault(ReturnRef(idRoad2));
    ON_CALL(road2, GetLength()).WillByDefault(Return(lengthRoad2));
    ON_CALL(road2, GetId()).WillByDefault(ReturnRef(idRoad2));
    ON_CALL(road2, GetLength()).WillByDefault(Return(lengthRoad2));
    OWL::Fakes::Section section1;
    ON_CALL(section1, Covers(_)).WillByDefault(Return(true));
    OWL::Fakes::Lane lane1;
    ON_CALL(lane1, GetOdId()).WillByDefault(Return(1));
    ON_CALL(lane1, GetWidth(_)).WillByDefault(Return(4.));
    OWL::Fakes::Lane lane2;
    ON_CALL(lane2, GetOdId()).WillByDefault(Return(2));
    ON_CALL(lane2, GetWidth(_)).WillByDefault(Return(5.));
    OWL::Interfaces::Lanes lanesSection1{&lane1, &lane2};
    ON_CALL(section1, GetLanes()).WillByDefault(ReturnRef(lanesSection1));
    OWL::Interfaces::Sections sectionsRoad2{&section1};
    ON_CALL(road2, GetSections()).WillByDefault(ReturnRef(sectionsRoad2));
    RoadStreamElement element2 {&road2, lengthRoad1, true};

    RoadStream roadStream{{element1, element2}};

    const auto result = roadStream.GetRoadPosition(StreamPosition{130., 5.5});

    ASSERT_THAT(result.roadId, Eq(idRoad2));
    ASSERT_THAT(result.laneId, Eq(2));
    ASSERT_THAT(result.roadPosition.s, DoubleEq(20.));
    ASSERT_THAT(result.roadPosition.t, DoubleEq(-1.));
}

TEST(RoadStream_Tests, GetRoadPosition_SecondRoadAgainstStreamDirection)
{
    constexpr double lengthRoad1 = 110;
    constexpr double lengthRoad2 = 210;
    const std::string idRoad1 = "Road1";
    const std::string idRoad2 = "Road2";

    OWL::Fakes::Road road1;
    ON_CALL(road1, GetId()).WillByDefault(ReturnRef(idRoad1));
    ON_CALL(road1, GetLength()).WillByDefault(Return(lengthRoad1));
    RoadStreamElement element1 {&road1, 0., true};

    OWL::Fakes::Road road2;
    ON_CALL(road2, GetId()).WillByDefault(ReturnRef(idRoad2));
    ON_CALL(road2, GetLength()).WillByDefault(Return(lengthRoad2));
    ON_CALL(road2, GetId()).WillByDefault(ReturnRef(idRoad2));
    ON_CALL(road2, GetLength()).WillByDefault(Return(lengthRoad2));
    OWL::Fakes::Section section1;
    ON_CALL(section1, Covers(_)).WillByDefault(Return(true));
    OWL::Fakes::Lane lane1;
    ON_CALL(lane1, GetOdId()).WillByDefault(Return(1));
    ON_CALL(lane1, GetWidth(_)).WillByDefault(Return(4.));
    OWL::Fakes::Lane lane2;
    ON_CALL(lane2, GetOdId()).WillByDefault(Return(2));
    ON_CALL(lane2, GetWidth(_)).WillByDefault(Return(5.));
    OWL::Interfaces::Lanes lanesSection1{&lane1, &lane2};
    ON_CALL(section1, GetLanes()).WillByDefault(ReturnRef(lanesSection1));
    OWL::Interfaces::Sections sectionsRoad2{&section1};
    ON_CALL(road2, GetSections()).WillByDefault(ReturnRef(sectionsRoad2));
    RoadStreamElement element2 {&road2, lengthRoad1 + lengthRoad2, false};

    RoadStream roadStream{{element1, element2}};

    const auto result = roadStream.GetRoadPosition(StreamPosition{130., -5.5});

    ASSERT_THAT(result.roadId, Eq(idRoad2));
    ASSERT_THAT(result.laneId, Eq(2));
    ASSERT_THAT(result.roadPosition.s, DoubleEq(190.));
    ASSERT_THAT(result.roadPosition.t, DoubleEq(-1.));
}

TEST(RoadStream_Tests, CreateLaneStream_InStreamDirection)
{
    constexpr double lengthSection1A = 40;
    constexpr double lengthSection1B = 70;
    constexpr double lengthRoad1 = lengthSection1A + lengthSection1B;
    constexpr double lengthSection2A = 80;
    constexpr double lengthSection2B = 130;
    constexpr double lengthRoad2 = lengthSection2A + lengthSection2B;
    const std::string idRoad1 = "Road1";
    const std::string idRoad2 = "Road2";
    constexpr OWL::Id idLane2AMinus1 = 1;
    constexpr OWL::Id idLane2AMinus2 = 2;
    constexpr OWL::Id idLane2BMinus1 = 3;
    constexpr OWL::Id idOtherLane = 4;

    OWL::Fakes::Road road1;
    ON_CALL(road1, GetId()).WillByDefault(ReturnRef(idRoad1));
    ON_CALL(road1, GetLength()).WillByDefault(Return(lengthRoad1));
    OWL::Fakes::Section section1A;
    ON_CALL(section1A, Covers(_)).WillByDefault([&](const double& s){return s >= 0 && s <= lengthSection1A;});
    OWL::Fakes::Section section1B;
    ON_CALL(section1B, Covers(_)).WillByDefault([&](const double& s){return s >= lengthSection1A && s <= lengthRoad1;});
    OWL::Fakes::Lane lane1BMinus1;
    ON_CALL(lane1BMinus1, GetOdId()).WillByDefault(Return(-1));
    OWL::Fakes::Lane lane1BMinus2;
    ON_CALL(lane1BMinus2, GetOdId()).WillByDefault(Return(-2));
    ON_CALL(lane1BMinus2, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(lengthSection1A));
    std::vector<OWL::Id> successorsLane1BMinus2{idOtherLane, idLane2AMinus2};
    ON_CALL(lane1BMinus2, GetNext()).WillByDefault(ReturnRef(successorsLane1BMinus2));
    OWL::Interfaces::Lanes lanesSection1B{&lane1BMinus1, &lane1BMinus2};
    ON_CALL(section1B, GetLanes()).WillByDefault(ReturnRef(lanesSection1B));
    std::list<const OWL::Interfaces::Section*> sections1{&section1A, &section1B};
    ON_CALL(road1, GetSections()).WillByDefault(ReturnRef(sections1));
    RoadStreamElement element1 {&road1, 0., true};

    OWL::Fakes::Road road2;
    ON_CALL(road2, GetId()).WillByDefault(ReturnRef(idRoad2));
    ON_CALL(road2, GetLength()).WillByDefault(Return(lengthRoad2));
    OWL::Fakes::Section section2A;
    OWL::Fakes::Section section2B;
    OWL::Fakes::Lane lane2AMinus1;
    ON_CALL(lane2AMinus1, GetId()).WillByDefault(Return(idLane2AMinus1));
    ON_CALL(lane2AMinus1, GetOdId()).WillByDefault(Return(-1));
    OWL::Fakes::Lane lane2AMinus2;
    ON_CALL(lane2AMinus2, GetId()).WillByDefault(Return(idLane2AMinus2));
    ON_CALL(lane2AMinus2, GetOdId()).WillByDefault(Return(-2));
    ON_CALL(lane2AMinus2, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(0));
    std::vector<OWL::Id> successorsLane2AMinus2{idLane2BMinus1};
    ON_CALL(lane2AMinus2, GetNext()).WillByDefault(ReturnRef(successorsLane2AMinus2));
    OWL::Interfaces::Lanes lanesSection2A{&lane2AMinus1, &lane2AMinus2};
    ON_CALL(section2A, GetLanes()).WillByDefault(ReturnRef(lanesSection2A));
    OWL::Fakes::Lane lane2BMinus1;
    ON_CALL(lane2BMinus1, GetId()).WillByDefault(Return(idLane2BMinus1));
    ON_CALL(lane2BMinus1, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(lane2BMinus1, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(lengthSection2A));
    std::vector<OWL::Id> successorsLane2BMinus1{};
    ON_CALL(lane2BMinus1, GetNext()).WillByDefault(ReturnRef(successorsLane2BMinus1));
    OWL::Interfaces::Lanes lanesSection2B{&lane2BMinus1};
    ON_CALL(section2B, GetLanes()).WillByDefault(ReturnRef(lanesSection2B));
    std::list<const OWL::Interfaces::Section*> sections2{&section2A, &section2B};
    ON_CALL(road2, GetSections()).WillByDefault(ReturnRef(sections2));
    RoadStreamElement element2 {&road2, lengthRoad1, true};

    RoadStream roadStream{{element1, element2}};

    const auto result = roadStream.CreateLaneStream(StreamPosition{50., 0.}, -2);
    ASSERT_THAT(result, SizeIs(3));
    EXPECT_THAT(result.at(0).lane, Eq(&lane1BMinus2));
    EXPECT_THAT(result.at(0).inStreamDirection, Eq(true));
    EXPECT_THAT(result.at(0).sOffset, Eq(lengthSection1A));
    EXPECT_THAT(result.at(1).lane, Eq(&lane2AMinus2));
    EXPECT_THAT(result.at(1).inStreamDirection, Eq(true));
    EXPECT_THAT(result.at(1).sOffset, Eq(lengthRoad1));
    EXPECT_THAT(result.at(2).lane, Eq(&lane2BMinus1));
    EXPECT_THAT(result.at(2).inStreamDirection, Eq(true));
    EXPECT_THAT(result.at(2).sOffset, Eq(lengthRoad1 + lengthSection2A));
}

TEST(RoadStream_Tests, CreateLaneStream_AgainstStreamDirection)
{
    constexpr double lengthSection1A = 40;
    constexpr double lengthSection1B = 70;
    constexpr double lengthRoad1 = lengthSection1A + lengthSection1B;
    constexpr double lengthSection2A = 80;
    constexpr double lengthSection2B = 130;
    constexpr double lengthRoad2 = lengthSection2A + lengthSection2B;
    const std::string idRoad1 = "Road1";
    const std::string idRoad2 = "Road2";
    constexpr OWL::Id idLane2AMinus1 = 1;
    constexpr OWL::Id idLane2AMinus2 = 2;
    constexpr OWL::Id idLane2BMinus1 = 3;
    constexpr OWL::Id idOtherLane = 4;

    OWL::Fakes::Road road1;
    ON_CALL(road1, GetId()).WillByDefault(ReturnRef(idRoad1));
    ON_CALL(road1, GetLength()).WillByDefault(Return(lengthRoad1));
    OWL::Fakes::Section section1A;
    ON_CALL(section1A, Covers(_)).WillByDefault([&](const double& s){return s >= 0 && s <= lengthSection1A;});
    OWL::Fakes::Section section1B;
    ON_CALL(section1B, Covers(_)).WillByDefault([&](const double& s){return s >= lengthSection1A && s <= lengthRoad1;});
    OWL::Fakes::Lane lane1AMinus1;
    ON_CALL(lane1AMinus1, GetOdId()).WillByDefault(Return(-1));
    OWL::Fakes::Lane lane1AMinus2;
    ON_CALL(lane1AMinus2, GetOdId()).WillByDefault(Return(-2));
    ON_CALL(lane1AMinus2, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(0.));
    std::vector<OWL::Id> predecessorsLane1AMinus2{idOtherLane, idLane2BMinus1};
    ON_CALL(lane1AMinus2, GetPrevious()).WillByDefault(ReturnRef(predecessorsLane1AMinus2));
    OWL::Interfaces::Lanes lanesSection1A{&lane1AMinus1, &lane1AMinus2};
    ON_CALL(section1A, GetLanes()).WillByDefault(ReturnRef(lanesSection1A));
    std::list<const OWL::Interfaces::Section*> sections1{&section1A, &section1B};
    ON_CALL(road1, GetSections()).WillByDefault(ReturnRef(sections1));
    RoadStreamElement element1 {&road1, lengthRoad1, false};

    OWL::Fakes::Road road2;
    ON_CALL(road2, GetId()).WillByDefault(ReturnRef(idRoad2));
    ON_CALL(road2, GetLength()).WillByDefault(Return(lengthRoad2));
    OWL::Fakes::Section section2A;
    OWL::Fakes::Section section2B;
    OWL::Fakes::Lane lane2AMinus1;
    ON_CALL(lane2AMinus1, GetId()).WillByDefault(Return(idLane2AMinus1));
    ON_CALL(lane2AMinus1, GetOdId()).WillByDefault(Return(-1));
    OWL::Fakes::Lane lane2AMinus2;
    ON_CALL(lane2AMinus2, GetId()).WillByDefault(Return(idLane2AMinus2));
    ON_CALL(lane2AMinus2, GetOdId()).WillByDefault(Return(-2));
    ON_CALL(lane2AMinus2, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(0));
    std::vector<OWL::Id> predecessorsLane2AMinus2{};
    ON_CALL(lane2AMinus2, GetPrevious()).WillByDefault(ReturnRef(predecessorsLane2AMinus2));
    OWL::Interfaces::Lanes lanesSection2A{&lane2AMinus1, &lane2AMinus2};
    ON_CALL(section2A, GetLanes()).WillByDefault(ReturnRef(lanesSection2A));
    OWL::Fakes::Lane lane2BMinus1;
    ON_CALL(lane2BMinus1, GetId()).WillByDefault(Return(idLane2BMinus1));
    ON_CALL(lane2BMinus1, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(lane2BMinus1, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(lengthSection2A));
    std::vector<OWL::Id> predecessorsLane2BMinus1{idLane2AMinus2};
    ON_CALL(lane2BMinus1, GetPrevious()).WillByDefault(ReturnRef(predecessorsLane2BMinus1));
    OWL::Interfaces::Lanes lanesSection2B{&lane2BMinus1};
    ON_CALL(section2B, GetLanes()).WillByDefault(ReturnRef(lanesSection2B));
    std::list<const OWL::Interfaces::Section*> sections2{&section2A, &section2B};
    ON_CALL(road2, GetSections()).WillByDefault(ReturnRef(sections2));
    RoadStreamElement element2 {&road2, lengthRoad1 + lengthRoad2, false};

    RoadStream roadStream{{element1, element2}};

    const auto result = roadStream.CreateLaneStream(StreamPosition{90., 0.}, -2);
    ASSERT_THAT(result, SizeIs(3));
    EXPECT_THAT(result.at(0).lane, Eq(&lane1AMinus2));
    EXPECT_THAT(result.at(0).inStreamDirection, Eq(false));
    EXPECT_THAT(result.at(0).sOffset, Eq(lengthRoad1));
    EXPECT_THAT(result.at(1).lane, Eq(&lane2BMinus1));
    EXPECT_THAT(result.at(1).inStreamDirection, Eq(false));
    EXPECT_THAT(result.at(1).sOffset, Eq(lengthRoad1 + lengthSection2B));
    EXPECT_THAT(result.at(2).lane, Eq(&lane2AMinus2));
    EXPECT_THAT(result.at(2).inStreamDirection, Eq(false));
    EXPECT_THAT(result.at(2).sOffset, Eq(lengthRoad1 + lengthRoad2));
}

TEST(RoadStream_Tests, GetAllLaneStreams_SingleRoadWithTwoSections)
{
    constexpr double lengthSection1 = 40;
    constexpr double lengthSection2 = 70;
    constexpr OWL::Id idLane1A = 10;
    constexpr OWL::Id idLane1B = 11;
    constexpr OWL::Id idLane2A = 12;
    constexpr OWL::Id idLane2B = 13;

    std::vector<OWL::Id> emptyIds{};
    OWL::Fakes::Road road;

    OWL::Fakes::Section section1;
    ON_CALL(section1, Covers(_)).WillByDefault([&](const double& s){return s >= 0 && s <= lengthSection1;});
    OWL::Fakes::Lane lane1A;
    ON_CALL(lane1A, GetId()).WillByDefault(Return(idLane1A));
    ON_CALL(lane1A, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(lane1A, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(0.));
    ON_CALL(lane1A, GetNext()).WillByDefault(ReturnRef(emptyIds));
    ON_CALL(lane1A, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    OWL::Fakes::Lane lane1B;
    ON_CALL(lane1B, GetId()).WillByDefault(Return(idLane1B));
    ON_CALL(lane1B, GetOdId()).WillByDefault(Return(-2));
    ON_CALL(lane1B, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(0.));
    std::vector<OWL::Id> successorsLane1B{idLane2A};
    ON_CALL(lane1B, GetNext()).WillByDefault(ReturnRef(successorsLane1B));
    ON_CALL(lane1B, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    OWL::Interfaces::Lanes lanesSection1{&lane1A, &lane1B};
    ON_CALL(section1, GetLanes()).WillByDefault(ReturnRef(lanesSection1));

    OWL::Fakes::Section section2;
    ON_CALL(section2, Covers(_)).WillByDefault([&](const double& s){return s >= lengthSection1 && s <= lengthSection1 + lengthSection2;});
    OWL::Fakes::Lane lane2A;
    ON_CALL(lane2A, GetId()).WillByDefault(Return(idLane2A));
    ON_CALL(lane2A, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(lane2A, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(lengthSection1));
    ON_CALL(lane2A, GetNext()).WillByDefault(ReturnRef(emptyIds));
    std::vector<OWL::Id> predecessorsLane2A{idLane1B};
    ON_CALL(lane2A, GetPrevious()).WillByDefault(ReturnRef(predecessorsLane2A));
    OWL::Fakes::Lane lane2B;
    ON_CALL(lane2B, GetId()).WillByDefault(Return(idLane2B));
    ON_CALL(lane2B, GetOdId()).WillByDefault(Return(-2));
    ON_CALL(lane2B, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(lengthSection1));
    ON_CALL(lane2B, GetNext()).WillByDefault(ReturnRef(emptyIds));
    ON_CALL(lane2B, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    OWL::Interfaces::Lanes lanesSection2{&lane2A, &lane2B};
    ON_CALL(section2, GetLanes()).WillByDefault(ReturnRef(lanesSection2));

    OWL::Interfaces::Sections sections{&section1, &section2};
    ON_CALL(road, GetSections()).WillByDefault(ReturnRef(sections));
    RoadStreamElement element{&road, 0., true};

    RoadStream roadStream{{element}};

    const auto result = roadStream.GetAllLaneStreams();

    ASSERT_THAT(result, SizeIs(3));
}

TEST(LaneStream_Tests, GetAgentsInRange_EmptyLanes)
{
    constexpr double lengthLane1 = 90;
    constexpr double lengthLane2 = 120;
    OWL::Interfaces::LaneAssignments emptyObjects{};
    OWL::Fakes::Lane lane1;
    ON_CALL(lane1, GetLength()).WillByDefault(Return(lengthLane1));
    ON_CALL(lane1, GetWorldObjects(true)).WillByDefault(ReturnRef(emptyObjects));
    LaneStreamElement element1{&lane1, 0, true};
    OWL::Fakes::Lane lane2;
    ON_CALL(lane2, GetLength()).WillByDefault(Return(lengthLane2));
    ON_CALL(lane2, GetWorldObjects(true)).WillByDefault(ReturnRef(emptyObjects));
    LaneStreamElement element2{&lane2, lengthLane1, true};

    LaneStream laneStream{{element1, element2}};

    auto result = laneStream.GetAgentsInRange({0,0}, {210,0});
    ASSERT_THAT(result, IsEmpty());
}

TEST(LaneStream_Tests, GetAgentsInRange_InStreamDirection)
{
    constexpr double lengthLane1 = 90;
    constexpr double lengthLane2 = 120;
    OWL::Fakes::MovingObject moving1;
    OWL::Fakes::MovingObject moving2;
    OWL::Fakes::MovingObject moving3;
    OWL::Fakes::MovingObject moving4;
    OWL::Fakes::StationaryObject stationary;
    FakeAgent agent1;
    FakeAgent agent2;
    FakeAgent agent3;
    FakeAgent agent4;
    moving1.SetLinkedObjectForTesting(&agent1);
    moving2.SetLinkedObjectForTesting(&agent2);
    moving3.SetLinkedObjectForTesting(&agent3);
    moving4.SetLinkedObjectForTesting(&agent4);
    OWL::Interfaces::LaneAssignments objectsLane1{{{10., 15., 0., 0.}, &moving1},
                                                  {{40., 45., 0., 0.}, &moving2},
                                                  {{60., 65., 0., 0.}, &stationary}};
    OWL::Interfaces::LaneAssignments objectsLane2{{{100., 105., 0., 0.}, &moving2},
                                                  {{130., 135., 0., 0.}, &moving3},
                                                  {{150., 155., 0., 0.}, &moving4}};
    OWL::Fakes::Lane lane1;
    ON_CALL(lane1, GetLength()).WillByDefault(Return(lengthLane1));
    ON_CALL(lane1, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(0.));
    ON_CALL(lane1, GetWorldObjects(true)).WillByDefault(ReturnRef(objectsLane1));
    LaneStreamElement element1{&lane1, 0, true};
    OWL::Fakes::Lane lane2;
    ON_CALL(lane2, GetLength()).WillByDefault(Return(lengthLane2));
    ON_CALL(lane2, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(lengthLane1));
    ON_CALL(lane2, GetWorldObjects(true)).WillByDefault(ReturnRef(objectsLane2));
    LaneStreamElement element2{&lane2, lengthLane1, true};

    LaneStream laneStream{{element1, element2}};

    auto result = laneStream.GetAgentsInRange({20,0}, {140,0});
    ASSERT_THAT(result, ElementsAre(&agent2, &agent3));
}

TEST(LaneStream_Tests, GetAgentsInRange_AgainstStreamDirection)
{
    constexpr double lengthLane1 = 90;
    constexpr double lengthLane2 = 120;
    OWL::Fakes::MovingObject moving1;
    OWL::Fakes::MovingObject moving2;
    OWL::Fakes::MovingObject moving3;
    OWL::Fakes::MovingObject moving4;
    OWL::Fakes::StationaryObject stationary;
    FakeAgent agent1;
    FakeAgent agent2;
    FakeAgent agent3;
    FakeAgent agent4;
    moving1.SetLinkedObjectForTesting(&agent1);
    moving2.SetLinkedObjectForTesting(&agent2);
    moving3.SetLinkedObjectForTesting(&agent3);
    moving4.SetLinkedObjectForTesting(&agent4);
    OWL::Interfaces::LaneAssignments objectsLane1{{{80., 85., 0., 0.}, &moving1},
                                                  {{60., 65., 0., 0.}, &stationary},
                                                  {{10., 15., 0., 0.}, &moving2}};
    OWL::Interfaces::LaneAssignments objectsLane2{{{170., 175., 0., 0.}, &moving2},
                                                  {{130., 135., 0., 0.}, &moving3},
                                                  {{100., 105., 0., 0.}, &moving4}};
    OWL::Fakes::Lane lane1;
    ON_CALL(lane1, GetLength()).WillByDefault(Return(lengthLane1));
    ON_CALL(lane1, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(0.));
    ON_CALL(lane1, GetWorldObjects(false)).WillByDefault(ReturnRef(objectsLane1));
    LaneStreamElement element1{&lane1, lengthLane1, false};
    OWL::Fakes::Lane lane2;
    ON_CALL(lane2, GetLength()).WillByDefault(Return(lengthLane2));
    ON_CALL(lane2, GetWorldObjects(false)).WillByDefault(ReturnRef(objectsLane2));
    ON_CALL(lane2, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(lengthLane1));
    LaneStreamElement element2{&lane2, lengthLane1 + lengthLane2, false};

    LaneStream laneStream{{element1, element2}};

    auto result = laneStream.GetAgentsInRange({20,0}, {190,0});
    ASSERT_THAT(result, ElementsAre(&agent2, &agent3));
}

TEST(LaneStream_Tests, GetStreamDistanceWithAgent_SingleRoadWithThreeSections)
{
    constexpr double lengthLane1 = 90;
    constexpr double lengthLane2 = 120;
    constexpr double lengthLane3 = 210;

    OWL::Fakes::Road road;
    const std::string idRoad = "RoadB";
    ON_CALL(road, GetId()).WillByDefault(ReturnRef(idRoad));

    OWL::Fakes::Lane lane1;
    OWL::Fakes::Section section1;
    ON_CALL(section1, Covers(_)).WillByDefault([&](double s){return s >= 0 && s <= lengthLane1;});
    ON_CALL(lane1, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(0.));
    ON_CALL(lane1, GetSection()).WillByDefault(ReturnRef(section1));
    ON_CALL(lane1, GetRoad()).WillByDefault(ReturnRef(road));
    ON_CALL(lane1, GetOdId()).WillByDefault(Return(-2));
    LaneStreamElement element1{&lane1, 0, true};

    OWL::Fakes::Lane lane2;
    OWL::Fakes::Section section2;
    ON_CALL(section2, Covers(_)).WillByDefault([&](double s){return s >= lengthLane1 && s <= lengthLane1 +  lengthLane2;});
    ON_CALL(lane2, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(lengthLane1));
    ON_CALL(lane2, GetSection()).WillByDefault(ReturnRef(section2));
    ON_CALL(lane2, GetRoad()).WillByDefault(ReturnRef(road));
    ON_CALL(lane2, GetOdId()).WillByDefault(Return(-2));
    LaneStreamElement element2{&lane2, lengthLane1, true};

    OWL::Fakes::Lane lane3;
    OWL::Fakes::Section section3;
    ON_CALL(section3, Covers(_)).WillByDefault([&](double s){return s >= lengthLane1 + lengthLane2 && s <= lengthLane1 + lengthLane2 + lengthLane3;});
    ON_CALL(lane3, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(lengthLane1 + lengthLane2));
    ON_CALL(lane3, GetSection()).WillByDefault(ReturnRef(section3));
    ON_CALL(lane3, GetRoad()).WillByDefault(ReturnRef(road));
    ON_CALL(lane3, GetOdId()).WillByDefault(Return(-2));
    LaneStreamElement element3{&lane3, lengthLane1 + lengthLane2, true};

    FakeAgent agent;
    ObjectPosition agentPosition;
    agentPosition.referencePoint = {{"RoadA", {}}, {"RoadB", GlobalRoadPosition{"RoadB", -2, 200., 1., 0.}}};
    agentPosition.mainLocatePoint = {{"RoadA", {}}, {"RoadB", GlobalRoadPosition{"RoadB", -2, 220., -1., 0.}}};
    agentPosition.touchedRoads = {{"RoadA", {}}, {"RoadB", {{-2, -3}, 190., 225., {}}}};
    ON_CALL(agent, GetObjectPosition()).WillByDefault(ReturnRef(agentPosition));
    LaneStream laneStream{{element1, element2, element3}};

    auto referencePoint = laneStream.GetStreamPosition(&agent, MeasurementPoint::Reference);
    auto front = laneStream.GetStreamPosition(&agent, MeasurementPoint::Front);
    auto rear = laneStream.GetStreamPosition(&agent, MeasurementPoint::Rear);

    EXPECT_THAT(referencePoint->s, DoubleEq(200.));
    EXPECT_THAT(referencePoint->t, DoubleEq(1.));
    EXPECT_THAT(front->s, DoubleEq(220.));
    EXPECT_THAT(front->t, DoubleEq(-1.));
    EXPECT_THAT(rear->s, DoubleEq(190.));
}

TEST(LaneStream_Tests, GetStreamDistanceWithAgent_ThreeRoads)
{
    constexpr double lengthLane1 = 90;
    constexpr double lengthLane2 = 120;
    constexpr double lengthLane3 = 210;

    OWL::Fakes::Road road1;
    const std::string idRoad1 = "RoadA";
    ON_CALL(road1, GetId()).WillByDefault(ReturnRef(idRoad1));
    OWL::Fakes::Road road2;
    const std::string idRoad2 = "RoadB";
    ON_CALL(road2, GetId()).WillByDefault(ReturnRef(idRoad2));
    OWL::Fakes::Road road3;
    const std::string idRoad3 = "RoadC";
    ON_CALL(road3, GetId()).WillByDefault(ReturnRef(idRoad3));

    OWL::Fakes::Lane lane1;
    OWL::Fakes::Section section1;
    ON_CALL(section1, Covers(_)).WillByDefault([&](double s){return s >= 0 && s <= lengthLane1;});
    ON_CALL(lane1, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(0.));
    ON_CALL(lane1, GetSection()).WillByDefault(ReturnRef(section1));
    ON_CALL(lane1, GetRoad()).WillByDefault(ReturnRef(road1));
    ON_CALL(lane1, GetOdId()).WillByDefault(Return(-2));
    LaneStreamElement element1{&lane1, 0, true};

    OWL::Fakes::Lane lane2;
    OWL::Fakes::Section section2;
    ON_CALL(section2, Covers(_)).WillByDefault([&](double s){return s >= 0 && s <= lengthLane2;});
    ON_CALL(lane2, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(0.));
    ON_CALL(lane2, GetSection()).WillByDefault(ReturnRef(section2));
    ON_CALL(lane2, GetRoad()).WillByDefault(ReturnRef(road2));
    ON_CALL(lane2, GetOdId()).WillByDefault(Return(-2));
    LaneStreamElement element2{&lane2, lengthLane1 + lengthLane2, false};

    OWL::Fakes::Lane lane3;
    OWL::Fakes::Section section3;
    ON_CALL(section3, Covers(_)).WillByDefault([&](double s){return s >= 0 && s <= lengthLane3;});
    ON_CALL(lane3, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(0.));
    ON_CALL(lane3, GetSection()).WillByDefault(ReturnRef(section3));
    ON_CALL(lane3, GetRoad()).WillByDefault(ReturnRef(road3));
    ON_CALL(lane3, GetOdId()).WillByDefault(Return(-2));
    LaneStreamElement element3{&lane3, lengthLane1 + lengthLane2, true};

    FakeAgent agent;
    ObjectPosition agentPosition;
    agentPosition.referencePoint = {{"RoadX", {}}, {"RoadB", GlobalRoadPosition{"RoadB", -2, 10., 1., 0.}}};
    agentPosition.mainLocatePoint = {{"RoadX", {}}, {"RoadC", GlobalRoadPosition{"RoadC", -2, 5., -1., 0.}}};
    agentPosition.touchedRoads = {{"RoadX", {}}, {"RoadB", {{-2, -3}, 0., 15., {}}}, {"RoadC", {{-2, -3}, 0., 10., {}}}};
    ON_CALL(agent, GetObjectPosition()).WillByDefault(ReturnRef(agentPosition));
    LaneStream laneStream{{element1, element2, element3}};

    auto referencePoint = laneStream.GetStreamPosition(&agent, MeasurementPoint::Reference);
    auto front = laneStream.GetStreamPosition(&agent, MeasurementPoint::Front);
    auto rear = laneStream.GetStreamPosition(&agent, MeasurementPoint::Rear);

    EXPECT_THAT(referencePoint->s, DoubleEq(200.));
    EXPECT_THAT(referencePoint->t, DoubleEq(1.));
    EXPECT_THAT(front->s, DoubleEq(215.));
    EXPECT_THAT(front->t, DoubleEq(-1.));
    EXPECT_THAT(rear->s, DoubleEq(195.));
}

TEST(LaneStream_Tests, GetStreamDistanceWithAgent_AgentOffLane)
{
    constexpr double lengthLane1 = 90;
    constexpr double lengthLane2 = 120;
    constexpr double lengthLane3 = 210;

    OWL::Fakes::Road road;
    const std::string idRoad = "RoadB";
    ON_CALL(road, GetId()).WillByDefault(ReturnRef(idRoad));

    OWL::Fakes::Lane lane1;
    OWL::Fakes::Section section1;
    ON_CALL(section1, Covers(_)).WillByDefault([&](double s){return s >= 0 && s <= lengthLane1;});
    ON_CALL(lane1, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(0.));
    ON_CALL(lane1, GetSection()).WillByDefault(ReturnRef(section1));
    ON_CALL(lane1, GetRoad()).WillByDefault(ReturnRef(road));
    ON_CALL(lane1, GetOdId()).WillByDefault(Return(-2));
    LaneStreamElement element1{&lane1, 0, true};

    OWL::Fakes::Lane lane2;
    OWL::Fakes::Section section2;
    ON_CALL(section2, Covers(_)).WillByDefault([&](double s){return s >= lengthLane1 && s <= lengthLane1 +  lengthLane2;});
    ON_CALL(lane2, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(lengthLane1));
    ON_CALL(lane2, GetSection()).WillByDefault(ReturnRef(section2));
    ON_CALL(lane2, GetRoad()).WillByDefault(ReturnRef(road));
    ON_CALL(lane2, GetOdId()).WillByDefault(Return(-2));
    LaneStreamElement element2{&lane2, lengthLane1, true};

    OWL::Fakes::Lane lane3;
    OWL::Fakes::Section section3;
    ON_CALL(section3, Covers(_)).WillByDefault([&](double s){return s >= lengthLane1 + lengthLane2 && s <= lengthLane1 + lengthLane2 + lengthLane3;});
    ON_CALL(lane3, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(lengthLane1 + lengthLane2));
    ON_CALL(lane3, GetSection()).WillByDefault(ReturnRef(section3));
    ON_CALL(lane3, GetRoad()).WillByDefault(ReturnRef(road));
    ON_CALL(lane3, GetOdId()).WillByDefault(Return(-3));
    LaneStreamElement element3{&lane3, lengthLane1 + lengthLane2, true};

    FakeAgent agent;
    ObjectPosition agentPosition;
    agentPosition.referencePoint = {{"RoadA", {}}, {"RoadB", GlobalRoadPosition{"RoadB", -3, 200., 1., 0.}}};
    agentPosition.mainLocatePoint = {{"RoadA", {}}, {"RoadB", GlobalRoadPosition{"RoadB", -2, 220., -1., 0.}}};
    agentPosition.touchedRoads = {{"RoadA", {}}, {"RoadB", {{-4}, 190., 225., {}}}};
    ON_CALL(agent, GetObjectPosition()).WillByDefault(ReturnRef(agentPosition));
    LaneStream laneStream{{element1, element2, element3}};

    auto referencePoint = laneStream.GetStreamPosition(&agent, MeasurementPoint::Reference);
    auto front = laneStream.GetStreamPosition(&agent, MeasurementPoint::Front);
    auto rear = laneStream.GetStreamPosition(&agent, MeasurementPoint::Rear);

    EXPECT_THAT(referencePoint, Eq(std::nullopt));
    EXPECT_THAT(front, Eq(std::nullopt));
    EXPECT_THAT(rear, Eq(std::nullopt));
}

TEST(LaneStream_Tests, GetLaneTypes_AllOfSameType)
{
    constexpr double lengthLane1 = 90;
    constexpr double lengthLane2 = 120;
    constexpr double lengthLane3 = 150;
    OWL::Interfaces::LaneAssignments emptyObjects{};
    OWL::Fakes::Lane lane1;
    ON_CALL(lane1, GetLength()).WillByDefault(Return(lengthLane1));
    ON_CALL(lane1, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(lane1, GetWorldObjects(true)).WillByDefault(ReturnRef(emptyObjects));
    LaneStreamElement element1{&lane1, 0, true};
    OWL::Fakes::Lane lane2;
    ON_CALL(lane2, GetLength()).WillByDefault(Return(lengthLane2));
    ON_CALL(lane2, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(lane2, GetWorldObjects(true)).WillByDefault(ReturnRef(emptyObjects));
    LaneStreamElement element2{&lane2, lengthLane1, true};
    OWL::Fakes::Lane lane3;
    ON_CALL(lane3, GetLength()).WillByDefault(Return(lengthLane3));
    ON_CALL(lane3, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(lane3, GetWorldObjects(true)).WillByDefault(ReturnRef(emptyObjects));
    LaneStreamElement element3{&lane3, lengthLane1 + lengthLane2, true};

    LaneStream laneStream{{element1, element2, element3}};

    auto result = laneStream.GetLaneTypes();

    ASSERT_THAT(result, ElementsAre(std::make_pair(0, LaneType::Driving)));
}

TEST(LaneStream_Tests, GetLaneTypes_MixedTypes)
{
    constexpr double lengthLane1 = 90;
    constexpr double lengthLane2 = 120;
    constexpr double lengthLane3 = 150;
    OWL::Interfaces::LaneAssignments emptyObjects{};
    OWL::Fakes::Lane lane1;
    ON_CALL(lane1, GetLength()).WillByDefault(Return(lengthLane1));
    ON_CALL(lane1, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(lane1, GetWorldObjects(true)).WillByDefault(ReturnRef(emptyObjects));
    LaneStreamElement element1{&lane1, 0, true};
    OWL::Fakes::Lane lane2;
    ON_CALL(lane2, GetLength()).WillByDefault(Return(lengthLane2));
    ON_CALL(lane2, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(lane2, GetWorldObjects(true)).WillByDefault(ReturnRef(emptyObjects));
    LaneStreamElement element2{&lane2, lengthLane1, true};
    OWL::Fakes::Lane lane3;
    ON_CALL(lane3, GetLength()).WillByDefault(Return(lengthLane3));
    ON_CALL(lane3, GetLaneType()).WillByDefault(Return(LaneType::None));
    ON_CALL(lane3, GetWorldObjects(true)).WillByDefault(ReturnRef(emptyObjects));
    LaneStreamElement element3{&lane3, lengthLane1 + lengthLane2, true};

    LaneStream laneStream{{element1, element2, element3}};

    auto result = laneStream.GetLaneTypes();

    ASSERT_THAT(result, ElementsAre(std::make_pair(0, LaneType::Driving),
                                    std::make_pair(lengthLane1 + lengthLane2, LaneType::None)));
}
