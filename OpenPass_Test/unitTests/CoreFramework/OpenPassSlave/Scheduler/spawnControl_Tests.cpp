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

#include "spawnControl.h"

#include "fakeAgentBlueprint.h"
#include "fakeWorldObject.h"
#include "fakeWorld.h"
#include "fakeSpawnPointNetwork.h"
#include "fakeAgent.h"

using namespace SimulationSlave;

using ::testing::DoubleNear;
using ::testing::SizeIs;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::_;
using ::testing::NiceMock;

constexpr int ttb = 1;
constexpr int assumedBrakeAccelerationEgo = -6;

TEST(DrivingCorridorCheck, NewAgentGoingWest_InsideTccToOpponent_ReportsOverlap)
{
    VehicleModelParameters fakeEgoVehicleModelParameters;
    fakeEgoVehicleModelParameters.distanceReferencePointToLeadingEdge = 3;
    fakeEgoVehicleModelParameters.width = 2;
    fakeEgoVehicleModelParameters.length = 4;

    SpawnParameter fakeEgoSpawnParameter;
    fakeEgoSpawnParameter.yawAngle = 0;
    fakeEgoSpawnParameter.positionX = 20;
    fakeEgoSpawnParameter.positionY = 50;

    NiceMock<FakeAgentBlueprint> fakeEgo;
    ON_CALL(fakeEgo, GetVehicleModelParameters()).WillByDefault(Return(fakeEgoVehicleModelParameters));
    ON_CALL(fakeEgo, GetSpawnParameter()).WillByDefault(ReturnRef(fakeEgoSpawnParameter));

    NiceMock<FakeWorldObject> fakeOpponent;
    ON_CALL(fakeOpponent, GetDistanceReferencePointToLeadingEdge()).WillByDefault(Return(3.0));
    ON_CALL(fakeOpponent, GetLength()).WillByDefault(Return(4.0));
    ON_CALL(fakeOpponent, GetWidth()).WillByDefault(Return(2.0));
    ON_CALL(fakeOpponent, GetPositionX()).WillByDefault(Return(30.0));
    ON_CALL(fakeOpponent, GetPositionY()).WillByDefault(Return(50.0));
    ON_CALL(fakeOpponent, GetYaw()).WillByDefault(Return(0.0));

    int vEgo = 30;
    int vFront = 20;
    double fullBrakingDistance = vEgo * ttb + vEgo * vEgo / (2 * std::abs(assumedBrakeAccelerationEgo));

    ASSERT_FALSE(drivingCorridorDoesNotOverlap(ttb, vFront, 0, fullBrakingDistance, &fakeEgo, &fakeOpponent));
}

TEST(DrivingCorridorCheck, NewAgentGoingNorth_InsideTccToOpponent_ReportsOverlap)
{
    VehicleModelParameters fakeEgoVehicleModelParameters;
    fakeEgoVehicleModelParameters.distanceReferencePointToLeadingEdge = 3;
    fakeEgoVehicleModelParameters.width = 2;
    fakeEgoVehicleModelParameters.length = 4;

    SpawnParameter fakeEgoSpawnParameter;
    fakeEgoSpawnParameter.yawAngle = 1.5708;
    fakeEgoSpawnParameter.positionX = 20;
    fakeEgoSpawnParameter.positionY = 50;

    NiceMock<FakeAgentBlueprint> fakeEgo;
    ON_CALL(fakeEgo, GetVehicleModelParameters()).WillByDefault(Return(fakeEgoVehicleModelParameters));
    ON_CALL(fakeEgo, GetSpawnParameter()).WillByDefault(ReturnRef(fakeEgoSpawnParameter));

    NiceMock<FakeWorldObject> fakeOpponent;
    ON_CALL(fakeOpponent, GetDistanceReferencePointToLeadingEdge()).WillByDefault(Return(3.0));
    ON_CALL(fakeOpponent, GetLength()).WillByDefault(Return(4.0));
    ON_CALL(fakeOpponent, GetWidth()).WillByDefault(Return(2.0));
    ON_CALL(fakeOpponent, GetPositionX()).WillByDefault(Return(20.0));
    ON_CALL(fakeOpponent, GetPositionY()).WillByDefault(Return(60.0));
    ON_CALL(fakeOpponent, GetYaw()).WillByDefault(Return(1.5708));

    int vEgo = 30;
    int vFront = 20;
    double fullBrakingDistance = vEgo * ttb + vEgo * vEgo / (2 * std::abs(assumedBrakeAccelerationEgo));

    ASSERT_FALSE(drivingCorridorDoesNotOverlap(ttb, vFront, 0, fullBrakingDistance, &fakeEgo, &fakeOpponent));
}

TEST(DrivingCorridorCheck, NewAgentGoingEast_InsideTccToOpponent_ReportsOverlap)
{
    VehicleModelParameters fakeEgoVehicleModelParameters;
    fakeEgoVehicleModelParameters.distanceReferencePointToLeadingEdge = 3;
    fakeEgoVehicleModelParameters.width = 2;
    fakeEgoVehicleModelParameters.length = 4;

    SpawnParameter fakeEgoSpawnParameter;
    fakeEgoSpawnParameter.yawAngle = 3.14159;
    fakeEgoSpawnParameter.positionX = 20;
    fakeEgoSpawnParameter.positionY = 50;

    NiceMock<FakeAgentBlueprint> fakeEgo;
    ON_CALL(fakeEgo, GetVehicleModelParameters()).WillByDefault(Return(fakeEgoVehicleModelParameters));
    ON_CALL(fakeEgo, GetSpawnParameter()).WillByDefault(ReturnRef(fakeEgoSpawnParameter));

    NiceMock<FakeWorldObject> fakeOpponent;
    ON_CALL(fakeOpponent, GetDistanceReferencePointToLeadingEdge()).WillByDefault(Return(3.0));
    ON_CALL(fakeOpponent, GetLength()).WillByDefault(Return(4.0));
    ON_CALL(fakeOpponent, GetWidth()).WillByDefault(Return(2.0));
    ON_CALL(fakeOpponent, GetPositionX()).WillByDefault(Return(10.0));
    ON_CALL(fakeOpponent, GetPositionY()).WillByDefault(Return(50.0));
    ON_CALL(fakeOpponent, GetYaw()).WillByDefault(Return(3.14159));

    int vEgo = 30;
    int vFront = 20;
    double fullBrakingDistance = vEgo * ttb + vEgo * vEgo / (2 * std::abs(assumedBrakeAccelerationEgo));

    ASSERT_FALSE(drivingCorridorDoesNotOverlap(ttb, vFront, 0, fullBrakingDistance, &fakeEgo, &fakeOpponent));
}

TEST(DrivingCorridorCheck, NewAgentGoingSouth_InsideTccToOpponent_ReportsOverlap)
{
    VehicleModelParameters fakeEgoVehicleModelParameters;
    fakeEgoVehicleModelParameters.distanceReferencePointToLeadingEdge = 3;
    fakeEgoVehicleModelParameters.width = 2;
    fakeEgoVehicleModelParameters.length = 4;

    SpawnParameter fakeEgoSpawnParameter;
    fakeEgoSpawnParameter.yawAngle = -1.5708;
    fakeEgoSpawnParameter.positionX = 20;
    fakeEgoSpawnParameter.positionY = 50;

    NiceMock<FakeAgentBlueprint> fakeEgo;
    ON_CALL(fakeEgo, GetVehicleModelParameters()).WillByDefault(Return(fakeEgoVehicleModelParameters));
    ON_CALL(fakeEgo, GetSpawnParameter()).WillByDefault(ReturnRef(fakeEgoSpawnParameter));

    NiceMock<FakeWorldObject> fakeOpponent;
    ON_CALL(fakeOpponent, GetDistanceReferencePointToLeadingEdge()).WillByDefault(Return(3.0));
    ON_CALL(fakeOpponent, GetLength()).WillByDefault(Return(4.0));
    ON_CALL(fakeOpponent, GetWidth()).WillByDefault(Return(2.0));
    ON_CALL(fakeOpponent, GetPositionX()).WillByDefault(Return(20.0));
    ON_CALL(fakeOpponent, GetPositionY()).WillByDefault(Return(40.0));
    ON_CALL(fakeOpponent, GetYaw()).WillByDefault(Return(-1.5708));

    int vEgo = 30;
    int vFront = 20;
    double fullBrakingDistance = vEgo * ttb + vEgo * vEgo / (2 * std::abs(assumedBrakeAccelerationEgo));

    ASSERT_FALSE(drivingCorridorDoesNotOverlap(ttb, vFront, 0, fullBrakingDistance, &fakeEgo, &fakeOpponent));
}

