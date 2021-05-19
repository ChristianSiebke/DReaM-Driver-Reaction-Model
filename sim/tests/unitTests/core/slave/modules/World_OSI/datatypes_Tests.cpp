/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "DataTypes.h"

#include "fakeRoadSignal.h"
#include "OWL/fakes/fakeMovingObject.h"

using ::testing::Return;
using ::testing::Eq;
using ::testing::DoubleEq;
using ::testing::DoubleNear;
using ::testing::SizeIs;
using ::testing::ElementsAre;

TEST(TrafficSigns, SetSpecificationWithUnsupportedMainType_ReturnsFalse)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("unsupported_type_string"));
    Position position{};

    osi3::TrafficSign osiSign;
    OWL::Implementation::TrafficSign trafficSign(&osiSign);

    ASSERT_THAT(trafficSign.SetSpecification(&roadSignal, position), Eq(false));
}

TEST(TrafficSigns, SetSpecificationTypeOnly)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("333"));
    Position position{};

    osi3::TrafficSign osiSign;
    OWL::Implementation::TrafficSign trafficSign(&osiSign);

    ASSERT_THAT(trafficSign.SetSpecification(&roadSignal, position), Eq(true));

    const auto specification = trafficSign.GetSpecification(5);

    ASSERT_THAT(specification.type, Eq(CommonTrafficSign::Type::HighWayExit));
    ASSERT_THAT(osiSign.main_sign().classification().type(), Eq(osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_HIGHWAY_EXIT));
};

TEST(TrafficSigns, SetSpecificationSubtypeDefinesValue)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("450"));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("51"));
    Position position{};

    osi3::TrafficSign osiSign;
    OWL::Implementation::TrafficSign trafficSign(&osiSign);

    ASSERT_THAT(trafficSign.SetSpecification(&roadSignal, position), Eq(true));

    const auto specification = trafficSign.GetSpecification(5);

    ASSERT_THAT(specification.type, Eq(CommonTrafficSign::Type::HighwayExitPole));
    ASSERT_THAT(specification.value, Eq(200.0));
    ASSERT_THAT(specification.unit, Eq(CommonTrafficSign::Unit::Meter));
    ASSERT_THAT(osiSign.main_sign().classification().type(), Eq(osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_POLE_EXIT));
};

TEST(TrafficSigns, SetSpecificationSubtypeIsValue)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("278"));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("80"));
    Position position{};

    osi3::TrafficSign osiSign;
    OWL::Implementation::TrafficSign trafficSign(&osiSign);

    ASSERT_THAT(trafficSign.SetSpecification(&roadSignal, position), Eq(true));

    const auto specification = trafficSign.GetSpecification(5);

    ASSERT_THAT(specification.type, Eq(CommonTrafficSign::Type::EndOfMaximumSpeedLimit));
    ASSERT_THAT(specification.value, DoubleNear(80.0 / 3.6, 1e-3));
    ASSERT_THAT(specification.unit, Eq(CommonTrafficSign::Unit::MeterPerSecond));
    ASSERT_THAT(osiSign.main_sign().classification().type(), Eq(osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_SPEED_LIMIT_END));
};

TEST(TrafficSigns, SetSpecificationWithText)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("310"));
    ON_CALL(roadSignal, GetText()).WillByDefault(Return("SomeText"));
    Position position{};

    osi3::TrafficSign osiSign;
    OWL::Implementation::TrafficSign trafficSign(&osiSign);

    ASSERT_THAT(trafficSign.SetSpecification(&roadSignal, position), Eq(true));

    const auto specification = trafficSign.GetSpecification(5);

    ASSERT_THAT(specification.type, Eq(CommonTrafficSign::Type::TownBegin));
    ASSERT_THAT(specification.text, Eq("SomeText"));
    ASSERT_THAT(osiSign.main_sign().classification().type(), Eq(osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_TOWN_BEGIN));
};

