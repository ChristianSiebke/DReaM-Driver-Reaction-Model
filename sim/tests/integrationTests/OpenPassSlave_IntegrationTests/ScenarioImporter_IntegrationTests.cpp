/*********************************************************************
* Copyright (c) 2017, 2018, 2019, 2020, 2021 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <filesystem>

#include "importer/scenario.h"
#include "importer/scenarioImporter.h"
#include "fakeScenario.h"

using ::testing::NiceMock;
using ::testing::Eq;
using ::testing::Ne;

using namespace Configuration;
using namespace Importer;

TEST(ScenarioImporter_IntegrationTests, ImportScenario_WithWrongInternalVersion_Fails)
{
    const std::filesystem::path testScenarioFile = std::filesystem::current_path() / "Resources" / "ImporterTest" / "WrongVersionScenario.xosc";
    Scenario scenario;
    ScenarioImporter importer;

    ASSERT_FALSE(importer.Import(testScenarioFile.string(), &scenario));
}

TEST(ScenarioImporter_IntegrationTests, ImportScenario_WithoutInternalVersion_Fails)
{
    const std::filesystem::path testScenarioFile = std::filesystem::current_path() / "Resources" / "ImporterTest" / "NoVersionScenario.xosc";
    Scenario scenario;
    ScenarioImporter importer;

    ASSERT_FALSE(importer.Import(testScenarioFile.string(), &scenario));
}

TEST(ScenarioImporter_IntegrationTests, ImportScenario_WithMinimalInfoForOpenPass_Succeeds)
{
    const std::filesystem::path testScenarioFile = std::filesystem::current_path() / "Resources" / "ImporterTest" / "Scenario.xosc";
    Scenario scenario;

    ScenarioImporter importer;
    ASSERT_TRUE(importer.Import(testScenarioFile.string(), &scenario));

    const auto entities = scenario.GetEntities();
    const auto resultEgoEntityIter = std::find_if(entities.cbegin(),
                                                  entities.cend(),
                                                  [](const auto& entity) -> bool
    {
        return entity.name == "Ego";
    });
    ASSERT_THAT(resultEgoEntityIter, Ne(entities.cend()));
    ScenarioEntity resultEgoEntity = *resultEgoEntityIter;
    SpawnInfo resultSpawnInfoEgo = resultEgoEntity.spawnInfo;

    openScenario::LanePosition lanePositionEgo;
    ASSERT_NO_THROW(lanePositionEgo = std::get<openScenario::LanePosition>(resultSpawnInfoEgo.position));
    ASSERT_EQ(lanePositionEgo.laneId, -3);
    ASSERT_DOUBLE_EQ(lanePositionEgo.s, 1000.0);
    ASSERT_DOUBLE_EQ(resultSpawnInfoEgo.velocity, 5.0);
    ASSERT_DOUBLE_EQ(resultSpawnInfoEgo.acceleration.value(), 0.5);

    auto sceneryEntities = scenario.GetScenarioEntities();
    ASSERT_EQ(sceneryEntities.size(), (size_t) 2);

    ScenarioEntity *resultSceneryEntitySecond = sceneryEntities.at(1);
    SpawnInfo resultSpawnInfoScenery = resultSceneryEntitySecond->spawnInfo;

    openScenario::LanePosition lanePositionScenery2;
    ASSERT_NO_THROW(lanePositionScenery2 = std::get<openScenario::LanePosition>(resultSpawnInfoEgo.position));

    ASSERT_EQ(resultSceneryEntitySecond->name, "Scenery2");
    ASSERT_EQ(lanePositionScenery2.laneId, -3);
    ASSERT_DOUBLE_EQ(lanePositionScenery2.s, 1000.0);
    ASSERT_DOUBLE_EQ(resultSpawnInfoScenery.velocity, 5.0);
    ASSERT_DOUBLE_EQ(resultSpawnInfoScenery.acceleration.value(), 0.5);
}

TEST(ScenarioImporter_IntegrationTests, ImportScenario_SetsSceneryPath)
{
    const std::filesystem::path testScenarioFile = std::filesystem::current_path() / "Resources" / "ImporterTest" / "Scenario.xosc";
    NiceMock<FakeScenario> scenario;

    EXPECT_CALL(scenario, SetSceneryPath("testSceneryConfiguration.xodr")).Times(1);

    ScenarioImporter importer;
    ASSERT_TRUE(importer.Import(testScenarioFile.string(), &scenario));
}

TEST(ScenarioImporter_IntegrationTests, ImportScenario_SetsCatalogPaths)
{
    const std::filesystem::path testScenarioFile = std::filesystem::current_path() / "Resources" / "ImporterTest" / "Scenario.xosc";
    NiceMock<FakeScenario> scenario;

    EXPECT_CALL(scenario, SetVehicleCatalogPath("VehicleModelsCatalog.xosc")).Times(1);
    EXPECT_CALL(scenario, SetPedestrianCatalogPath("PedestrianModelsCatalog.xosc")).Times(1);

    ScenarioImporter importer;
    ASSERT_TRUE(importer.Import(testScenarioFile.string(), &scenario));
}

TEST(ScenarioImporter_IntegrationTests, ImportScenario_WithMissingRequiredParameters_ThrowsException)
{
    const std::filesystem::path testScenarioFile = std::filesystem::current_path() / "Resources" / "ImporterTest" / "FailingScenario.xosc";
    Scenario scenario;

    ASSERT_FALSE(ScenarioImporter::Import(testScenarioFile.string(), &scenario));
}

TEST(ScenarioImporter_IntegrationTests, ImportScenario_WithReferencedButUndeclaredActorEntity_ThrowsException)
{
    const std::filesystem::path testScenarioFile = std::filesystem::current_path() / "Resources" / "ImporterTest" / "FailingScenarioUndeclaredActorEntity.xosc";
    Scenario scenario;

    ASSERT_FALSE(ScenarioImporter::Import(testScenarioFile.string(), &scenario));
}

TEST(ScenarioImporter_IntegrationTests, ImportScenario_WithReferencedButUndeclaredConditionEntity_ThrowsException)
{
    const std::filesystem::path testScenarioFile = std::filesystem::current_path() / "Resources" / "ImporterTest" / "FailingScenarioUndeclaredConditionEntity.xosc";
    Scenario scenario;

    ASSERT_FALSE(ScenarioImporter::Import(testScenarioFile.string(), &scenario));
}

TEST(ScenarioImporter_IntegrationTests, ImportScenario_WithReferencedButUndeclaredInitPrivateEntity_ThrowsException)
{
    const std::filesystem::path testScenarioFile = std::filesystem::current_path() / "Resources" / "ImporterTest" / "FailingScenarioUndeclaredInitPrivateEntity.xosc";
    Scenario scenario;

    ASSERT_FALSE(ScenarioImporter::Import(testScenarioFile.string(), &scenario));
}

TEST(ScenarioImporter_IntegrationTests, ImportScenario_WithMultipleStories_Succeeds)
{
    const std::filesystem::path testScenarioFile = std::filesystem::current_path() / "Resources" / "ImporterTest" / "MultipleStoriesScenario.xosc";
    Scenario scenario;

    ScenarioImporter importer;
    ASSERT_TRUE(importer.Import(testScenarioFile.string(), &scenario));

    const auto &numberOfEventDetectors = scenario.GetEventDetectorInformations().size();
    const auto &numberOfManipulators = scenario.GetActions().size();

    ASSERT_THAT(numberOfEventDetectors, 2);
    ASSERT_THAT(numberOfManipulators, 2);

}