TEST(DrivingCorridorCheck, NewAgent_OutsideTccToOpponent_ReportsNoOverlap)
{
    VehicleModelParameters fakeEgoVehicleModelParameters;
    fakeEgoVehicleModelParameters.distanceReferencePointToLeadingEdge = 3;
    fakeEgoVehicleModelParameters.width = 2;
    fakeEgoVehicleModelParameters.length = 4;

    SpawnParameter fakeEgoSpawnParameter;
    fakeEgoSpawnParameter.yawAngle = 0;
    fakeEgoSpawnParameter.positionX = 20;
    fakeEgoSpawnParameter.positionY = 50;

    NiceMock<FakeAgentBlueprint> fakeEgo;
    ON_CALL(fakeEgo, GetVehicleModelParameters()).WillByDefault(Return(fakeEgoVehicleModelParameters));
    ON_CALL(fakeEgo, GetSpawnParameter()).WillByDefault(ReturnRef(fakeEgoSpawnParameter));

    NiceMock<FakeWorldObject> fakeOpponent;
    ON_CALL(fakeOpponent, GetDistanceReferencePointToLeadingEdge()).WillByDefault(Return(3.0));
    ON_CALL(fakeOpponent, GetLength()).WillByDefault(Return(4.0));
    ON_CALL(fakeOpponent, GetWidth()).WillByDefault(Return(2.0));
    ON_CALL(fakeOpponent, GetPositionX()).WillByDefault(Return(20.0));
    ON_CALL(fakeOpponent, GetPositionY()).WillByDefault(Return(200.0));
    ON_CALL(fakeOpponent, GetYaw()).WillByDefault(Return(0.0));

    int vEgo = 30;
    int vFront = 20;
    double fullBrakingDistance = vEgo * ttb + vEgo * vEgo / (2 * std::abs(assumedBrakeAccelerationEgo));

    ASSERT_TRUE(drivingCorridorDoesNotOverlap(ttb, vFront, 0, fullBrakingDistance, &fakeEgo, &fakeOpponent));
}

TEST(DrivingCorridorCheck, NewAgent_InsideTccToOpponentButSideBySide_ReportsNoOverlap)
{
    VehicleModelParameters fakeEgoVehicleModelParameters;
    fakeEgoVehicleModelParameters.distanceReferencePointToLeadingEdge = 3;
    fakeEgoVehicleModelParameters.width = 2;
    fakeEgoVehicleModelParameters.length = 4;

    SpawnParameter fakeEgoSpawnParameter;
    fakeEgoSpawnParameter.yawAngle = 0;
    fakeEgoSpawnParameter.positionX = 20;
    fakeEgoSpawnParameter.positionY = 50;

    NiceMock<FakeAgentBlueprint> fakeEgo;
    ON_CALL(fakeEgo, GetVehicleModelParameters()).WillByDefault(Return(fakeEgoVehicleModelParameters));
    ON_CALL(fakeEgo, GetSpawnParameter()).WillByDefault(ReturnRef(fakeEgoSpawnParameter));

    NiceMock<FakeWorldObject> fakeOpponent;
    ON_CALL(fakeOpponent, GetDistanceReferencePointToLeadingEdge()).WillByDefault(Return(3.0));
    ON_CALL(fakeOpponent, GetLength()).WillByDefault(Return(4.0));
    ON_CALL(fakeOpponent, GetWidth()).WillByDefault(Return(2.0));
    ON_CALL(fakeOpponent, GetPositionX()).WillByDefault(Return(20.0));
    ON_CALL(fakeOpponent, GetPositionY()).WillByDefault(Return(53.0));
    ON_CALL(fakeOpponent, GetYaw()).WillByDefault(Return(0.0));

    int vEgo = 30;
    int vFront = 20;
    double fullBrakingDistance = vEgo * ttb + vEgo * vEgo / (2 * std::abs(assumedBrakeAccelerationEgo));

    ASSERT_TRUE(drivingCorridorDoesNotOverlap(ttb, vFront, 0, fullBrakingDistance, &fakeEgo, &fakeOpponent));
}

TEST(DrivingCorridorCheck, NewAgent_InsideTccToOpponentSideBySideButConverging_ReportsOverlap)
{
    VehicleModelParameters fakeEgoVehicleModelParameters;
    fakeEgoVehicleModelParameters.distanceReferencePointToLeadingEdge = 3;
    fakeEgoVehicleModelParameters.width = 2;
    fakeEgoVehicleModelParameters.length = 4;

    SpawnParameter fakeEgoSpawnParameter;
    fakeEgoSpawnParameter.yawAngle = 0.2;
    fakeEgoSpawnParameter.positionX = 20;
    fakeEgoSpawnParameter.positionY = 50;

    NiceMock<FakeAgentBlueprint> fakeEgo;
    ON_CALL(fakeEgo, GetVehicleModelParameters()).WillByDefault(Return(fakeEgoVehicleModelParameters));
    ON_CALL(fakeEgo, GetSpawnParameter()).WillByDefault(ReturnRef(fakeEgoSpawnParameter));

    NiceMock<FakeWorldObject> fakeOpponent;
    ON_CALL(fakeOpponent, GetDistanceReferencePointToLeadingEdge()).WillByDefault(Return(3.0));
    ON_CALL(fakeOpponent, GetLength()).WillByDefault(Return(4.0));
    ON_CALL(fakeOpponent, GetWidth()).WillByDefault(Return(2.0));
    ON_CALL(fakeOpponent, GetPositionX()).WillByDefault(Return(20.0));
    ON_CALL(fakeOpponent, GetPositionY()).WillByDefault(Return(53.0));
    ON_CALL(fakeOpponent, GetYaw()).WillByDefault(Return(0.0));

    int vEgo = 30;
    int vFront = 20;
    double fullBrakingDistance = vEgo * ttb + vEgo * vEgo / (2 * std::abs(assumedBrakeAccelerationEgo));

    ASSERT_FALSE(drivingCorridorDoesNotOverlap(ttb, vFront, 0, fullBrakingDistance, &fakeEgo, &fakeOpponent));
}

TEST(SpawnControl, PullNewAgents_ReturnsNoAgent)
{
    NiceMock<FakeWorld> world;
    NiceMock<FakeSpawnPointNetwork> spawnPointNetwork;
    SpawnPoint* spawnPoint{nullptr};
    ON_CALL(spawnPointNetwork, GetSpawnPoint()).WillByDefault(Return(spawnPoint));

    SpawnControl spawnControl(&spawnPointNetwork, &world, 100);
    std::list<const Agent*> newAgents;
    newAgents = spawnControl.PullNewAgents();

    ASSERT_THAT(newAgents, SizeIs(0));
}

TEST(SpawnControl, Execute_NoSpawningWithWrongTimestamp)
{
    int noValidTimeStamp = -1;

    NiceMock<FakeSpawnPointNetwork> fakeSpawnPoint;
    NiceMock<FakeWorld> fakeWorld;

    SpawnPoint* spawnPoint{nullptr};
    ON_CALL(fakeSpawnPoint, GetSpawnPoint()).WillByDefault(Return(spawnPoint));

    SpawnControl spawnControl(&fakeSpawnPoint, &fakeWorld, 100);
    // TODO: test skipping of while loop

    ASSERT_TRUE(spawnControl.Execute(noValidTimeStamp));
}

