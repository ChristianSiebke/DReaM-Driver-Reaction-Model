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

using ::testing::Return;
using ::testing::Eq;
using ::testing::DoubleNear;

TEST(TrafficSigns, SetSpecificationTypeOnly)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("333"));

    osi3::TrafficSign osiSign;
    OWL::Implementation::TrafficSign trafficSign(&osiSign);

    trafficSign.SetSpecification(&roadSignal);

    const auto specification = trafficSign.GetSpecification(5);
    ASSERT_THAT(specification.type, Eq(CommonTrafficSign::Type::HighWayExit));
    ASSERT_THAT(osiSign.main_sign().classification().type(), Eq(osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_HIGHWAY_EXIT));
};

TEST(TrafficSigns, SetSpecificationSubtypeDefinesValue)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("450"));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("51"));

    osi3::TrafficSign osiSign;
    OWL::Implementation::TrafficSign trafficSign(&osiSign);

    trafficSign.SetSpecification(&roadSignal);

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

    osi3::TrafficSign osiSign;
    OWL::Implementation::TrafficSign trafficSign(&osiSign);

    trafficSign.SetSpecification(&roadSignal);

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

    osi3::TrafficSign osiSign;
    OWL::Implementation::TrafficSign trafficSign(&osiSign);

    trafficSign.SetSpecification(&roadSignal);

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

    osi3::TrafficSign osiSign;
    OWL::Implementation::TrafficSign trafficSign(&osiSign);

    trafficSign.SetSpecification(&mainSignal);
    trafficSign.AddSupplementarySign(&supplementarySignal);

    const auto specification = trafficSign.GetSpecification(5);
    ASSERT_THAT(specification.type, Eq(CommonTrafficSign::Type::OvertakingBanBegin));
    ASSERT_THAT(specification.supplementarySigns.size(), Eq(1));
    ASSERT_THAT(specification.supplementarySigns.front().type, Eq(CommonTrafficSign::Type::DistanceIndication));
    ASSERT_THAT(specification.supplementarySigns.front().value, Eq(100));
    ASSERT_THAT(specification.supplementarySigns.front().unit, Eq(CommonTrafficSign::Unit::Meter));
    ASSERT_THAT(osiSign.main_sign().classification().type(), Eq(osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_OVERTAKING_BAN_BEGIN));
    ASSERT_THAT(osiSign.supplementary_sign().Get(0).classification().type(), Eq(osi3::TrafficSign_SupplementarySign_Classification_Type::TrafficSign_SupplementarySign_Classification_Type_TYPE_SPACE));
};
