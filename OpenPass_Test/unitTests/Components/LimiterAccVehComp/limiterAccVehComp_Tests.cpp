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

#include "dontCare.h"
#include "fakeAgent.h"
#include "fakeWorld.h"

#include "limiterAccVehCompImplementation.h"

using ::testing::Return;
using ::testing::DontCare;
using ::testing::NiceMock;

struct LimiterTestParameter {
    LimiterTestParameter(const double &maximumLimit,
                         const double &minimumLimit,
                         const double &currentVelocity,
                         const double &airDragCoefficient,
                         const double &axleRatio,
                         const double &frontSurface,
                         const double &maximumEngineTorque,
                         const double &maximumEngineSpeed,
                         const double &minimumEngineSpeed,
                         const double &staticWheelRadius,
                         const double &weight,
                         const std::vector<double> &gearRatios):
        maximumLimit(maximumLimit),
        minimumLimit(minimumLimit),
        currentVelocity(currentVelocity),
        airDragCoefficient(airDragCoefficient),
        axleRatio(axleRatio),
        frontSurface(frontSurface),
        maximumEngineTorque(maximumEngineTorque),
        maximumEngineSpeed(maximumEngineSpeed),
        minimumEngineSpeed(minimumEngineSpeed),
        staticWheelRadius(staticWheelRadius),
        weight(weight),
        gearRatios(gearRatios)
    {}

    const double maximumLimit;
    const double minimumLimit;
    const double currentVelocity; // in kmh
    const double airDragCoefficient;
    const double axleRatio;
    const double frontSurface;
    const double maximumEngineTorque;
    const double maximumEngineSpeed;
    const double minimumEngineSpeed;
    const double staticWheelRadius;
    const double weight;
    const std::vector<double> gearRatios;

};

class MaximumLimit :  public ::testing::Test,
                      public ::testing::WithParamInterface<LimiterTestParameter>
{
};

TEST_P(MaximumLimit,
       CalculateCorrectMaximumValue)
{
    const LimiterTestParameter testInput = GetParam();

    NiceMock<FakeAgent> fakeAgent;
    ON_CALL(fakeAgent, GetVelocity()).WillByDefault(Return(testInput.currentVelocity / 3.6));

    NiceMock<FakeWorld> fakeWorld;
    ON_CALL(fakeWorld, GetFriction()).WillByDefault(Return(1.0));

    LimiterAccelerationVehicleComponentsImplementation limiter ("",
                                                                false,
                                                                DontCare<int>(),
                                                                DontCare<int>(),
                                                                DontCare<int>(),
                                                                DontCare<int>(),
                                                                nullptr,
                                                                &fakeWorld,
                                                                nullptr,
                                                                nullptr,
                                                                nullptr,
                                                                &fakeAgent);

    const auto accelerationInputSignal = std::make_shared<AccelerationSignal const>(ComponentState::Acting, INFINITY);

    VehicleModelParameters fakeVehicleParameters;
    fakeVehicleParameters.weight = testInput.weight;
    fakeVehicleParameters.frontSurface = testInput.frontSurface;
    fakeVehicleParameters.airDragCoefficient = testInput.airDragCoefficient;
    fakeVehicleParameters.maximumEngineTorque = testInput.maximumEngineTorque;
    fakeVehicleParameters.maximumEngineSpeed = testInput.maximumEngineSpeed;
    fakeVehicleParameters.minimumEngineSpeed = testInput.minimumEngineSpeed;
    fakeVehicleParameters.gearRatios = testInput.gearRatios;
    fakeVehicleParameters.axleRatio = testInput.axleRatio;
    fakeVehicleParameters.staticWheelRadius = testInput.staticWheelRadius;

    const auto vehicleParameterInputSignal = std::make_shared<ParametersVehicleSignal const>(fakeVehicleParameters);

    limiter.UpdateInput(0, accelerationInputSignal, DontCare<int>());
    limiter.UpdateInput(100, vehicleParameterInputSignal, DontCare<int>());

    limiter.Trigger(DontCare<double>());

    std::shared_ptr<SignalInterface const> signalInterface;
    limiter.UpdateOutput(0, signalInterface, DontCare<double>());

    double resultAcceleration = std::dynamic_pointer_cast<AccelerationSignal const>(signalInterface)->acceleration;

    // ASSERT
    ASSERT_NEAR(resultAcceleration, testInput.maximumLimit, 0.001);
}

INSTANTIATE_TEST_CASE_P(Default, MaximumLimit,

    // maximumLimit, minimumLimit, currentVelocity, airDragCoefficient, axleRatio, frontSurface,
    // maximumEngineTorque, maximumEngineSpeed, minimumEngineSpeed, staticWheelRadius, weight, gearRatios

    testing::Values(
        LimiterTestParameter{7.6668,  0,  30.0, 0.28, 2.813, 2.2, 270, 6000, 900, 0.318, 1525.0, { 0.0, 5.0, 3.2, 2.143, 1.72, 1.314, 1.0, 0.822, 0.64 }},
        LimiterTestParameter{6.5065,  0,  50.0, 0.28, 2.813, 2.2, 270, 6000, 900, 0.318, 1525.0, { 0.0, 5.0, 3.2, 2.143, 1.72, 1.314, 1.0, 0.822, 0.64 }},
        LimiterTestParameter{4.5591,  0,  70.0, 0.28, 2.813, 2.2, 270, 6000, 900, 0.318, 1525.0, { 0.0, 5.0, 3.2, 2.143, 1.72, 1.314, 1.0, 0.822, 0.64 }},
        LimiterTestParameter{3.0062,  0, 100.0, 0.28, 2.813, 2.2, 270, 6000, 900, 0.318, 1525.0, { 0.0, 5.0, 3.2, 2.143, 1.72, 1.314, 1.0, 0.822, 0.64 }},
        LimiterTestParameter{0.67098, 0, 200.0, 0.28, 2.813, 2.2, 270, 6000, 900, 0.318, 1525.0, { 0.0, 5.0, 3.2, 2.143, 1.72, 1.314, 1.0, 0.822, 0.64 }}
    )
);

