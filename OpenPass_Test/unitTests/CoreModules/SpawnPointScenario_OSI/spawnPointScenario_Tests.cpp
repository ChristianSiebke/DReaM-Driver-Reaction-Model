/*********************************************************************
* Copyright (c) 2019 in-tech
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "dontCare.h"

#include "SpawnPointScenario.h"
#include "agentBlueprint.h"

#include "fakeWorld.h"
#include "fakeScenario.h"
#include "fakeAgentBlueprintProvider.h"
#include "fakeAgentFactory.h"
#include "fakeSampler.h"
#include "fakeAgent.h"

using ::testing::ReturnRef;
using ::testing::Return;
using ::testing::_;
using ::testing::DontCare;
using ::testing::Matcher;
using ::testing::NiceMock;

MATCHER_P(MatchesAgentBlueprint, referenceAgentBlueprint, "matches blueprint")
{
    if (!(arg->GetAgentProfileName() == referenceAgentBlueprint.GetAgentProfileName()
       && arg->GetAgentCategory() == referenceAgentBlueprint.GetAgentCategory())
       && arg->GetObjectName() == referenceAgentBlueprint.GetObjectName())
    {
        return false;
    }
    const auto actualSpawnParameters = arg->GetSpawnParameter();
    const auto expectedSpawnParameters = referenceAgentBlueprint.GetSpawnParameter();
    if (!(actualSpawnParameters.velocity == expectedSpawnParameters.velocity
       && actualSpawnParameters.SpawningLaneId == expectedSpawnParameters.SpawningLaneId
       && actualSpawnParameters.SpawningRoadId == expectedSpawnParameters.SpawningRoadId
       && actualSpawnParameters.positionX == expectedSpawnParameters.positionX
       && actualSpawnParameters.heading == expectedSpawnParameters.heading))
    {
        return false;
    }
    const auto actualVehicleModelParameters = arg->GetVehicleModelParameters();
    const auto expectedVehicleModelParameters = referenceAgentBlueprint.GetVehicleModelParameters();
    if (!(actualVehicleModelParameters.length == expectedVehicleModelParameters.length
       && actualVehicleModelParameters.width == expectedVehicleModelParameters.width
       && actualVehicleModelParameters.distanceReferencePointToLeadingEdge == expectedVehicleModelParameters.distanceReferencePointToLeadingEdge))
    {
        return false;
    }

    return true;
}
TEST(SpawnPointScenario, Trigger_SpawnsEgoAgentAccordingToScenario)
{
    NiceMock<FakeWorld> fakeWorld;
    NiceMock<FakeScenario> fakeScenario;
    NiceMock<FakeAgentBlueprintProvider> fakeAgentBlueprintProvider;
    NiceMock<FakeAgentFactory> fakeAgentFactory;
    NiceMock<FakeSampler> fakeSampler;

    SpawnPointDependencies dependencies(&fakeAgentFactory, &fakeWorld, &fakeAgentBlueprintProvider, &fakeSampler);
    dependencies.scenario = &fakeScenario;

    const std::string entityName = "Ego";
    const std::string roadId = "ROADID";
    const int laneId = -1;
    const double s = 10;
    const double offset = 0;
    const double velocity = 25;
    const double acceleration = 25;

    ScenarioEntity entity;
    entity.name = entityName;
    entity.catalogReference.entryName = entityName;
    entity.spawnInfo.roadId = roadId;
    entity.spawnInfo.ILane = laneId;
    entity.spawnInfo.s.value = s;
    entity.spawnInfo.offset.value = offset;
    entity.spawnInfo.velocity.value = velocity;
    entity.spawnInfo.acceleration.value = acceleration;

    std::vector<ScenarioEntity> entities{entity};
    std::optional<AgentBlueprint> actualAgentBlueprintOptional;
    AgentBlueprint actualAgentBlueprint;
    VehicleModelParameters vehicleModelParameters;
    vehicleModelParameters.length = 1;
    vehicleModelParameters.width = 0.5;
    vehicleModelParameters.distanceReferencePointToLeadingEdge = 0.5;
    actualAgentBlueprint.SetVehicleModelParameters(vehicleModelParameters);
    actualAgentBlueprintOptional = actualAgentBlueprint;

    const double agentFrontLength = vehicleModelParameters.length - vehicleModelParameters.distanceReferencePointToLeadingEdge;

    SpawnParameter expectedSpawnParameter;
    expectedSpawnParameter.velocity = velocity;
    expectedSpawnParameter.SpawningLaneId = laneId;
    expectedSpawnParameter.SpawningRoadId = roadId;
    expectedSpawnParameter.positionX = s;
    expectedSpawnParameter.heading = 0;
    AgentBlueprint expectedAgentBlueprint;
    expectedAgentBlueprint.SetVehicleModelParameters(vehicleModelParameters);
    expectedAgentBlueprint.SetAgentProfileName(entityName);
    expectedAgentBlueprint.SetAgentCategory(AgentCategory::Ego);
    expectedAgentBlueprint.SetObjectName(entityName);
    expectedAgentBlueprint.SetSpawnParameter(expectedSpawnParameter);

    Position position{s, 0, 0, 0};

    ON_CALL(fakeScenario, GetEntities())
            .WillByDefault(ReturnRef(entities));

    ON_CALL(fakeAgentBlueprintProvider, SampleAgent(entity.catalogReference.entryName))
            .WillByDefault(Return(actualAgentBlueprint));
    ON_CALL(fakeWorld, IsSValidOnLane(roadId, laneId, s))
            .WillByDefault(Return(true));
    ON_CALL(fakeWorld, IsSValidOnLane(roadId, laneId, s + agentFrontLength))
            .WillByDefault(Return(true));
    ON_CALL(fakeWorld, GetLaneWidth(_, roadId, laneId, s + agentFrontLength, 0))
            .WillByDefault(Return(0.75));
    ON_CALL(fakeWorld, LaneCoord2WorldCoord(s + agentFrontLength, offset, roadId, laneId))
            .WillByDefault(Return(position));
    ON_CALL(fakeWorld, IntersectsWithAgent(_, _, _, _, _, _))
            .WillByDefault(Return(false));

    SimulationSlave::Agent agent(0, &fakeWorld);
    // if this is called and the blueprints match, we're creating our Agent correctly
    EXPECT_CALL(fakeAgentFactory, AddAgent(MatchesAgentBlueprint(expectedAgentBlueprint)))
            .WillOnce(Return(&agent));

    SpawnPointScenario spawnPointScenario{&dependencies, nullptr};
    spawnPointScenario.Trigger();
}

TEST(SpawnPointScenario, Trigger_SpawnsScenarioAgentAccordingToScenario)
{
    NiceMock<FakeWorld> fakeWorld;
    NiceMock<FakeScenario> fakeScenario;
    NiceMock<FakeAgentBlueprintProvider> fakeAgentBlueprintProvider;
    NiceMock<FakeAgentFactory> fakeAgentFactory;
    NiceMock<FakeSampler> fakeSampler;

    SpawnPointDependencies dependencies(&fakeAgentFactory, &fakeWorld, &fakeAgentBlueprintProvider, &fakeSampler);
    dependencies.scenario = &fakeScenario;

    const std::string entityName = "ENTITY";
    const std::string roadId = "ROADID";
    const int laneId = -1;
    const double s = 10;
    const double offset = 0;
    const double velocity = 25;
    const double acceleration = 25;

    ScenarioEntity entity;
    entity.name = entityName;
    entity.catalogReference.entryName = entityName;
    entity.spawnInfo.roadId = roadId;
    entity.spawnInfo.ILane = laneId;
    entity.spawnInfo.s.value = s;
    entity.spawnInfo.offset.value = offset;
    entity.spawnInfo.velocity.value = velocity;
    entity.spawnInfo.acceleration.value = acceleration;

    std::vector<ScenarioEntity> entities{entity};
    std::optional<AgentBlueprint> actualAgentBlueprintOptional;
    AgentBlueprint actualAgentBlueprint;
    VehicleModelParameters vehicleModelParameters;
    vehicleModelParameters.length = 1;
    vehicleModelParameters.width = 0.5;
    vehicleModelParameters.distanceReferencePointToLeadingEdge = 0.5;
    actualAgentBlueprint.SetVehicleModelParameters(vehicleModelParameters);
    actualAgentBlueprintOptional = actualAgentBlueprint;

    const double agentFrontLength = vehicleModelParameters.length - vehicleModelParameters.distanceReferencePointToLeadingEdge;

    SpawnParameter expectedSpawnParameter;
    expectedSpawnParameter.velocity = velocity;
    expectedSpawnParameter.SpawningLaneId = laneId;
    expectedSpawnParameter.SpawningRoadId = roadId;
    expectedSpawnParameter.positionX = s;
    expectedSpawnParameter.heading = 0;
    AgentBlueprint expectedAgentBlueprint;
    expectedAgentBlueprint.SetVehicleModelParameters(vehicleModelParameters);
    expectedAgentBlueprint.SetAgentProfileName(entityName);
    expectedAgentBlueprint.SetAgentCategory(AgentCategory::Scenario);
    expectedAgentBlueprint.SetObjectName(entityName);
    expectedAgentBlueprint.SetSpawnParameter(expectedSpawnParameter);

    Position position{s, 0, 0, 0};

    ON_CALL(fakeScenario, GetEntities())
            .WillByDefault(ReturnRef(entities));

    ON_CALL(fakeAgentBlueprintProvider, SampleAgent(entity.catalogReference.entryName))
            .WillByDefault(Return(actualAgentBlueprint));
    ON_CALL(fakeWorld, IsSValidOnLane(roadId, laneId, s))
            .WillByDefault(Return(true));
    ON_CALL(fakeWorld, IsSValidOnLane(roadId, laneId, s + agentFrontLength))
            .WillByDefault(Return(true));
    ON_CALL(fakeWorld, GetLaneWidth(_, roadId, laneId, s + agentFrontLength, 0))
            .WillByDefault(Return(0.75));
    ON_CALL(fakeWorld, LaneCoord2WorldCoord(s + agentFrontLength, offset, roadId, laneId))
            .WillByDefault(Return(position));
    ON_CALL(fakeWorld, IntersectsWithAgent(_, _, _, _, _, _))
            .WillByDefault(Return(false));

    SimulationSlave::Agent agent(0, &fakeWorld);
    // if this is called and the blueprints match, we're creating our Agent correctly
    EXPECT_CALL(fakeAgentFactory, AddAgent(MatchesAgentBlueprint(expectedAgentBlueprint)))
            .WillOnce(Return(&agent));

    SpawnPointScenario spawnPointScenario{&dependencies, nullptr};
    spawnPointScenario.Trigger();
}
