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

#include "common/helper/importerHelper.h"
#include "Common/openScenarioDefinitions.h"

#include "dontCare.h"
#include "fakeParameter.h"

#include "manipulatorImporter.h"

using Importer::ManipulatorImporter;

TEST(ManipulatorImporter, SuccessfullyImportsUserDefinedCommandAction)
{
    const std::string expectedCommand = "SetComponentState DynamicsTrajectoryFollower Acting";
    QDomElement fakeEventElement = documentRootFromString(
                "<Event name=\"ActivateTFEvent\" priority=\"overwrite\">"
                "	<Action name=\"ActivateTFAction\">"
                "		<UserDefined>"
                "			<Command>"
                "				" + expectedCommand + ""
                "			</Command>"
                "		</UserDefined>"
                "	</Action>"
                "</Event>"
    );

    const std::vector<std::string> eventDetectorNames{"TestEventDetectorA, TestEventDetectorB"};
    const std::string sequenceName{"TestSequence"};

    std::shared_ptr<ScenarioActionInterface> action = ManipulatorImporter::ImportManipulator(fakeEventElement,
                                                                                             sequenceName);

    std::shared_ptr<openScenario::UserDefinedCommandAction> castedAction = std::dynamic_pointer_cast<openScenario::UserDefinedCommandAction>(action);

    ASSERT_NE(castedAction, nullptr);
    EXPECT_EQ(castedAction->GetCommand(), expectedCommand);
    EXPECT_EQ(castedAction->GetSequenceName(), sequenceName);
}

TEST(ManipulatorImporter, SuccessfullyImportsGlobalEntityDeleteAction)
{
    const std::string expectedEntityName = "TestEntity";
    QDomElement fakeEventElement = documentRootFromString(
                "<Event name=\"RemoveAgentsEvent\" priority=\"overwrite\">"
                "	<Action name=\"RemoveAgentsAction\">"
                "		<Global>"
                "			<Entity name=\"" + expectedEntityName + "\">"
                "				<Delete />"
                "			</Entity>"
                "		</Global>"
                "	</Action>"
                "</Event>"
    );

    std::shared_ptr<ScenarioActionInterface> action = ManipulatorImporter::ImportManipulator(fakeEventElement,
                                                                                             testing::DontCare<std::string>());

    std::shared_ptr<openScenario::GlobalEntityAction> castedAction = std::dynamic_pointer_cast<openScenario::GlobalEntityAction>(action);

    ASSERT_NE(castedAction, nullptr);
    EXPECT_EQ(castedAction->GetName(), expectedEntityName);
    EXPECT_EQ(castedAction->GetType(), openScenario::GlobalEntityActionType::Delete);
}

TEST(ManipulatorImporter, SuccessfullyImportsGlobalEntityAddAction)
{
    const std::string expectedEntityName = "TestEntity";
    QDomElement fakeEventElement = documentRootFromString(
                "<Event name=\"RemoveAgentsEvent\" priority=\"overwrite\">"
                "	<Action name=\"RemoveAgentsAction\">"
                "		<Global>"
                "			<Entity name=\"" + expectedEntityName + "\">"
                "				<Add>"
                "					<Position>"
                "						<World x=\"0\" y=\"0\" />"
                "					</Position>"
                "				</Add>"
                "			</Entity>"
                "		</Global>"
                "	</Action>"
                "</Event>"
    );

    std::shared_ptr<ScenarioActionInterface> action = ManipulatorImporter::ImportManipulator(fakeEventElement,
                                                                                             testing::DontCare<std::string>());

    std::shared_ptr<openScenario::GlobalEntityAction> castedAction = std::dynamic_pointer_cast<openScenario::GlobalEntityAction>(action);

    ASSERT_NE(castedAction, nullptr);
    EXPECT_EQ(castedAction->GetName(), expectedEntityName);
    EXPECT_EQ(castedAction->GetType(), openScenario::GlobalEntityActionType::Add);
}

TEST(ManipulatorImporter, SuccessfullyImportsPrivateLateralLaneChangeAbsoluteAction)
{
    const double expectedLaneOffset = -1;
    QDomElement fakeEventElement = documentRootFromString(
                "<Event name=\"LaneChangeEvent\" priority=\"overwrite\">"
                "	<Action name=\"LaneChangeAction\">"
                "		<Private>"
                "			<Lateral>"
                "				<LaneChange>"
                "					<Dynamics shape=\"linear\" />"
                "					<Target>"
                "						<Absolute value=\"" + std::to_string(expectedLaneOffset) + "\" />"
                "					</Target>"
                "				</LaneChange>"
                "			</Lateral>"
                "		</Private>"
                "	</Action>"
                "</Event>"
    );

    std::shared_ptr<ScenarioActionInterface> action = ManipulatorImporter::ImportManipulator(fakeEventElement,
                                                                                             testing::DontCare<std::string>());

    std::shared_ptr<openScenario::PrivateLateralLaneChangeAction> castedAction = std::dynamic_pointer_cast<openScenario::PrivateLateralLaneChangeAction>(action);

    ASSERT_NE(castedAction, nullptr);
    EXPECT_EQ(castedAction->GetType(), openScenario::PrivateLateralLaneChangeActionType::Absolute);
    EXPECT_EQ(castedAction->GetValue(), static_cast<int>(std::rint(expectedLaneOffset)));
    EXPECT_EQ(castedAction->GetObject(), "");
}

TEST(ManipulatorImporter, SuccessfullyImportsPrivateLateralLaneChangeRelativeAction)
{
    const std::string expectedObject{"TestObject"};
    const double expectedLaneOffset = -1;
    QDomElement fakeEventElement = documentRootFromString(
                "<Event name=\"LaneChangeEvent\" priority=\"overwrite\">"
                "	<Action name=\"LaneChangeAction\">"
                "		<Private>"
                "			<Lateral>"
                "				<LaneChange>"
                "					<Dynamics shape=\"linear\" />"
                "					<Target>"
                "						<Relative object=\"" + expectedObject + "\" value=\"" + std::to_string(expectedLaneOffset) + "\" />"
                "					</Target>"
                "				</LaneChange>"
                "			</Lateral>"
                "		</Private>"
                "	</Action>"
                "</Event>"
    );

    std::shared_ptr<ScenarioActionInterface> action = ManipulatorImporter::ImportManipulator(fakeEventElement,
                                                                                             testing::DontCare<std::string>());

    std::shared_ptr<openScenario::PrivateLateralLaneChangeAction> castedAction = std::dynamic_pointer_cast<openScenario::PrivateLateralLaneChangeAction>(action);

    ASSERT_NE(castedAction, nullptr);
    EXPECT_EQ(castedAction->GetType(), openScenario::PrivateLateralLaneChangeActionType::Relative);
    EXPECT_EQ(castedAction->GetValue(), static_cast<int>(std::rint(expectedLaneOffset)));
    EXPECT_EQ(castedAction->GetObject(), expectedObject);
}
