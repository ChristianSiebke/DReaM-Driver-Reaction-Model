/********************************************************************************
 * Copyright (c) 2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <filesystem>

#include "importer/simulationConfig.h"
#include "importer/simulationConfigImporter.h"

using ::testing::NiceMock;
using ::testing::Eq;

using namespace Importer;
using namespace Configuration;

TEST(SimulationConfigImporter, ImportInvalidSpawnPoint_Fails)
{
    const std::filesystem::path testSimulationConfigDir = std::filesystem::current_path() / "Resources" / "ImporterTest";
    const std::filesystem::path testSimulationConfigFile = "invalidSpawnPointSimulationConfig.xml";

    SimulationConfig config;
    SimulationConfigImporter importer;

    EXPECT_THAT(importer.Import(testSimulationConfigDir.string(), testSimulationConfigFile.string(), config), false);
}

TEST(SimulationConfigImporter, ImportValidSpawnPoint_Succeeds)
{
    const std::filesystem::path testSimulationConfigDir = std::filesystem::current_path() / "Resources" / "ImporterTest";
    const std::filesystem::path testSimulationConfigFile = testSimulationConfigDir / "validSpawnPointSimulationConfig.xml";

    SimulationConfig config;
    SimulationConfigImporter importer;

    const auto result = importer.Import(testSimulationConfigDir.string(), testSimulationConfigFile.string(), config);

    ASSERT_THAT(result, true);
}
