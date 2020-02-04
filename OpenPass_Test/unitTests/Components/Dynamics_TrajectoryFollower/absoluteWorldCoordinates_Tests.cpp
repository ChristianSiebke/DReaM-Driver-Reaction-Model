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
#include "dontCare.h"

#include "absoluteWorldCoordinateTrajectoryFollower.h"

#include "fakeAgent.h"
#include "fakeTrajectory.h"
#include "fakeParameter.h"
#include "trajectoryTester.h"

using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::_;
using ::testing::SaveArg;
using ::testing::NiceMock;
using ::testing::DontCare;

static void DynamicsSignalComparision(std::shared_ptr<DynamicsSignal const> signal,
                                      double x,
                                      double y,
                                      double yaw,
                                      double yawRate,
                                      double velocity,
                                      double acceleration,
                                      double distance)
{
    ASSERT_DOUBLE_EQ(signal->positionX, x);
    ASSERT_DOUBLE_EQ(signal->positionY, y);
    ASSERT_DOUBLE_EQ(signal->yaw, yaw);
    ASSERT_DOUBLE_EQ(signal->yawRate, yawRate);
    ASSERT_DOUBLE_EQ(signal->velocity, velocity);
    ASSERT_DOUBLE_EQ(signal->acceleration, acceleration);
    ASSERT_DOUBLE_EQ(signal->travelDistance, distance);
}

//WorldCoordinates WITHOUT external acceleration
TEST(AbsoluteWorldCoordinateTrajectoryFollower_WithoutExternalAcceleration_Unittests, DeactivationAfterEndOfTrajectory)
{
    Position fakePosition1(0, 0, 0, 0);
    Position fakePosition2(0, 2, 0.1, 0);

    WorldCoordinateTrajectory fakeCoordinates = {{0, fakePosition1},
                                                 {100, fakePosition2}};

    NiceMock<FakeTrajectory> fakeTrajectory;
    ON_CALL(fakeTrajectory, GetWorldCoordinates()).WillByDefault(Return(&fakeCoordinates));

    TrajectoryTester<AbsoluteWorldCoordinateTrajectoryFollower> trajectoryTester(DontCare<bool>(),
                                      true,
                                      &fakeTrajectory);
    std::shared_ptr<AbsoluteWorldCoordinateTrajectoryFollower> trajectoryFollower = trajectoryTester.trajectoryFollower;

    std::shared_ptr<SignalInterface const> resultSignalInterface;
    std::shared_ptr<DynamicsSignal const> result;

    trajectoryFollower->Trigger(0);
    trajectoryFollower->Trigger(100);
    trajectoryFollower->Trigger(200);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 200);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);

    ASSERT_EQ(result->componentState, ComponentState::Disabled);
}

TEST(AbsoluteWorldCoordinateTrajectoryFollower_WithoutExternalAcceleration_Unittests, LinearTrajectoryWithoutInterpolation)
{
    Position fakePosition1(0, 0, 0, 0);
    Position fakePosition2(3, 4, 0.1, 0);
    Position fakePosition3(9, 12, 0.4 ,0);
    WorldCoordinateTrajectory fakeCoordinates = {{0, fakePosition1},
                                                 {200, fakePosition2},
                                                 {400, fakePosition3}};

    TrajectoryTester<AbsoluteWorldCoordinateTrajectoryFollower> trajectoryTester(DontCare<bool>(),
                                      DontCare<bool>(),
                                      &fakeCoordinates,
                                      200);
    std::shared_ptr<AbsoluteWorldCoordinateTrajectoryFollower> trajectoryFollower = trajectoryTester.trajectoryFollower;

    std::shared_ptr<SignalInterface const> resultSignalInterface;
    std::shared_ptr<DynamicsSignal const> result;

    trajectoryFollower->Trigger(0);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 0);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    DynamicsSignalComparision(result, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

    trajectoryFollower->Trigger(200);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 200);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    DynamicsSignalComparision(result, 3.0, 4.0, 0.1, 0.5, 25.0, 125.0, 5.0);

    trajectoryFollower->Trigger(400);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 400);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    DynamicsSignalComparision(result, 9.0, 12.0, 0.4, 1.5, 50.0, 125.0, 10.0);
}

