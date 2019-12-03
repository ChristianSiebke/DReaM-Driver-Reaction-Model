/*********************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <boost/filesystem.hpp>

#include "scenario.h"
#include "scenarioImporter.h"
#include "fakeScenario.h"

using ::testing::NiceMock;
using ::testing::Eq;

using namespace Configuration;
using namespace Importer;

TEST(ScenarioImporter_IntegrationTests, ImportScenario_WithWrongInternalVersion_Fails)
{
    const boost::filesystem::path testScenarioFile = boost::filesystem::initial_path() / "Resources" / "ImporterTest" / "WrongVersionScenario.xosc";
    Scenario scenario;
    ScenarioImporter importer;

    ASSERT_FALSE(importer.Import(testScenarioFile.string(), &scenario));
}

TEST(ScenarioImporter_IntegrationTests, ImportScenario_WithoutInternalVersion_Fails)
{
    const boost::filesystem::path testScenarioFile = boost::filesystem::initial_path() / "Resources" / "ImporterTest" / "NoVersionScenario.xosc";
    Scenario scenario;
    ScenarioImporter importer;

    ASSERT_FALSE(importer.Import(testScenarioFile.string(), &scenario));
}

TEST(ScenarioImporter_IntegrationTests, ImportScenario_WithMinimalInfoForOpenPass_Succeeds)
{
    const boost::filesystem::path testScenarioFile = boost::filesystem::initial_path() / "Resources" / "ImporterTest" / "Scenario.xosc";
    Scenario scenario;

    ScenarioImporter importer;
    ASSERT_TRUE(importer.Import(testScenarioFile.string(), &scenario));

    ScenarioEntity resultEgoEntity = scenario.GetEgoEntity();
    SpawnInfo resultSpawnInfoEgo = resultEgoEntity.spawnInfo;

    ASSERT_EQ(resultEgoEntity.name, "Ego");

    ASSERT_EQ(resultSpawnInfoEgo.ILane, -3);
    ASSERT_EQ(resultSpawnInfoEgo.s.value, 1000.0);
    ASSERT_EQ(resultSpawnInfoEgo.TStart, 0.0);
    ASSERT_EQ(resultSpawnInfoEgo.velocity.value, 5.0);
    ASSERT_EQ(resultSpawnInfoEgo.acceleration.value, 0.5);

    auto sceneryEntities = scenario.GetScenarioEntities();
    ASSERT_EQ(sceneryEntities.size(), (size_t) 2);

    ScenarioEntity *resultSceneryEntitySecond = sceneryEntities.at(1);
    SpawnInfo resultSpawnInfoScenery = resultSceneryEntitySecond->spawnInfo;

    ASSERT_EQ(resultSceneryEntitySecond->name, "Scenery2");

    ASSERT_EQ(resultSpawnInfoScenery.ILane, -3);
    ASSERT_EQ(resultSpawnInfoScenery.s.value, 1000.0);
    ASSERT_EQ(resultSpawnInfoScenery.TStart, 0.0);
    ASSERT_EQ(resultSpawnInfoScenery.velocity.value, 5.0);
    ASSERT_EQ(resultSpawnInfoScenery.acceleration.value, 0.5);
}

TEST(ScenarioImporter_IntegrationTests, ImportScenario_WithMoreInfoThanOpenPassNeeds_Succeeds)
{
    const boost::filesystem::path testScenarioFile = boost::filesystem::initial_path() / "Resources" / "ImporterTest" / "OverheadScenario.xosc";
    Scenario scenario;

    ScenarioImporter importer;
    ASSERT_TRUE(importer.Import(testScenarioFile.string(), &scenario));

    ScenarioEntity resultEgoEntity = scenario.GetEgoEntity();
    SpawnInfo resultSpawnInfoEgo = resultEgoEntity.spawnInfo;

    ASSERT_EQ(resultEgoEntity.name, "Ego");

    ASSERT_EQ(resultSpawnInfoEgo.ILane, -3);
    ASSERT_EQ(resultSpawnInfoEgo.s.value, 1000.0);
    ASSERT_EQ(resultSpawnInfoEgo.TStart, 0.0);
    ASSERT_EQ(resultSpawnInfoEgo.velocity.value, 5.0);
    ASSERT_EQ(resultSpawnInfoEgo.acceleration.value, 0.5);

    auto sceneryEntities = scenario.GetScenarioEntities();
    ASSERT_EQ(sceneryEntities.size(), (size_t) 2);

    ScenarioEntity *resultSceneryEntitySecond = sceneryEntities.at(1);
    SpawnInfo resultSpawnInfoScenery = resultSceneryEntitySecond->spawnInfo;

    ASSERT_EQ(resultSceneryEntitySecond->name, "Scenery2");

    ASSERT_EQ(resultSpawnInfoScenery.ILane, -3);
    ASSERT_EQ(resultSpawnInfoScenery.s.value, 1000.0);
    ASSERT_EQ(resultSpawnInfoScenery.TStart, 0.0);
    ASSERT_EQ(resultSpawnInfoScenery.velocity.value, -999.0);
    ASSERT_EQ(resultSpawnInfoScenery.acceleration.value, -999);
}

TEST(ScenarioImporter_IntegrationTests, ImportScenario_SetsSceneryPath)
{
    const boost::filesystem::path testScenarioFile = boost::filesystem::initial_path() / "Resources" / "ImporterTest" / "Scenario.xosc";
    NiceMock<FakeScenario> scenario;

    EXPECT_CALL(scenario, SetSceneryPath("testSceneryConfiguration.xodr")).Times(1);

    ScenarioImporter importer;
    ASSERT_TRUE(importer.Import(testScenarioFile.string(), &scenario));
}

TEST(ScenarioImporter_IntegrationTests, ImportScenario_SetsCatalogPaths)
{
    const boost::filesystem::path testScenarioFile = boost::filesystem::initial_path() / "Resources" / "ImporterTest" / "Scenario.xosc";
    NiceMock<FakeScenario> scenario;

    EXPECT_CALL(scenario, SetVehicleCatalogPath("VehicleModelsCatalog.xosc")).Times(1);
    EXPECT_CALL(scenario, SetPedestrianCatalogPath("PedestrianModelsCatalog.xosc")).Times(1);

    ScenarioImporter importer;
    ASSERT_TRUE(importer.Import(testScenarioFile.string(), &scenario));
}

TEST(ScenarioImporter_IntegrationTests, ImportScenario_WithMissingRequiredParameters_ThrowsException)
{
    const boost::filesystem::path testScenarioFile = boost::filesystem::initial_path() / "Resources" / "ImporterTest" / "FailingScenario.xosc";
    Scenario scenario;

    ASSERT_FALSE(ScenarioImporter::Import(testScenarioFile.string(), &scenario));
}

TEST(ScenarioImporter_IntegrationTests, ImportScenario_WithReferencedButUndeclaredActorEntity_ThrowsException)
{
    const boost::filesystem::path testScenarioFile = boost::filesystem::initial_path() / "Resources" / "ImporterTest" / "FailingScenarioUndeclaredActorEntity.xosc";
    Scenario scenario;

    ASSERT_FALSE(ScenarioImporter::Import(testScenarioFile.string(), &scenario));
}

TEST(ScenarioImporter_IntegrationTests, ImportScenario_WithReferencedButUndeclaredConditionEntity_ThrowsException)
{
    const boost::filesystem::path testScenarioFile = boost::filesystem::initial_path() / "Resources" / "ImporterTest" / "FailingScenarioUndeclaredConditionEntity.xosc";
    Scenario scenario;

    ASSERT_FALSE(ScenarioImporter::Import(testScenarioFile.string(), &scenario));
}

TEST(ScenarioImporter_IntegrationTests, ImportScenario_WithReferencedButUndeclaredInitPrivateEntity_ThrowsException)
{
    const boost::filesystem::path testScenarioFile = boost::filesystem::initial_path() / "Resources" / "ImporterTest" / "FailingScenarioUndeclaredInitPrivateEntity.xosc";
    Scenario scenario;

    ASSERT_FALSE(ScenarioImporter::Import(testScenarioFile.string(), &scenario));
}
