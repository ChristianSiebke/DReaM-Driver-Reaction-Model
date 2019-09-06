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

#include "roadCoordinateTrajectoryFollower.h"

#include "fakeAgent.h"
#include "fakeWorld.h"
#include "fakeTrajectory.h"
#include "trajectoryTester.h"

using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::_;
using ::testing::SaveArg;
using ::testing::NiceMock;

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

//RoadCoordinates WTIHOUT external acc
TEST(AbsoluteRoadCoordinateTrajectoryFollower_WithoutExternalAcceleration_Unittests, DeactivationAfterEndOfTrajectory)
{

    RoadPosition fakeCoordinate1(1, 1, 0);
    RoadPosition fakeCoordinate2(3, 4, 0.1);

    RoadCoordinateTrajectory fakeCoordinates = {{200, fakeCoordinate1},
                                                                               {400, fakeCoordinate2}};

    NiceMock<FakeTrajectory> fakeTrajectory;
    ON_CALL(fakeTrajectory, GetRoadCoordinates()).WillByDefault(Return(&fakeCoordinates));
    ON_CALL(fakeTrajectory, GetTrajectoryType()).WillByDefault(Return(TrajectoryType::RoadCoordinatesAbsolute));

    Position fakeResult(0, 0, 0, 0);

    NiceMock<FakeWorld> fakeWorld;
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(fakeCoordinate1, _)).WillOnce(Return(fakeResult));
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(fakeCoordinate2, _)).WillOnce(Return(fakeResult));

    double fakeStartVelocity {};
    RoadPosition fakeStartPosition {};

    NiceMock<FakeAgent> fakeAgent;
    EXPECT_CALL(fakeAgent, GetVelocity()).WillOnce(Return(fakeStartVelocity));
    EXPECT_CALL(fakeAgent, GetRoadPosition()).WillOnce(Return(fakeStartPosition));

    TrajectoryTester<RoadCoordinateTrajectoryFollower> trajectoryTester(DontCare<bool>(),
                                                                        true,
                                                                        &fakeTrajectory,
                                                                        &fakeWorld,
                                                                        &fakeAgent,
                                                                        200);
    std::shared_ptr<RoadCoordinateTrajectoryFollower> trajectoryFollower = trajectoryTester.trajectoryFollower;

    std::shared_ptr<SignalInterface const> resultSignalInterface;
    std::shared_ptr<DynamicsSignal const> result;

    trajectoryFollower->Trigger(0);
    trajectoryFollower->Trigger(200);
    trajectoryFollower->Trigger(400);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 400);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);

    ASSERT_EQ(result->componentState, ComponentState::Disabled);
}

