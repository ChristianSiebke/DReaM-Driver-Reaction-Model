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

#include "importer/slaveConfig.h"
#include "importer/slaveConfigImporter.h"

using ::testing::NiceMock;
using ::testing::Eq;

using namespace Importer;
using namespace Configuration;

TEST(SlaveConfigImporter, ImportInvalidSpawnPoint_Fails)
{
    const boost::filesystem::path testSlaveConfigDir = boost::filesystem::initial_path() / "Resources" / "ImporterTest";
    const boost::filesystem::path testSlaveConfigFile = "invalidSpawnPointSlaveConfig.xml";

    SlaveConfig config;
    SlaveConfigImporter importer;

    EXPECT_THAT(importer.Import(testSlaveConfigDir.string(), testSlaveConfigFile.string(), config), false);
}

TEST(SlaveConfigImporter, ImportValidSpawnPoint_Succeeds)
{
    const boost::filesystem::path testSlaveConfigDir = boost::filesystem::initial_path() / "Resources" / "ImporterTest";
    const boost::filesystem::path testSlaveConfigFile = testSlaveConfigDir / "validSpawnPointSlaveConfig.xml";

    SlaveConfig config;
    SlaveConfigImporter importer;

    const auto result = importer.Import(testSlaveConfigDir.string(), testSlaveConfigFile.string(), config);

    ASSERT_THAT(result, true);
}