TEST(AdaptVelocityForAgentBlueprint, NoOppentAndEnoughLanespace_NoVelocityDecrease)
{
    NiceMock<FakeAgentBlueprint> agentBlueprint;
    SpawnParameter spawnParameter;
    spawnParameter.SpawningLaneId = -1;
    spawnParameter.distance = 103.0;
    spawnParameter.velocity = 30.0;
    VehicleModelParameters vehicleModelParameter;
    vehicleModelParameter.distanceReferencePointToLeadingEdge = 2.0;
    ON_CALL(agentBlueprint, GetSpawnParameter()).WillByDefault(ReturnRef(spawnParameter));
    ON_CALL(agentBlueprint, GetVehicleModelParameters()).WillByDefault(Return(vehicleModelParameter));

    NiceMock<FakeSpawnPointNetwork> fakeSpawnPoint;

    NiceMock<FakeWorld> fakeWorld;
    ON_CALL(fakeWorld, GetNextObjectInLane(_, _, -1, DoubleNear(101, 1), true)).WillByDefault(Return(nullptr));
    ON_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(_, _, -1, DoubleNear(105, 1), _)).WillByDefault(Return(1000.0));

    SpawnPoint* spawnPoint{nullptr};
    ON_CALL(fakeSpawnPoint, GetSpawnPoint()).WillByDefault(Return(spawnPoint));

    SpawnControl spawnControl(&fakeSpawnPoint, &fakeWorld, 100);

    ASSERT_TRUE(spawnControl.AdaptVelocityForAgentBlueprint(&agentBlueprint));

    ASSERT_EQ(spawnParameter.velocity, 30.0);
}

TEST(AdaptVelocityForAgentBlueprint, NoOpponentAndNotEnoughLanespace_VelocityDecrease)
{
    NiceMock<FakeAgentBlueprint> agentBlueprint;
    SpawnParameter spawnParameter;
    spawnParameter.SpawningLaneId = -1;
    spawnParameter.distance = 103.0;
    spawnParameter.velocity = 30.0;
    VehicleModelParameters vehicleModelParameter;
    vehicleModelParameter.distanceReferencePointToLeadingEdge = 2.0;
    vehicleModelParameter.length = 4.0;
    ON_CALL(agentBlueprint, GetSpawnParameter()).WillByDefault(ReturnRef(spawnParameter));
    ON_CALL(agentBlueprint, GetVehicleModelParameters()).WillByDefault(Return(vehicleModelParameter));

    NiceMock<FakeSpawnPointNetwork> fakeSpawnPoint;

    NiceMock<FakeWorld> fakeWorld;
    ON_CALL(fakeWorld, GetNextObjectInLane(_, _, -1, DoubleNear(101, 1), true)).WillByDefault(Return(nullptr));
    ON_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(_, _, -1, DoubleNear(105, 1), _)).WillByDefault(Return(77.0));

    SpawnPoint* spawnPoint{nullptr};
    ON_CALL(fakeSpawnPoint, GetSpawnPoint()).WillByDefault(Return(spawnPoint));

    SpawnControl spawnControl(&fakeSpawnPoint, &fakeWorld, 100);

    ASSERT_TRUE(spawnControl.AdaptVelocityForAgentBlueprint(&agentBlueprint));

    ASSERT_LT(spawnParameter.velocity, 25.0);
}

TEST(AdaptVelocityForAgentBlueprint, OneOpponentToNear_ReturnsFlase)
{
    NiceMock<FakeAgentBlueprint> agentBlueprint;
    SpawnParameter spawnParameter;
    spawnParameter.SpawningLaneId = -1;
    spawnParameter.distance = 103.0;
    spawnParameter.velocity = 30.0;
    spawnParameter.yawAngle = 0;
    spawnParameter.positionX = 100;
    spawnParameter.positionY = 0;

    VehicleModelParameters vehicleModelParameters;
    vehicleModelParameters.distanceReferencePointToLeadingEdge = 2.0;
    vehicleModelParameters.width = 2;
    vehicleModelParameters.length = 4;

    ON_CALL(agentBlueprint, GetSpawnParameter()).WillByDefault(ReturnRef(spawnParameter));
    ON_CALL(agentBlueprint, GetVehicleModelParameters()).WillByDefault(Return(vehicleModelParameters));

    NiceMock<FakeAgent> opponent;
    ON_CALL(opponent, GetVelocity()).WillByDefault(Return(0));
    ON_CALL(opponent, GetDistanceToStartOfRoad(_)).WillByDefault(Return(99));
    ON_CALL(opponent, GetLength()).WillByDefault(Return(4.0));
    ON_CALL(opponent, GetWidth()).WillByDefault(Return(2.0));
    ON_CALL(opponent, GetPositionX()).WillByDefault(Return(177.0));
    ON_CALL(opponent, GetPositionY()).WillByDefault(Return(0.0));
    ON_CALL(opponent, GetYaw()).WillByDefault(Return(0.0));

    NiceMock<FakeSpawnPointNetwork> fakeSpawnPoint;

    NiceMock<FakeWorld> fakeWorld;
    ON_CALL(fakeWorld, GetNextObjectInLane(_, _, -1, DoubleNear(101, 1), true)).WillByDefault(Return(&opponent));
    ON_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(_, _, -1, _, _)).WillByDefault(Return(1000.0));

    SpawnPoint* spawnPoint{nullptr};
    ON_CALL(fakeSpawnPoint, GetSpawnPoint()).WillByDefault(Return(spawnPoint));

    SpawnControl spawnControl(&fakeSpawnPoint, &fakeWorld, 100);

    ASSERT_FALSE(spawnControl.AdaptVelocityForAgentBlueprint(&agentBlueprint));
}

TEST(AdaptVelocityForAgentBlueprint, OneOpponentFarAway_NoVelocityDecrease)
{
    NiceMock<FakeAgentBlueprint> agentBlueprint;
    SpawnParameter spawnParameter;
    spawnParameter.SpawningLaneId = -1;
    spawnParameter.distance = 103.0;
    spawnParameter.velocity = 30.0;
    spawnParameter.yawAngle = 0;
    spawnParameter.positionX = 100;
    spawnParameter.positionY = 0;

    VehicleModelParameters vehicleModelParameter;
    vehicleModelParameter.distanceReferencePointToLeadingEdge = 2.0;
    vehicleModelParameter.width = 2;
    vehicleModelParameter.length = 4;

    ON_CALL(agentBlueprint, GetSpawnParameter()).WillByDefault(ReturnRef(spawnParameter));
    ON_CALL(agentBlueprint, GetVehicleModelParameters()).WillByDefault(Return(vehicleModelParameter));

    NiceMock<FakeAgent> opponent;
    ON_CALL(opponent, GetVelocity()).WillByDefault(Return(0));
    ON_CALL(opponent, GetDistanceToStartOfRoad(_)).WillByDefault(Return(500));
    ON_CALL(opponent, GetLength()).WillByDefault(Return(4.0));
    ON_CALL(opponent, GetWidth()).WillByDefault(Return(2.0));
    ON_CALL(opponent, GetPositionX()).WillByDefault(Return(177.0));
    ON_CALL(opponent, GetPositionY()).WillByDefault(Return(0.0));
    ON_CALL(opponent, GetYaw()).WillByDefault(Return(0.0));

    NiceMock<FakeSpawnPointNetwork> fakeSpawnPoint;

    NiceMock<FakeWorld> fakeWorld;
    ON_CALL(fakeWorld, GetNextObjectInLane(_, _, -1, DoubleNear(101, 1), true)).WillByDefault(Return(&opponent));
    ON_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(_, _, -1, _, _)).WillByDefault(Return(1000.0));

    SpawnPoint* spawnPoint{nullptr};
    ON_CALL(fakeSpawnPoint, GetSpawnPoint()).WillByDefault(Return(spawnPoint));

    SpawnControl spawnControl(&fakeSpawnPoint, &fakeWorld, 100);

    ASSERT_TRUE(spawnControl.AdaptVelocityForAgentBlueprint(&agentBlueprint));

    ASSERT_EQ(spawnParameter.velocity, 30.0);
}