TEST(AbsoluteRoadCoordinateTrajectoryFollower_WithoutExternalAcceleration_Unittests, LinearTrajectoryWithoutInterpolation)
{


    RoadPosition fakeCoordinate1(1, 0, 0);
    RoadPosition fakeCoordinate2(4, 4, 0.1);
    RoadPosition fakeCoordinate3(10, 12, 0.4);

    RoadCoordinateTrajectory fakeCoordinates = {{200, fakeCoordinate1},
                                                                               {400, fakeCoordinate2},
                                                                               {600, fakeCoordinate3}};

    NiceMock<FakeTrajectory> fakeTrajectory;
    ON_CALL(fakeTrajectory, GetRoadCoordinates()).WillByDefault(Return(&fakeCoordinates));
    ON_CALL(fakeTrajectory, GetTrajectoryType()).WillByDefault(Return(TrajectoryType::RoadCoordinatesAbsolute));

    Position fakeResult1{1, 0, 0, 0};
    Position fakeResult2{4, 4, 0.1, 0};
    Position fakeResult3{10, 12, 0.4, 0};
    NiceMock<FakeWorld> fakeWorld;
    //ret Position -----------------------------------(RPos, string)
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(fakeCoordinate1, _)).WillOnce(Return(fakeResult1));
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(fakeCoordinate2, _)).WillOnce(Return(fakeResult2));
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(fakeCoordinate3, _)).WillOnce(Return(fakeResult3));

    double fakeStartVelocity {0.0};
    RoadPosition fakeStartPosition {0, 0, 0};

    NiceMock<FakeAgent> fakeAgent;
    EXPECT_CALL(fakeAgent, GetVelocity()).WillOnce(Return(fakeStartVelocity));
    EXPECT_CALL(fakeAgent, GetRoadPosition()).WillOnce(Return(fakeStartPosition));

    TrajectoryTester<RoadCoordinateTrajectoryFollower> trajectoryTester(DontCare<bool>(),
                                                                        DontCare<bool>(),
                                                                        &fakeTrajectory,
                                                                        &fakeWorld,
                                                                        &fakeAgent,
                                                                        200);
    std::shared_ptr<RoadCoordinateTrajectoryFollower> trajectoryFollower = trajectoryTester.trajectoryFollower;

    std::shared_ptr<SignalInterface const> resultSignalInterface;
    std::shared_ptr<DynamicsSignal const> result;

    trajectoryFollower->Trigger(0);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 0);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    DynamicsSignalComparision(result, 1, 0, 0, 0, 5.0, 25.0, 1.0);

    trajectoryFollower->Trigger(200);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 200);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    DynamicsSignalComparision(result, 4, 4, 0.1, 0.5, 25.0, 100.0, 5.0);

    trajectoryFollower->Trigger(400);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 400);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    DynamicsSignalComparision(result, 10, 12, 0.4, 1.5, 50.0, 125.0, 10.0);
}

TEST(AbsoluteRoadCoordinateTrajectoryFollower_WithoutExternalAcceleration_Unittests, LinearTrajectoryWithInterpolation)
{
    Position fakePosition1(10, 10, -1, 0);
    Position fakePosition3(14, 7, 0.0, 0);
    Position fakePosition4(16, 9, 1, 0);
    Position fakePosition5(16, 11, 1, 0);

    RoadPosition fakeRoadPosition1(0, 0, -1.0);
    RoadPosition fakeRoadPosition2(3, -4, -0.5);
    RoadPosition fakeRoadPosition3(4, -3, 0.0);
    RoadPosition fakeRoadPosition4(6, -1, 1.0);
    RoadPosition fakeRoadPosition5(6, 1, 1.0);
    RoadPosition fakeRoadPosition6(6, 3, 1.0);

    RoadCoordinateTrajectory fakeCoordinates = {{200, fakeRoadPosition1},
                                                 {300, fakeRoadPosition2},
                                                 {600, fakeRoadPosition4},
                                                 {1000, fakeRoadPosition6}};

    NiceMock<FakeTrajectory> fakeTrajectory;
    ON_CALL(fakeTrajectory, GetRoadCoordinates()).WillByDefault(Return(&fakeCoordinates));

    NiceMock<FakeWorld> fakeWorld;
    //ret Position -----------------------------------(RPos, string)
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(fakeRoadPosition1, _)).WillOnce(Return(fakePosition1));
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(fakeRoadPosition3, _)).WillOnce(Return(fakePosition3));
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(fakeRoadPosition4, _)).WillOnce(Return(fakePosition4));
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(fakeRoadPosition5, _)).WillOnce(Return(fakePosition5));

    TrajectoryTester<RoadCoordinateTrajectoryFollower> trajectoryTester(DontCare<bool>(),
                                                                        DontCare<bool>(),
                                                                        &fakeTrajectory,
                                                                        &fakeWorld,
                                                                        200);
    std::shared_ptr<RoadCoordinateTrajectoryFollower> trajectoryFollower = trajectoryTester.trajectoryFollower;

    std::shared_ptr<SignalInterface const> resultSignalInterface;
    std::shared_ptr<DynamicsSignal const> result;

    double velocity {0.0};
    double acceleration {0.0};
    double distance {0.0};

    const double cycleTimeInSeconds = 0.2;

    trajectoryFollower->Trigger(0);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 0);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    DynamicsSignalComparision(result, 10, 10, -1, -5, velocity, acceleration, distance);

    trajectoryFollower->Trigger(200);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 100);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    distance = 5 + std::sqrt(2);
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
    velocity =  distance / cycleTimeInSeconds;
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