TEST(AbsoluteWorldCoordinateTrajectoryFollower_WithoutExternalAcceleration_Unittests, LinearTrajectoryWithInterpolation)
{
    Position fakePosition1(10, 10, -1, 0);
    Position fakePosition2(13, 6, -0.5, 0);
    Position fakePosition3(16, 9, 1, 0);
    Position fakePosition4(16, 13, 1, 0);
    WorldCoordinateTrajectory fakeCoordinates = {{0, fakePosition1},
                                                 {100, fakePosition2},
                                                 {400, fakePosition3},
                                                 {800, fakePosition4}};

    TrajectoryTester<AbsoluteWorldCoordinateTrajectoryFollower> trajectoryTester(DontCare<bool>(),
                                      DontCare<bool>(),
                                      &fakeCoordinates,
                                      200);
    std::shared_ptr<AbsoluteWorldCoordinateTrajectoryFollower> trajectoryFollower = trajectoryTester.trajectoryFollower;

    std::shared_ptr<SignalInterface const> resultSignalInterface;
    std::shared_ptr<DynamicsSignal const> result;

    double velocity {0};
    double acceleration {0};
    double distance {0.0};

    const double cycleTimeInSeconds = 0.2;

    trajectoryFollower->Trigger(0);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 0);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    DynamicsSignalComparision(result, 10, 10, -1, -5, velocity, acceleration, 0);

    trajectoryFollower->Trigger(200);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 100);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    distance =  (5 + std::sqrt(2));
    velocity = distance / cycleTimeInSeconds;
    acceleration = velocity / cycleTimeInSeconds;
    DynamicsSignalComparision(result,
                              14,
                              7,
                              0.0,
                              1.0 / cycleTimeInSeconds,
                              velocity,
                              acceleration,
                              distance);

    trajectoryFollower->Trigger(400);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 400);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);

    double previousVelocity = velocity;
    distance = std::sqrt(8);
    velocity = distance / cycleTimeInSeconds;
    acceleration = (velocity - previousVelocity) / cycleTimeInSeconds;

    DynamicsSignalComparision(result,
                              16.0,
                              9.0,
                              1.0,
                              1.0 / cycleTimeInSeconds,
                              velocity,
                              acceleration,
                              distance);

    trajectoryFollower->Trigger(600);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 600);

    previousVelocity = velocity;
    distance = 2.0;
    velocity = distance / cycleTimeInSeconds;
    acceleration = (velocity - previousVelocity) / cycleTimeInSeconds;

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    DynamicsSignalComparision(result,
                              16.0,
                              11.0,
                              1.0,
                              0.0,
                              velocity,
                              acceleration,
                              distance);
}


//WorldCoordinates WITH external acc
TEST(AbsoluteWorldCoordinateTrajectoryFollower_WithExternalAcceleration_Unittests, LinearTrajectoryWithInterpolation)
{
    Position fakePosition1(10.0, 10.0, 0, 0);
    Position fakePosition2(13.0, 14.0, 0.2, 0);
    Position fakePosition3(15.0, 14.0, 0.4 ,0);
    Position fakePosition4(15.0, 16.0, 0.6 ,0);
    Position fakePosition5(17.0, 16.0, 0.8 ,0);
    WorldCoordinateTrajectory fakeCoordinates = {{0, fakePosition1},
                                                 {200, fakePosition2},
                                                 {400, fakePosition3},
                                                 {600, fakePosition4},
                                                 {800, fakePosition5}};

    TrajectoryTester<AbsoluteWorldCoordinateTrajectoryFollower> trajectoryTester(DontCare<bool>(),
                                      DontCare<bool>(),
                                      &fakeCoordinates,
                                      200);
    std::shared_ptr<AbsoluteWorldCoordinateTrajectoryFollower> trajectoryFollower = trajectoryTester.trajectoryFollower;

    std::shared_ptr<SignalInterface const> resultSignalInterface;
    std::shared_ptr<DynamicsSignal const> result;
    auto inputSignal = std::make_shared<AccelerationSignal>(ComponentState::Acting, -50.0);

    trajectoryFollower->Trigger(0);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 0);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    DynamicsSignalComparision(result, 10.0, 10.0, 0.0, 0.0, 0.0, 0.0, 0.0);

    trajectoryFollower->Trigger(200);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 200);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    DynamicsSignalComparision(result, 13.0, 14.0, 0.2, 1.0, 25.0, 125.0, 5.0);

    trajectoryFollower->UpdateInput(1, inputSignal, 0);
    trajectoryFollower->Trigger(400);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 400);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    DynamicsSignalComparision(result, 15.0, 15.0, 0.5, 1.5, 15.0, -50.0, 3.0);

    trajectoryFollower->UpdateInput(1, inputSignal, 0);
    trajectoryFollower->Trigger(600);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 600);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    DynamicsSignalComparision(result, 15.0, 16.0, 0.6, 0.5, 5.0, -50.0, 1.0);

    inputSignal = std::make_shared<AccelerationSignal>(ComponentState::Acting, 0.0);
    trajectoryFollower->UpdateInput(1, inputSignal, 0);
    trajectoryFollower->Trigger(800);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 800);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    DynamicsSignalComparision(result, 16.0, 16.0, 0.7, 0.5, 5.0, 0.0, 1.0);
}