TEST(AdaptVelocityForAgentBlueprint, OneOpponentNearAndSlow_VelocityDecrease)
{
    NiceMock<FakeAgentBlueprint> agentBlueprint;
    SpawnParameter spawnParameter;
    spawnParameter.SpawningLaneId = -1;
    spawnParameter.distance = 103.0;
    spawnParameter.velocity = 30.0;
    spawnParameter.yawAngle = 0;
    spawnParameter.positionX = 100;
    spawnParameter.positionY = 0;

    VehicleModelParameters vehicleModelParameter;
    vehicleModelParameter.distanceReferencePointToLeadingEdge = 2.0;
    vehicleModelParameter.width = 2;
    vehicleModelParameter.length = 4;


    ON_CALL(agentBlueprint, GetSpawnParameter()).WillByDefault(ReturnRef(spawnParameter));
    ON_CALL(agentBlueprint, GetVehicleModelParameters()).WillByDefault(Return(vehicleModelParameter));

    NiceMock<FakeAgent> opponent;
    ON_CALL(opponent, GetVelocity()).WillByDefault(Return(10));
    ON_CALL(opponent, GetDistanceToStartOfRoad(_)).WillByDefault(Return(177));
    ON_CALL(opponent, GetLength()).WillByDefault(Return(4.0));
    ON_CALL(opponent, GetWidth()).WillByDefault(Return(2.0));
    ON_CALL(opponent, GetPositionX()).WillByDefault(Return(177.0));
    ON_CALL(opponent, GetPositionY()).WillByDefault(Return(0.0));
    ON_CALL(opponent, GetYaw()).WillByDefault(Return(0.0));

    NiceMock<FakeSpawnPointNetwork> fakeSpawnPoint;

    NiceMock<FakeWorld> fakeWorld;
    ON_CALL(fakeWorld, GetNextObjectInLane(_, _, -1, 101, true)).WillByDefault(Return(&opponent));
    ON_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(_, _, -1, _, _)).WillByDefault(Return(1000.0));

    SpawnPoint* spawnPoint{nullptr};
    ON_CALL(fakeSpawnPoint, GetSpawnPoint()).WillByDefault(Return(spawnPoint));

    SpawnControl spawnControl(&fakeSpawnPoint, &fakeWorld, 100);

    ASSERT_TRUE(spawnControl.AdaptVelocityForAgentBlueprint(&agentBlueprint));

    ASSERT_LT(spawnParameter.velocity, 25.0);
}

TEST(AdaptVelocityForAgentBlueprint, OneOpponentNearAndFast_NoVelocityDecrease)
{
    NiceMock<FakeAgentBlueprint> agentBlueprint;
    SpawnParameter spawnParameter;
    spawnParameter.SpawningLaneId = -1;
    spawnParameter.distance = 103.0;
    spawnParameter.velocity = 30.0;
    spawnParameter.yawAngle = 0;
    spawnParameter.positionX = 100;
    spawnParameter.positionY = 0;

    VehicleModelParameters vehicleModelParameter;
    vehicleModelParameter.distanceReferencePointToLeadingEdge = 2.0;
    vehicleModelParameter.width = 2;
    vehicleModelParameter.length = 4;


    ON_CALL(agentBlueprint, GetSpawnParameter()).WillByDefault(ReturnRef(spawnParameter));
    ON_CALL(agentBlueprint, GetVehicleModelParameters()).WillByDefault(Return(vehicleModelParameter));

    NiceMock<FakeAgent> opponent;
    ON_CALL(opponent, GetVelocity()).WillByDefault(Return(25));
    ON_CALL(opponent, GetDistanceToStartOfRoad(_)).WillByDefault(Return(195));
    ON_CALL(opponent, GetLength()).WillByDefault(Return(4.0));
    ON_CALL(opponent, GetWidth()).WillByDefault(Return(2.0));
    ON_CALL(opponent, GetPositionX()).WillByDefault(Return(195.0));
    ON_CALL(opponent, GetPositionY()).WillByDefault(Return(0.0));
    ON_CALL(opponent, GetYaw()).WillByDefault(Return(0.0));

    NiceMock<FakeSpawnPointNetwork> fakeSpawnPoint;

    NiceMock<FakeWorld> fakeWorld;
    ON_CALL(fakeWorld, GetNextObjectInLane(_, _, -1, DoubleNear(101, 1), true)).WillByDefault(Return(&opponent));
    ON_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(_, _, -1, _, _)).WillByDefault(Return(1000.0));

    SpawnPoint* spawnPoint{nullptr};
    ON_CALL(fakeSpawnPoint, GetSpawnPoint()).WillByDefault(Return(spawnPoint));

    SpawnControl spawnControl(&fakeSpawnPoint, &fakeWorld, 100);

    ASSERT_TRUE(spawnControl.AdaptVelocityForAgentBlueprint(&agentBlueprint));

    ASSERT_EQ(spawnParameter.velocity, 30.0);
}

TEST(AdaptVelocityForAgentBlueprint, TwoOpponentsBothFast_NoVelocityDecrease)
{
    NiceMock<FakeAgentBlueprint> agentBlueprint;
    SpawnParameter spawnParameter;
    spawnParameter.SpawningLaneId = -1;
    spawnParameter.distance = 103.0;
    spawnParameter.velocity = 30.0;
    spawnParameter.yawAngle = 0;
    spawnParameter.positionX = 100;
    spawnParameter.positionY = 0;

    VehicleModelParameters vehicleModelParameter;
    vehicleModelParameter.distanceReferencePointToLeadingEdge = 2.0;
    vehicleModelParameter.width = 2;
    vehicleModelParameter.length = 4;


    ON_CALL(agentBlueprint, GetSpawnParameter()).WillByDefault(ReturnRef(spawnParameter));
    ON_CALL(agentBlueprint, GetVehicleModelParameters()).WillByDefault(Return(vehicleModelParameter));

    NiceMock<FakeAgent> opponent1;
    ON_CALL(opponent1, GetVelocity()).WillByDefault(Return(45));
    ON_CALL(opponent1, GetDistanceToStartOfRoad(_)).WillByDefault(Return(155));
    ON_CALL(opponent1, GetLength()).WillByDefault(Return(4.0));
    ON_CALL(opponent1, GetWidth()).WillByDefault(Return(2.0));
    ON_CALL(opponent1, GetPositionX()).WillByDefault(Return(155.0));
    ON_CALL(opponent1, GetPositionY()).WillByDefault(Return(0.0));
    ON_CALL(opponent1, GetYaw()).WillByDefault(Return(0.0));

    NiceMock<FakeAgent> opponent2;
    ON_CALL(opponent2, GetVelocity()).WillByDefault(Return(25));
    ON_CALL(opponent2, GetDistanceToStartOfRoad(_)).WillByDefault(Return(195));
    ON_CALL(opponent2, GetLength()).WillByDefault(Return(4.0));
    ON_CALL(opponent2, GetWidth()).WillByDefault(Return(2.0));
    ON_CALL(opponent2, GetPositionX()).WillByDefault(Return(195.0));
    ON_CALL(opponent2, GetPositionY()).WillByDefault(Return(0.0));
    ON_CALL(opponent2, GetYaw()).WillByDefault(Return(0.0));

    NiceMock<FakeSpawnPointNetwork> fakeSpawnPoint;

    NiceMock<FakeWorld> fakeWorld;
    ON_CALL(fakeWorld, GetNextObjectInLane(_, _, -1, DoubleNear(101, 1), true)).WillByDefault(Return(&opponent1));
    ON_CALL(fakeWorld, GetNextObjectInLane(_, _, -1, DoubleNear(155, 1), true)).WillByDefault(Return(&opponent2));
    ON_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(_, _, -1, _, _)).WillByDefault(Return(1000.0));

    SpawnPoint* spawnPoint{nullptr};
    ON_CALL(fakeSpawnPoint, GetSpawnPoint()).WillByDefault(Return(spawnPoint));

    SpawnControl spawnControl(&fakeSpawnPoint, &fakeWorld, 100);

    ASSERT_TRUE(spawnControl.AdaptVelocityForAgentBlueprint(&agentBlueprint));

    ASSERT_EQ(spawnParameter.velocity, 30.0);
}

