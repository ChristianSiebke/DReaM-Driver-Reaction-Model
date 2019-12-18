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

#include "common/gtest/dontCare.h"
#include "common/helper/importerHelper.h"
#include "scenario.h"
#include "scenarioImporter.h"
#include "fakeScenario.h"

using namespace Configuration;
using namespace Importer;

using ::testing::Eq;
using ::testing::StrEq;
using ::testing::DontCare;
using ::testing::ElementsAre;

TEST(ScenarioImporter_UnitTests, ImportPositionElementLaneWithStocastics)
{
    QDomElement positionElement = documentRootFromString(
              "<Position>"
                     "<Lane roadId=\"RoadId1\" s=\"1470.0\" laneId=\"-4\" offset=\"0.5\" > "
                    "<Stochastics value=\"s\"  stdDeviation =\"5\" lowerBound = \"95\" upperBound=\"105\"/>"
                    "<Stochastics value=\"offset\" stdDeviation =\"4\" lowerBound = \"44\" upperBound=\"54\"/>"
                     "</Lane>"
                    "</Position>"
              );

    ScenarioEntity scenarioEntity;

    EXPECT_NO_THROW(ScenarioImporter::ImportPositionElement(scenarioEntity,positionElement));

    ASSERT_DOUBLE_EQ(scenarioEntity.spawnInfo.ILane,-4);
    ASSERT_DOUBLE_EQ(scenarioEntity.spawnInfo.TStart,0);

    ASSERT_DOUBLE_EQ(scenarioEntity.spawnInfo.s.value,1470.0);
    ASSERT_TRUE(scenarioEntity.spawnInfo.s.isStochastic);
    ASSERT_DOUBLE_EQ(scenarioEntity.spawnInfo.s.mean,1470.0);
    ASSERT_DOUBLE_EQ(scenarioEntity.spawnInfo.s.lowerBoundary,95);
    ASSERT_DOUBLE_EQ(scenarioEntity.spawnInfo.s.upperBoundary,105);
    ASSERT_DOUBLE_EQ(scenarioEntity.spawnInfo.s.stdDeviation,5);

    ASSERT_DOUBLE_EQ(scenarioEntity.spawnInfo.offset.value,0.5);
    ASSERT_TRUE(scenarioEntity.spawnInfo.offset.isStochastic);
    ASSERT_DOUBLE_EQ(scenarioEntity.spawnInfo.offset.mean,0.5);
    ASSERT_DOUBLE_EQ(scenarioEntity.spawnInfo.offset.lowerBoundary,44);
    ASSERT_DOUBLE_EQ(scenarioEntity.spawnInfo.offset.upperBoundary,54);
    ASSERT_DOUBLE_EQ(scenarioEntity.spawnInfo.offset.stdDeviation,4);
}

TEST(ScenarioImporter_UnitTests, ImportPositionElementLaneWithOrientation)
{
    QDomElement positionElement = documentRootFromString(
              "<Position>"
                     "<Lane roadId=\"RoadId1\" s=\"1470.0\" laneId=\"-4\" offset=\"0.5\" > "
                     "<Orientation type=\"relative\" h=\"1.57\"/>"
                     "</Lane>"
                    "</Position>"
              );

    ScenarioEntity scenarioEntity;

    EXPECT_NO_THROW(ScenarioImporter::ImportPositionElement(scenarioEntity,positionElement));


    ASSERT_DOUBLE_EQ(scenarioEntity.spawnInfo.s.value, 1470.0);
    ASSERT_DOUBLE_EQ(scenarioEntity.spawnInfo.ILane, -4);
    ASSERT_DOUBLE_EQ(scenarioEntity.spawnInfo.TStart, 0);
    ASSERT_DOUBLE_EQ(scenarioEntity.spawnInfo.offset.value,0.5);

    ASSERT_DOUBLE_EQ(scenarioEntity.spawnInfo.heading,1.57);
}


TEST(ScenarioImporter_UnitTests, ImportPositionElementLane)
{
    QDomElement positionElement = documentRootFromString(
              "<Position>"
                    "<Lane roadId=\"RoadId1\" s=\"1470.0\" laneId=\"-4\" offset=\"0.5\" /> "
               "</Position>"
              );

    ScenarioEntity scenarioEntity;

    EXPECT_NO_THROW(ScenarioImporter::ImportPositionElement(scenarioEntity,positionElement));

    ASSERT_DOUBLE_EQ(scenarioEntity.spawnInfo.s.value, 1470.0);
    ASSERT_DOUBLE_EQ(scenarioEntity.spawnInfo.ILane, -4);
    ASSERT_DOUBLE_EQ(scenarioEntity.spawnInfo.TStart, 0);
    ASSERT_DOUBLE_EQ(scenarioEntity.spawnInfo.offset.value, 0.5);
}

