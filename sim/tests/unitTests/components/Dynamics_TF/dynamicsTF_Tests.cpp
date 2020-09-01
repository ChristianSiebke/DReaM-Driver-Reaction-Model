/*******************************************************************************
* Copyright (c) 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/
#include "gtest/gtest.h"
#include "dontCare.h"

#include "tfImplementation.h"

#include "fakeAgent.h"
#include "fakeParameter.h"
#include "trajectoryTester.h"
#include "common/trajectorySignal.h"

using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::_;
using ::testing::SaveArg;
using ::testing::NiceMock;
using ::testing::DontCare;
using ::testing::DoubleNear;

static void AssertDynamicsSignalEquality(std::shared_ptr<DynamicsSignal const> signal,
                                         double x,
                                         double y,
                                         double yaw,
                                         double yawRate,
                                         double velocity,
                                         double acceleration,
                                         double distance)
{
    ASSERT_THAT(signal->positionX, DoubleNear(x, 1e-3));
    ASSERT_THAT(signal->positionY, DoubleNear(y, 1e-3));
    ASSERT_THAT(signal->yaw, DoubleNear(yaw, 1e-3));
    ASSERT_THAT(signal->yawRate, DoubleNear(yawRate, 1e-3));
    ASSERT_THAT(signal->velocity, DoubleNear(velocity, 1e-3));
    ASSERT_THAT(signal->acceleration, DoubleNear(acceleration, 1e-3));
    ASSERT_THAT(signal->travelDistance, DoubleNear(distance, 1e-3));
}

TEST(TrajectoryFollowerImplementation_WithoutExternalAcceleration_Unittests, DeactivationAfterEndOfTrajectory)
{
    TrajectoryPoint fakePosition1{0, 0, 0, 0};
    TrajectoryPoint fakePosition2{0.1, 0, 2, 0.1};
    TrajectoryPoint fakePosition3{0.2, 0, 4, 0.1};

    Trajectory fakeCoordinates = {{fakePosition1, fakePosition2, fakePosition3}, ""};

    TrajectoryTester trajectoryTester(DontCare<bool>(),
                                      true);
    std::shared_ptr<TrajectoryFollowerImplementation> trajectoryFollower = trajectoryTester.trajectoryFollower;

    std::shared_ptr<SignalInterface const> resultSignalInterface;
    std::shared_ptr<DynamicsSignal const> result;

    const auto trajectorySignal = std::make_shared<TrajectorySignal>(ComponentState::Acting, fakeCoordinates);
    trajectoryFollower->UpdateInput(2, trajectorySignal, 0);
    trajectoryFollower->Trigger(0);
    trajectoryFollower->Trigger(100);
    trajectoryFollower->Trigger(200);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 200);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);

    ASSERT_EQ(result->componentState, ComponentState::Disabled);
}

TEST(TrajectoryFollowerImplementation_WithoutExternalAcceleration_Unittests, LinearTrajectoryWithoutInterpolation)
{
    TrajectoryPoint fakePosition1{0, 0, 0, 0};
    TrajectoryPoint fakePosition2{0.2, 3, 4, 0.1};
    TrajectoryPoint fakePosition3{0.4, 9, 12, 0.4};
    Trajectory fakeCoordinates = {{fakePosition1, fakePosition2, fakePosition3}, ""};

    TrajectoryTester trajectoryTester(DontCare<bool>(),
                                      DontCare<bool>(),
                                      200);
    std::shared_ptr<TrajectoryFollowerImplementation> trajectoryFollower = trajectoryTester.trajectoryFollower;

    std::shared_ptr<SignalInterface const> resultSignalInterface;
    std::shared_ptr<DynamicsSignal const> result;

    const auto trajectorySignal = std::make_shared<TrajectorySignal>(ComponentState::Acting, fakeCoordinates);
    trajectoryFollower->UpdateInput(2, trajectorySignal, 0);
    trajectoryFollower->Trigger(0);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 0);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    AssertDynamicsSignalEquality(result, 3.0, 4.0, 0.1, 0.5, 25.0, 125.0, 5.0);

    trajectoryFollower->Trigger(200);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 200);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    AssertDynamicsSignalEquality(result, 9.0, 12.0, 0.4, 1.5, 50.0, 125.0, 10.0);
}

TEST(TrajectoryFollowerImplementation_WithoutExternalAcceleration_Unittests, LinearTrajectoryWithInterpolation)
{
    TrajectoryPoint fakePosition1{0, 10, 10, -1};
    TrajectoryPoint fakePosition2{0.1, 13, 6, -0.5};
    TrajectoryPoint fakePosition3{0.4, 16, 9, 1};
    TrajectoryPoint fakePosition4{0.8, 16, 13, 1};
    Trajectory fakeCoordinates = {{fakePosition1, fakePosition2, fakePosition3, fakePosition4}, ""};

    FakeAgent fakeAgent;
    ON_CALL(fakeAgent, GetPositionX()).WillByDefault(Return(10.0));
    ON_CALL(fakeAgent, GetPositionY()).WillByDefault(Return(10.0));
    ON_CALL(fakeAgent, GetYaw()).WillByDefault(Return(-1.0));

    TrajectoryTester trajectoryTester(DontCare<bool>(),
                                      DontCare<bool>(),
                                      nullptr,
                                      &fakeAgent,
                                      200);
    std::shared_ptr<TrajectoryFollowerImplementation> trajectoryFollower = trajectoryTester.trajectoryFollower;

    std::shared_ptr<SignalInterface const> resultSignalInterface;
    std::shared_ptr<DynamicsSignal const> result;

    double velocity {0};
    double acceleration {0};
    double distance {0.0};

    const double cycleTimeInSeconds = 0.2;

    const auto trajectorySignal = std::make_shared<TrajectorySignal>(ComponentState::Acting, fakeCoordinates);
    trajectoryFollower->UpdateInput(2, trajectorySignal, 0);
    trajectoryFollower->Trigger(0);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 0);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    distance =  (5 + std::sqrt(2));
    velocity = distance / cycleTimeInSeconds;
    acceleration = velocity / cycleTimeInSeconds;
    AssertDynamicsSignalEquality(result,
                              14,
                              7,
                              0.0,
                              1.0 / cycleTimeInSeconds,
                              velocity,
                              acceleration,
                              distance);

    trajectoryFollower->Trigger(200);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 100);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    double previousVelocity = velocity;
    distance = std::sqrt(8);
    velocity = distance / cycleTimeInSeconds;
    acceleration = (velocity - previousVelocity) / cycleTimeInSeconds;

    AssertDynamicsSignalEquality(result,
                              16.0,
                              9.0,
                              1.0,
                              1.0 / cycleTimeInSeconds,
                              velocity,
                              acceleration,
                              distance);

    trajectoryFollower->Trigger(400);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 400);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);

    previousVelocity = velocity;
    distance = 2.0;
    velocity = distance / cycleTimeInSeconds;
    acceleration = (velocity - previousVelocity) / cycleTimeInSeconds;

    AssertDynamicsSignalEquality(result,
                              16.0,
                              11.0,
                              1.0,
                              0.0,
                              velocity,
                              acceleration,
                              distance);
}


TEST(TrajectoryFollowerImplementation_WithExternalAcceleration_Unittests, LinearTrajectoryWithInterpolation)
{
    TrajectoryPoint fakePosition1{0, 10.0, 10.0, 0};
    TrajectoryPoint fakePosition2{0.2, 13.0, 14.0, 0.2};
    TrajectoryPoint fakePosition3{0.4, 15.0, 14.0, 0.4};
    TrajectoryPoint fakePosition4{0.6, 15.0, 16.0, 0.6};
    TrajectoryPoint fakePosition5{0.8, 17.0, 16.0, 0.8};
    Trajectory fakeCoordinates = {{fakePosition1, fakePosition2, fakePosition3, fakePosition4, fakePosition5}, ""};

    FakeAgent fakeAgent;
    ON_CALL(fakeAgent, GetPositionX()).WillByDefault(Return(10.0));
    ON_CALL(fakeAgent, GetPositionY()).WillByDefault(Return(10.0));
    ON_CALL(fakeAgent, GetYaw()).WillByDefault(Return(0.0));

    TrajectoryTester trajectoryTester(DontCare<bool>(),
                                      DontCare<bool>(),
                                      nullptr,
                                      &fakeAgent,
                                      200);
    std::shared_ptr<TrajectoryFollowerImplementation> trajectoryFollower = trajectoryTester.trajectoryFollower;

    std::shared_ptr<SignalInterface const> resultSignalInterface;
    std::shared_ptr<DynamicsSignal const> result;
    auto inputSignal = std::make_shared<AccelerationSignal>(ComponentState::Acting, -50.0);

    const auto trajectorySignal = std::make_shared<TrajectorySignal>(ComponentState::Acting, fakeCoordinates);
    trajectoryFollower->UpdateInput(2, trajectorySignal, 0);
    trajectoryFollower->Trigger(0);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 0);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    AssertDynamicsSignalEquality(result, 13.0, 14.0, 0.2, 1.0, 25.0, 125.0, 5.0);

    trajectoryFollower->UpdateInput(1, inputSignal, 0);
    trajectoryFollower->Trigger(200);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 200);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    AssertDynamicsSignalEquality(result, 15.0, 15.0, 0.5, 1.5, 15.0, -50.0, 3.0);

    trajectoryFollower->UpdateInput(1, inputSignal, 0);
    trajectoryFollower->Trigger(400);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 400);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    AssertDynamicsSignalEquality(result, 15.0, 16.0, 0.6, 0.5, 5.0, -50.0, 1.0);

    inputSignal = std::make_shared<AccelerationSignal>(ComponentState::Acting, 0.0);
    trajectoryFollower->UpdateInput(1, inputSignal, 0);
    trajectoryFollower->Trigger(600);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 600);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    AssertDynamicsSignalEquality(result, 16.0, 16.0, 0.7, 0.5, 5.0, 0.0, 1.0);
}

TEST(TrajectoryFollowerImplementation_WithExternalAcceleration_Unittests, DeactivationForNegativeVelocity)
{
    TrajectoryPoint fakePosition1{0, 10.0, 10.0, 0};
    TrajectoryPoint fakePosition2{0.2, 13.0, 14.0, 0.2};
    TrajectoryPoint fakePosition3{0.4, 15.0, 14.0, 0.4};
    TrajectoryPoint fakePosition4{0.6, 15.0, 16.0, 0.6};
    TrajectoryPoint fakePosition5{0.8, 17.0, 16.0, 0.8};
    Trajectory fakeCoordinates = {{fakePosition1, fakePosition2, fakePosition3, fakePosition4, fakePosition5}, ""};

    FakeAgent fakeAgent;
    ON_CALL(fakeAgent, GetPositionX()).WillByDefault(Return(10.0));
    ON_CALL(fakeAgent, GetPositionY()).WillByDefault(Return(10.0));
    ON_CALL(fakeAgent, GetYaw()).WillByDefault(Return(0.0));

    TrajectoryTester trajectoryTester(DontCare<bool>(),
                                      true,
                                      nullptr,
                                      &fakeAgent,
                                      200);
    std::shared_ptr<TrajectoryFollowerImplementation> trajectoryFollower = trajectoryTester.trajectoryFollower;

    std::shared_ptr<SignalInterface const> resultSignalInterface;
    std::shared_ptr<DynamicsSignal const> result;
    auto inputSignal = std::make_shared<AccelerationSignal>(ComponentState::Acting, -50.0);

    const auto trajectorySignal = std::make_shared<TrajectorySignal>(ComponentState::Acting, fakeCoordinates);
    trajectoryFollower->UpdateInput(2, trajectorySignal, 0);
    trajectoryFollower->Trigger(0);

    trajectoryFollower->UpdateInput(1, inputSignal, 0);
    trajectoryFollower->Trigger(200);

    trajectoryFollower->UpdateInput(1, inputSignal, 0);
    trajectoryFollower->Trigger(400);

    trajectoryFollower->UpdateInput(1, inputSignal, 0);
    trajectoryFollower->Trigger(600);

    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 600);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);

    ASSERT_EQ(result->componentState, ComponentState::Disabled);
}

TEST(TrajectoryFollowerImplementation_WithExternalAcceleration_Unittests, MultipleTimestepsWithinTwoCoordinates)
{
    TrajectoryPoint fakePosition1{0.0, 0.0, 0, 0};
    TrajectoryPoint fakePosition2{0.3, 9.0, 0.0, 0.0};
    Trajectory fakeCoordinates = {{fakePosition1, fakePosition2}, ""};

    TrajectoryTester trajectoryTester(DontCare<bool>(),
                                      DontCare<bool>());
    std::shared_ptr<TrajectoryFollowerImplementation> trajectoryFollower = trajectoryTester.trajectoryFollower;

    std::shared_ptr<SignalInterface const> resultSignalInterface;
    std::shared_ptr<DynamicsSignal const> result;
    auto inputSignal = std::make_shared<AccelerationSignal>(ComponentState::Acting, -150.0);

    const auto trajectorySignal = std::make_shared<TrajectorySignal>(ComponentState::Acting, fakeCoordinates);
    trajectoryFollower->UpdateInput(2, trajectorySignal, 0);
    trajectoryFollower->Trigger(0);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 0);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    AssertDynamicsSignalEquality(result, 3.0, 0.0, 0.0, 0.0, 30.0, 300.0, 3.0);

    trajectoryFollower->Trigger(100);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 100);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    AssertDynamicsSignalEquality(result, 6.0, 0.0, 0, 0, 30.0, 0.0, 3.0);

    trajectoryFollower->UpdateInput(1, inputSignal, 200);
    trajectoryFollower->Trigger(200);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 200);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    AssertDynamicsSignalEquality(result, 7.5, 0.0, 0.0, 0, 15.0, -150.0, 1.5);

    inputSignal = std::make_shared<AccelerationSignal>(ComponentState::Disabled, 0.0);
    trajectoryFollower->UpdateInput(1, inputSignal, 300);
    trajectoryFollower->Trigger(300);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 300);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    AssertDynamicsSignalEquality(result, 9.0, 0.0, 0, 0, 15.0, 0.0, 1.5);
}