TEST(TrafficSigns, SetSpecificationWithSupplementarySign)
{
    FakeRoadSignal mainSignal;
    ON_CALL(mainSignal, GetType()).WillByDefault(Return("276"));
    FakeRoadSignal supplementarySignal;
    ON_CALL(supplementarySignal, GetType()).WillByDefault(Return("1004"));
    ON_CALL(supplementarySignal, GetSubType()).WillByDefault(Return("30"));
    ON_CALL(supplementarySignal, GetValue()).WillByDefault(Return(100));
    Position position{};

    osi3::TrafficSign osiSign;
    OWL::Implementation::TrafficSign trafficSign(&osiSign);

    ASSERT_THAT(trafficSign.SetSpecification(&mainSignal, position), Eq(true));

    trafficSign.AddSupplementarySign(&supplementarySignal, position);
    const auto specification = trafficSign.GetSpecification(5);

    ASSERT_THAT(specification.type, Eq(CommonTrafficSign::Type::OvertakingBanBegin));
    ASSERT_THAT(specification.supplementarySigns.size(), Eq(1));
    ASSERT_THAT(specification.supplementarySigns.front().type, Eq(CommonTrafficSign::Type::DistanceIndication));
    ASSERT_THAT(specification.supplementarySigns.front().value, Eq(100));
    ASSERT_THAT(specification.supplementarySigns.front().unit, Eq(CommonTrafficSign::Unit::Meter));
    ASSERT_THAT(osiSign.main_sign().classification().type(), Eq(osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_OVERTAKING_BAN_BEGIN));
    ASSERT_THAT(osiSign.supplementary_sign().Get(0).classification().type(), Eq(osi3::TrafficSign_SupplementarySign_Classification_Type::TrafficSign_SupplementarySign_Classification_Type_TYPE_SPACE));
};

TEST(TrafficSigns, SetSpecification_SetsCorrectBaseStationaryForPositiveOrientation)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("333"));
    ON_CALL(roadSignal, GetWidth()).WillByDefault(Return(4.0));
    ON_CALL(roadSignal, GetHeight()).WillByDefault(Return(5.0));
    ON_CALL(roadSignal, GetZOffset()).WillByDefault(Return(3.0));
    ON_CALL(roadSignal, GetHOffset()).WillByDefault(Return(2.0));
    ON_CALL(roadSignal, GetOrientation()).WillByDefault(Return(true));
    Position position{10, 11, -1.5, 0};

    osi3::TrafficSign osiSign;
    OWL::Implementation::TrafficSign trafficSign(&osiSign);

    ASSERT_THAT(trafficSign.SetSpecification(&roadSignal, position), Eq(true));
    ASSERT_THAT(osiSign.main_sign().base().position().x(), Eq(10));
    ASSERT_THAT(osiSign.main_sign().base().position().y(), Eq(11));
    ASSERT_THAT(osiSign.main_sign().base().position().z(), Eq(5.5));
    ASSERT_THAT(osiSign.main_sign().base().dimension().width(), Eq(4));
    ASSERT_THAT(osiSign.main_sign().base().dimension().height(), Eq(5));
    ASSERT_THAT(osiSign.main_sign().base().orientation().yaw(), DoubleEq(0.5));
}

TEST(TrafficSigns, SetSpecification_SetsCorrectBaseStationaryForNegativeOrientation)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("333"));
    ON_CALL(roadSignal, GetWidth()).WillByDefault(Return(4.0));
    ON_CALL(roadSignal, GetHeight()).WillByDefault(Return(5.0));
    ON_CALL(roadSignal, GetZOffset()).WillByDefault(Return(3.0));
    ON_CALL(roadSignal, GetHOffset()).WillByDefault(Return(1.0));
    ON_CALL(roadSignal, GetOrientation()).WillByDefault(Return(false));
    Position position{10, 11, 1.5, 0};

    osi3::TrafficSign osiSign;
    OWL::Implementation::TrafficSign trafficSign(&osiSign);

    ASSERT_THAT(trafficSign.SetSpecification(&roadSignal, position), Eq(true));
    ASSERT_THAT(osiSign.main_sign().base().position().x(), Eq(10));
    ASSERT_THAT(osiSign.main_sign().base().position().y(), Eq(11));
    ASSERT_THAT(osiSign.main_sign().base().position().z(), Eq(5.5));
    ASSERT_THAT(osiSign.main_sign().base().dimension().width(), Eq(4));
    ASSERT_THAT(osiSign.main_sign().base().dimension().height(), Eq(5));
    ASSERT_THAT(osiSign.main_sign().base().orientation().yaw(), DoubleEq(2.5 - M_PI));
}