TEST(ScenarioImporter_UnitTests, ImportLongitudinalWithStochastics)
{
    QDomElement positionElement = documentRootFromString(
        "<Longitudinal>"
            "<Speed>"
                "<Dynamics rate=\"0\" />"
                "<Target>"
                    "<Absolute value=\"27.7\" />"
                "</Target>"
                "<Stochastics value=\"velocity\"  stdDeviation =\"3\" lowerBound = \"12\" upperBound=\"40.0\"/>"
                "<Stochastics value=\"rate\" stdDeviation =\"4\" lowerBound = \"0\" upperBound=\"4\"/>"
            "</Speed>"
        "</Longitudinal>"
    );

    ScenarioEntity scenarioEntity;

    EXPECT_NO_THROW(ScenarioImporter::ImportLongitudinalElement(scenarioEntity, positionElement));

    SpawnAttribute velocity = scenarioEntity.spawnInfo.velocity;
    ASSERT_DOUBLE_EQ(velocity.value, 27.7);
    ASSERT_DOUBLE_EQ(velocity.mean, 27.7);
    ASSERT_DOUBLE_EQ(velocity.stdDeviation, 3.0);
    ASSERT_DOUBLE_EQ(velocity.lowerBoundary, 12.0);
    ASSERT_DOUBLE_EQ(velocity.upperBoundary, 40.0);
    ASSERT_TRUE(velocity.isStochastic);

    SpawnAttribute acceleration = scenarioEntity.spawnInfo.acceleration;
    ASSERT_DOUBLE_EQ(acceleration.value, 0.0);
    ASSERT_DOUBLE_EQ(acceleration.mean, 0.0);
    ASSERT_DOUBLE_EQ(acceleration.stdDeviation, 4.0);
    ASSERT_DOUBLE_EQ(acceleration.lowerBoundary, 0.0);
    ASSERT_DOUBLE_EQ(acceleration.upperBoundary, 4.0);
    ASSERT_TRUE(acceleration.isStochastic);
}

std::ostream& operator<<(std::ostream& os, const RouteElement& obj)
{
    return os
            << "road " << obj.roadId << " "
            << (obj.inRoadDirection ? "in" : "against") << " OpenDrive direction";
}

TEST(ScenarioImporter_UnitTests, ImportRoutingElement)
{
    QDomElement routingElement = documentRootFromString(
              "<Routing>"
                    "<FollowRoute>"
                        "<Route>"
                            "<Waypoint>"
                                "<Position>"
                                    "<Road roadId=\"RoadId1\" t=\"-1.0\" />"
                                "</Position>"
                            "</Waypoint>"
                            "<Waypoint>"
                                "<Position>"
                                    "<Road roadId=\"RoadId2\" t=\"1.0\" />"
                                "</Position>"
                            "</Waypoint>"
                            "<Waypoint>"
                                "<Position>"
                                    "<Road roadId=\"RoadId3\" t=\"-1.0\" />"
                                "</Position>"
                            "</Waypoint>"
                        "</Route>"
                    "</FollowRoute>"
               "</Routing>"
              );

    ScenarioEntity scenarioEntity;

    ScenarioImporter::ImportRoutingElement(scenarioEntity, routingElement);

    auto route = scenarioEntity.spawnInfo.route;
    ASSERT_THAT(route.has_value(), Eq(true));
    ASSERT_THAT(route->roads, ElementsAre(RouteElement{"RoadId1", true}, RouteElement{"RoadId2", false}, RouteElement{"RoadId3", true}));
}

TEST(ScenarioImporter_UnitTests, ImportVehicleCatalog_ReturnsSuccess)
{
    std::string catalogPath{};

    QDomElement catalogsElement = documentRootFromString(
              "<Catalogs>"
                    "<VehicleCatalog name=\"vcat\">"
                        "<Directory path=\"vpath\"/>"
                    "</VehicleCatalog>"
                    "<PedestrianCatalog name=\"pcat\">"
                        "<Directory path=\"ppath\"/>"
                    "</PedestrianCatalog>"
              "</Catalogs>"
              );

    ASSERT_NO_THROW(catalogPath = ScenarioImporter::ImportCatalog("VehicleCatalog", catalogsElement));
    EXPECT_THAT(catalogPath, StrEq("vpath"));
}

TEST(ScenarioImporter_UnitTests, ImportPedestrianCatalog_ReturnsSuccess)
{
    std::string catalogPath{};

    QDomElement catalogsElement = documentRootFromString(
              "<Catalogs>"
                    "<VehicleCatalog name=\"vcat\">"
                        "<Directory path=\"vpath\"/>"
                    "</VehicleCatalog>"
                    "<PedestrianCatalog name=\"pcat\">"
                        "<Directory path=\"ppath\"/>"
                    "</PedestrianCatalog>"
              "</Catalogs>"
              );

    ASSERT_NO_THROW(catalogPath = ScenarioImporter::ImportCatalog("PedestrianCatalog", catalogsElement));
    EXPECT_THAT(catalogPath, StrEq("ppath"));
}

