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

#include "scenarioImporterHelper.h"

using ::testing::ElementsAre;

openScenario::Parameters EMPTY_PARAMETERS{};

TEST(ManipulatorImporter, SuccessfullyImportsUserDefinedCommandAction_ComponentStateChange)
{
    const std::string expectedCommand = "SetComponentState DynamicsTrajectoryFollower Acting";
    QDomElement fakeRoot = documentRootFromString(
                               "<root>"
                                    "<CustomCommandAction>"
                                    + expectedCommand + ""
                                    "</CustomCommandAction>"
                               "</root>"
    );

    const std::vector<std::string> eventDetectorNames{"TestEventDetectorA, TestEventDetectorB"};
    const std::string eventName{"ActivateTFEvent"};
    openScenario::Parameters parameters;

    const auto action = openScenario::ScenarioImporterHelper::ImportUserDefinedAction(fakeRoot);

    ASSERT_TRUE(std::holds_alternative<openScenario::CustomCommandAction>(action));

    EXPECT_EQ(std::get<openScenario::CustomCommandAction>(action).command, expectedCommand);
}

TEST(ManipulatorImporter, SuccessfullyImportsGlobalEntityDeleteAction)
{
    const std::string expectedEntityName = "TestEntity";
    QDomElement fakeRoot = documentRootFromString(
                               "<root>"
                                    "<EntityAction entityRef=\"" + expectedEntityName + "\">"
                                        "<DeleteEntityAction />"
                                    "</EntityAction>"
                               "</root>"
    );

    openScenario::Parameters parameters;

    const auto action = openScenario::ScenarioImporterHelper::ImportGlobalAction(fakeRoot,
                                                                                             parameters);

    ASSERT_TRUE(std::holds_alternative<openScenario::EntityAction>(action));

    const auto entityAction = std::get<openScenario::EntityAction>(action);
    EXPECT_EQ(entityAction.entityRef, expectedEntityName);
    EXPECT_EQ(entityAction.type, openScenario::EntityActionType::Delete);
}

TEST(ManipulatorImporter, SuccessfullyImportsGlobalEntityAddAction)
{
    const std::string expectedEntityName = "TestEntity";
    QDomElement fakeRoot = documentRootFromString(
                               "<root>"
                                    "<EntityAction entityRef=\"" + expectedEntityName + "\">"
                                        "<AddEntityAction>"
                                            "<Position>"
                                                "<World x=\"0\" y=\"0\" />"
                                            "</Position>"
                                        "</AddEntityAction>"
                                    "</EntityAction>"
                                "</root>"
    );

    openScenario::Parameters parameters;

    const auto action = openScenario::ScenarioImporterHelper::ImportGlobalAction(fakeRoot,
                                                                                             parameters);

    ASSERT_TRUE(std::holds_alternative<openScenario::EntityAction>(action));

    const auto entityAction = std::get<openScenario::EntityAction>(action);
    EXPECT_EQ(entityAction.entityRef, expectedEntityName);
    EXPECT_EQ(entityAction.type, openScenario::EntityActionType::Add);
}

TEST(ManipulatorImporter, SuccessfullyImportsPrivateLateralLaneChangeAbsoluteAction)
{
    const int expectedLaneOffset = -1;
    QDomElement fakeEventElement = documentRootFromString(
                                       "<root>"
                                            "<LateralAction>"
                                                "<LaneChangeAction>"
                                                    "<LaneChangeActionDynamics value=\"2.0\" dynamicsShape=\"sinusoidal\" dynamicsDimension=\"time\"/>"
                                                    "<LaneChangeTarget>"
                                                        "<AbsoluteTargetLane value=\"" + std::to_string(expectedLaneOffset) + "\" />"
                                                    "</LaneChangeTarget>"
                                                "</LaneChangeAction>"
                                            "</LateralAction>"
                                       "</root>"
    );

    openScenario::Parameters parameters;

    const auto action = openScenario::ScenarioImporterHelper::ImportPrivateAction(fakeEventElement,
                                                                                             parameters);

    ASSERT_TRUE(std::holds_alternative<openScenario::LateralAction>(action));
    const auto lateralAction = std::get<openScenario::LateralAction>(action);

    ASSERT_TRUE(std::holds_alternative<openScenario::LaneChangeAction>(lateralAction));
    const auto laneChangeAction = std::get<openScenario::LaneChangeAction>(lateralAction);

    EXPECT_EQ(laneChangeAction.laneChangeParameter.type, openScenario::LaneChangeParameter::Type::Absolute);
    EXPECT_EQ(laneChangeAction.laneChangeParameter.value, expectedLaneOffset);
    EXPECT_EQ(laneChangeAction.laneChangeParameter.object, "");
    EXPECT_EQ(laneChangeAction.laneChangeParameter.dynamicsType, openScenario::LaneChangeParameter::DynamicsType::Time);
    EXPECT_EQ(laneChangeAction.laneChangeParameter.dynamicsTarget, 2.0);
}

