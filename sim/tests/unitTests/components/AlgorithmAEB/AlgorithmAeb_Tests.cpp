/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include <cmath>

#include "AlgorithmAebOSIUnitTests.h"
#include "autonomousEmergencyBraking.h"
#include "fakeAgent.h"
#include "fakeParameter.h"
#include "fakeWorldObject.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnRef;

TEST_F(AlgorithmAutonomousEmergencyBraking_UnitTest, ActualTTCLessThanBrakeTTCWithStationaryObject_ShouldActivateAEB)
{
    SetEgoValues(20.0, 0.0, 0.0);

    osi3::SensorData sensorData;
    auto stationaryObject = sensorData.add_stationary_object();
    stationaryObject->mutable_header()->add_sensor_id()->set_value(0);
    stationaryObject->mutable_base()->mutable_position()->set_x(8.0);
    stationaryObject->mutable_base()->mutable_position()->set_y(0.0);
    stationaryObject->mutable_base()->mutable_orientation()->set_yaw(0.0);
    stationaryObject->mutable_base()->mutable_dimension()->set_length(2.0);
    stationaryObject->mutable_base()->mutable_dimension()->set_width(1.0);

    implementation->UpdateInput(0, std::make_shared<SensorDataSignal>(sensorData), 0);

    implementation->Trigger(0);
    EXPECT_LT(implementation->GetAcceleration(), 0.0);
}

TEST_F(AlgorithmAutonomousEmergencyBraking_UnitTest, ActualTTCLessThanBrakeTTCWithMovingObject_ShouldActivateAEB)
{
    SetEgoValues(20.0, 0.0, 0.0);

    osi3::SensorData sensorData;
    auto movingObject = sensorData.add_moving_object();
    movingObject->mutable_header()->add_sensor_id()->set_value(0);
    movingObject->mutable_base()->mutable_position()->set_x(8.0);
    movingObject->mutable_base()->mutable_position()->set_y(0.0);
    movingObject->mutable_base()->mutable_orientation()->set_yaw(0.0);
    movingObject->mutable_base()->mutable_orientation_rate()->set_yaw(0.0);
    movingObject->mutable_base()->mutable_velocity()->set_x(-10.0);
    movingObject->mutable_base()->mutable_velocity()->set_y(0.0);
    movingObject->mutable_base()->mutable_dimension()->set_length(2.0);
    movingObject->mutable_base()->mutable_dimension()->set_width(1.0);

    implementation->UpdateInput(0, std::make_shared<SensorDataSignal>(sensorData), 0);

    implementation->Trigger(0);
    EXPECT_LT(implementation->GetAcceleration(), 0.0);
}

TEST_F(AlgorithmAutonomousEmergencyBraking_UnitTest, ActualTTCMoreThanBrakeTTC_ShouldNotActivateAEB)
{
    SetEgoValues(20.0, 0.0, 0.0);

    osi3::SensorData sensorData;
    auto movingObject = sensorData.add_moving_object();
    movingObject->mutable_header()->add_sensor_id()->set_value(0);
    movingObject->mutable_base()->mutable_position()->set_x(8.0);
    movingObject->mutable_base()->mutable_position()->set_y(0.0);
    movingObject->mutable_base()->mutable_orientation()->set_yaw(0.0);
    movingObject->mutable_base()->mutable_orientation_rate()->set_yaw(0.0);
    movingObject->mutable_base()->mutable_velocity()->set_x(0.0);
    movingObject->mutable_base()->mutable_velocity()->set_y(0.0);
    movingObject->mutable_base()->mutable_dimension()->set_length(2.0);
    movingObject->mutable_base()->mutable_dimension()->set_width(1.0);

    implementation->UpdateInput(0, std::make_shared<SensorDataSignal>(sensorData), 0);

    implementation->Trigger(0);
    EXPECT_GE(implementation->GetAcceleration(), 0.0);
}

AlgorithmAutonomousEmergencyBraking_UnitTest::AlgorithmAutonomousEmergencyBraking_UnitTest()
{
    std::map<std::string, int> intParametersSensorLink = {{"SensorId", 0}};
    std::map<std::string, const std::string> stringParametersSensorLink = {{"InputId", "Camera"}};
    auto sensorLink = std::make_shared<NiceMock<FakeParameter>>();
    ON_CALL(*sensorLink, GetParametersString()).WillByDefault(ReturnRef(stringParametersSensorLink));
    ON_CALL(*sensorLink, GetParametersInt()).WillByDefault(ReturnRef(intParametersSensorLink));
    std::map<std::string, ParameterInterface::ParameterLists> parameterLists =
        {
            {"SensorLinks", {sensorLink}}};

    std::map<std::string, double> doubleParameters = {{"TTC", 4.0},
                                                      {"Acceleration", -4},
                                                      {"CollisionDetectionLateralBoundary", 0.0},
                                                      {"CollisionDetectionLongitudinalBoundary", 0.0}};

    auto aebParameters = std::make_shared<NiceMock<FakeParameter>>();
    ON_CALL(*aebParameters, GetParameterLists()).WillByDefault(ReturnRef(parameterLists));
    ON_CALL(*aebParameters, GetParametersDouble()).WillByDefault(ReturnRef(doubleParameters));

    ON_CALL(fakeEgoAgent, GetLength()).WillByDefault(Return(2.0));
    ON_CALL(fakeEgoAgent, GetWidth()).WillByDefault(Return(1.0));
    ON_CALL(fakeEgoAgent, GetDistanceReferencePointToLeadingEdge()).WillByDefault(Return(1.0));

    int fakeCycleTime = 50;

    implementation = new AlgorithmAutonomousEmergencyBrakingImplementation("",
                                                                           false,
                                                                           0,
                                                                           0,
                                                                           0,
                                                                           fakeCycleTime,
                                                                           nullptr,
                                                                           aebParameters.get(),
                                                                           &fakePublisher,
                                                                           nullptr,
                                                                           &fakeEgoAgent);
}

void AlgorithmAutonomousEmergencyBraking_UnitTest::SetEgoValues(double velocity, double acceleration, double yawRate)
{
    ON_CALL(fakeEgoAgent, GetVelocity()).WillByDefault(Return(velocity));
    ON_CALL(fakeEgoAgent, GetAcceleration()).WillByDefault(Return(acceleration));
    ON_CALL(fakeEgoAgent, GetYawRate()).WillByDefault(Return(yawRate));
}