TEST(AdaptVelocityForAgentBlueprint, TwoOpponentsOneFastOneSlow_VelocityDecrease)
{
    NiceMock<FakeAgentBlueprint> agentBlueprint;
    SpawnParameter spawnParameter;
    spawnParameter.SpawningLaneId = -1;
    spawnParameter.distance = 103.0;
    spawnParameter.velocity = 30.0;
    spawnParameter.yawAngle = 0;
    spawnParameter.positionX = 100;
    spawnParameter.positionY = 0;

    VehicleModelParameters vehicleModelParameter;
    vehicleModelParameter.distanceReferencePointToLeadingEdge = 2.0;
    vehicleModelParameter.width = 2;
    vehicleModelParameter.length = 4;


    ON_CALL(agentBlueprint, GetSpawnParameter()).WillByDefault(ReturnRef(spawnParameter));
    ON_CALL(agentBlueprint, GetVehicleModelParameters()).WillByDefault(Return(vehicleModelParameter));

    NiceMock<FakeAgent> opponent1;
    ON_CALL(opponent1, GetVelocity()).WillByDefault(Return(45));
    ON_CALL(opponent1, GetDistanceToStartOfRoad(_)).WillByDefault(Return(155));
    ON_CALL(opponent1, GetLength()).WillByDefault(Return(4.0));
    ON_CALL(opponent1, GetWidth()).WillByDefault(Return(2.0));
    ON_CALL(opponent1, GetPositionX()).WillByDefault(Return(155.0));
    ON_CALL(opponent1, GetPositionY()).WillByDefault(Return(0.0));
    ON_CALL(opponent1, GetYaw()).WillByDefault(Return(0.0));

    NiceMock<FakeAgent> opponent2;
    ON_CALL(opponent2, GetVelocity()).WillByDefault(Return(10));
    ON_CALL(opponent2, GetDistanceToStartOfRoad(_)).WillByDefault(Return(177));
    ON_CALL(opponent2, GetLength()).WillByDefault(Return(4.0));
    ON_CALL(opponent2, GetWidth()).WillByDefault(Return(2.0));
    ON_CALL(opponent2, GetPositionX()).WillByDefault(Return(177.0));
    ON_CALL(opponent2, GetPositionY()).WillByDefault(Return(0.0));
    ON_CALL(opponent2, GetYaw()).WillByDefault(Return(0.0));

    NiceMock<FakeSpawnPointNetwork> fakeSpawnPoint;

    NiceMock<FakeWorld> fakeWorld;
    ON_CALL(fakeWorld, GetNextObjectInLane(_, _, -1, DoubleNear(101, 1), true)).WillByDefault(Return(&opponent1));
    ON_CALL(fakeWorld, GetNextObjectInLane(_, _, -1, DoubleNear(155, 1), true)).WillByDefault(Return(&opponent2));
    ON_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(_, _, -1, _, _)).WillByDefault(Return(1000.0));

    SpawnPoint* spawnPoint{nullptr};
    ON_CALL(fakeSpawnPoint, GetSpawnPoint()).WillByDefault(Return(spawnPoint));

    SpawnControl spawnControl(&fakeSpawnPoint, &fakeWorld, 100);

    ASSERT_TRUE(spawnControl.AdaptVelocityForAgentBlueprint(&agentBlueprint));

    ASSERT_LT(spawnParameter.velocity, 25.0);
}

TEST(AdaptVelocityForAgentBlueprint, TwoOpponentsBothSlow_VelocityDecrease)
{
    NiceMock<FakeAgentBlueprint> agentBlueprint;
    SpawnParameter spawnParameter;
    spawnParameter.SpawningLaneId = -1;
    spawnParameter.distance = 103.0;
    spawnParameter.velocity = 30.0;
    spawnParameter.yawAngle = 0;
    spawnParameter.positionX = 100;
    spawnParameter.positionY = 0;

    VehicleModelParameters vehicleModelParameter;
    vehicleModelParameter.distanceReferencePointToLeadingEdge = 2.0;
    vehicleModelParameter.width = 2;
    vehicleModelParameter.length = 4;


    ON_CALL(agentBlueprint, GetSpawnParameter()).WillByDefault(ReturnRef(spawnParameter));
    ON_CALL(agentBlueprint, GetVehicleModelParameters()).WillByDefault(Return(vehicleModelParameter));

    NiceMock<FakeAgent> opponent1;
    ON_CALL(opponent1, GetVelocity()).WillByDefault(Return(25));
    ON_CALL(opponent1, GetDistanceToStartOfRoad(_)).WillByDefault(Return(170));
    ON_CALL(opponent1, GetLength()).WillByDefault(Return(4.0));
    ON_CALL(opponent1, GetWidth()).WillByDefault(Return(2.0));
    ON_CALL(opponent1, GetPositionX()).WillByDefault(Return(170.0));
    ON_CALL(opponent1, GetPositionY()).WillByDefault(Return(0.0));
    ON_CALL(opponent1, GetYaw()).WillByDefault(Return(0.0));

    NiceMock<FakeAgent> opponent2;
    ON_CALL(opponent2, GetVelocity()).WillByDefault(Return(10));
    ON_CALL(opponent2, GetDistanceToStartOfRoad(_)).WillByDefault(Return(177));
    ON_CALL(opponent2, GetLength()).WillByDefault(Return(4.0));
    ON_CALL(opponent2, GetWidth()).WillByDefault(Return(2.0));
    ON_CALL(opponent2, GetPositionX()).WillByDefault(Return(177.0));
    ON_CALL(opponent2, GetPositionY()).WillByDefault(Return(0.0));
    ON_CALL(opponent2, GetYaw()).WillByDefault(Return(0.0));

    NiceMock<FakeSpawnPointNetwork> fakeSpawnPoint;

    NiceMock<FakeWorld> fakeWorld;
    ON_CALL(fakeWorld, GetNextObjectInLane(_, _, -1, DoubleNear(101, 1), true)).WillByDefault(Return(&opponent1));
    ON_CALL(fakeWorld, GetNextObjectInLane(_, _, -1, DoubleNear(170, 1), true)).WillByDefault(Return(&opponent2));
    ON_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(_, _, -1, _, _)).WillByDefault(Return(1000.0));

    SpawnPoint* spawnPoint{nullptr};
    ON_CALL(fakeSpawnPoint, GetSpawnPoint()).WillByDefault(Return(spawnPoint));

    SpawnControl spawnControl(&fakeSpawnPoint, &fakeWorld, 100);

    ASSERT_TRUE(spawnControl.AdaptVelocityForAgentBlueprint(&agentBlueprint));

    ASSERT_LT(spawnParameter.velocity, 25.0);
}

