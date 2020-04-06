/*********************************************************************
* Copyright (c) 2019, 2020 in-tech
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "SpawnPointScenario.h"
#include "agentBlueprint.h"
#include "dontCare.h"
#include "fakeAgent.h"
#include "fakeAgentBlueprintProvider.h"
#include "fakeAgentFactory.h"
#include "fakeStochastics.h"
#include "fakeScenario.h"
#include "fakeWorld.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::_;
using ::testing::DontCare;
using ::testing::Matcher;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnRef;

MATCHER_P(MatchesAgentBlueprint, referenceAgentBlueprint, "matches blueprint")
{
    if (!(arg->GetAgentProfileName() == referenceAgentBlueprint.GetAgentProfileName() && arg->GetAgentCategory() == referenceAgentBlueprint.GetAgentCategory()) && arg->GetObjectName() == referenceAgentBlueprint.GetObjectName())
    {
        return false;
    }
    const auto actualSpawnParameters = arg->GetSpawnParameter();
    const auto expectedSpawnParameters = referenceAgentBlueprint.GetSpawnParameter();
    if (!(actualSpawnParameters.velocity == expectedSpawnParameters.velocity && actualSpawnParameters.positionX == expectedSpawnParameters.positionX && actualSpawnParameters.yawAngle == expectedSpawnParameters.yawAngle))
    {
        return false;
    }
    const auto actualVehicleModelParameters = arg->GetVehicleModelParameters();
    const auto expectedVehicleModelParameters = referenceAgentBlueprint.GetVehicleModelParameters();
    if (!(actualVehicleModelParameters.length == expectedVehicleModelParameters.length && actualVehicleModelParameters.width == expectedVehicleModelParameters.width && actualVehicleModelParameters.distanceReferencePointToLeadingEdge == expectedVehicleModelParameters.distanceReferencePointToLeadingEdge))
    {
        return false;
    }

    return true;
}

TEST(SpawnPointScenario, Trigger_SpawnsEgoAgentAccordingToScenarioWorldPosition)
{
    NiceMock<FakeWorld> fakeWorld;
    NiceMock<FakeScenario> fakeScenario;
    NiceMock<FakeAgentBlueprintProvider> fakeAgentBlueprintProvider;
    NiceMock<FakeAgentFactory> fakeAgentFactory;
    NiceMock<FakeStochastics> fakeStochastics;

    SpawnPointDependencies dependencies(&fakeAgentFactory, &fakeWorld, &fakeAgentBlueprintProvider, &fakeStochastics);
    dependencies.scenario = &fakeScenario;

    const std::string entityName = "Ego";
    constexpr double x = 10.0;
    constexpr double y = 5.0;
    constexpr double heading = 0.5;
    constexpr double velocity = 25;
    constexpr double acceleration = 25;

    openScenario::WorldPosition worldPosition{x, y, heading};

    ScenarioEntity entity;
    entity.name = entityName;
    entity.catalogReference.entryName = entityName;
    entity.spawnInfo.velocity = velocity;
    entity.spawnInfo.acceleration = acceleration;
    entity.spawnInfo.position = worldPosition;

    std::vector<ScenarioEntity> entities{entity};
    std::optional<AgentBlueprint> actualAgentBlueprintOptional;
    AgentBlueprint actualAgentBlueprint;
    VehicleModelParameters vehicleModelParameters;
    vehicleModelParameters.length = 1;
    vehicleModelParameters.width = 0.5;
    vehicleModelParameters.distanceReferencePointToLeadingEdge = 0.5;
    actualAgentBlueprint.SetVehicleModelParameters(vehicleModelParameters);
    actualAgentBlueprintOptional = actualAgentBlueprint;

    SpawnParameter expectedSpawnParameter;
    expectedSpawnParameter.velocity = velocity;
    expectedSpawnParameter.positionX = x;
    expectedSpawnParameter.positionY = y;
    expectedSpawnParameter.yawAngle = heading;

    AgentBlueprint expectedAgentBlueprint;
    expectedAgentBlueprint.SetVehicleModelParameters(vehicleModelParameters);
    expectedAgentBlueprint.SetAgentProfileName(entityName);
    expectedAgentBlueprint.SetAgentCategory(AgentCategory::Ego);
    expectedAgentBlueprint.SetObjectName(entityName);
    expectedAgentBlueprint.SetSpawnParameter(expectedSpawnParameter);

    ON_CALL(fakeScenario, GetEntities())
        .WillByDefault(ReturnRef(entities));

    ON_CALL(fakeAgentBlueprintProvider, SampleAgent(entity.catalogReference.entryName, _))
        .WillByDefault(Return(actualAgentBlueprint));
    ON_CALL(fakeWorld, IntersectsWithAgent(_, _, _, _, _, _))
        .WillByDefault(Return(false));

    SimulationSlave::Agent agent(0, &fakeWorld);
    // if this is called and the blueprints match, we're creating our Agent correctly
    EXPECT_CALL(fakeAgentFactory, AddAgent(MatchesAgentBlueprint(expectedAgentBlueprint)))
        .WillOnce(Return(&agent));

    SpawnPointScenario spawnPointScenario{&dependencies, nullptr};
    spawnPointScenario.Trigger(0);
}

TEST(SpawnPointScenario, Trigger_SpawnsEgoAgentAccordingToScenarioLanePosition)
{
    NiceMock<FakeWorld> fakeWorld;
    NiceMock<FakeScenario> fakeScenario;
    NiceMock<FakeAgentBlueprintProvider> fakeAgentBlueprintProvider;
    NiceMock<FakeAgentFactory> fakeAgentFactory;
    NiceMock<FakeStochastics> fakeStochastics;

    SpawnPointDependencies dependencies(&fakeAgentFactory, &fakeWorld, &fakeAgentBlueprintProvider, &fakeStochastics);
    dependencies.scenario = &fakeScenario;

    const std::string entityName = "Ego";
    const std::string roadId = "ROADID";
    constexpr int laneId = -1;
    constexpr double s = 10;
    constexpr double offset = 0;
    constexpr double velocity = 25;
    constexpr double acceleration = 25;

    openScenario::LanePosition lanePosition;
    lanePosition.roadId = roadId;
    lanePosition.laneId = laneId;
    lanePosition.s = s;
    lanePosition.offset = offset;

    ScenarioEntity entity;
    entity.name = entityName;
    entity.catalogReference.entryName = entityName;
    entity.spawnInfo.velocity = velocity;
    entity.spawnInfo.acceleration = acceleration;
    entity.spawnInfo.position = lanePosition;

    std::vector<ScenarioEntity> entities{entity};
    std::optional<AgentBlueprint> actualAgentBlueprintOptional;
    AgentBlueprint actualAgentBlueprint;
    VehicleModelParameters vehicleModelParameters;
    vehicleModelParameters.length = 1;
    vehicleModelParameters.width = 0.5;
    vehicleModelParameters.distanceReferencePointToLeadingEdge = 0.5;
    actualAgentBlueprint.SetVehicleModelParameters(vehicleModelParameters);
    actualAgentBlueprintOptional = actualAgentBlueprint;

    SpawnParameter expectedSpawnParameter;
    constexpr double expectedX = 100.0;
    constexpr double expectedY = 10.0;
    constexpr double expectedYaw = 1.0;
    expectedSpawnParameter.velocity = velocity;
    expectedSpawnParameter.positionX = expectedX;
    expectedSpawnParameter.positionY = expectedY;
    expectedSpawnParameter.yawAngle = expectedYaw;

    AgentBlueprint expectedAgentBlueprint;
    expectedAgentBlueprint.SetVehicleModelParameters(vehicleModelParameters);
    expectedAgentBlueprint.SetAgentProfileName(entityName);
    expectedAgentBlueprint.SetAgentCategory(AgentCategory::Ego);
    expectedAgentBlueprint.SetObjectName(entityName);
    expectedAgentBlueprint.SetSpawnParameter(expectedSpawnParameter);

    Position position{expectedX, expectedY, expectedYaw, 0};

    ON_CALL(fakeScenario, GetEntities()).WillByDefault(ReturnRef(entities));

    ON_CALL(fakeAgentBlueprintProvider, SampleAgent(entity.catalogReference.entryName, _))
        .WillByDefault(Return(actualAgentBlueprint));
    ON_CALL(fakeWorld, IsSValidOnLane(roadId, laneId, s))
        .WillByDefault(Return(true));
    ON_CALL(fakeWorld, IsSValidOnLane(roadId, laneId, s))
        .WillByDefault(Return(true));
    ON_CALL(fakeWorld, GetLaneWidth(roadId, laneId, s))
        .WillByDefault(Return(0.75));
    ON_CALL(fakeWorld, LaneCoord2WorldCoord(s, offset, roadId, laneId))
        .WillByDefault(Return(position));
    ON_CALL(fakeWorld, IntersectsWithAgent(_, _, _, _, _, _))
        .WillByDefault(Return(false));

    SimulationSlave::Agent agent(0, &fakeWorld);
    // if this is called and the blueprints match, we're creating our Agent correctly
    EXPECT_CALL(fakeAgentFactory, AddAgent(MatchesAgentBlueprint(expectedAgentBlueprint)))
        .WillOnce(Return(&agent));

    SpawnPointScenario spawnPointScenario{&dependencies, nullptr};
    spawnPointScenario.Trigger(0);
}

TEST(SpawnPointScenario, Trigger_SpawnsScenarioAgentAccordingToScenarioWorldPosition)
{
    NiceMock<FakeWorld> fakeWorld;
    NiceMock<FakeScenario> fakeScenario;
    NiceMock<FakeAgentBlueprintProvider> fakeAgentBlueprintProvider;
    NiceMock<FakeAgentFactory> fakeAgentFactory;
    NiceMock<FakeStochastics> fakeStochastics;

    SpawnPointDependencies dependencies(&fakeAgentFactory, &fakeWorld, &fakeAgentBlueprintProvider, &fakeStochastics);
    dependencies.scenario = &fakeScenario;

    const std::string entityName = "ENTITY";
    constexpr double x = 10.0;
    constexpr double y = 5.0;
    constexpr double heading = 0.5;
    constexpr double velocity = 25;
    constexpr double acceleration = 25;

    openScenario::WorldPosition worldPosition{x, y, heading};

    ScenarioEntity entity;
    entity.name = entityName;
    entity.catalogReference.entryName = entityName;
    entity.spawnInfo.velocity = velocity;
    entity.spawnInfo.acceleration = acceleration;
    entity.spawnInfo.position = worldPosition;

    std::vector<ScenarioEntity> entities{entity};
    std::optional<AgentBlueprint> actualAgentBlueprintOptional;
    AgentBlueprint actualAgentBlueprint;
    VehicleModelParameters vehicleModelParameters;
    vehicleModelParameters.length = 1;
    vehicleModelParameters.width = 0.5;
    vehicleModelParameters.distanceReferencePointToLeadingEdge = 0.5;
    actualAgentBlueprint.SetVehicleModelParameters(vehicleModelParameters);
    actualAgentBlueprintOptional = actualAgentBlueprint;

    SpawnParameter expectedSpawnParameter;
    expectedSpawnParameter.velocity = velocity;
    expectedSpawnParameter.positionX = x;
    expectedSpawnParameter.positionY = y;
    expectedSpawnParameter.yawAngle = heading;

    AgentBlueprint expectedAgentBlueprint;
    expectedAgentBlueprint.SetVehicleModelParameters(vehicleModelParameters);
    expectedAgentBlueprint.SetAgentProfileName(entityName);
    expectedAgentBlueprint.SetAgentCategory(AgentCategory::Scenario);
    expectedAgentBlueprint.SetObjectName(entityName);
    expectedAgentBlueprint.SetSpawnParameter(expectedSpawnParameter);

    ON_CALL(fakeScenario, GetEntities())
        .WillByDefault(ReturnRef(entities));

    ON_CALL(fakeAgentBlueprintProvider, SampleAgent(entity.catalogReference.entryName, _))
        .WillByDefault(Return(actualAgentBlueprint));
    ON_CALL(fakeWorld, IntersectsWithAgent(_, _, _, _, _, _))
        .WillByDefault(Return(false));

    SimulationSlave::Agent agent(0, &fakeWorld);
    // if this is called and the blueprints match, we're creating our Agent correctly
    EXPECT_CALL(fakeAgentFactory, AddAgent(MatchesAgentBlueprint(expectedAgentBlueprint)))
        .WillOnce(Return(&agent));

    SpawnPointScenario spawnPointScenario{&dependencies, nullptr};
    spawnPointScenario.Trigger(0);
}

TEST(SpawnPointScenario, Trigger_SpawnsScenarioAgentAccordingToScenarioLanePosition)
{
    NiceMock<FakeWorld> fakeWorld;
    NiceMock<FakeScenario> fakeScenario;
    NiceMock<FakeAgentBlueprintProvider> fakeAgentBlueprintProvider;
    NiceMock<FakeAgentFactory> fakeAgentFactory;
    NiceMock<FakeStochastics> fakeStochastics;

    SpawnPointDependencies dependencies(&fakeAgentFactory, &fakeWorld, &fakeAgentBlueprintProvider, &fakeStochastics);
    dependencies.scenario = &fakeScenario;

    const std::string entityName = "ENTITY";
    const std::string roadId = "ROADID";
    constexpr int laneId = -1;
    constexpr double s = 10;
    constexpr double offset = 0;
    constexpr double velocity = 25;
    constexpr double acceleration = 25;

    openScenario::LanePosition lanePosition;
    lanePosition.roadId = roadId;
    lanePosition.laneId = laneId;
    lanePosition.s = s;
    lanePosition.offset = offset;

    ScenarioEntity entity;
    entity.name = entityName;
    entity.catalogReference.entryName = entityName;
    entity.spawnInfo.velocity = velocity;
    entity.spawnInfo.acceleration = acceleration;
    entity.spawnInfo.position = lanePosition;

    std::vector<ScenarioEntity> entities{entity};
    std::optional<AgentBlueprint> actualAgentBlueprintOptional;
    AgentBlueprint actualAgentBlueprint;
    VehicleModelParameters vehicleModelParameters;
    vehicleModelParameters.length = 1;
    vehicleModelParameters.width = 0.5;
    vehicleModelParameters.distanceReferencePointToLeadingEdge = 0.5;
    actualAgentBlueprint.SetVehicleModelParameters(vehicleModelParameters);
    actualAgentBlueprintOptional = actualAgentBlueprint;

    SpawnParameter expectedSpawnParameter;
    constexpr double expectedX = 100.0;
    constexpr double expectedY = 10.0;
    constexpr double expectedYaw = 1.0;
    expectedSpawnParameter.velocity = velocity;
    expectedSpawnParameter.positionX = expectedX;
    expectedSpawnParameter.positionY = expectedY;
    expectedSpawnParameter.yawAngle = expectedYaw;

    AgentBlueprint expectedAgentBlueprint;
    expectedAgentBlueprint.SetVehicleModelParameters(vehicleModelParameters);
    expectedAgentBlueprint.SetAgentProfileName(entityName);
    expectedAgentBlueprint.SetAgentCategory(AgentCategory::Scenario);
    expectedAgentBlueprint.SetObjectName(entityName);
    expectedAgentBlueprint.SetSpawnParameter(expectedSpawnParameter);

    Position position{expectedX, expectedY, expectedYaw, 0};

    ON_CALL(fakeScenario, GetEntities())
        .WillByDefault(ReturnRef(entities));

    ON_CALL(fakeAgentBlueprintProvider, SampleAgent(entity.catalogReference.entryName, _))
        .WillByDefault(Return(actualAgentBlueprint));
    ON_CALL(fakeWorld, IsSValidOnLane(roadId, laneId, s))
        .WillByDefault(Return(true));
    ON_CALL(fakeWorld, IsSValidOnLane(roadId, laneId, s))
        .WillByDefault(Return(true));
    ON_CALL(fakeWorld, GetLaneWidth(roadId, laneId, s))
        .WillByDefault(Return(0.75));
    ON_CALL(fakeWorld, LaneCoord2WorldCoord(s, offset, roadId, laneId))
        .WillByDefault(Return(position));
    ON_CALL(fakeWorld, IntersectsWithAgent(_, _, _, _, _, _))
        .WillByDefault(Return(false));

    SimulationSlave::Agent agent(0, &fakeWorld);
    // if this is called and the blueprints match, we're creating our Agent correctly
    EXPECT_CALL(fakeAgentFactory, AddAgent(MatchesAgentBlueprint(expectedAgentBlueprint)))
        .WillOnce(Return(&agent));

    SpawnPointScenario spawnPointScenario{&dependencies, nullptr};
    spawnPointScenario.Trigger(0);
}
