# /*********************************************************************
# * Copyright (c) 2019 in-tech GmbH
# *               2019 BMW AG
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/
#include <cmath>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "fakeAgent.h"
#include "fakeParameter.h"
#include "fakeWorld.h"
#include "fakeWorldObject.h"

#include "common/gtest/dontCare.h"

#include "common/commonTools.h"

using ::testing::TestWithParam;
using ::testing::Values;
using ::testing::Return;
using ::testing::NiceMock;
using ::testing::DoubleNear;

struct TtcCalculationTest_Data
{
    double egoX;
    double egoY;
    double egoVelocityX;
    double egoVelocityY;
    double egoAcceleration;
    double egoYaw;
    double egoYawRate;
    double egoYawAcceleration;
    double opponentX;
    double opponentY;
    double opponentVelocityX;
    double opponentVelocityY;
    double opponentAcceleration;
    double opponentYaw;
    double opponentYawRate;
    double opponentYawAcceleration;
    double longitudinalBoundary;
    double lateralBoundary;

    double expectedTtc;
};

class TtcCalcualtionTest: public ::TestWithParam<TtcCalculationTest_Data>
{
};

TEST_P(TtcCalcualtionTest, CalculateObjectTTC_ReturnsCorrectTtc)
{
    auto data = GetParam();
    double fakeCycleTime = 10;
    double fakeMaxTtc = 10.0;

    NiceMock<FakeAgent> ego;
    ON_CALL(ego, GetPositionX()).WillByDefault(Return(data.egoX));
    ON_CALL(ego, GetPositionY()).WillByDefault(Return(data.egoY));
    ON_CALL(ego, GetYaw()).WillByDefault(Return(data.egoYaw));
    ON_CALL(ego, GetRoll()).WillByDefault(Return(0.0));
    ON_CALL(ego, GetYawRate()).WillByDefault(Return(data.egoYawRate));
    ON_CALL(ego, GetYawAcceleration()).WillByDefault(Return(data.egoYawAcceleration));
    ON_CALL(ego, GetVelocity(VelocityScope::DirectionX)).WillByDefault(Return(data.egoVelocityX));
    ON_CALL(ego, GetVelocity(VelocityScope::DirectionY)).WillByDefault(Return(data.egoVelocityY));
    ON_CALL(ego, GetAcceleration()).WillByDefault(Return(data.egoAcceleration));
    ON_CALL(ego, GetLength()).WillByDefault(Return(2.0));
    ON_CALL(ego, GetWidth()).WillByDefault(Return(1.0));
    ON_CALL(ego, GetDistanceReferencePointToLeadingEdge()).WillByDefault(Return(1.0));

    NiceMock<FakeAgent> opponent;
    ON_CALL(opponent, GetPositionX()).WillByDefault(Return(data.opponentX));
    ON_CALL(opponent, GetPositionY()).WillByDefault(Return(data.opponentY));
    ON_CALL(opponent, GetYaw()).WillByDefault(Return(data.opponentYaw));
    ON_CALL(opponent, GetRoll()).WillByDefault(Return(0.0));
    ON_CALL(opponent, GetYawRate()).WillByDefault(Return(data.opponentYawRate));
    ON_CALL(opponent, GetYawAcceleration()).WillByDefault(Return(data.opponentYawAcceleration));
    ON_CALL(opponent, GetVelocity(VelocityScope::DirectionX)).WillByDefault(Return(data.opponentVelocityX));
    ON_CALL(opponent, GetVelocity(VelocityScope::DirectionY)).WillByDefault(Return(data.opponentVelocityY));
    ON_CALL(opponent, GetAcceleration()).WillByDefault(Return(data.opponentAcceleration));
    ON_CALL(opponent, GetLength()).WillByDefault(Return(2.0));
    ON_CALL(opponent, GetWidth()).WillByDefault(Return(1.0));
    ON_CALL(opponent, GetDistanceReferencePointToLeadingEdge()).WillByDefault(Return(1.0));

    auto result = TtcCalculations::CalculateObjectTTC(ego, opponent, fakeMaxTtc, data.longitudinalBoundary, data.lateralBoundary, fakeCycleTime);
    ASSERT_THAT(result, DoubleNear(data.expectedTtc, 0.001));
}

INSTANTIATE_TEST_CASE_P(TtcCalculationTestCase, TtcCalcualtionTest, ::testing::Values(
//                       Ego                                           opponent
//                           x,     y,  v_x, v_y,   a, yaw, yawRate, yawAcc,     x,     y,  v_x,  v_y,   a,     yaw, yawRate, yawAcc, long, lat,     ttc
TtcCalculationTest_Data{ -20.0,   0.0, 10.0, 0.0, 0.0, 0.0,     0.0,    0.0,   0.0,   0.0, 12.0,  0.0, 0.0,     0.0,     0.0,    0.0,  0.0, 0.0, DBL_MAX},
TtcCalculationTest_Data{   0.0,   0.0, 12.0, 0.0, 0.0, 0.0,     0.0,    0.0, -20.0,   0.0, 10.0,  0.0, 0.0,     0.0,     0.0,    0.0,  0.0, 0.0, DBL_MAX},
TtcCalculationTest_Data{   0.0,  10.0, 10.0, 0.0, 0.0, 0.0,     0.0,    0.0,  40.5,  10.0,  2.0,  0.0, 0.0,     0.0,     0.0,    0.0,  0.0, 0.0,    4.82},
TtcCalculationTest_Data{  20.0, -10.0, 10.0, 0.0, 0.0, 0.0,     0.0,    0.0,   5.0, -10.0, 40.0,  0.0, 0.0,     0.0,     0.0,    0.0,  0.0, 0.0,    0.44},
TtcCalculationTest_Data{   0.0,   0.0,  0.0, 0.0, 0.0, 0.0,     0.0,    0.0,   0.0,  14.0,  0.0, -5.0, 0.0, -M_PI_2,     0.0,    0.0, 10.0, 5.0,     1.0},
TtcCalculationTest_Data{   0.0,   0.0,  0.0, 0.0, 0.0, 0.0,     0.0,    0.0, -50.0,   0.0,  5.0,  0.0, 2.0,     0.0,     0.0,    0.0,  0.0, 0.0,    4.87},
TtcCalculationTest_Data{   0.0,   0.0,  0.0, 0.0, 0.0, 0.0,     0.0,    0.0, -10.0, -10.0,  0.0, 10.0, 0.0,  M_PI_2,    -1.0,    0.0,  0.0, 0.0,    1.38}
));