TEST(CalculateHoldbackTime, NoOppentAndEnoughLanespace_NoHoldBackTime)
{
    NiceMock<FakeAgentBlueprint> agentBlueprint;
    SpawnParameter spawnParameter;
    spawnParameter.SpawningLaneId = -1;
    spawnParameter.distance = 103.0;
    spawnParameter.velocity = 30.0;
    VehicleModelParameters vehicleModelParameter;
    vehicleModelParameter.distanceReferencePointToLeadingEdge = 2.0;
    ON_CALL(agentBlueprint, GetSpawnParameter()).WillByDefault(ReturnRef(spawnParameter));
    ON_CALL(agentBlueprint, GetVehicleModelParameters()).WillByDefault(Return(vehicleModelParameter));

    NiceMock<FakeSpawnPointNetwork> fakeSpawnPoint;

    NiceMock<FakeWorld> fakeWorld;
    ON_CALL(fakeWorld, GetNextObjectInLane(_, _, -1, DoubleNear(101, 1), true)).WillByDefault(Return(nullptr));
    ON_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(_, _, -1, DoubleNear(105, 1), _)).WillByDefault(Return(1000.0));

    SpawnPoint* spawnPoint{nullptr};
    ON_CALL(fakeSpawnPoint, GetSpawnPoint()).WillByDefault(Return(spawnPoint));

    SpawnControl spawnControl(&fakeSpawnPoint, &fakeWorld, 100);

    ASSERT_EQ(spawnControl.CalculateHoldbackTime(&agentBlueprint), 0);
}

TEST(CalculateHoldbackTime, NoOpponentAndNotEnoughLanespace_ReturnsNegative)
{
    NiceMock<FakeAgentBlueprint> agentBlueprint;
    SpawnParameter spawnParameter;
    spawnParameter.SpawningLaneId = -1;
    spawnParameter.distance = 103.0;
    spawnParameter.velocity = 30.0;
    VehicleModelParameters vehicleModelParameter;
    vehicleModelParameter.distanceReferencePointToLeadingEdge = 2.0;
    vehicleModelParameter.length = 4.0;
    ON_CALL(agentBlueprint, GetSpawnParameter()).WillByDefault(ReturnRef(spawnParameter));
    ON_CALL(agentBlueprint, GetVehicleModelParameters()).WillByDefault(Return(vehicleModelParameter));

    NiceMock<FakeSpawnPointNetwork> fakeSpawnPoint;

    NiceMock<FakeWorld> fakeWorld;
    ON_CALL(fakeWorld, GetNextObjectInLane(_, _, -1, DoubleNear(101, 1), true)).WillByDefault(Return(nullptr));
    ON_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(_, _, -1, DoubleNear(105, 1), _)).WillByDefault(Return(77.0));

    SpawnPoint* spawnPoint{nullptr};
    ON_CALL(fakeSpawnPoint, GetSpawnPoint()).WillByDefault(Return(spawnPoint));

    SpawnControl spawnControl(&fakeSpawnPoint, &fakeWorld, 100);

    ASSERT_LT(spawnControl.CalculateHoldbackTime(&agentBlueprint), 0);
}

TEST(CalculateHoldbackTime, OneOpponentFarAway_NoHoldBackTime)
{
    NiceMock<FakeAgentBlueprint> agentBlueprint;
    SpawnParameter spawnParameter;
    spawnParameter.SpawningLaneId = -1;
    spawnParameter.distance = 103.0;
    spawnParameter.velocity = 30.0;
    spawnParameter.yawAngle = 0;
    spawnParameter.positionX = 100;
    spawnParameter.positionY = 0;

    VehicleModelParameters vehicleModelParameter;
    vehicleModelParameter.distanceReferencePointToLeadingEdge = 2.0;
    vehicleModelParameter.width = 2;
    vehicleModelParameter.length = 4;

    ON_CALL(agentBlueprint, GetSpawnParameter()).WillByDefault(ReturnRef(spawnParameter));
    ON_CALL(agentBlueprint, GetVehicleModelParameters()).WillByDefault(Return(vehicleModelParameter));

    NiceMock<FakeAgent> opponent;
    ON_CALL(opponent, GetVelocity()).WillByDefault(Return(0));
    ON_CALL(opponent, GetDistanceToStartOfRoad(_)).WillByDefault(Return(500));
    ON_CALL(opponent, GetLength()).WillByDefault(Return(4.0));
    ON_CALL(opponent, GetWidth()).WillByDefault(Return(2.0));
    ON_CALL(opponent, GetPositionX()).WillByDefault(Return(177.0));
    ON_CALL(opponent, GetPositionY()).WillByDefault(Return(0.0));
    ON_CALL(opponent, GetYaw()).WillByDefault(Return(0.0));

    NiceMock<FakeSpawnPointNetwork> fakeSpawnPoint;

    NiceMock<FakeWorld> fakeWorld;
    ON_CALL(fakeWorld, GetNextObjectInLane(_, _, -1, DoubleNear(101, 1), true)).WillByDefault(Return(&opponent));
    ON_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(_, _, -1, _, _)).WillByDefault(Return(1000.0));

    SpawnPoint* spawnPoint{nullptr};
    ON_CALL(fakeSpawnPoint, GetSpawnPoint()).WillByDefault(Return(spawnPoint));

    SpawnControl spawnControl(&fakeSpawnPoint, &fakeWorld, 100);

    ASSERT_EQ(spawnControl.CalculateHoldbackTime(&agentBlueprint), 0);
}

TEST(CalculateHoldbackTime, OneOpponentToNear_ReturnsNegative)
{
    NiceMock<FakeAgentBlueprint> agentBlueprint;
    SpawnParameter spawnParameter;
    spawnParameter.SpawningLaneId = -1;
    spawnParameter.distance = 103.0;
    spawnParameter.velocity = 30.0;
    spawnParameter.yawAngle = 0;
    spawnParameter.positionX = 100;
    spawnParameter.positionY = 0;

    VehicleModelParameters vehicleModelParameters;
    vehicleModelParameters.distanceReferencePointToLeadingEdge = 2.0;
    vehicleModelParameters.width = 2;
    vehicleModelParameters.length = 4;

    ON_CALL(agentBlueprint, GetSpawnParameter()).WillByDefault(ReturnRef(spawnParameter));
    ON_CALL(agentBlueprint, GetVehicleModelParameters()).WillByDefault(Return(vehicleModelParameters));

    NiceMock<FakeAgent> opponent;
    ON_CALL(opponent, GetVelocity()).WillByDefault(Return(0));
    ON_CALL(opponent, GetDistanceToStartOfRoad(_)).WillByDefault(Return(99));
    ON_CALL(opponent, GetLength()).WillByDefault(Return(4.0));
    ON_CALL(opponent, GetWidth()).WillByDefault(Return(2.0));
    ON_CALL(opponent, GetPositionX()).WillByDefault(Return(177.0));
    ON_CALL(opponent, GetPositionY()).WillByDefault(Return(0.0));
    ON_CALL(opponent, GetYaw()).WillByDefault(Return(0.0));

    NiceMock<FakeSpawnPointNetwork> fakeSpawnPoint;

    NiceMock<FakeWorld> fakeWorld;
    ON_CALL(fakeWorld, GetNextObjectInLane(_, _, -1, DoubleNear(101, 1), true)).WillByDefault(Return(&opponent));
    ON_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(_, _, -1, _, _)).WillByDefault(Return(1000.0));

    SpawnPoint* spawnPoint{nullptr};
    ON_CALL(fakeSpawnPoint, GetSpawnPoint()).WillByDefault(Return(spawnPoint));

    SpawnControl spawnControl(&fakeSpawnPoint, &fakeWorld, 100);

    ASSERT_LT(spawnControl.CalculateHoldbackTime(&agentBlueprint), 0);
}

