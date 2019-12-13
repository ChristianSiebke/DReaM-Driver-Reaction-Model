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
using ::testing::DoubleEq;
using ::testing::DoubleNear;
using ::testing::SizeIs;

TEST(TrafficSigns, SetSpecificationWithUnsupportedMainType_ReturnsFalse)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("unsupported_type_string"));

    osi3::TrafficSign osiSign;
    OWL::Implementation::TrafficSign trafficSign(&osiSign);

    ASSERT_THAT(trafficSign.SetSpecification(&roadSignal), Eq(false));
}

TEST(TrafficSigns, SetSpecificationTypeOnly)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("333"));

    osi3::TrafficSign osiSign;
    OWL::Implementation::TrafficSign trafficSign(&osiSign);

    ASSERT_THAT(trafficSign.SetSpecification(&roadSignal), Eq(true));

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

    ASSERT_THAT(trafficSign.SetSpecification(&roadSignal), Eq(true));

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

    ASSERT_THAT(trafficSign.SetSpecification(&roadSignal), Eq(true));

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

    ASSERT_THAT(trafficSign.SetSpecification(&roadSignal), Eq(true));

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

    ASSERT_THAT(trafficSign.SetSpecification(&mainSignal), Eq(true));

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
