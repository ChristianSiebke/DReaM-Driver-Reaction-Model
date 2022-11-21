/********************************************************************************
 * Copyright (c) 2019 in-tech GmbH
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
#include <memory>

#include "importer/systemConfig.h"
#include "importer/systemConfigImporter.h"
#include "modelElements/agentType.h"

using namespace Configuration;
using namespace Importer;

TEST(SystemConfigImporter_IntegrationTests, ImportConfigWithTwoSystems_Succeeds)
{
    const std::filesystem::path testScenarioFile = std::filesystem::current_path() / "Resources" / "ImporterTest" / "SystemConfiguration.xml";
    auto systemConfig = std::make_shared<Configuration::SystemConfig>();

    ASSERT_TRUE(Importer::SystemConfigImporter::Import(testScenarioFile.string(), systemConfig));

    const auto& systems = systemConfig->GetSystems();
    ASSERT_EQ(systems.size(), 2);

    const auto& firstSystemComponents = systems.at(0)->GetComponents();
    ASSERT_EQ(firstSystemComponents.size(), 7);
    ASSERT_EQ(firstSystemComponents.at("0")->GetPriority(), 201);
    ASSERT_EQ(firstSystemComponents.at("0")->GetCycleTime(), 10);

    const auto& outputLinksModule0 = firstSystemComponents.at("0")->GetOutputLinks();
    ASSERT_EQ(outputLinksModule0.size(), 4);
    ASSERT_EQ(outputLinksModule0.at(0), 0);
    ASSERT_EQ(outputLinksModule0.at(1), 1);
    ASSERT_EQ(outputLinksModule0.at(4), 3);
    ASSERT_EQ(outputLinksModule0.at(12), 2);

    const auto& inputLinksModule0 = firstSystemComponents.at("0")->GetInputLinks();
    ASSERT_EQ(inputLinksModule0.size(), 0);

    const auto& parametersModule1 = firstSystemComponents.at("1")->GetModelParameters();
    ASSERT_EQ(openpass::parameter::Get<double>(parametersModule1, "0").value(), 50);
    ASSERT_EQ(openpass::parameter::Get<double>(parametersModule1, "2").value(), 0.44);
    ASSERT_EQ(openpass::parameter::Get<double>(parametersModule1, "3").value(), 3.7);

    const auto& inputLinksModule4 = firstSystemComponents.at("4")->GetInputLinks();
    ASSERT_EQ(inputLinksModule4.size(), 4);
    ASSERT_EQ(inputLinksModule4.at(0), 11);
    ASSERT_EQ(inputLinksModule4.at(1), 12);
    ASSERT_EQ(inputLinksModule4.at(2), 13);
    ASSERT_EQ(inputLinksModule4.at(3), 14);

    const auto& secondSystemComponents = systems.at(1)->GetComponents();
    ASSERT_EQ(secondSystemComponents.size(), 1);
    ASSERT_EQ(secondSystemComponents.at("0")->GetPriority(), 0);
    ASSERT_EQ(secondSystemComponents.at("0")->GetCycleTime(), 10);
}