TEST(CalculateHoldbackTime, OneOpponentNearAndSlow_HoldBackTime)
{
    NiceMock<FakeAgentBlueprint> agentBlueprint;
    SpawnParameter spawnParameter;
    spawnParameter.SpawningLaneId = -1;
    spawnParameter.distance = 103.0;
    spawnParameter.velocity = 30.0;
    spawnParameter.yawAngle = 0;
    spawnParameter.positionX = 100;
    spawnParameter.positionY = 0;

    VehicleModelParameters vehicleModelParameter;
    vehicleModelParameter.distanceReferencePointToLeadingEdge = 2.0;
    vehicleModelParameter.width = 2;
    vehicleModelParameter.length = 4;


    ON_CALL(agentBlueprint, GetSpawnParameter()).WillByDefault(ReturnRef(spawnParameter));
    ON_CALL(agentBlueprint, GetVehicleModelParameters()).WillByDefault(Return(vehicleModelParameter));

    NiceMock<FakeAgent> opponent;
    ON_CALL(opponent, GetVelocity()).WillByDefault(Return(10));
    ON_CALL(opponent, GetDistanceToStartOfRoad(_)).WillByDefault(Return(175));
    ON_CALL(opponent, GetLength()).WillByDefault(Return(4.0));
    ON_CALL(opponent, GetWidth()).WillByDefault(Return(2.0));
    ON_CALL(opponent, GetPositionX()).WillByDefault(Return(175.0));
    ON_CALL(opponent, GetPositionY()).WillByDefault(Return(0.0));
    ON_CALL(opponent, GetYaw()).WillByDefault(Return(0.0));

    NiceMock<FakeSpawnPointNetwork> fakeSpawnPoint;

    NiceMock<FakeWorld> fakeWorld;
    ON_CALL(fakeWorld, GetNextObjectInLane(_, _, -1, DoubleNear(101, 1), true)).WillByDefault(Return(&opponent));
    ON_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(_, _, -1, _, _)).WillByDefault(Return(1000.0));

    SpawnPoint* spawnPoint{nullptr};
    ON_CALL(fakeSpawnPoint, GetSpawnPoint()).WillByDefault(Return(spawnPoint));

    SpawnControl spawnControl(&fakeSpawnPoint, &fakeWorld, 100);

    ASSERT_EQ(spawnControl.CalculateHoldbackTime(&agentBlueprint), 3100);
}

TEST(CalculateHoldbackTime, OneOpponentNearAndFast_NoHoldBackTime)
{
    NiceMock<FakeAgentBlueprint> agentBlueprint;
    SpawnParameter spawnParameter;
    spawnParameter.SpawningLaneId = -1;
    spawnParameter.distance = 103.0;
    spawnParameter.velocity = 30.0;
    spawnParameter.yawAngle = 0;
    spawnParameter.positionX = 100;
    spawnParameter.positionY = 0;

    VehicleModelParameters vehicleModelParameter;
    vehicleModelParameter.distanceReferencePointToLeadingEdge = 2.0;
    vehicleModelParameter.width = 2;
    vehicleModelParameter.length = 4;


    ON_CALL(agentBlueprint, GetSpawnParameter()).WillByDefault(ReturnRef(spawnParameter));
    ON_CALL(agentBlueprint, GetVehicleModelParameters()).WillByDefault(Return(vehicleModelParameter));

    NiceMock<FakeAgent> opponent;
    ON_CALL(opponent, GetVelocity()).WillByDefault(Return(25));
    ON_CALL(opponent, GetDistanceToStartOfRoad(_)).WillByDefault(Return(195));
    ON_CALL(opponent, GetLength()).WillByDefault(Return(4.0));
    ON_CALL(opponent, GetWidth()).WillByDefault(Return(2.0));
    ON_CALL(opponent, GetPositionX()).WillByDefault(Return(195.0));
    ON_CALL(opponent, GetPositionY()).WillByDefault(Return(0.0));
    ON_CALL(opponent, GetYaw()).WillByDefault(Return(0.0));

    NiceMock<FakeSpawnPointNetwork> fakeSpawnPoint;

    NiceMock<FakeWorld> fakeWorld;
    ON_CALL(fakeWorld, GetNextObjectInLane(_, _, -1, DoubleNear(101, 1), true)).WillByDefault(Return(&opponent));
    ON_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(_, _, -1, _, _)).WillByDefault(Return(1000.0));

    SpawnPoint* spawnPoint{nullptr};
    ON_CALL(fakeSpawnPoint, GetSpawnPoint()).WillByDefault(Return(spawnPoint));

    SpawnControl spawnControl(&fakeSpawnPoint, &fakeWorld, 100);

    ASSERT_EQ(spawnControl.CalculateHoldbackTime(&agentBlueprint), 0);
}

TEST(CalculateHoldbackTime, TwoOpponentsBothFast_NoHoldBackTime)
{
    NiceMock<FakeAgentBlueprint> agentBlueprint;
    SpawnParameter spawnParameter;
    spawnParameter.SpawningLaneId = -1;
    spawnParameter.distance = 103.0;
    spawnParameter.velocity = 30.0;
    spawnParameter.yawAngle = 0;
    spawnParameter.positionX = 100;
    spawnParameter.positionY = 0;

    VehicleModelParameters vehicleModelParameter;
    vehicleModelParameter.distanceReferencePointToLeadingEdge = 2.0;
    vehicleModelParameter.width = 2;
    vehicleModelParameter.length = 4;


    ON_CALL(agentBlueprint, GetSpawnParameter()).WillByDefault(ReturnRef(spawnParameter));
    ON_CALL(agentBlueprint, GetVehicleModelParameters()).WillByDefault(Return(vehicleModelParameter));

    NiceMock<FakeAgent> opponent1;
    ON_CALL(opponent1, GetVelocity()).WillByDefault(Return(45));
    ON_CALL(opponent1, GetDistanceToStartOfRoad(_)).WillByDefault(Return(155));
    ON_CALL(opponent1, GetLength()).WillByDefault(Return(4.0));
    ON_CALL(opponent1, GetWidth()).WillByDefault(Return(2.0));
    ON_CALL(opponent1, GetPositionX()).WillByDefault(Return(155.0));
    ON_CALL(opponent1, GetPositionY()).WillByDefault(Return(0.0));
    ON_CALL(opponent1, GetYaw()).WillByDefault(Return(0.0));

    NiceMock<FakeAgent> opponent2;
    ON_CALL(opponent2, GetVelocity()).WillByDefault(Return(25));
    ON_CALL(opponent2, GetDistanceToStartOfRoad(_)).WillByDefault(Return(195));
    ON_CALL(opponent2, GetLength()).WillByDefault(Return(4.0));
    ON_CALL(opponent2, GetWidth()).WillByDefault(Return(2.0));
    ON_CALL(opponent2, GetPositionX()).WillByDefault(Return(195.0));
    ON_CALL(opponent2, GetPositionY()).WillByDefault(Return(0.0));
    ON_CALL(opponent2, GetYaw()).WillByDefault(Return(0.0));

    NiceMock<FakeSpawnPointNetwork> fakeSpawnPoint;

    NiceMock<FakeWorld> fakeWorld;
    ON_CALL(fakeWorld, GetNextObjectInLane(_, _, -1, DoubleNear(101, 1), true)).WillByDefault(Return(&opponent1));
    ON_CALL(fakeWorld, GetNextObjectInLane(_, _, -1, DoubleNear(155, 1), true)).WillByDefault(Return(&opponent2));
    ON_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(_, _, -1, _, _)).WillByDefault(Return(1000.0));

    SpawnPoint* spawnPoint{nullptr};
    ON_CALL(fakeSpawnPoint, GetSpawnPoint()).WillByDefault(Return(spawnPoint));

    SpawnControl spawnControl(&fakeSpawnPoint, &fakeWorld, 100);

    ASSERT_EQ(spawnControl.CalculateHoldbackTime(&agentBlueprint), 0);
}

