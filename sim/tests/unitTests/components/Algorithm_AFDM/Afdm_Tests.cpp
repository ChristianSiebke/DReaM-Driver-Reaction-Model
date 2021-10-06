/********************************************************************************
 * Copyright (c) 2019-2020 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "followingDriverModel.h"
#include "common/accelerationSignal.h"
#include "sensorDriverSignal.h"

#include "fakeAgent.h"
#include "fakeParameter.h"

using ::testing::DoubleEq;
using ::testing::Eq;
using ::testing::Ge;
using ::testing::Le;
using ::testing::Lt;
using ::testing::ReturnRef;
using ::testing::NiceMock;

TEST(AgentFollowingDriverModel, TriggerWithNoFrontAgentAndVelocityHigherWish_AccWithinLimits)
{
    const double maxAcceleration = 1.4;
    const double maxDeceleration = 2.0;

    NiceMock<FakeParameter> fakeParameters;
    std::map<std::string, double> fakeParametersDouble;

    ON_CALL(fakeParameters, GetParametersDouble()).WillByDefault(ReturnRef(fakeParametersDouble));

    AlgorithmAgentFollowingDriverModelImplementation implementation("",
                                                                    false,
                                                                    0,
                                                                    0,
                                                                    0,
                                                                    0,
                                                                    nullptr,
                                                                    nullptr,
                                                                    &fakeParameters,
                                                                    nullptr,
                                                                    nullptr,
                                                                    nullptr);

    OwnVehicleInformation vehicleInfo;
    TrafficRuleInformation trafficRuleInfo;
    GeometryInformation geometricInfo;
    SurroundingObjects surroundingObjects;

    surroundingObjects.objectFront.exist = false;
    vehicleInfo.absoluteVelocity = 200.0/3.6;
    const auto sensorDriverSignal0 = std::make_shared<SensorDriverSignal const>(vehicleInfo,
                                                                                trafficRuleInfo,
                                                                                geometricInfo,
                                                                                surroundingObjects);
    std::shared_ptr<SignalInterface const> outputSignal;

    implementation.UpdateInput(0, sensorDriverSignal0, 0);
    implementation.Trigger(0);
    implementation.UpdateOutput(2, outputSignal, 0);

    auto brakingSignal = std::static_pointer_cast<AccelerationSignal const>(outputSignal);
    double acceleration = brakingSignal->acceleration;

    ASSERT_THAT(acceleration, Ge(-maxDeceleration));
    ASSERT_THAT(acceleration, Le(maxAcceleration));

    vehicleInfo.absoluteVelocity = 20.0/3.6;
    const auto sensorDriverSignal1 = std::make_shared<SensorDriverSignal const>(vehicleInfo,
                                                                                trafficRuleInfo,
                                                                                geometricInfo,
                                                                                surroundingObjects);

    implementation.UpdateInput(0, sensorDriverSignal1, 100);
    implementation.Trigger(100);
    implementation.UpdateOutput(2, outputSignal, 100);

    brakingSignal = std::static_pointer_cast<AccelerationSignal const>(outputSignal);
    acceleration = brakingSignal->acceleration;

    ASSERT_THAT(acceleration, Ge(-maxDeceleration));
    ASSERT_THAT(acceleration, Le(maxAcceleration));
}

TEST(AgentFollowingDriverModel, TriggerWithNoFrontAgentAndVelocityIsWish_HoldVWish)
{
    NiceMock<FakeParameter> fakeParameters;
    std::map<std::string, double> fakeParametersDouble;

    ON_CALL(fakeParameters, GetParametersDouble()).WillByDefault(ReturnRef(fakeParametersDouble));

    AlgorithmAgentFollowingDriverModelImplementation implementation("",
                                                                    false,
                                                                    0,
                                                                    0,
                                                                    0,
                                                                    0,
                                                                    nullptr,
                                                                    nullptr,
                                                                    &fakeParameters,
                                                                    nullptr,
                                                                    nullptr,
                                                                    nullptr);

    OwnVehicleInformation vehicleInfo;
    TrafficRuleInformation trafficRuleInfo;
    GeometryInformation geometricInfo;
    SurroundingObjects surroundingObjects;

    surroundingObjects.objectFront.exist = false;
    vehicleInfo.absoluteVelocity = 120.0/3.6;
    const auto sensorDriverSignal = std::make_shared<SensorDriverSignal const>(vehicleInfo,
                                                                               trafficRuleInfo,
                                                                               geometricInfo,
                                                                               surroundingObjects);
    std::shared_ptr<SignalInterface const> outputSignal;

    implementation.UpdateInput(0, sensorDriverSignal, 0);
    implementation.Trigger(0);
    implementation.UpdateOutput(2, outputSignal, 0);

    auto brakingSignal = std::static_pointer_cast<AccelerationSignal const>(outputSignal);
    double acceleration = brakingSignal->acceleration;
    ASSERT_THAT(acceleration, DoubleEq(0.0));
}

TEST(AgentFollowingDriverModel, TriggerWithNoFrontAgentAndVelocityHigherWish_AccUntilWish)
{
    NiceMock<FakeParameter> fakeParameters;
    std::map<std::string, double> fakeParametersDouble;

    ON_CALL(fakeParameters, GetParametersDouble()).WillByDefault(ReturnRef(fakeParametersDouble));

    AlgorithmAgentFollowingDriverModelImplementation implementation("",
                                                                    false,
                                                                    0,
                                                                    0,
                                                                    0,
                                                                    0,
                                                                    nullptr,
                                                                    nullptr,
                                                                    &fakeParameters,
                                                                    nullptr,
                                                                    nullptr,
                                                                    nullptr);

    OwnVehicleInformation vehicleInfo;
    TrafficRuleInformation trafficRuleInfo;
    GeometryInformation geometricInfo;
    SurroundingObjects surroundingObjects;

    surroundingObjects.objectFront.exist = false;
    vehicleInfo.absoluteVelocity = 150.0/3.6;
    const auto sensorDriverSignal0 = std::make_shared<SensorDriverSignal const>(vehicleInfo,
                                                                                trafficRuleInfo,
                                                                                geometricInfo,
                                                                                surroundingObjects);
    std::shared_ptr<SignalInterface const> outputSignal;

    implementation.UpdateInput(0, sensorDriverSignal0, 0);
    implementation.Trigger(0);
    implementation.UpdateOutput(2, outputSignal, 0);

    auto brakingSignal = std::static_pointer_cast<AccelerationSignal const>(outputSignal);
    double acceleration = brakingSignal->acceleration;
    ASSERT_THAT(acceleration, Lt(0.0));

    vehicleInfo.absoluteVelocity = 120.0/3.6;
    const auto sensorDriverSignal1 = std::make_shared<SensorDriverSignal const>(vehicleInfo,
                                                                                trafficRuleInfo,
                                                                                geometricInfo,
                                                                                surroundingObjects);

    implementation.UpdateInput(0, sensorDriverSignal1, 100);
    implementation.Trigger(100);
    implementation.UpdateOutput(2, outputSignal, 0);

    brakingSignal = std::static_pointer_cast<AccelerationSignal const>(outputSignal);
    acceleration = brakingSignal->acceleration;
    ASSERT_THAT(acceleration, DoubleEq(0.0));
}
