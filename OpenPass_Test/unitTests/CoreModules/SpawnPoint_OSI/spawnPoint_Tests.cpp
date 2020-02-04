/*********************************************************************
* Copyright (c) 2018 2019 in-tech
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "SpawnPoint.h"

#include "fakeAgent.h"
#include "fakeAgentBlueprint.h"
#include "fakeSampler.h"
#include "fakeScenario.h"
#include "fakeWorld.h"
#include "fakeParameter.h"

using ::testing::_;
using ::testing::AllOf;
using ::testing::DoubleEq;
using ::testing::ElementsAre;
using ::testing::Field;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnRef;

TEST(SpawnPoint, GetEgoSpawnCarInfoWithValidStartPosition)
{
    LaneParameters laneParameters;
    laneParameters.laneId = -1;

    VehicleModelParameters fakeVehicleModelParameter;
    fakeVehicleModelParameter.length = 3.0;
    fakeVehicleModelParameter.distanceReferencePointToLeadingEdge = 2.0;

    NiceMock<FakeAgentBlueprint> fakeAgentBlueprint;
    ON_CALL(fakeAgentBlueprint, GetVehicleModelParameters()).WillByDefault(Return(fakeVehicleModelParameter));

    std::map<std::string, double> fakeDoubles = {{"TrafficVolume", 600.0}, {"PlatoonRate", 1.0}, {"Velocity", 30.0}, {"Homogenity", 1.0}};

    NiceMock<FakeParameter> fakeParameters;
    ON_CALL(fakeParameters, GetParametersDouble()).WillByDefault(ReturnRef(fakeDoubles));

    // SCENARIO
    ScenarioEntity scenarioEntity;
    SpawnInfo initialPreSimSpawnInfo(0.0, 305.0, 31.0, "", -1, 0, 0);
    scenarioEntity.spawnInfo = initialPreSimSpawnInfo;

    NiceMock<FakeScenario> fakeScenario;
    std::vector<ScenarioEntity*> emptyEntities;
    ON_CALL(fakeScenario, GetScenarioEntities()).WillByDefault(ReturnRef(emptyEntities));
    ON_CALL(fakeScenario, GetEgoEntity()).WillByDefault(ReturnRef(scenarioEntity));

    NiceMock<FakeSampler> fakeSampler;
    ON_CALL(fakeSampler, RollGapBetweenCars(_)).WillByDefault(Return(5.0));
    ON_CALL(fakeSampler, RollFor(_)).WillByDefault(Return(true));
    ON_CALL(fakeSampler, RollForVelocity(_, _)).WillByDefault(Return(30.0));

    NiceMock<FakeWorld> fakeWorld;
    std::vector<int> driveableLanes {-1};
    ON_CALL(fakeWorld, GetNumberOfLanes(_, _)).WillByDefault(Return(1));
    ON_CALL(fakeWorld, GetDrivingLanesAtDistance(_, _)).WillByDefault(Return(driveableLanes));
    ON_CALL(fakeWorld, IsSValidOnLane(_, _, _)).WillByDefault(Return(true));
    ON_CALL(fakeWorld, IntersectsWithAgent(_, _, _, _, _, _)).WillByDefault(Return(false));
    ON_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(_, _, _, _,_)).WillByDefault(Return(500.0));
    ON_CALL(fakeWorld, QueryLane(_, _, _)).WillByDefault(Return(LaneQueryResult::InvalidResult()));

    SpawnPoint spawnPoint(&fakeWorld,
                          &fakeParameters,
                          nullptr,
                          &fakeSampler,
                          &fakeScenario,
                          SpawnPointState::EgoPlacement);

    spawnPoint.ExtractSpawnPointParameters(&fakeParameters);
    spawnPoint.CalculateDerivedParameters();

    spawnPoint.GetNextSpawnCarInfo(laneParameters, &fakeAgentBlueprint);
    SpawnInfo resultSpawnInfo = spawnPoint.GetSpawnInfo();

    ASSERT_EQ(resultSpawnInfo.ILane, -1);
    ASSERT_EQ(resultSpawnInfo.s.value, 306.0);
    ASSERT_EQ(resultSpawnInfo.velocity.value, 31.0);
    ASSERT_EQ(resultSpawnInfo.TStart, 0);
}

TEST(SpawnPoint, GetEgoSpawnCarInfoWithInvalidStartPosition)
{
    LaneParameters laneParameters;
    laneParameters.laneId = -1;

    VehicleModelParameters fakeVehicleModelParameter;
    fakeVehicleModelParameter.length = 3;
    fakeVehicleModelParameter.distanceReferencePointToLeadingEdge = 2;

    NiceMock<FakeAgentBlueprint> fakeAgentBlueprint;
    ON_CALL(fakeAgentBlueprint, GetVehicleModelParameters()).WillByDefault(Return(fakeVehicleModelParameter));

    std::map<std::string, double> fakeDoubles = {{"TrafficVolume", 600.0}, {"PlatoonRate", 1.0}, {"Velocity", 30.0}, {"Homogenity", 1.0}};

    NiceMock<FakeParameter> fakeParameters;
    ON_CALL(fakeParameters, GetParametersDouble()).WillByDefault(ReturnRef(fakeDoubles));

    NiceMock<FakeSampler> fakeSampler;
    ON_CALL(fakeSampler, RollGapBetweenCars(_)).WillByDefault(Return(5.0));
    ON_CALL(fakeSampler, RollFor(_)).WillByDefault(Return(true));
    ON_CALL(fakeSampler, RollForVelocity(_, _)).WillByDefault(Return(30.0));

    // FAKE SCENARIO
    ScenarioEntity scenarioEntity;
    SpawnInfo initialPreSimSpawnInfo(0.0, -200.0, 30.0, "", -1, 0, 0);
    scenarioEntity.spawnInfo = initialPreSimSpawnInfo;

    NiceMock<FakeScenario> fakeScenario;
    std::vector<ScenarioEntity*> emptyEntities;
    ON_CALL(fakeScenario, GetScenarioEntities()).WillByDefault(ReturnRef(emptyEntities));
    ON_CALL(fakeScenario, GetEgoEntity()).WillByDefault(ReturnRef(scenarioEntity));

    NiceMock<FakeWorld> fakeWorld;
    std::vector<int> driveableLanes {-1};
    ON_CALL(fakeWorld, GetNumberOfLanes(_, _)).WillByDefault(Return(1));
    ON_CALL(fakeWorld, GetDrivingLanesAtDistance(_, _)).WillByDefault(Return(driveableLanes));
    ON_CALL(fakeWorld, IsSValidOnLane(_, _, 301.0)).WillByDefault(Return(false));
    ON_CALL(fakeWorld, IsSValidOnLane(_, _, 1.0)).WillByDefault(Return(true));
    ON_CALL(fakeWorld, IntersectsWithAgent(_, _, _, _, _, _)).WillByDefault(Return(false));
    ON_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(_, _, _, _,_)).WillByDefault(Return(500.0));
    ON_CALL(fakeWorld, QueryLane(_, _, _)).WillByDefault(Return(LaneQueryResult::InvalidResult()));

    SpawnPoint spawnPoint(&fakeWorld,
                          &fakeParameters,
                          nullptr,
                          &fakeSampler,
                          &fakeScenario,
                          SpawnPointState::EgoPlacement);

    spawnPoint.ExtractSpawnPointParameters(&fakeParameters);
    spawnPoint.CalculateDerivedParameters();

    spawnPoint.GetNextSpawnCarInfo(laneParameters, &fakeAgentBlueprint);
    SpawnInfo resultSpawnInfo = spawnPoint.GetSpawnInfo();

    ASSERT_EQ(resultSpawnInfo.ILane, -1);
    ASSERT_EQ(resultSpawnInfo.s.value, 1.0);
    ASSERT_EQ(resultSpawnInfo.velocity.value, 30.0);
    ASSERT_EQ(resultSpawnInfo.TStart, 0);
}

TEST(SpawnPoint, GetNextSpawnCarInfoFrontWithValidStartPosition)
{
    LaneParameters laneParameters;
    laneParameters.laneId = -1;

    VehicleModelParameters fakeVehicleModelParameter;
    fakeVehicleModelParameter.length = 3;
    fakeVehicleModelParameter.distanceReferencePointToLeadingEdge = 2;

    NiceMock<FakeAgentBlueprint> fakeAgentBlueprint;
    ON_CALL(fakeAgentBlueprint, GetVehicleModelParameters()).WillByDefault(Return(fakeVehicleModelParameter));

    std::map<std::string, double> fakeDoubles = {{"TrafficVolume", 600.0}, {"PlatoonRate", 1.0}, {"Velocity", 30.0}, {"Homogenity", 1.0}};

    NiceMock<FakeParameter> fakeParameters;
    ON_CALL(fakeParameters, GetParametersDouble()).WillByDefault(ReturnRef(fakeDoubles));

    NiceMock<FakeSampler> fakeSampler;
    ON_CALL(fakeSampler, RollGapBetweenCars(_)).WillByDefault(Return(5.0));
    ON_CALL(fakeSampler, RollFor(_)).WillByDefault(Return(true));
    ON_CALL(fakeSampler, RollForVelocity(_, _)).WillByDefault(Return(30.0));

    // FAKE SCENARIO
    ScenarioEntity scenarioEntity;
    SpawnInfo initialPreSimSpawnInfo(0.0, 425.0, 30.0, "", -1, 0, 0);
    scenarioEntity.spawnInfo = initialPreSimSpawnInfo;

    NiceMock<FakeScenario> fakeScenario;
    std::vector<ScenarioEntity*> emptyEntities;
    ON_CALL(fakeScenario, GetScenarioEntities()).WillByDefault(ReturnRef(emptyEntities));
    ON_CALL(fakeScenario, GetEgoEntity()).WillByDefault(ReturnRef(scenarioEntity));

    NiceMock<FakeAgent> fakeAgent;
    ON_CALL(fakeAgent, GetDistanceToStartOfRoad()).WillByDefault(Return(300));
    ON_CALL(fakeAgent, GetVelocity()).WillByDefault(Return(25.0));

    NiceMock<FakeWorld> fakeWorld;
    std::vector<int> driveableLanes {-1};
    ON_CALL(fakeWorld, GetNumberOfLanes(_, _)).WillByDefault(Return(1));
    ON_CALL(fakeWorld, GetDrivingLanesAtDistance(_, _)).WillByDefault(Return(driveableLanes));
    ON_CALL(fakeWorld, IsSValidOnLane(_, _, _)).WillByDefault(Return(true));
    ON_CALL(fakeWorld, IntersectsWithAgent(_, _, _, _, _, _)).WillByDefault(Return(false));
    ON_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(_, _, _, _,_)).WillByDefault(Return(500.0));
    ON_CALL(fakeWorld, QueryLane(_, _, _)).WillByDefault(Return(LaneQueryResult::InvalidResult()));
    EXPECT_CALL(fakeWorld, GetLastObjectInLane(_, _, _,_,_)).WillOnce(Return(&fakeAgent));


    SpawnPoint spawnPoint(&fakeWorld,
                          &fakeParameters,
                          nullptr,
                          &fakeSampler,
                          &fakeScenario,
                          SpawnPointState::CommonSpecificFrontPlacement);

    spawnPoint.ExtractSpawnPointParameters(&fakeParameters);
    spawnPoint.CalculateDerivedParameters();

    spawnPoint.GetNextSpawnCarInfo(laneParameters, &fakeAgentBlueprint);


    SpawnInfo resultSpawnInfo = spawnPoint.GetSpawnInfo();

    ASSERT_EQ(resultSpawnInfo.ILane, -1);
    ASSERT_EQ(resultSpawnInfo.s.value, 426.0);
    ASSERT_EQ(resultSpawnInfo.velocity.value, 30.0);
    ASSERT_EQ(resultSpawnInfo.TStart, 0);
}

TEST(SpawnPoint, GetNextSpawnCarInfoFrontWithInvalidStartPosition)
{
    LaneParameters laneParameters;
    laneParameters.laneId = -1;

    VehicleModelParameters fakeVehicleModelParameter;
    fakeVehicleModelParameter.length = 3;
    fakeVehicleModelParameter.distanceReferencePointToLeadingEdge = 2;

    NiceMock<FakeAgentBlueprint> fakeAgentBlueprint;
    ON_CALL(fakeAgentBlueprint, GetVehicleModelParameters()).WillByDefault(Return(fakeVehicleModelParameter));

    std::map<std::string, double> fakeDoubles = {{"TrafficVolume", 600.0}, {"PlatoonRate", 1.0}, {"Velocity", 30.0}, {"Homogenity", 10.0}};

    NiceMock<FakeParameter> fakeParameters;
    ON_CALL(fakeParameters, GetParametersDouble()).WillByDefault(ReturnRef(fakeDoubles));

    NiceMock<FakeSampler> fakeSampler;
    ON_CALL(fakeSampler, RollGapBetweenCars(_)).WillByDefault(Return(5.0));
    ON_CALL(fakeSampler, RollFor(_)).WillByDefault(Return(true));
    ON_CALL(fakeSampler, RollForVelocity(_, _)).WillByDefault(Return(30.0));

    ScenarioEntity scenarioEntity;
    SpawnInfo initialPreSimSpawnInfo(0.0, 302.0, 31.0, "", -1, 0, 0);
    scenarioEntity.spawnInfo = initialPreSimSpawnInfo;

    NiceMock<FakeScenario> fakeScenario;
    std::vector<ScenarioEntity*> emptyEntities;
    ON_CALL(fakeScenario, GetScenarioEntities()).WillByDefault(ReturnRef(emptyEntities));
    ON_CALL(fakeScenario, GetEgoEntity()).WillByDefault(ReturnRef(scenarioEntity));

    NiceMock<FakeAgent> fakeAgent;
    ON_CALL(fakeAgent, GetDistanceToStartOfRoad()).WillByDefault(Return(1300));
    ON_CALL(fakeAgent, GetVelocity()).WillByDefault(Return(25.0));

    NiceMock<FakeWorld> fakeWorld;
    std::vector<int> driveableLanes {-1};
    ON_CALL(fakeWorld, GetNumberOfLanes(_, _)).WillByDefault(Return(1));
    ON_CALL(fakeWorld, GetDrivingLanesAtDistance(_, _)).WillByDefault(Return(driveableLanes));
    ON_CALL(fakeWorld, IsSValidOnLane(_, _, _)).WillByDefault(Return(true));
    ON_CALL(fakeWorld, IntersectsWithAgent(_, _, _, _, _, _)).WillByDefault(Return(false));
    ON_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(_, _, _, _,_)).WillByDefault(Return(500.0));
    ON_CALL(fakeWorld, QueryLane(_, _, _)).WillByDefault(Return(LaneQueryResult::InvalidResult()));
    ON_CALL(fakeWorld, GetLastObjectInLane(_, _, _,_, true)).WillByDefault(Return(&fakeAgent));

    SpawnPoint spawnPoint(&fakeWorld,
                          &fakeParameters,
                          nullptr,
                          &fakeSampler,
                          &fakeScenario,
                          SpawnPointState::CommonSpecificFrontPlacement);

    spawnPoint.ExtractSpawnPointParameters(&fakeParameters);
    spawnPoint.CalculateDerivedParameters();

    ASSERT_FALSE(spawnPoint.GetNextSpawnCarInfo(laneParameters, &fakeAgentBlueprint));
}

TEST(SpawnPoint, GetNextSpawnCarInfoRearWithValidStartPosition)
{
    LaneParameters laneParameters;
    laneParameters.laneId = -1;

    VehicleModelParameters fakeVehicleModelParameter;
    fakeVehicleModelParameter.length = 3.0;
    fakeVehicleModelParameter.distanceReferencePointToLeadingEdge = 2.0;

    NiceMock<FakeAgentBlueprint> fakeAgentBlueprint;
    ON_CALL(fakeAgentBlueprint, GetVehicleModelParameters()).WillByDefault(Return(fakeVehicleModelParameter));

    std::map<std::string, double> fakeDoubles = {{"TrafficVolume", 600.0}, {"PlatoonRate", 1.0}, {"Velocity", 30.0}, {"Homogenity", 1.0}};

    NiceMock<FakeParameter> fakeParameters;
    ON_CALL(fakeParameters, GetParametersDouble()).WillByDefault(ReturnRef(fakeDoubles));

    NiceMock<FakeSampler> fakeSampler;
    ON_CALL(fakeSampler, RollGapBetweenCars(_)).WillByDefault(Return(5.0));
    ON_CALL(fakeSampler, RollFor(_)).WillByDefault(Return(true));
    ON_CALL(fakeSampler, RollForVelocity(_, _)).WillByDefault(Return(30.0));

    ScenarioEntity scenarioEntity;
    SpawnInfo initialPreSimSpawnInfo(0.0, 146.0, 30.0, "", -1, 0, 0);
    scenarioEntity.spawnInfo = initialPreSimSpawnInfo;

    NiceMock<FakeScenario> fakeScenario;
    std::vector<ScenarioEntity*> emptyEntities;
    ON_CALL(fakeScenario, GetScenarioEntities()).WillByDefault(ReturnRef(emptyEntities));
    ON_CALL(fakeScenario, GetEgoEntity()).WillByDefault(ReturnRef(scenarioEntity));

    NiceMock<FakeAgent> fakeAgent;
    ON_CALL(fakeAgent, GetDistanceToStartOfRoad()).WillByDefault(Return(300.0));
    ON_CALL(fakeAgent, GetVelocity()).WillByDefault(Return(40.0));
    ON_CALL(fakeAgent, GetLength()).WillByDefault(Return(3.0));
    ON_CALL(fakeAgent, GetDistanceReferencePointToLeadingEdge()).WillByDefault(Return(2.0));

    NiceMock<FakeWorld> fakeWorld;
    std::vector<int> driveableLanes {-1};
    ON_CALL(fakeWorld, GetNumberOfLanes(_, _)).WillByDefault(Return(1));
    ON_CALL(fakeWorld, GetDrivingLanesAtDistance(_, _)).WillByDefault(Return(driveableLanes));
    ON_CALL(fakeWorld, IsSValidOnLane(_, _, _)).WillByDefault(Return(true));
    ON_CALL(fakeWorld, IntersectsWithAgent(_, _, _, _, _, _)).WillByDefault(Return(false));
    ON_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(_, _, _, _,_)).WillByDefault(Return(500.0));
    ON_CALL(fakeWorld, GetLastObjectInLane(_, _, _, _, false)).WillByDefault(Return(&fakeAgent));
    ON_CALL(fakeWorld, QueryLane(_, _, _)).WillByDefault(Return(LaneQueryResult::InvalidResult()));

    SpawnPoint spawnPoint(&fakeWorld,
                          &fakeParameters,
                          nullptr,
                          &fakeSampler,
                          &fakeScenario,
                          SpawnPointState::CommonSpecificRearPlacement);

    spawnPoint.ExtractSpawnPointParameters(&fakeParameters);
    spawnPoint.CalculateDerivedParameters();

    spawnPoint.GetNextSpawnCarInfo(laneParameters, &fakeAgentBlueprint);

    SpawnInfo resultSpawnInfo = spawnPoint.GetSpawnInfo();

    ASSERT_EQ(resultSpawnInfo.ILane, -1);
    ASSERT_EQ(resultSpawnInfo.s.value, 147.0);
    ASSERT_EQ(resultSpawnInfo.velocity.value, 30.0);
    ASSERT_EQ(resultSpawnInfo.TStart, 0.0);
}

TEST(SpawnPoint, GetScenerySpawnCarInfoWithValidStartPosition)
{
    LaneParameters laneParameters;
    laneParameters.laneId = -1;

    VehicleModelParameters fakeVehicleModelParameter;
    fakeVehicleModelParameter.length = 3.0;
    fakeVehicleModelParameter.distanceReferencePointToLeadingEdge = 2.0;

    NiceMock<FakeAgentBlueprint> fakeAgentBlueprint;
    ON_CALL(fakeAgentBlueprint, GetVehicleModelParameters()).WillByDefault(Return(fakeVehicleModelParameter));

    std::map<std::string, double> fakeDoubles = {{"TrafficVolume", 600.0}, {"PlatoonRate", 1.0}, {"Velocity", 30.0}, {"Homogenity", 1.0}};

    NiceMock<FakeParameter> fakeParameters;
    ON_CALL(fakeParameters, GetParametersDouble()).WillByDefault(ReturnRef(fakeDoubles));

    ScenarioEntity scenarioEntity;
    SpawnInfo initialPreSimSpawnInfo(0.0, 404.0, 25.0, "", -1, 0, 0);
    std::vector<ScenarioEntity*> sceneryEntities;

    scenarioEntity.spawnInfo = initialPreSimSpawnInfo;
    sceneryEntities.push_back(&scenarioEntity);

    NiceMock<FakeScenario> fakeScenario;
    ON_CALL(fakeScenario, GetScenarioEntities()).WillByDefault(ReturnRef(sceneryEntities));

    ScenarioEntity emptyEntity;
    ON_CALL(fakeScenario, GetEgoEntity()).WillByDefault(ReturnRef(emptyEntity));


    NiceMock<FakeSampler> fakeSampler;
    ON_CALL(fakeSampler, RollGapBetweenCars(_)).WillByDefault(Return(5.0));
    ON_CALL(fakeSampler, RollFor(_)).WillByDefault(Return(true));
    ON_CALL(fakeSampler, RollForVelocity(_, _)).WillByDefault(Return(30.0));

    NiceMock<FakeWorld> fakeWorld;
    std::vector<int> driveableLanes {-1};
    ON_CALL(fakeWorld, GetNumberOfLanes(_, _)).WillByDefault(Return(1));
    ON_CALL(fakeWorld, GetDrivingLanesAtDistance(_, _)).WillByDefault(Return(driveableLanes));
    ON_CALL(fakeWorld, IsSValidOnLane(_, _, _)).WillByDefault(Return(true));
    ON_CALL(fakeWorld, IntersectsWithAgent(_, _, _, _, _, _)).WillByDefault(Return(false));
    ON_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(_, _, _, _,_)).WillByDefault(Return(500.0));
    ON_CALL(fakeWorld, QueryLane(_, _, _)).WillByDefault(Return(LaneQueryResult::InvalidResult()));

    SpawnPoint spawnPoint(&fakeWorld,
                          &fakeParameters,
                          nullptr,
                          &fakeSampler,
                          &fakeScenario,
                          SpawnPointState::SceneryPlacement);

    spawnPoint.ExtractSpawnPointParameters(&fakeParameters);
    spawnPoint.CalculateDerivedParameters();

    spawnPoint.GetNextSpawnCarInfo(laneParameters, &fakeAgentBlueprint);

    SpawnInfo resultSpawnInfo = spawnPoint.GetSpawnInfo();

    ASSERT_EQ(resultSpawnInfo.ILane, -1);
    ASSERT_EQ(resultSpawnInfo.s.value, 405.0);
    ASSERT_EQ(resultSpawnInfo.velocity.value, 25.0);
    ASSERT_EQ(resultSpawnInfo.TStart, 0);
}

TEST(SpawnPoint, GetScenerySpawnCarInfo_IntersectsExistingAgent)
{
    LaneParameters laneParameters;
    laneParameters.laneId = -1;

    VehicleModelParameters fakeVehicleModelParameter;
    fakeVehicleModelParameter.length = 3.0;
    fakeVehicleModelParameter.distanceReferencePointToLeadingEdge = 2.0;

    NiceMock<FakeAgentBlueprint> fakeAgentBlueprint;
    ON_CALL(fakeAgentBlueprint, GetVehicleModelParameters()).WillByDefault(Return(fakeVehicleModelParameter));

    std::map<std::string, double> fakeDoubles = {{"TrafficVolume", 600.0}, {"PlatoonRate", 1.0}, {"Velocity", 30.0}, {"Homogenity", 1.0}};

    NiceMock<FakeParameter> fakeParameters;
    ON_CALL(fakeParameters, GetParametersDouble()).WillByDefault(ReturnRef(fakeDoubles));

    ScenarioEntity scenarioEntity;
    SpawnInfo initialPreSimSpawnInfo(0.0, 404.0, 25.0, "", -1, 0, 0);
    std::vector<ScenarioEntity*> sceneryEntities;

    scenarioEntity.spawnInfo = initialPreSimSpawnInfo;
    sceneryEntities.push_back(&scenarioEntity);

    NiceMock<FakeScenario> fakeScenario;
    ON_CALL(fakeScenario, GetScenarioEntities()).WillByDefault(ReturnRef(sceneryEntities));

    ScenarioEntity emptyEntity;
    ON_CALL(fakeScenario, GetEgoEntity()).WillByDefault(ReturnRef(emptyEntity));

    NiceMock<FakeSampler> fakeSampler;
    ON_CALL(fakeSampler, RollGapBetweenCars(_)).WillByDefault(Return(5.0));
    ON_CALL(fakeSampler, RollFor(_)).WillByDefault(Return(true));
    ON_CALL(fakeSampler, RollForVelocity(_, _)).WillByDefault(Return(30.0));

    NiceMock<FakeWorld> fakeWorld;
    std::vector<int> driveableLanes {-1};
    ON_CALL(fakeWorld, GetNumberOfLanes(_, _)).WillByDefault(Return(1));
    ON_CALL(fakeWorld, GetDrivingLanesAtDistance(_, _)).WillByDefault(Return(driveableLanes));
    ON_CALL(fakeWorld, IsSValidOnLane(_, _, _)).WillByDefault(Return(true));
    ON_CALL(fakeWorld, IntersectsWithAgent(_, _, _, _, _, _)).WillByDefault(Return(true));
    ON_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(_, _, _, _,_)).WillByDefault(Return(500.0));
    ON_CALL(fakeWorld, QueryLane(_, _, _)).WillByDefault(Return(LaneQueryResult::InvalidResult()));

    SpawnPoint spawnPoint(&fakeWorld,
                          &fakeParameters,
                          nullptr,
                          &fakeSampler,
                          &fakeScenario,
                          SpawnPointState::SceneryPlacement);

    spawnPoint.ExtractSpawnPointParameters(&fakeParameters);
    spawnPoint.CalculateDerivedParameters();

    ASSERT_THROW(spawnPoint.GetNextSpawnCarInfo(laneParameters, &fakeAgentBlueprint), std::logic_error);
}

TEST(SpawnPoint, CalculateSpawnParameterWritesCorrectlyIntoBlueprint)
{
    LaneParameters laneParameters;
    laneParameters.laneId = -1;
    laneParameters.distanceToLaneStart = 500.0;
    laneParameters.offset = 0.5;

    SpawnInfo spawnInfo {0.5, 1500.0, 30.0, "", -1, 0.2, 1.1, 0.3};

    NiceMock<FakeAgentBlueprint> fakeAgentBlueprint;
    SpawnParameter spawnParameter;
    ON_CALL(fakeAgentBlueprint, GetSpawnParameter()).WillByDefault(ReturnRef(spawnParameter));

    std::map<std::string, double> fakeDoubles = {{"TrafficVolume", 600.0}, {"PlatoonRate", 1.0}, {"Velocity", 30.0}, {"Homogenity", 1.0}};

    NiceMock<FakeParameter> fakeParameters;
    ON_CALL(fakeParameters, GetParametersDouble()).WillByDefault(ReturnRef(fakeDoubles));

    NiceMock<FakeScenario> fakeScenario;
    std::vector<ScenarioEntity*> emptyEntities;
    ON_CALL(fakeScenario, GetScenarioEntities()).WillByDefault(ReturnRef(emptyEntities));

    ScenarioEntity scenarioEntity;
    ON_CALL(fakeScenario, GetEgoEntity()).WillByDefault(ReturnRef(scenarioEntity));

    NiceMock<FakeSampler> fakeSampler;
    NiceMock<FakeWorld> fakeWorld;

    Position egoPosition {1.0, 2.0, 0.4, -0.1};
    EXPECT_CALL(fakeWorld, LaneCoord2WorldCoord(DoubleEq(1000.0), DoubleEq(0.5), _, -1)).WillOnce(Return(egoPosition));
    ON_CALL(fakeWorld, QueryLane(_, _, _)).WillByDefault(Return(LaneQueryResult::InvalidResult()));

    SpawnPoint spawnPoint(&fakeWorld,
                          &fakeParameters,
                          nullptr,
                          &fakeSampler,
                          &fakeScenario,
                          SpawnPointState::EgoPlacement);

    spawnPoint.SetSpawnInfo(spawnInfo);

    ASSERT_TRUE(spawnPoint.CalculateSpawnParameter(&fakeAgentBlueprint, laneParameters));
    ASSERT_EQ(spawnParameter.SpawningLaneId, -1);
    ASSERT_DOUBLE_EQ(spawnParameter.distance, 1500.0);
    ASSERT_DOUBLE_EQ(spawnParameter.positionX, 1.0);
    ASSERT_DOUBLE_EQ(spawnParameter.positionY, 2.0);
    ASSERT_DOUBLE_EQ(spawnParameter.velocity, 30.0);
    ASSERT_DOUBLE_EQ(spawnParameter.acceleration, 1.1);
    ASSERT_DOUBLE_EQ(spawnParameter.yawAngle, 0.4);
}

TEST(SpawnPoint, IsOffsetValid_LaneNotWideEnough)
{
    const double distFromStart = 0.0;
    const double offset = 0.0;
    const double carWidth = 21.0;
    const double laneWidth = 0.0;

    // FAKE SCENARIO
    ScenarioEntity scenarioEntity;
    SpawnInfo initialPreSimSpawnInfo(0.0, 0.0, 0.0, "", -1, 0, 0);
    scenarioEntity.spawnInfo = initialPreSimSpawnInfo;

    NiceMock<FakeScenario> fakeScenario;
    std::vector<ScenarioEntity*> emptyEntities;
    ON_CALL(fakeScenario, GetScenarioEntities()).WillByDefault(ReturnRef(emptyEntities));
    ON_CALL(fakeScenario, GetEgoEntity()).WillByDefault(ReturnRef(scenarioEntity));

    NiceMock<FakeWorld> fakeWorld;
    EXPECT_CALL(fakeWorld, IsSValidOnLane(_, -1, distFromStart)).WillOnce(Return(true));
    ON_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(_, _, _, _,_)).WillByDefault(Return(500.0));
    ON_CALL(fakeWorld, QueryLane(_, _, _)).WillByDefault(Return(LaneQueryResult::InvalidResult()));
    ON_CALL(fakeWorld, GetLaneWidth(_,_, -1,_,_)).WillByDefault(Return(laneWidth));

    SpawnPoint spawnPoint(&fakeWorld,
                          nullptr,
                          nullptr,
                          nullptr,
                          &fakeScenario,
                          SpawnPointState::SceneryPlacement);

    ASSERT_FALSE(spawnPoint.IsOffsetValidForLane("", -1, distFromStart, offset, carWidth));
}

TEST(SpawnPoint, IsOffsetValid_CompletelyInLane)
{
    const double distFromStart = 0.0;
    const double offset = 4.9;
    const double carWidth = 10.0;
    const double laneWidth = 20.0;

    // FAKE SCENARIO
    ScenarioEntity scenarioEntity;
    SpawnInfo initialPreSimSpawnInfo(0.0, 0.0, 0.0, "", -1, 0, 0);
    scenarioEntity.spawnInfo = initialPreSimSpawnInfo;

    NiceMock<FakeScenario> fakeScenario;
    std::vector<ScenarioEntity*> emptyEntities;
    ON_CALL(fakeScenario, GetScenarioEntities()).WillByDefault(ReturnRef(emptyEntities));
    ON_CALL(fakeScenario, GetEgoEntity()).WillByDefault(ReturnRef(scenarioEntity));

    NiceMock<FakeWorld> fakeWorld;
    EXPECT_CALL(fakeWorld, IsSValidOnLane(_, -1, distFromStart)).WillOnce(Return(true));
    ON_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(_, _, _, _,_)).WillByDefault(Return(500.0));
    ON_CALL(fakeWorld, QueryLane(_, _, _)).WillByDefault(Return(LaneQueryResult::InvalidResult()));
    ON_CALL(fakeWorld, GetLaneWidth(_, _, -1, _, _)).WillByDefault(Return(laneWidth));

    SpawnPoint spawnPoint(&fakeWorld,
                          nullptr,
                          nullptr,
                          nullptr,
                          &fakeScenario,
                          SpawnPointState::SceneryPlacement);

    ASSERT_TRUE(spawnPoint.IsOffsetValidForLane("", -1, distFromStart, offset, carWidth));
}

TEST(SpawnPoint, IsOffsetValid_OutsideOfAllowedRange)
{
    const double distFromStart = 0.0;
    const double offset = 11.0;
    const double carWidth = 10.0;
    const double laneWidth = 20.0;

    // FAKE SCENARIO
    ScenarioEntity scenarioEntity;
    SpawnInfo initialPreSimSpawnInfo(0.0, 0.0, 0.0, "", -1, 0, 0);
    scenarioEntity.spawnInfo = initialPreSimSpawnInfo;

    NiceMock<FakeScenario> fakeScenario;
    std::vector<ScenarioEntity*> emptyEntities;
    ON_CALL(fakeScenario, GetScenarioEntities()).WillByDefault(ReturnRef(emptyEntities));
    ON_CALL(fakeScenario, GetEgoEntity()).WillByDefault(ReturnRef(scenarioEntity));

    NiceMock<FakeWorld> fakeWorld;
    EXPECT_CALL(fakeWorld, IsSValidOnLane(_, -1, distFromStart)).WillOnce(Return(true));
    ON_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(_, _, _, _, _)).WillByDefault(Return(500.0));
    ON_CALL(fakeWorld, QueryLane(_, _, _)).WillByDefault(Return(LaneQueryResult::InvalidResult()));
    ON_CALL(fakeWorld, GetLaneWidth(_, _, -1, _, _)).WillByDefault(Return(laneWidth));

    SpawnPoint spawnPoint(&fakeWorld,
                          nullptr,
                          nullptr,
                          nullptr,
                          &fakeScenario,
                          SpawnPointState::SceneryPlacement);

    ASSERT_FALSE(spawnPoint.IsOffsetValidForLane("", -1, distFromStart, offset, carWidth));
}

TEST(SpawnPoint, IsOffsetValid_OtherLaneIsInvalid)
{
    const double distFromStart = 0.0;
    const double offset = -7.0;
    const double carWidth = 10.0;
    const double laneWidth = 20.0;
    const int mainLane = -1;
    const int otherLane = -2;

    // FAKE SCENARIO
    ScenarioEntity scenarioEntity;
    SpawnInfo initialPreSimSpawnInfo(0.0, 0.0, 0.0, "", -1, 0, 0);
    scenarioEntity.spawnInfo = initialPreSimSpawnInfo;

    NiceMock<FakeScenario> fakeScenario;
    std::vector<ScenarioEntity*> emptyEntities;
    ON_CALL(fakeScenario, GetScenarioEntities()).WillByDefault(ReturnRef(emptyEntities));
    ON_CALL(fakeScenario, GetEgoEntity()).WillByDefault(ReturnRef(scenarioEntity));

    NiceMock<FakeWorld> fakeWorld;
    EXPECT_CALL(fakeWorld, IsSValidOnLane(_, mainLane, distFromStart)).WillOnce(Return(true));
    EXPECT_CALL(fakeWorld, IsSValidOnLane(_, otherLane, distFromStart)).WillOnce(Return(false));

    ON_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(_, _, _, _, _)).WillByDefault(Return(500.0));
    ON_CALL(fakeWorld, QueryLane(_, _, _)).WillByDefault(Return(LaneQueryResult::InvalidResult()));
    ON_CALL(fakeWorld, GetLaneWidth(_, _, _, _, _)).WillByDefault(Return(laneWidth));

    SpawnPoint spawnPoint(&fakeWorld,
                          nullptr,
                          nullptr,
                          nullptr,
                          &fakeScenario,
                          SpawnPointState::SceneryPlacement);
    bool isValid = spawnPoint.IsOffsetValidForLane("", mainLane, distFromStart, offset, carWidth);

    ASSERT_FALSE(isValid);
}

TEST(SpawnPoint, IsOffsetValid_OK)
{
    const double distFromStart = 0.0;
    const double offset = -7.0;
    const double carWidth = 10.0;
    const double laneWidth = 20.0;
    const int mainLane = -1;
    const int otherLane = -2;

    // FAKE SCENARIO
    ScenarioEntity scenarioEntity;
    SpawnInfo initialPreSimSpawnInfo(0.0, 0.0, 0.0, "", -1, 0, 0);
    scenarioEntity.spawnInfo = initialPreSimSpawnInfo;

    NiceMock<FakeScenario> fakeScenario;
    std::vector<ScenarioEntity*> emptyEntities;
    ON_CALL(fakeScenario, GetScenarioEntities()).WillByDefault(ReturnRef(emptyEntities));
    ON_CALL(fakeScenario, GetEgoEntity()).WillByDefault(ReturnRef(scenarioEntity));

    NiceMock<FakeWorld> fakeWorld;
    EXPECT_CALL(fakeWorld, IsSValidOnLane(_, mainLane, distFromStart)).WillOnce(Return(true));
    EXPECT_CALL(fakeWorld, IsSValidOnLane(_, otherLane, distFromStart)).WillOnce(Return(true));

    ON_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(_, _, _, _, _)).WillByDefault(Return(500.0));
    ON_CALL(fakeWorld, QueryLane(_, _, _)).WillByDefault(Return(LaneQueryResult::InvalidResult()));
    ON_CALL(fakeWorld, GetLaneWidth(_, _, _, _, _)).WillByDefault(Return(laneWidth));

    SpawnPoint spawnPoint(&fakeWorld,
                          nullptr,
                          nullptr,
                          nullptr,
                          &fakeScenario,
                          SpawnPointState::SceneryPlacement);

    ASSERT_TRUE(spawnPoint.IsOffsetValidForLane("", -1, distFromStart, offset, carWidth));
}

TEST(SpawnPoint, CalculateMeanVelocitiesThreeDriveableLanesHomogenityPointFive)
{
    ScenarioEntity scenarioEntity;
    SpawnInfo initialPreSimSpawnInfo(0.0, -200.0, 30.0, "", -1, 0, 0);
    scenarioEntity.spawnInfo = initialPreSimSpawnInfo;

    NiceMock<FakeScenario> fakeScenario;
    std::vector<ScenarioEntity*> emptyEntities;
    ON_CALL(fakeScenario, GetScenarioEntities()).WillByDefault(ReturnRef(emptyEntities));
    ON_CALL(fakeScenario, GetEgoEntity()).WillByDefault(ReturnRef(scenarioEntity));

    NiceMock<FakeWorld> fakeWorld;
    std::vector<int> driveableLanes { { -1, -2, -3} };
    ON_CALL(fakeWorld, GetNumberOfLanes(_, _)).WillByDefault(Return(3));
    ON_CALL(fakeWorld, GetDrivingLanesAtDistance(_, _)).WillByDefault(Return(driveableLanes));
    ON_CALL(fakeWorld, QueryLane(_, _, _)).WillByDefault(Return(LaneQueryResult::InvalidResult()));


    NiceMock<FakeParameter> fakeParameter;
    std::map<std::string, double> doubleParameters {{"TrafficVolume", 600.0}, {"PlatoonRate", 1.0}, {"Velocity", 10.0}, {"Homogenity", 0.5}};
    ON_CALL(fakeParameter, GetParametersDouble()).WillByDefault(ReturnRef(doubleParameters));

    SpawnPoint spawnPoint(&fakeWorld,
                          &fakeParameter,
                          nullptr,
                          nullptr,
                          nullptr,
                          &fakeScenario);

    EXPECT_THAT(spawnPoint.CalculateMeanVelocities("", 150), ElementsAre(22.5, 15.0, 10.0));
}

TEST(SpawnPoint, CalculateMeanVelocitiesThreeDriveableLanesHomogenityZero)
{
    ScenarioEntity scenarioEntity;
    SpawnInfo initialPreSimSpawnInfo(0.0, -200.0, 30.0, "", -1, 0, 0);
    scenarioEntity.spawnInfo = initialPreSimSpawnInfo;

    NiceMock<FakeScenario> fakeScenario;
    std::vector<ScenarioEntity*> emptyEntities;
    ON_CALL(fakeScenario, GetScenarioEntities()).WillByDefault(ReturnRef(emptyEntities));
    ON_CALL(fakeScenario, GetEgoEntity()).WillByDefault(ReturnRef(scenarioEntity));

    NiceMock<FakeWorld> fakeWorld;
    std::vector<int> driveableLanes { { -1, -2, -3} };
    ON_CALL(fakeWorld, GetNumberOfLanes(_, _)).WillByDefault(Return(3));
    ON_CALL(fakeWorld, GetDrivingLanesAtDistance(_, _)).WillByDefault(Return(driveableLanes));
    ON_CALL(fakeWorld, QueryLane(_, _, _)).WillByDefault(Return(LaneQueryResult::InvalidResult()));


    NiceMock<FakeParameter> fakeParameter;
    std::map<std::string, double> doubleParameters {{"TrafficVolume", 600.0}, {"PlatoonRate", 1.0}, {"Velocity", 10.0}, {"Homogenity", 0.0}};
    ON_CALL(fakeParameter, GetParametersDouble()).WillByDefault(ReturnRef(doubleParameters));

    SpawnPoint spawnPoint(&fakeWorld,
                          &fakeParameter,
                          nullptr,
                          nullptr,
                          nullptr,
                          &fakeScenario);

    EXPECT_THAT(spawnPoint.CalculateMeanVelocities("", 150), ElementsAre(40.0, 20.0, 10.0));
}

TEST(SpawnPoint, CalculateMeanVelocitiesThreeDriveableLanesHomogenityOne)
{
    ScenarioEntity scenarioEntity;
    SpawnInfo initialPreSimSpawnInfo(0.0, -200.0, 30.0,"", -1, 0, 0);
    scenarioEntity.spawnInfo = initialPreSimSpawnInfo;

    NiceMock<FakeScenario> fakeScenario;
    std::vector<ScenarioEntity*> emptyEntities;
    ON_CALL(fakeScenario, GetScenarioEntities()).WillByDefault(ReturnRef(emptyEntities));
    ON_CALL(fakeScenario, GetEgoEntity()).WillByDefault(ReturnRef(scenarioEntity));

    NiceMock<FakeWorld> fakeWorld;
    std::vector<int> driveableLanes { { -1, -2, -3} };
    ON_CALL(fakeWorld, GetNumberOfLanes(_, _)).WillByDefault(Return(3));
    ON_CALL(fakeWorld, GetDrivingLanesAtDistance(_, _)).WillByDefault(Return(driveableLanes));
    ON_CALL(fakeWorld, QueryLane(_, _, _)).WillByDefault(Return(LaneQueryResult::InvalidResult()));


    NiceMock<FakeParameter> fakeParameter;
    std::map<std::string, double> doubleParameters {{"TrafficVolume", 600.0}, {"PlatoonRate", 1.0}, {"Velocity", 10.0}, {"Homogenity", 1.0}};
    ON_CALL(fakeParameter, GetParametersDouble()).WillByDefault(ReturnRef(doubleParameters));

    SpawnPoint spawnPoint(&fakeWorld,
                          &fakeParameter,
                          nullptr,
                          nullptr,
                          nullptr,
                          &fakeScenario);

    EXPECT_THAT(spawnPoint.CalculateMeanVelocities("", 150), ElementsAre(10.0, 10.0, 10.0));
}

TEST(SpawnPoint, CalculateMeanVelocitiesWithNondriveabelLane)
{
    ScenarioEntity scenarioEntity;
    SpawnInfo initialPreSimSpawnInfo(0.0, -200.0, 30.0,"", -1, 0, 0);
    scenarioEntity.spawnInfo = initialPreSimSpawnInfo;

    NiceMock<FakeScenario> fakeScenario;
    std::vector<ScenarioEntity*> emptyEntities;
    ON_CALL(fakeScenario, GetScenarioEntities()).WillByDefault(ReturnRef(emptyEntities));
    ON_CALL(fakeScenario, GetEgoEntity()).WillByDefault(ReturnRef(scenarioEntity));

    NiceMock<FakeWorld> fakeWorld;
    std::vector<int> driveableLanes { { -2, -3, -5} };
    ON_CALL(fakeWorld, GetNumberOfLanes(_, _)).WillByDefault(Return(7));
    ON_CALL(fakeWorld, GetDrivingLanesAtDistance(_, _)).WillByDefault(Return(driveableLanes));
    ON_CALL(fakeWorld, QueryLane(_, _, _)).WillByDefault(Return(LaneQueryResult::InvalidResult()));


    NiceMock<FakeParameter> fakeParameter;
    std::map<std::string, double> doubleParameters {{"TrafficVolume", 600.0}, {"PlatoonRate", 1.0}, {"Velocity", 10.0}, {"Homogenity", 0.5}};
    ON_CALL(fakeParameter, GetParametersDouble()).WillByDefault(ReturnRef(doubleParameters));

    SpawnPoint spawnPoint(&fakeWorld,
                          &fakeParameter,
                          nullptr,
                          nullptr,
                          nullptr,
                          &fakeScenario);

    EXPECT_THAT(spawnPoint.CalculateMeanVelocities("", 150), ElementsAre(22.5, 22.5, 15.0, 15.0, 10.0, 10.0, 10.0));
}

TEST(SpawnPoint, SetPredefinedParameterForEgo)
{
    ScenarioEntity scenarioEntity;
    scenarioEntity.name = "NameOfEgo";
    scenarioEntity.spawnInfo.s.value = 123.0;
    scenarioEntity.spawnInfo.ILane = -1;
    scenarioEntity.spawnInfo.velocity.value = 12.0;
    scenarioEntity.spawnInfo.heading = 0.3;

    NiceMock<FakeScenario> fakeScenario;
    std::vector<ScenarioEntity*> emptyEntities;
    ON_CALL(fakeScenario, GetScenarioEntities()).WillByDefault(ReturnRef(emptyEntities));
    ON_CALL(fakeScenario, GetEgoEntity()).WillByDefault(ReturnRef(scenarioEntity));

    NiceMock<FakeWorld> fakeWorld;
    ON_CALL(fakeWorld, QueryLane(_, _, _)).WillByDefault(Return(LaneQueryResult::InvalidResult()));

    SpawnParameter spawnParameter;
    NiceMock<FakeAgentBlueprint> fakeAgentBlueprint;
    ON_CALL(fakeAgentBlueprint, GetSpawnParameter()).WillByDefault(ReturnRef(spawnParameter));
    EXPECT_CALL(fakeAgentBlueprint, SetAgentCategory(AgentCategory::Ego));
    EXPECT_CALL(fakeAgentBlueprint, SetObjectName(scenarioEntity.name));
    EXPECT_CALL(fakeAgentBlueprint, SetSpawnParameter(AllOf(Field(&SpawnParameter::SpawningLaneId, -1),
                                                            Field(&SpawnParameter::positionX, DoubleEq(123.0)),
                                                            Field(&SpawnParameter::heading, DoubleEq(0.3)),
                                                            Field(&SpawnParameter::velocity, DoubleEq(12.0)))));

    SpawnPoint spawnPoint(&fakeWorld,
                          nullptr,
                          nullptr,
                          nullptr,
                          &fakeScenario,
                          SpawnPointState::EgoPlacement);

    spawnPoint.SetPredefinedParameter(&fakeAgentBlueprint);
}

TEST(SpawnPoint, SetPredefinedParameterForScenery)
{
    ScenarioEntity scenarioEntity;
    scenarioEntity.name = "NameOfScenery";
    scenarioEntity.spawnInfo.s.value = 123.0;
    scenarioEntity.spawnInfo.ILane = -1;
    scenarioEntity.spawnInfo.velocity.value = 12.0;
    scenarioEntity.spawnInfo.heading = 0.3;

    NiceMock<FakeScenario> fakeScenario;
    std::vector<ScenarioEntity*> scenarioEntityVector { {&scenarioEntity} };
    ON_CALL(fakeScenario, GetScenarioEntities()).WillByDefault(ReturnRef(scenarioEntityVector));

    ScenarioEntity egoEntity;
    ON_CALL(fakeScenario, GetEgoEntity()).WillByDefault(ReturnRef(egoEntity));

    NiceMock<FakeWorld> fakeWorld;
    ON_CALL(fakeWorld, QueryLane(_, _, _)).WillByDefault(Return(LaneQueryResult::InvalidResult()));

    SpawnParameter spawnParameter;
    NiceMock<FakeAgentBlueprint> fakeAgentBlueprint;
    ON_CALL(fakeAgentBlueprint, GetSpawnParameter()).WillByDefault(ReturnRef(spawnParameter));
    EXPECT_CALL(fakeAgentBlueprint, SetAgentCategory(AgentCategory::Scenario));
    EXPECT_CALL(fakeAgentBlueprint, SetObjectName(scenarioEntity.name));
    EXPECT_CALL(fakeAgentBlueprint, SetSpawnParameter(AllOf(Field(&SpawnParameter::SpawningLaneId, -1),
                                                            Field(&SpawnParameter::positionX, DoubleEq(123.0)),
                                                            Field(&SpawnParameter::heading, DoubleEq(0.3)),
                                                            Field(&SpawnParameter::velocity, DoubleEq(12.0)))));

    SpawnPoint spawnPoint(&fakeWorld,
                          nullptr,
                          nullptr,
                          nullptr,
                          &fakeScenario,
                          SpawnPointState::SceneryPlacement);

    spawnPoint.SetPredefinedParameter(&fakeAgentBlueprint);
}

TEST(SpawnPoint, GetSpawnLaneEgo)
{
    ScenarioEntity scenarioEntity;
    scenarioEntity.name = "NameOfEgo";
    scenarioEntity.spawnInfo.s.value = 123.0;
    scenarioEntity.spawnInfo.ILane = -1;
    scenarioEntity.spawnInfo.offset.value = 0.3;

    NiceMock<FakeScenario> fakeScenario;
    std::vector<ScenarioEntity*> emptyEntities;
    ON_CALL(fakeScenario, GetScenarioEntities()).WillByDefault(ReturnRef(emptyEntities));
    ON_CALL(fakeScenario, GetEgoEntity()).WillByDefault(ReturnRef(scenarioEntity));

    LaneQueryResult queryResult {5, 100.0, 5000.0, LaneCategory::RegularLane, true};

    std::map<std::string, double> fakeDoubles = {{"TrafficVolume", 600.0}, {"PlatoonRate", 1.0}, {"Velocity", 30.0}, {"Homogenity", 1.0}};

    NiceMock<FakeParameter> fakeParameters;
    ON_CALL(fakeParameters, GetParametersDouble()).WillByDefault(ReturnRef(fakeDoubles));

    NiceMock<FakeWorld> fakeWorld;
    ON_CALL(fakeWorld, QueryLane(_, _, _)).WillByDefault(Return(LaneQueryResult::InvalidResult()));
    ON_CALL(fakeWorld, QueryLane(_, -1, DoubleEq(123.0))).WillByDefault(Return(queryResult));
    ON_CALL(fakeWorld, IsSValidOnLane(_, _, _)).WillByDefault((Return(true)));

    SpawnPoint spawnPoint(&fakeWorld,
                          nullptr,
                          nullptr,
                          nullptr,
                          &fakeScenario,
                          SpawnPointState::EgoPlacement);

    LaneParameters laneParameters;

    spawnPoint.ExtractSpawnPointParameters(&fakeParameters);
    spawnPoint.CalculateDerivedParameters();

    ASSERT_TRUE(spawnPoint.GetSpawningLane(laneParameters));

    ASSERT_EQ(laneParameters.laneId, -1);
    ASSERT_EQ(laneParameters.streamId, 5);
    ASSERT_EQ(laneParameters.laneCategory, LaneCategory::RegularLane);
    ASSERT_DOUBLE_EQ(laneParameters.distanceToLaneStart, 100.0);
    ASSERT_DOUBLE_EQ(laneParameters.offset, 0.3);
}

TEST(SpawnPoint, GetSpawnLaneScenery)
{
    ScenarioEntity scenarioEntity;
    scenarioEntity.name = "NameOfScenery";
    scenarioEntity.spawnInfo.s.value = 123.0;
    scenarioEntity.spawnInfo.ILane = -1;
    scenarioEntity.spawnInfo.offset.value = 0.3;

    NiceMock<FakeScenario> fakeScenario;
    std::vector<ScenarioEntity*> scenarioEntityVector { &scenarioEntity };
    ON_CALL(fakeScenario, GetScenarioEntities()).WillByDefault(ReturnRef(scenarioEntityVector));

    ScenarioEntity egoEntity;
    ON_CALL(fakeScenario, GetEgoEntity()).WillByDefault(ReturnRef(egoEntity));

    LaneQueryResult queryResult {5, 100.0, 5000.0, LaneCategory::RegularLane, true};

    std::map<std::string, double> fakeDoubles = {{"TrafficVolume", 600.0}, {"PlatoonRate", 1.0}, {"Velocity", 30.0}, {"Homogenity", 1.0}};

    NiceMock<FakeParameter> fakeParameters;
    ON_CALL(fakeParameters, GetParametersDouble()).WillByDefault(ReturnRef(fakeDoubles));

    NiceMock<FakeWorld> fakeWorld;
    ON_CALL(fakeWorld, QueryLane(_, _, _)).WillByDefault(Return(LaneQueryResult::InvalidResult()));
    ON_CALL(fakeWorld, QueryLane(_, -1, DoubleEq(123.0))).WillByDefault(Return(queryResult));
    ON_CALL(fakeWorld, IsSValidOnLane(_, _, _)).WillByDefault((Return(true)));

    SpawnPoint spawnPoint(&fakeWorld,
                          nullptr,
                          nullptr,
                          nullptr,
                          &fakeScenario,
                          SpawnPointState::SceneryPlacement);

    LaneParameters laneParameters;

    spawnPoint.ExtractSpawnPointParameters(&fakeParameters);
    spawnPoint.CalculateDerivedParameters();

    ASSERT_TRUE(spawnPoint.GetSpawningLane(laneParameters));

    ASSERT_EQ(laneParameters.laneId, -1);
    ASSERT_EQ(laneParameters.streamId, 5);
    ASSERT_EQ(laneParameters.laneCategory, LaneCategory::RegularLane);
    ASSERT_DOUBLE_EQ(laneParameters.distanceToLaneStart, 100.0);
    ASSERT_DOUBLE_EQ(laneParameters.offset, 0.3);
}