//RoadCoordinates WITH external acc
TEST(AbsoluteRoadCoordinateTrajectoryFollower_WithExternalAcceleration_Unittests, LinearTrajectoryWithInterpolation)
{
    Position fakePosition1(10, 10, 0.0, 0);
    Position fakePosition2(13, 14, 0.2, 0);

    Position fakePosition4(15, 15, 0.5, 0);
    Position fakePosition5(15, 16, 0.6, 0);
    Position fakePosition6(16, 16, 0.7, 0);


    RoadPosition fakeRoadPosition1(0, 0, 0);
    RoadPosition fakeRoadPosition2(3, 4, 0.2);
    RoadPosition fakeRoadPosition3(5, 4, 0.4);
    RoadPosition fakeRoadPosition4(5, 5, 0.5);
    RoadPosition fakeRoadPosition5(5, 6, 0.6);
    RoadPosition fakeRoadPosition6(6, 6, 0.7);
    RoadPosition fakeRoadPosition7(7, 6, 0.8);


    RoadCoordinateTrajectory fakeCoordinates = {{200, fakeRoadPosition1},
                                                {400, fakeRoadPosition2},
                                                {600, fakeRoadPosition3},
                                                {800, fakeRoadPosition5},
                                                {1000, fakeRoadPosition7}};

    NiceMock<FakeTrajectory> fakeTrajectory;
    ON_CALL(fakeTrajectory, GetRoadCoordinates()).WillByDefault(Return(&fakeCoordinates));

    NiceMock<FakeWorld> fakeWorld;
    //ret Position -----------------------------------(RPos, string)
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(fakeRoadPosition1, _)).WillOnce(Return(fakePosition1));
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(fakeRoadPosition2, _)).WillOnce(Return(fakePosition2));
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(fakeRoadPosition4, _)).WillOnce(Return(fakePosition4));
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(fakeRoadPosition5, _)).WillOnce(Return(fakePosition5));
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(fakeRoadPosition6, _)).WillOnce(Return(fakePosition6));

    TrajectoryTester<RoadCoordinateTrajectoryFollower> trajectoryTester(DontCare<bool>(),
                                                                        DontCare<bool>(),
                                                                        &fakeTrajectory,
                                                                        &fakeWorld,
                                                                        200);
    std::shared_ptr<RoadCoordinateTrajectoryFollower> trajectoryFollower = trajectoryTester.trajectoryFollower;

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

TEST(AbsoluteRoadCoordinateTrajectoryFollower_WithExternalAcceleration_Unittests, DeactivationAfterNonPositiveVelocity)
{
    Position fakePosition1(10, 10, 0.0, 0);
    Position fakePosition2(13, 14, 0.2, 0);

    Position fakePosition4(15, 15, 0.5, 0);
    Position fakePosition5(15, 16, 0.6, 0);
    Position fakePosition6(16, 16, 0.7, 0);

    RoadPosition fakeRoadPosition1(0, 0, 0);
    RoadPosition fakeRoadPosition2(3, 4, 0.2);
    RoadPosition fakeRoadPosition3(5, 4, 0.4);
    RoadPosition fakeRoadPosition4(5, 5, 0.5);
    RoadPosition fakeRoadPosition5(5, 6, 0.6);
    RoadPosition fakeRoadPosition6(6, 6, 0.7);
    RoadPosition fakeRoadPosition7(7, 6, 0.8);


    RoadCoordinateTrajectory fakeCoordinates = {{200, fakeRoadPosition1},
                                                {400, fakeRoadPosition2},
                                                {600, fakeRoadPosition3},
                                                {800, fakeRoadPosition5},
                                                {1000, fakeRoadPosition7}};

    NiceMock<FakeTrajectory> fakeTrajectory;
    ON_CALL(fakeTrajectory, GetRoadCoordinates()).WillByDefault(Return(&fakeCoordinates));

    NiceMock<FakeWorld> fakeWorld;
    //ret Position -----------------------------------(RPos, string)
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(fakeRoadPosition1, _)).WillOnce(Return(fakePosition1));
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(fakeRoadPosition2, _)).WillOnce(Return(fakePosition2));
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(fakeRoadPosition4, _)).WillOnce(Return(fakePosition4));
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(fakeRoadPosition5, _)).WillOnce(Return(fakePosition5));

    TrajectoryTester<RoadCoordinateTrajectoryFollower> trajectoryTester(DontCare<bool>(),
                                                                        true,
                                                                        &fakeTrajectory,
                                                                        &fakeWorld,
                                                                        200);
    std::shared_ptr<RoadCoordinateTrajectoryFollower> trajectoryFollower = trajectoryTester.trajectoryFollower;

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

