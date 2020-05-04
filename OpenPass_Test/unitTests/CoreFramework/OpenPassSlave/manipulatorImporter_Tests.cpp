/*******************************************************************************
* Copyright (c) 2019, 2020 in-tech GmbH
*               2020 BMW AG
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
using ::testing::ElementsAre;

openScenario::Parameters EMPTY_PARAMETERS{};

TEST(ManipulatorImporter, SuccessfullyImportsUserDefinedCommandAction_ComponentStateChange)
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
    const std::string eventName{"ActivateTFEvent"};
    openScenario::Parameters parameters;

    std::shared_ptr<ScenarioActionInterface> action = ManipulatorImporter::ImportManipulator(fakeEventElement,
                                                                                             eventName,
                                                                                             testing::DontCare<std::string>(),
                                                                                             parameters);

    std::shared_ptr<openScenario::UserDefinedCommandAction> castedAction = std::dynamic_pointer_cast<openScenario::UserDefinedCommandAction>(action);

    ASSERT_NE(castedAction, nullptr);
    EXPECT_EQ(castedAction->GetCommand(), expectedCommand);
    EXPECT_EQ(castedAction->GetEventName(), eventName);
}

TEST(ManipulatorImporter, SuccessfullyImportsUserDefinedCommandAction_GazeFollower)
{
    const std::string expectedCommand = "SetGazeFollower Active GazeFollowerFileName.csv";
    QDomElement fakeEventElement = documentRootFromString(
                "<Event name=\"ActiveGFEvent\" priority=\"overwrite\">"
                "	<Action name=\"GazeFollower\">"
                "		<UserDefined>"
                "			<Command>"
                "				" + expectedCommand + ""
                "			</Command>"
                "		</UserDefined>"
                "	</Action>"
                "</Event>"
    );

    const std::string eventName{"ActiveGFEvent"};

    std::shared_ptr<ScenarioActionInterface> action = ManipulatorImporter::ImportManipulator(fakeEventElement,
                                                                                             eventName,
                                                                                             testing::DontCare<std::string>(),
                                                                                             EMPTY_PARAMETERS);

    std::shared_ptr<openScenario::UserDefinedCommandAction> castedAction = std::dynamic_pointer_cast<openScenario::UserDefinedCommandAction>(action);

    ASSERT_NE(castedAction, nullptr);
    EXPECT_EQ(castedAction->GetCommand(), expectedCommand);
    EXPECT_EQ(castedAction->GetEventName(), eventName);
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

    openScenario::Parameters parameters;

    std::shared_ptr<ScenarioActionInterface> action = ManipulatorImporter::ImportManipulator(fakeEventElement,
                                                                                             testing::DontCare<std::string>(),
                                                                                             testing::DontCare<std::string>(),
                                                                                             parameters);

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

    openScenario::Parameters parameters;

    std::shared_ptr<ScenarioActionInterface> action = ManipulatorImporter::ImportManipulator(fakeEventElement,
                                                                                             testing::DontCare<std::string>(),
                                                                                             testing::DontCare<std::string>(),
                                                                                             parameters);

    std::shared_ptr<openScenario::GlobalEntityAction> castedAction = std::dynamic_pointer_cast<openScenario::GlobalEntityAction>(action);

    ASSERT_NE(castedAction, nullptr);
    EXPECT_EQ(castedAction->GetName(), expectedEntityName);
    EXPECT_EQ(castedAction->GetType(), openScenario::GlobalEntityActionType::Add);
}

TEST(ManipulatorImporter, SuccessfullyImportsPrivateLateralLaneChangeAbsoluteAction)
{
    const int expectedLaneOffset = -1;
    QDomElement fakeEventElement = documentRootFromString(
                "<Event name=\"LaneChangeEvent\" priority=\"overwrite\">"
                "	<Action name=\"LaneChangeAction\">"
                "		<Private>"
                "			<Lateral>"
                "				<LaneChange>"
                "					<Dynamics time=\"2.0\" shape=\"sinusoidal\" />"
                "					<Target>"
                "						<Absolute value=\"" + std::to_string(expectedLaneOffset) + "\" />"
                "					</Target>"
                "				</LaneChange>"
                "			</Lateral>"
                "		</Private>"
                "	</Action>"
                "</Event>"
    );

    openScenario::Parameters parameters;

    std::shared_ptr<ScenarioActionInterface> action = ManipulatorImporter::ImportManipulator(fakeEventElement,
                                                                                             testing::DontCare<std::string>(),
                                                                                             testing::DontCare<std::string>(),
                                                                                             parameters);

    std::shared_ptr<openScenario::PrivateLateralLaneChangeAction> castedAction = std::dynamic_pointer_cast<openScenario::PrivateLateralLaneChangeAction>(action);

    ASSERT_NE(castedAction, nullptr);
    EXPECT_EQ(castedAction->GetLaneChangeParameter().type, openScenario::LaneChangeParameter::Type::Absolute);
    EXPECT_EQ(castedAction->GetLaneChangeParameter().value, expectedLaneOffset);
    EXPECT_EQ(castedAction->GetLaneChangeParameter().object, "");
    EXPECT_EQ(castedAction->GetLaneChangeParameter().dynamicsType, openScenario::LaneChangeParameter::DynamicsType::Time);
    EXPECT_EQ(castedAction->GetLaneChangeParameter().dynamicsTarget, 2.0);
}

TEST(ManipulatorImporter, SuccessfullyImportsPrivateLateralLaneChangeRelativeAction)
{
    const std::string expectedObject{"TestObject"};
    const int expectedLaneOffset = -1;
    QDomElement fakeEventElement = documentRootFromString(
                "<Event name=\"LaneChangeEvent\" priority=\"overwrite\">"
                "	<Action name=\"LaneChangeAction\">"
                "		<Private>"
                "			<Lateral>"
                "				<LaneChange>"
                "					<Dynamics distance=\"100.0\" shape=\"sinusoidal\" />"
                "					<Target>"
                "						<Relative object=\"" + expectedObject + "\" value=\"" + std::to_string(expectedLaneOffset) + "\" />"
                "					</Target>"
                "				</LaneChange>"
                "			</Lateral>"
                "		</Private>"
                "	</Action>"
                "</Event>"
    );

    openScenario::Parameters parameters;

    std::shared_ptr<ScenarioActionInterface> action = ManipulatorImporter::ImportManipulator(fakeEventElement,
                                                                                             testing::DontCare<std::string>(),
                                                                                             testing::DontCare<std::string>(),
                                                                                             parameters);

    std::shared_ptr<openScenario::PrivateLateralLaneChangeAction> castedAction = std::dynamic_pointer_cast<openScenario::PrivateLateralLaneChangeAction>(action);

    ASSERT_NE(castedAction, nullptr);
    EXPECT_EQ(castedAction->GetLaneChangeParameter().type, openScenario::LaneChangeParameter::Type::Relative);
    EXPECT_EQ(castedAction->GetLaneChangeParameter().value, expectedLaneOffset);
    EXPECT_EQ(castedAction->GetLaneChangeParameter().object, expectedObject);
    EXPECT_EQ(castedAction->GetLaneChangeParameter().dynamicsType, openScenario::LaneChangeParameter::DynamicsType::Distance);
    EXPECT_EQ(castedAction->GetLaneChangeParameter().dynamicsTarget, 100.0);
}

TEST(ManipulatorImporter, SuccessfullyImportsPrivateFollowTrajectoryAction)
{
    QDomElement fakeEventElement = documentRootFromString(
                "<Event name=\"TrajectoryEvent\" priority=\"overwrite\">"
                "	<Action name=\"TrajectoryAction\">"
                "		<Private>"
                "			<Routing>"
                "				<FollowTrajectory>"
                "					<Trajectory name=\"MyTrajectory\" closed=\"false\" domain=\"time\">"
                "                       <Vertex reference=\"1.0\">"
                "                           <Position>"
                "                               <World x=\"-0.1\" y=\"0.2\" z=\"0\" h=\"0.3\" p=\"0\" r=\"0\" />"
                "                           </Position>"
                "                           <Shape>"
                "                               <Polyline />"
                "                           </Shape>"
                "                       </Vertex>"
                "                       <Vertex reference=\"10.0\">"
                "                           <Position>"
                "                               <World x=\"0.4\" y=\"-0.5\" z=\"0\" h=\"-0.6\" p=\"0\" r=\"0\" />"
                "                           </Position>"
                "                           <Shape>"
                "                               <Polyline />"
                "                           </Shape>"
                "                       </Vertex>"
                "					</Trajectory/>"
                "					<Longitudinal>"
                "						<None/>"
                "					</Longitudinal>"
                "					<Lateral/>"
                "				</FollowTrajectory>"
                "			</Routing>"
                "		</Private>"
                "	</Action>"
                "</Event>"
    );

    openScenario::Parameters parameters;

    std::shared_ptr<ScenarioActionInterface> action = ManipulatorImporter::ImportManipulator(fakeEventElement,
                                                                                             testing::DontCare<std::string>(),
                                                                                             testing::DontCare<std::string>(),
                                                                                             parameters);

    std::shared_ptr<openScenario::PrivateFollowTrajectoryAction> castedAction = std::dynamic_pointer_cast<openScenario::PrivateFollowTrajectoryAction>(action);

    ASSERT_NE(castedAction, nullptr);
    const auto& trajectory = castedAction->GetTrajectory();
    ASSERT_EQ(trajectory.name, "MyTrajectory");
    ASSERT_THAT(trajectory.points, ElementsAre(openScenario::TrajectoryPoint{1.0, -0.1, 0.2, 0.3},
                                               openScenario::TrajectoryPoint{10.0, 0.4, -0.5, -0.6}));
}