TEST(ScenarioImporter_UnitTests, ImportStoryboardWithoutEndCondition_Throws)
{
    QDomElement storyboardRootElement = documentRootFromString(
                "<root>"
                "	<Storyboard>"
                "		<Init>"
                "			<Actions>"
                "			</Actions>"
                "		</Init>"
                "	</Storyboard>"
                "</root>"
    );
    std::vector<ScenarioEntity> entities;
    FakeScenario mockScenario;

    EXPECT_THROW(ScenarioImporter::ImportStoryboard(storyboardRootElement, entities, &mockScenario), std::runtime_error);
}

TEST(ScenarioImporter_UnitTests, ImportStoryboardWithEndCondition_SetsScenarioEndTime)
{
    QDomElement storyboardRootElement = documentRootFromString(
                "<root>"
                "	<Storyboard>"
                "		<Init>"
                "			<Actions>"
                "			</Actions>"
                "		</Init>"
                "		<EndConditions>"
                "			<ConditionGroup>"
                "				<Condition name=\"\" delay=\"0.0\" edge=\"rising\">"
                "					<ByValue>"
                "						<SimulationTime value=\"3.000\" rule=\"greater_than\" />"
                "					</ByValue>"
                "				</Condition>"
                "			</ConditionGroup>"
                "		</EndConditions>"
                "	</Storyboard>"
                "</root>"
    );
    std::vector<ScenarioEntity> entities;
    FakeScenario mockScenario;
    EXPECT_CALL(mockScenario, SetEndTime(3.000)).Times(1);

    EXPECT_NO_THROW(ScenarioImporter::ImportStoryboard(storyboardRootElement, entities, &mockScenario));
}

TEST(ScenarioImporter_UnitTests, ImportStoryboardWithInvalidEndCondition_Throws)
{
    QDomElement storyboardRootElementConditionMissingName = documentRootFromString(
                "<root>"
                "	<Storyboard>"
                "		<Init>"
                "			<Actions>"
                "			</Actions>"
                "		</Init>"
                "		<EndConditions>"
                "			<ConditionGroup>"
                "				<Condition delay=\"0.0\" edge=\"rising\">"
                "					<ByValue>"
                "						<SimulationTime value=\"3.000\" rule=\"greater_than\" />"
                "					</ByValue>"
                "				</Condition>"
                "			</ConditionGroup>"
                "		</EndConditions>"
                "	</Storyboard>"
                "</root>"
    );

    QDomElement storyboardRootElementConditionMissingDelay = documentRootFromString(
                "<root>"
                "	<Storyboard>"
                "		<Init>"
                "			<Actions>"
                "			</Actions>"
                "		</Init>"
                "		<EndConditions>"
                "			<ConditionGroup>"
                "				<Condition name=\"\" edge=\"rising\">"
                "					<ByValue>"
                "						<SimulationTime value=\"3.000\" rule=\"greater_than\" />"
                "					</ByValue>"
                "				</Condition>"
                "			</ConditionGroup>"
                "		</EndConditions>"
                "	</Storyboard>"
                "</root>"
    );

    QDomElement storyboardRootElementConditionMissingEdge = documentRootFromString(
                "<root>"
                "	<Storyboard>"
                "		<Init>"
                "			<Actions>"
                "			</Actions>"
                "		</Init>"
                "		<EndConditions>"
                "			<ConditionGroup>"
                "				<Condition name=\"\" delay=\"0.0\">"
                "					<ByValue>"
                "						<SimulationTime value=\"3.000\" rule=\"greater_than\" />"
                "					</ByValue>"
                "				</Condition>"
                "			</ConditionGroup>"
                "		</EndConditions>"
                "	</Storyboard>"
                "</root>"
    );

    QDomElement storyboardRootElementConditionDelayNegative = documentRootFromString(
                "<root>"
                "	<Storyboard>"
                "		<Init>"
                "			<Actions>"
                "			</Actions>"
                "		</Init>"
                "		<EndConditions>"
                "			<ConditionGroup>"
                "				<Condition name=\"\" delay=\"-1.0\" edge=\"rising\">"
                "					<ByValue>"
                "						<SimulationTime value=\"3.000\" rule=\"greater_than\" />"
                "					</ByValue>"
                "				</Condition>"
                "			</ConditionGroup>"
                "		</EndConditions>"
                "	</Storyboard>"
                "</root>"
    );

    std::vector<ScenarioEntity> entities;
    FakeScenario mockScenario;

    EXPECT_THROW(ScenarioImporter::ImportStoryboard(storyboardRootElementConditionMissingName, entities, &mockScenario), std::runtime_error);
    EXPECT_THROW(ScenarioImporter::ImportStoryboard(storyboardRootElementConditionMissingDelay, entities, &mockScenario), std::runtime_error);
    EXPECT_THROW(ScenarioImporter::ImportStoryboard(storyboardRootElementConditionMissingEdge, entities, &mockScenario), std::runtime_error);
    EXPECT_THROW(ScenarioImporter::ImportStoryboard(storyboardRootElementConditionDelayNegative, entities, &mockScenario), std::runtime_error);
}