TEST(CalculateHoldbackTime, TwoOpponentsOneFastOneSlow_HoldBackTime)
{
    NiceMock<FakeAgentBlueprint> agentBlueprint;
    SpawnParameter spawnParameter;
    spawnParameter.SpawningLaneId = -1;
    spawnParameter.distance = 103.0;
    spawnParameter.velocity = 30.0;
    spawnParameter.yawAngle = 0;
    spawnParameter.positionX = 100;
    spawnParameter.positionY = 0;

    VehicleModelParameters vehicleModelParameter;
    vehicleModelParameter.distanceReferencePointToLeadingEdge = 2.0;
    vehicleModelParameter.width = 2;
    vehicleModelParameter.length = 4;


    ON_CALL(agentBlueprint, GetSpawnParameter()).WillByDefault(ReturnRef(spawnParameter));
    ON_CALL(agentBlueprint, GetVehicleModelParameters()).WillByDefault(Return(vehicleModelParameter));

    NiceMock<FakeAgent> opponent1;
    ON_CALL(opponent1, GetVelocity()).WillByDefault(Return(45));
    ON_CALL(opponent1, GetDistanceToStartOfRoad(_)).WillByDefault(Return(155));
    ON_CALL(opponent1, GetLength()).WillByDefault(Return(4.0));
    ON_CALL(opponent1, GetWidth()).WillByDefault(Return(2.0));
    ON_CALL(opponent1, GetPositionX()).WillByDefault(Return(155.0));
    ON_CALL(opponent1, GetPositionY()).WillByDefault(Return(0.0));
    ON_CALL(opponent1, GetYaw()).WillByDefault(Return(0.0));

    NiceMock<FakeAgent> opponent2;
    ON_CALL(opponent2, GetVelocity()).WillByDefault(Return(10));
    ON_CALL(opponent2, GetDistanceToStartOfRoad(_)).WillByDefault(Return(175));
    ON_CALL(opponent2, GetLength()).WillByDefault(Return(4.0));
    ON_CALL(opponent2, GetWidth()).WillByDefault(Return(2.0));
    ON_CALL(opponent2, GetPositionX()).WillByDefault(Return(175.0));
    ON_CALL(opponent2, GetPositionY()).WillByDefault(Return(0.0));
    ON_CALL(opponent2, GetYaw()).WillByDefault(Return(0.0));

    NiceMock<FakeSpawnPointNetwork> fakeSpawnPoint;

    NiceMock<FakeWorld> fakeWorld;
    ON_CALL(fakeWorld, GetNextObjectInLane(_, _, -1, DoubleNear(101, 1), true)).WillByDefault(Return(&opponent1));
    ON_CALL(fakeWorld, GetNextObjectInLane(_, _, -1, DoubleNear(155, 1), true)).WillByDefault(Return(&opponent2));
    ON_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(_, _, -1, _, _)).WillByDefault(Return(1000.0));

    SpawnPoint* spawnPoint{nullptr};
    ON_CALL(fakeSpawnPoint, GetSpawnPoint()).WillByDefault(Return(spawnPoint));

    SpawnControl spawnControl(&fakeSpawnPoint, &fakeWorld, 100);

    ASSERT_EQ(spawnControl.CalculateHoldbackTime(&agentBlueprint), 3100);
}

TEST(CalculateHoldbackTime, TwoOpponentsBothSlow_HoldBackTime)
{
    NiceMock<FakeAgentBlueprint> agentBlueprint;
    SpawnParameter spawnParameter;
    spawnParameter.SpawningLaneId = -1;
    spawnParameter.distance = 103.0;
    spawnParameter.velocity = 30.0;
    spawnParameter.yawAngle = 0;
    spawnParameter.positionX = 100;
    spawnParameter.positionY = 0;

    VehicleModelParameters vehicleModelParameter;
    vehicleModelParameter.distanceReferencePointToLeadingEdge = 2.0;
    vehicleModelParameter.width = 2;
    vehicleModelParameter.length = 4;


    ON_CALL(agentBlueprint, GetSpawnParameter()).WillByDefault(ReturnRef(spawnParameter));
    ON_CALL(agentBlueprint, GetVehicleModelParameters()).WillByDefault(Return(vehicleModelParameter));

    NiceMock<FakeAgent> opponent1;
    ON_CALL(opponent1, GetVelocity()).WillByDefault(Return(25));
    ON_CALL(opponent1, GetDistanceToStartOfRoad(_)).WillByDefault(Return(170));
    ON_CALL(opponent1, GetLength()).WillByDefault(Return(4.0));
    ON_CALL(opponent1, GetWidth()).WillByDefault(Return(2.0));
    ON_CALL(opponent1, GetPositionX()).WillByDefault(Return(170.0));
    ON_CALL(opponent1, GetPositionY()).WillByDefault(Return(0.0));
    ON_CALL(opponent1, GetYaw()).WillByDefault(Return(0.0));

    NiceMock<FakeAgent> opponent2;
    ON_CALL(opponent2, GetVelocity()).WillByDefault(Return(10));
    ON_CALL(opponent2, GetDistanceToStartOfRoad(_)).WillByDefault(Return(175));
    ON_CALL(opponent2, GetLength()).WillByDefault(Return(4.0));
    ON_CALL(opponent2, GetWidth()).WillByDefault(Return(2.0));
    ON_CALL(opponent2, GetPositionX()).WillByDefault(Return(175.0));
    ON_CALL(opponent2, GetPositionY()).WillByDefault(Return(0.0));
    ON_CALL(opponent2, GetYaw()).WillByDefault(Return(0.0));

    NiceMock<FakeSpawnPointNetwork> fakeSpawnPoint;

    NiceMock<FakeWorld> fakeWorld;
    ON_CALL(fakeWorld, GetNextObjectInLane(_, _, -1, DoubleNear(101, 1), true)).WillByDefault(Return(&opponent1));
    ON_CALL(fakeWorld, GetNextObjectInLane(_, _, -1, DoubleNear(170, 1), true)).WillByDefault(Return(&opponent2));
    ON_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(_, _, -1, _, _)).WillByDefault(Return(1000.0));

    SpawnPoint* spawnPoint{nullptr};
    ON_CALL(fakeSpawnPoint, GetSpawnPoint()).WillByDefault(Return(spawnPoint));

    SpawnControl spawnControl(&fakeSpawnPoint, &fakeWorld, 100);

    ASSERT_EQ(spawnControl.CalculateHoldbackTime(&agentBlueprint), 3100);
}

TEST(CalculateHoldbackTime, OneOpponentNearAndStanding_ReturnsNegative)
{
    NiceMock<FakeAgentBlueprint> agentBlueprint;
    SpawnParameter spawnParameter;
    spawnParameter.SpawningLaneId = -1;
    spawnParameter.distance = 103.0;
    spawnParameter.velocity = 30.0;
    spawnParameter.yawAngle = 0;
    spawnParameter.positionX = 100;
    spawnParameter.positionY = 0;

    VehicleModelParameters vehicleModelParameter;
    vehicleModelParameter.distanceReferencePointToLeadingEdge = 2.0;
    vehicleModelParameter.width = 2;
    vehicleModelParameter.length = 4;


    ON_CALL(agentBlueprint, GetSpawnParameter()).WillByDefault(ReturnRef(spawnParameter));
    ON_CALL(agentBlueprint, GetVehicleModelParameters()).WillByDefault(Return(vehicleModelParameter));

    NiceMock<FakeAgent> opponent;
    ON_CALL(opponent, GetVelocity()).WillByDefault(Return(0));
    ON_CALL(opponent, GetDistanceToStartOfRoad(_)).WillByDefault(Return(195));
    ON_CALL(opponent, GetLength()).WillByDefault(Return(4.0));
    ON_CALL(opponent, GetWidth()).WillByDefault(Return(2.0));
    ON_CALL(opponent, GetPositionX()).WillByDefault(Return(195.0));
    ON_CALL(opponent, GetPositionY()).WillByDefault(Return(0.0));
    ON_CALL(opponent, GetYaw()).WillByDefault(Return(0.0));

    NiceMock<FakeSpawnPointNetwork> fakeSpawnPoint;

    NiceMock<FakeWorld> fakeWorld;
    ON_CALL(fakeWorld, GetNextObjectInLane(_, _, -1, DoubleNear(101, 1), true)).WillByDefault(Return(&opponent));
    ON_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(_, _, -1, _, _)).WillByDefault(Return(1000.0));

    SpawnPoint* spawnPoint{nullptr};
    ON_CALL(fakeSpawnPoint, GetSpawnPoint()).WillByDefault(Return(spawnPoint));

    SpawnControl spawnControl(&fakeSpawnPoint, &fakeWorld, 100);

    ASSERT_LT(spawnControl.CalculateHoldbackTime(&agentBlueprint), 0);
}