TEST(TrafficSigns_GetSpecification, GivenSignWithoutSupplementarySigns_ReturnsCorrectEntity)
{
    osi3::TrafficSign osiSign;
    osiSign.mutable_id()->set_value(1);
    osiSign.mutable_main_sign()->mutable_classification()->mutable_value()->set_value(5.0);

    OWL::Implementation::TrafficSign sign{&osiSign};

    const auto spec = sign.GetSpecification(0.0);

    ASSERT_THAT(spec.value, DoubleEq(5.0));
    ASSERT_THAT(spec.supplementarySigns, SizeIs(0));
}

TEST(TrafficSigns_GetSpecification, GivenSignWithOneSupplementarySign_ReturnsCorrectEntity)
{
    osi3::TrafficSign osiSign;
    auto osiSupplementarySign = osiSign.add_supplementary_sign();

    osiSign.mutable_id()->set_value(1);
    osiSign.mutable_main_sign()->mutable_classification()->mutable_value()->set_value(6.0);

    osiSupplementarySign->mutable_classification()->add_value()->set_value(7.0);
    osiSupplementarySign->mutable_classification()->set_type(osi3::TrafficSign_SupplementarySign_Classification::TYPE_SPACE);

    OWL::Implementation::TrafficSign sign{&osiSign};

    const auto spec = sign.GetSpecification(0.0);

    ASSERT_THAT(spec.value, DoubleEq(6.0));
    ASSERT_THAT(spec.supplementarySigns, SizeIs(1));

    auto supplementary = spec.supplementarySigns.begin();

    EXPECT_THAT(supplementary->type, Eq(CommonTrafficSign::Type::DistanceIndication));
    EXPECT_THAT(supplementary->value, Eq(7.0));
}

TEST(TrafficSigns_GetSpecification, GivenSignWithTwoSupplementarySigns_ReturnsCorrectEntity)
{
    osi3::TrafficSign osiSign;
    auto osiSupplementarySign1 = osiSign.add_supplementary_sign();
    auto osiSupplementarySign2 = osiSign.add_supplementary_sign();

    osiSign.mutable_id()->set_value(1);
    osiSign.mutable_main_sign()->mutable_classification()->mutable_value()->set_value(8.0);

    osiSupplementarySign1->mutable_classification()->add_value()->set_value(9.0);
    osiSupplementarySign1->mutable_classification()->set_type(osi3::TrafficSign_SupplementarySign_Classification::TYPE_SPACE);
    osiSupplementarySign2->mutable_classification()->add_value()->set_value(10.0);
    osiSupplementarySign2->mutable_classification()->set_type(osi3::TrafficSign_SupplementarySign_Classification::TYPE_SPACE);

    OWL::Implementation::TrafficSign sign{&osiSign};

    const auto spec = sign.GetSpecification(0.0);

    ASSERT_THAT(spec.value, DoubleEq(8.0));
    ASSERT_THAT(spec.supplementarySigns, SizeIs(2));

    auto supplementary1 = spec.supplementarySigns.begin();
    auto supplementary2 = std::next(supplementary1);

    EXPECT_THAT(supplementary1->type, Eq(CommonTrafficSign::Type::DistanceIndication));
    EXPECT_THAT(supplementary1->value, Eq(9.0));
    EXPECT_THAT(supplementary2->type, Eq(CommonTrafficSign::Type::DistanceIndication));
    EXPECT_THAT(supplementary2->value, Eq(10.0));
}

TEST(RoadMarking, SetSpecificationWithUnsupportedMainType_ReturnsFalse)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("unsupported_type_string"));
    Position position{};

    osi3::RoadMarking osiMarking;
    OWL::Implementation::RoadMarking roadMarking(&osiMarking);

    ASSERT_THAT(roadMarking.SetSpecification(&roadSignal, position), Eq(false));
}

TEST(RoadMarking, SetSpecificationTypeOnly)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("294"));
    Position position{};

    osi3::RoadMarking osiMarking;
    OWL::Implementation::RoadMarking roadMarking(&osiMarking);

    ASSERT_THAT(roadMarking.SetSpecification(&roadSignal, position), Eq(true));

    const auto specification = roadMarking.GetSpecification(5);

    ASSERT_THAT(specification.type, Eq(CommonTrafficSign::Type::Stop));
    ASSERT_THAT(osiMarking.classification().type(), Eq(osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_SYMBOLIC_TRAFFIC_SIGN));
    ASSERT_THAT(osiMarking.classification().traffic_main_sign_type(), Eq(osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_STOP));
}