TEST(AbsoluteWorldCoordinateTrajectoryFollower_WithExternalAcceleration_Unittests, DeactivationForNegativeVelocity)
{
    Position fakePosition1(10.0, 10.0, 0, 0);
    Position fakePosition2(13.0, 14.0, 0.2, 0);
    Position fakePosition3(15.0, 14.0, 0.4 ,0);
    Position fakePosition4(15.0, 16.0, 0.6 ,0);
    Position fakePosition5(17.0, 16.0, 0.8 ,0);
    WorldCoordinateTrajectory fakeCoordinates = {{0, fakePosition1},
                                                 {200, fakePosition2},
                                                 {400, fakePosition3},
                                                {600, fakePosition4},
                                                {800, fakePosition5}};

    TrajectoryTester<AbsoluteWorldCoordinateTrajectoryFollower> trajectoryTester(DontCare<bool>(),
                                      true,
                                      &fakeCoordinates,
                                      200);
    std::shared_ptr<AbsoluteWorldCoordinateTrajectoryFollower> trajectoryFollower = trajectoryTester.trajectoryFollower;

    std::shared_ptr<SignalInterface const> resultSignalInterface;
    std::shared_ptr<DynamicsSignal const> result;
    auto inputSignal = std::make_shared<AccelerationSignal>(ComponentState::Acting, -50.0);

    trajectoryFollower->Trigger(0);
    trajectoryFollower->Trigger(200);

    trajectoryFollower->UpdateInput(1, inputSignal, 0);
    trajectoryFollower->Trigger(400);

    trajectoryFollower->UpdateInput(1, inputSignal, 0);
    trajectoryFollower->Trigger(600);

    trajectoryFollower->UpdateInput(1, inputSignal, 0);
    trajectoryFollower->Trigger(800);

    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 800);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);

    ASSERT_EQ(result->componentState, ComponentState::Disabled);
}

TEST(AbsoluteWorldCoordinateTrajectoryFollower_WithExternalAcceleration_Unittests, MultipleTimestepsWithinTwoCoordinates)
{
    Position fakePosition1(0.0, 0.0, 0, 0);
    Position fakePosition2(9.0, 0.0, 0.0, 0);
    WorldCoordinateTrajectory fakeCoordinates = {{0, fakePosition1},
                                                 {300, fakePosition2}};

    TrajectoryTester<AbsoluteWorldCoordinateTrajectoryFollower> trajectoryTester(DontCare<bool>(),
                                      DontCare<bool>(),
                                      &fakeCoordinates);
    std::shared_ptr<AbsoluteWorldCoordinateTrajectoryFollower> trajectoryFollower = trajectoryTester.trajectoryFollower;

    std::shared_ptr<SignalInterface const> resultSignalInterface;
    std::shared_ptr<DynamicsSignal const> result;
    auto inputSignal = std::make_shared<AccelerationSignal>(ComponentState::Acting, -150.0);

    trajectoryFollower->Trigger(0);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 0);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    DynamicsSignalComparision(result, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

    trajectoryFollower->Trigger(100);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 100);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    DynamicsSignalComparision(result, 3.0, 0.0, 0.0, 0.0, 30.0, 300.0, 3.0);

    trajectoryFollower->Trigger(200);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 200);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    DynamicsSignalComparision(result, 6.0, 0.0, 0, 0, 30.0, 0.0, 3.0);

    trajectoryFollower->UpdateInput(1, inputSignal, 300);
    trajectoryFollower->Trigger(300);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 300);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    DynamicsSignalComparision(result, 7.5, 0.0, 0.0, 0, 15.0, -150.0, 1.5);

    inputSignal = std::make_shared<AccelerationSignal>(ComponentState::Disabled, 0.0);
    trajectoryFollower->UpdateInput(1, inputSignal, 400);
    trajectoryFollower->Trigger(400);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 400);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    DynamicsSignalComparision(result, 9.0, 0.0, 0, 0, 15.0, 0.0, 1.5);
}