TEST(ManipulatorImporter, SuccessfullyImportsPrivateLateralLaneChangeRelativeAction)
{
    const std::string expectedObject{"TestObject"};
    const int expectedLaneOffset = -1;
    QDomElement fakeEventElement = documentRootFromString(
                                       "<root>"
                                            "<LateralAction>"
                                                "<LaneChangeAction>"
                                                    "<LaneChangeActionDynamics value=\"100.0\" dynamicsShape=\"sinusoidal\" dynamicsDimension=\"time\" />"
                                                    "<LaneChangeTarget>"
                                                        "<RelativeTargetLane entityRef=\"" + expectedObject + "\" value=\"" + std::to_string(expectedLaneOffset) + "\" />"
                                                    "</LaneChangeTarget>"
                                                "</LaneChangeAction>"
                                            "</LateralAction>"
                                      "</root>"
    );

    openScenario::Parameters parameters;

    const auto action = openScenario::ScenarioImporterHelper::ImportPrivateAction(fakeEventElement,
                                                                                             parameters);

    ASSERT_TRUE(std::holds_alternative<openScenario::LateralAction>(action));
    const auto lateralAction = std::get<openScenario::LateralAction>(action);

    ASSERT_TRUE(std::holds_alternative<openScenario::LaneChangeAction>(lateralAction));
    const auto laneChangeAction = std::get<openScenario::LaneChangeAction>(lateralAction);

    EXPECT_EQ(laneChangeAction.laneChangeParameter.type, openScenario::LaneChangeParameter::Type::Relative);
    EXPECT_EQ(laneChangeAction.laneChangeParameter.value, expectedLaneOffset);
    EXPECT_EQ(laneChangeAction.laneChangeParameter.object, expectedObject);
    EXPECT_EQ(laneChangeAction.laneChangeParameter.dynamicsType, openScenario::LaneChangeParameter::DynamicsType::Time);
    EXPECT_EQ(laneChangeAction.laneChangeParameter.dynamicsTarget, 100.0);
}

TEST(ManipulatorImporter, SuccessfullyImportsPrivateFollowTrajectoryAction)
{
    QDomElement fakeRoot = documentRootFromString(
                               "<root>"
                                "	<RoutingAction>"
                                "		<FollowTrajectoryAction>"
                                "			<Trajectory name=\"MyTrajectory\" closed=\"false\" domain=\"time\">"
                                "                       <Shape>"
                                "                           <Polyline>"
                                "                               <Vertex time=\"1.0\">"
                                "                                   <Position>"
                                "                                       <WorldPosition x=\"-0.1\" y=\"0.2\" z=\"0\" h=\"0.3\" p=\"0\" r=\"0\" />"
                                "                                   </Position>"
                                "                               </Vertex>"
                                "                               <Vertex time=\"10.0\">"
                                "                                   <Position>"
                                "                                       <WorldPosition x=\"0.4\" y=\"-0.5\" z=\"0\" h=\"-0.6\" p=\"0\" r=\"0\" />"
                                "                                   </Position>"
                                "                               </Vertex>"
                                "                           </Polyline>"
                                "                       </Shape>"
                                "			</Trajectory/>"
                                "		</FollowTrajectoryAction>"
                                "	</RoutingAction>"
                               "</root>"
    );

    openScenario::Parameters parameters;

    const auto action = openScenario::ScenarioImporterHelper::ImportPrivateAction(fakeRoot,
                                                                                             parameters);

    ASSERT_TRUE(std::holds_alternative<openScenario::RoutingAction>(action));
    const auto routingAction = std::get<openScenario::RoutingAction>(action);

    ASSERT_TRUE(std::holds_alternative<openScenario::FollowTrajectoryAction>(routingAction));
    const auto followTrajectoryAction = std::get<openScenario::FollowTrajectoryAction>(routingAction);

    ASSERT_EQ(followTrajectoryAction.trajectory.name, "MyTrajectory");
    ASSERT_THAT(followTrajectoryAction.trajectory.points, ElementsAre(openScenario::TrajectoryPoint{1.0, -0.1, 0.2, 0.3},
                                               openScenario::TrajectoryPoint{10.0, 0.4, -0.5, -0.6}));
}