TEST(RoadMarking, SetSpecification_SetsCorrectBaseStationaryForPositiveOrientation)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("294"));
    ON_CALL(roadSignal, GetWidth()).WillByDefault(Return(4.0));
    ON_CALL(roadSignal, GetHOffset()).WillByDefault(Return(2.0));
    ON_CALL(roadSignal, GetOrientation()).WillByDefault(Return(true));
    Position position{10, 11, -1.5, 0};

    osi3::RoadMarking osiMarking;
    OWL::Implementation::RoadMarking roadMarking(&osiMarking);

    ASSERT_THAT(roadMarking.SetSpecification(&roadSignal, position), Eq(true));
    ASSERT_THAT(osiMarking.base().position().x(), Eq(10));
    ASSERT_THAT(osiMarking.base().position().y(), Eq(11));
    ASSERT_THAT(osiMarking.base().position().z(), Eq(0));
    ASSERT_THAT(osiMarking.base().dimension().width(), Eq(4));
    ASSERT_THAT(osiMarking.base().orientation().yaw(), DoubleEq(0.5));
}

TEST(RoadMarking, SetSpecification_SetsCorrectBaseStationaryForNegativeOrientation)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("294"));
    ON_CALL(roadSignal, GetWidth()).WillByDefault(Return(4.0));
    ON_CALL(roadSignal, GetHOffset()).WillByDefault(Return(1.0));
    ON_CALL(roadSignal, GetOrientation()).WillByDefault(Return(false));
    Position position{10, 11, 1.5, 0};

    osi3::RoadMarking osiMarking;
    OWL::Implementation::RoadMarking roadMarking(&osiMarking);

    ASSERT_THAT(roadMarking.SetSpecification(&roadSignal, position), Eq(true));
    ASSERT_THAT(osiMarking.base().position().x(), Eq(10));
    ASSERT_THAT(osiMarking.base().position().y(), Eq(11));
    ASSERT_THAT(osiMarking.base().position().z(), Eq(0));
    ASSERT_THAT(osiMarking.base().dimension().width(), Eq(4));
    ASSERT_THAT(osiMarking.base().orientation().yaw(), DoubleEq(2.5 - M_PI));
}

namespace OWL
{
bool operator== (const OWL::LaneOverlap& rhs, const OWL::LaneOverlap& lhs)
{
    return rhs.s_min == lhs.s_min && rhs.s_max == lhs.s_max;
}
}

TEST(LaneAssignmentCollector, GetDownstream_ReturnObjectsInCorrectOrder)
{
    OWL::Implementation::Lane::LaneAssignmentCollector laneAssignmentCollector;
    OWL::Fakes::MovingObject object1;
    OWL::Fakes::MovingObject object2;
    OWL::Fakes::MovingObject object3;
    OWL::LaneOverlap overlap1{10,15,0,0};
    OWL::LaneOverlap overlap2{12,17,0,0};
    OWL::LaneOverlap overlap3{11,20,0,0};
    laneAssignmentCollector.Insert(overlap1, &object1);
    laneAssignmentCollector.Insert(overlap2, &object2);
    laneAssignmentCollector.Insert(overlap3, &object3);

    auto result = laneAssignmentCollector.Get(true);

    ASSERT_THAT(result, ElementsAre(OWL::Interfaces::LaneAssignment{overlap1, &object1},
                                    OWL::Interfaces::LaneAssignment{overlap3, &object3},
                                    OWL::Interfaces::LaneAssignment{overlap2, &object2}));
}

TEST(LaneAssignmentCollector, GetUpstream_ReturnObjectsInCorrectOrder)
{
    OWL::Implementation::Lane::LaneAssignmentCollector laneAssignmentCollector;
    OWL::Fakes::MovingObject object1;
    OWL::Fakes::MovingObject object2;
    OWL::Fakes::MovingObject object3;
    OWL::LaneOverlap overlap1{10,15,0,0};
    OWL::LaneOverlap overlap2{12,17,0,0};
    OWL::LaneOverlap overlap3{11,20,0,0};
    laneAssignmentCollector.Insert(overlap1, &object1);
    laneAssignmentCollector.Insert(overlap2, &object2);
    laneAssignmentCollector.Insert(overlap3, &object3);

    auto result = laneAssignmentCollector.Get(false);

    ASSERT_THAT(result, ElementsAre(OWL::Interfaces::LaneAssignment{overlap3, &object3},
                                    OWL::Interfaces::LaneAssignment{overlap2, &object2},
                                    OWL::Interfaces::LaneAssignment{overlap1, &object1}));
}