TEST(AbsoluteRoadCoordinateTrajectoryFollower_WithExternalAcceleration_Unittests, MultipleTimestepsWithinTwoCoordinates)
{

    Position fakePosition1(1.0, 1.0, 0, 0);
    Position fakePosition2(4.0, 1.0, 0.0, 0);
    Position fakePosition3(7.0, 1.0, 0.0, 0);
    Position fakePosition4(8.5, 1.0, 0.0, 0);
    Position fakePosition5(10.0, 1.0, 0.0, 0);

    RoadPosition fakeRoadPosition1(0, 0, 0);
    RoadPosition fakeRoadPosition2(3, 0, 0.0);
    RoadPosition fakeRoadPosition3(6, 0, 0.0);
    RoadPosition fakeRoadPosition4(7.5, 0, 0.0);
    RoadPosition fakeRoadPosition5(9, 0, 0.0);

    RoadCoordinateTrajectory fakeCoordinates = {{100, fakeRoadPosition1},
                                                {400, fakeRoadPosition5}};

    NiceMock<FakeTrajectory> fakeTrajectory;
    ON_CALL(fakeTrajectory, GetRoadCoordinates()).WillByDefault(Return(&fakeCoordinates));

    NiceMock<FakeWorld> fakeWorld;
    //ret Position -----------------------------------(RPos, string)
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(fakeRoadPosition1, _)).WillOnce(Return(fakePosition1));
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(fakeRoadPosition2, _)).WillOnce(Return(fakePosition2));
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(fakeRoadPosition3, _)).WillOnce(Return(fakePosition3));
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(fakeRoadPosition4, _)).WillOnce(Return(fakePosition4));
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(fakeRoadPosition5, _)).WillOnce(Return(fakePosition5));

    TrajectoryTester<RoadCoordinateTrajectoryFollower> trajectoryTester(DontCare<bool>(),
                                                                        DontCare<bool>(),
                                                                        &fakeTrajectory,
                                                                        &fakeWorld,
                                                                        100);
    std::shared_ptr<RoadCoordinateTrajectoryFollower> trajectoryFollower = trajectoryTester.trajectoryFollower;

    std::shared_ptr<SignalInterface const> resultSignalInterface;
    std::shared_ptr<DynamicsSignal const> result;
    auto inputSignal = std::make_shared<AccelerationSignal>(ComponentState::Acting, -150.0);

    trajectoryFollower->Trigger(0);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 0);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    DynamicsSignalComparision(result, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0);

    trajectoryFollower->Trigger(100);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 100);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    DynamicsSignalComparision(result, 4.0, 1.0, 0.0, 0.0, 30.0, 300.0, 3.0);

    trajectoryFollower->Trigger(200);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 200);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    DynamicsSignalComparision(result, 7.0, 1.0, 0, 0, 30.0, 0.0, 3.0);

    trajectoryFollower->UpdateInput(1, inputSignal, 300);
    trajectoryFollower->Trigger(300);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 300);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    DynamicsSignalComparision(result, 8.5, 1.0, 0.0, 0, 15.0, -150.0, 1.5);

    inputSignal = std::make_shared<AccelerationSignal>(ComponentState::Disabled, 0.0);
    trajectoryFollower->UpdateInput(1, inputSignal, 400);
    trajectoryFollower->Trigger(400);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 400);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    DynamicsSignalComparision(result, 10.0, 1.0, 0, 0, 15.0, 0.0, 1.5);
}


// Relative Road tests
TEST(RelativeRoadCoordinateTrajectoryFollower_WithoutExternalAcceleration_Unittests, LinearTrajectoryWithoutInterpolation)
{
    Position fakePosition1(4, 5, 0.1, 0);
    Position fakePosition2(10, 13, 0.4, 0);


    RoadPosition fakeRelativeRoadPosition1(0, 0, 0);
    RoadPosition fakeRelativeRoadPosition2(3, 4, 0.1);
    RoadPosition fakeRelativeRoadPosition3(9, 12, 0.4);

    RoadPosition fakeAbsolutRoadPosition1(103.0, 104.0, 0.1);
    RoadPosition fakeAbsolutRoadPosition2(109.0, 112.0, 0.4);

    RoadCoordinateTrajectory fakeCoordinates = {{0, fakeRelativeRoadPosition1},
                                                {200, fakeRelativeRoadPosition2},
                                                {400, fakeRelativeRoadPosition3}};

    NiceMock<FakeTrajectory> fakeTrajectory;
    ON_CALL(fakeTrajectory, GetRoadCoordinates()).WillByDefault(Return(&fakeCoordinates));
    ON_CALL(fakeTrajectory, GetTrajectoryType()).WillByDefault(Return(TrajectoryType::RoadCoordinatesRelative));

    double fakeStartVelocity {0.0};
    RoadPosition fakeAgentStartPosition(100.0, 100.0, 0);

    NiceMock<FakeAgent> fakeAgent;
    EXPECT_CALL(fakeAgent, GetVelocity()).WillOnce((Return(fakeStartVelocity)));
    EXPECT_CALL(fakeAgent, GetRoadPosition()).WillOnce(Return(fakeAgentStartPosition))
                                                                           .WillOnce(Return(fakeAgentStartPosition));

    NiceMock<FakeWorld> fakeWorld;
    //ret Position -----------------------------------(RPos, string)
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(fakeAbsolutRoadPosition1, _)).WillOnce(Return(fakePosition1));
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(fakeAbsolutRoadPosition2, _)).WillOnce(Return(fakePosition2));

    TrajectoryTester<RoadCoordinateTrajectoryFollower> trajectoryTester(DontCare<bool>(),
                                                                        DontCare<bool>(),
                                                                        &fakeTrajectory,
                                                                        &fakeWorld,
                                                                        &fakeAgent,
                                                                        200);
    std::shared_ptr<RoadCoordinateTrajectoryFollower> trajectoryFollower = trajectoryTester.trajectoryFollower;

    std::shared_ptr<SignalInterface const> resultSignalInterface;
    std::shared_ptr<DynamicsSignal const> result;

    trajectoryFollower->Trigger(0);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 0);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    DynamicsSignalComparision(result, 4, 5, 0.1, 0.5, 25.0, 125.0, 5.0);

    trajectoryFollower->Trigger(200);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 200);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    DynamicsSignalComparision(result, 10, 13, 0.4, 1.5, 50.0, 125.0, 10.0);
}

TEST(RelativeRoadCoordinateTrajectoryFollower_WithExternalAcceleration_Unittests, LinearTrajectoryWithInterpolation)
{
    Position fakePosition1(13, 14, 0.2, 0);

    Position fakePosition4(15, 15, 0.5, 0);
    Position fakePosition5(15, 16, 0.6, 0);
    Position fakePosition6(16, 16, 0.7, 0);

    RoadPosition fakeAgentStartPosition(100.0, 100.0, 0);

    RoadPosition fakeRelativeRoadPosition1(0, 0, 0);
    RoadPosition fakeRelativeRoadPosition2(3, 4, 0.2);
    RoadPosition fakeRelativeRoadPosition3(5, 4, 0.4);
    RoadPosition fakeRelativeRoadPosition5(5, 6, 0.6);
    RoadPosition fakeRelativeRoadPosition7(7, 6, 0.8);

    RoadPosition fakeAbsolutRoadPosition1(103, 104, 0.2);
    RoadPosition fakeAbsolutRoadPosition4(105, 105, 0.5);
    RoadPosition fakeAbsolutRoadPosition5(105, 106, 0.6);
    RoadPosition fakeAbsolutRoadPosition6(106, 106, 0.7);

    RoadCoordinateTrajectory fakeCoordinates = {{0, fakeRelativeRoadPosition1},
                                                {200, fakeRelativeRoadPosition2},
                                                {400, fakeRelativeRoadPosition3},
                                                {600, fakeRelativeRoadPosition5},
                                                {800, fakeRelativeRoadPosition7}};

    NiceMock<FakeTrajectory> fakeTrajectory;
    ON_CALL(fakeTrajectory, GetRoadCoordinates()).WillByDefault(Return(&fakeCoordinates));
    ON_CALL(fakeTrajectory, GetTrajectoryType()).WillByDefault(Return(TrajectoryType::RoadCoordinatesRelative));

    NiceMock<FakeWorld> fakeWorld;
    //ret Position -----------------------------------(RPos, string)
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(fakeAbsolutRoadPosition1, _)).WillOnce(Return(fakePosition1));
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(fakeAbsolutRoadPosition4, _)).WillOnce(Return(fakePosition4));
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(fakeAbsolutRoadPosition5, _)).WillOnce(Return(fakePosition5));
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(fakeAbsolutRoadPosition6, _)).WillOnce(Return(fakePosition6));


    NiceMock<FakeAgent> fakeAgent;
    EXPECT_CALL(fakeAgent, GetRoadPosition()).WillOnce(Return(fakeAgentStartPosition))
                                                                           .WillOnce(Return(fakeAgentStartPosition));

    TrajectoryTester<RoadCoordinateTrajectoryFollower> trajectoryTester(DontCare<bool>(),
                                                                        DontCare<bool>(),
                                                                        &fakeTrajectory,
                                                                        &fakeWorld,
                                                                        &fakeAgent,
                                                                        200);
    std::shared_ptr<RoadCoordinateTrajectoryFollower> trajectoryFollower = trajectoryTester.trajectoryFollower;

    std::shared_ptr<SignalInterface const> resultSignalInterface;
    std::shared_ptr<DynamicsSignal const> result;
    auto inputSignal = std::make_shared<AccelerationSignal>(ComponentState::Acting, -50.0);

    trajectoryFollower->Trigger(0);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 0);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    DynamicsSignalComparision(result, 13.0, 14.0, 0.2, 1.0, 25.0, 125.0, 5.0);

    trajectoryFollower->UpdateInput(1, inputSignal, 0);
    trajectoryFollower->Trigger(200);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 200);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    DynamicsSignalComparision(result, 15.0, 15.0, 0.5, 1.5, 15.0, -50.0, 3.0);

    trajectoryFollower->UpdateInput(1, inputSignal, 0);
    trajectoryFollower->Trigger(400);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 400);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    DynamicsSignalComparision(result, 15.0, 16.0, 0.6, 0.5, 5.0, -50.0, 1.0);

    inputSignal = std::make_shared<AccelerationSignal>(ComponentState::Acting, 0.0);
    trajectoryFollower->UpdateInput(1, inputSignal, 0);
    trajectoryFollower->Trigger(600);
    trajectoryFollower->UpdateOutput(0, resultSignalInterface, 600);

    result = std::dynamic_pointer_cast<DynamicsSignal const>(resultSignalInterface);
    DynamicsSignalComparision(result, 16.0, 16.0, 0.7, 0.5, 5.0, 0.0, 1.0);
}
