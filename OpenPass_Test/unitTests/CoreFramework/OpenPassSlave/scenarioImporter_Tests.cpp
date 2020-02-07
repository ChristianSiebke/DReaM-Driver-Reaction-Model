/*******************************************************************************
* Copyright (c) 2019, 2020 in-tech GmbH
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
using ::testing::SizeIs;

TEST(ScenarioImporter_UnitTests, ImportEntity)
{
    QDomElement entityElement = documentRootFromString(
        "<Object name=\"Ego\">"
          "<CatalogReference catalogName=\"ProfilesCatalog.xml\" entryName=\"EgoAgent\">"
              "<ParameterAssignments>"
                  "<ParameterAssignment parameterRef=\"TestParameter\" value=\"6\" />"
              "</ParameterAssignments>"
          "</CatalogReference>"
        "</Object>"
    );

    ScenarioEntity scenarioEntity;
    openScenario::Parameters parameters;

    EXPECT_NO_THROW(ScenarioImporter::ImportEntity(entityElement, scenarioEntity, parameters));

    ASSERT_THAT(scenarioEntity.name, Eq("Ego"));
    ASSERT_THAT(scenarioEntity.catalogReference.catalogName, Eq("ProfilesCatalog.xml"));
    ASSERT_THAT(scenarioEntity.catalogReference.entryName, Eq("EgoAgent"));
    ASSERT_THAT(scenarioEntity.assignedParameters, SizeIs(1));
    ASSERT_THAT(std::get<std::string>(scenarioEntity.assignedParameters.at("TestParameter")), Eq("6"));
}

TEST(ScenarioImporter_UnitTests, ImportPositionElementLaneWithStochastics)
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
    openScenario::Parameters parameters;

    EXPECT_NO_THROW(ScenarioImporter::ImportPositionElement(scenarioEntity, positionElement, parameters));

    const auto& spawnInfo = scenarioEntity.spawnInfo;
    openScenario::LanePosition lanePosition;
    ASSERT_NO_THROW(lanePosition = std::get<openScenario::LanePosition>(spawnInfo.position));

    ASSERT_EQ(lanePosition.laneId,-4);

    ASSERT_DOUBLE_EQ(lanePosition.s,1470.0);
    ASSERT_TRUE(lanePosition.stochasticS.has_value());

    const auto &stochasticS = lanePosition.stochasticS.value();
    ASSERT_DOUBLE_EQ(stochasticS.mean,1470.0);
    ASSERT_DOUBLE_EQ(stochasticS.lowerBoundary,95);
    ASSERT_DOUBLE_EQ(stochasticS.upperBoundary,105);
    ASSERT_DOUBLE_EQ(stochasticS.stdDeviation,5);

    ASSERT_DOUBLE_EQ(lanePosition.offset.value(),0.5);
    ASSERT_TRUE(lanePosition.stochasticOffset.has_value());

    const auto &stochasticOffset = lanePosition.stochasticOffset.value();
    ASSERT_DOUBLE_EQ(stochasticOffset.mean,0.5);
    ASSERT_DOUBLE_EQ(stochasticOffset.lowerBoundary,44);
    ASSERT_DOUBLE_EQ(stochasticOffset.upperBoundary,54);
    ASSERT_DOUBLE_EQ(stochasticOffset.stdDeviation,4);
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
    openScenario::Parameters parameters;

    EXPECT_NO_THROW(ScenarioImporter::ImportPositionElement(scenarioEntity,positionElement, parameters));
    const auto& spawnInfo = scenarioEntity.spawnInfo;
    openScenario::LanePosition lanePosition;
    ASSERT_NO_THROW(lanePosition = std::get<openScenario::LanePosition>(spawnInfo.position));

    ASSERT_DOUBLE_EQ(lanePosition.s, 1470.0);
    ASSERT_EQ(lanePosition.laneId, -4);
    ASSERT_DOUBLE_EQ(lanePosition.offset.value(),0.5);

    ASSERT_DOUBLE_EQ(lanePosition.orientation.value().h.value(), 1.57);
}


TEST(ScenarioImporter_UnitTests, ImportPositionElementWorld)
{
    QDomElement positionElement = documentRootFromString(
              "<Position>"
                    "<World x=\"10.0\" y=\"-4.0\" h=\"0.5\" /> "
               "</Position>"
              );

    ScenarioEntity scenarioEntity;
    openScenario::Parameters parameters;

    EXPECT_NO_THROW(ScenarioImporter::ImportPositionElement(scenarioEntity,positionElement, parameters));

    const auto& spawnInfo = scenarioEntity.spawnInfo;
    openScenario::WorldPosition worldPosition;
    ASSERT_NO_THROW(worldPosition = std::get<openScenario::WorldPosition>(spawnInfo.position));

    ASSERT_THAT(worldPosition.x, Eq(10.0));
    ASSERT_THAT(worldPosition.y, Eq(-4));
    ASSERT_THAT(worldPosition.heading.has_value(), Eq(true));
    ASSERT_THAT(worldPosition.heading.value(), Eq(0.5));
}

TEST(ScenarioImporter_UnitTests, ImportLongitudinal)
{
    QDomElement positionElement = documentRootFromString(
        "<Longitudinal>"
            "<Speed>"
                "<Dynamics rate=\"10.0\" />"
                "<Target>"
                    "<Absolute value=\"27.7\" />"
                "</Target>"
            "</Speed>"
        "</Longitudinal>"
    );

    ScenarioEntity scenarioEntity;
    openScenario::Parameters parameters;

    EXPECT_NO_THROW(ScenarioImporter::ImportLongitudinalElement(scenarioEntity, positionElement, parameters));

    ASSERT_THAT(scenarioEntity.spawnInfo.stochasticVelocity.has_value(), Eq(false));
    ASSERT_THAT(scenarioEntity.spawnInfo.velocity, Eq(27.7));

    ASSERT_THAT(scenarioEntity.spawnInfo.stochasticAcceleration.has_value(), Eq(false));
    ASSERT_THAT(scenarioEntity.spawnInfo.acceleration.has_value(), Eq(true));
    ASSERT_THAT(scenarioEntity.spawnInfo.acceleration.value(), Eq(10.0));
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
    openScenario::Parameters parameters;

    EXPECT_NO_THROW(ScenarioImporter::ImportLongitudinalElement(scenarioEntity, positionElement, parameters));

    ASSERT_THAT(scenarioEntity.spawnInfo.stochasticVelocity.has_value(), Eq(true));
    const auto& velocityAttribute = scenarioEntity.spawnInfo.stochasticVelocity.value();
    ASSERT_DOUBLE_EQ(velocityAttribute.mean, 27.7);
    ASSERT_DOUBLE_EQ(velocityAttribute.stdDeviation, 3.0);
    ASSERT_DOUBLE_EQ(velocityAttribute.lowerBoundary, 12.0);
    ASSERT_DOUBLE_EQ(velocityAttribute.upperBoundary, 40.0);

    ASSERT_THAT(scenarioEntity.spawnInfo.stochasticAcceleration.has_value(), Eq(true));
    const auto& accelerationAttribtue = scenarioEntity.spawnInfo.stochasticAcceleration.value();
    ASSERT_DOUBLE_EQ(accelerationAttribtue.mean, 0.0);
    ASSERT_DOUBLE_EQ(accelerationAttribtue.stdDeviation, 4.0);
    ASSERT_DOUBLE_EQ(accelerationAttribtue.lowerBoundary, 0.0);
    ASSERT_DOUBLE_EQ(accelerationAttribtue.upperBoundary, 4.0);
}

TEST(ScenarioImporter_UnitTests, ImportLongitudinalWithParameterDeclaration)
{
    QDomElement positionElement = documentRootFromString(
        "<Longitudinal>"
            "<Speed>"
                "<Dynamics rate=\"$Rate\" />"
                "<Target>"
                    "<Absolute value=\"$Velocity\" />"
                "</Target>"
            "</Speed>"
        "</Longitudinal>"
    );

    ScenarioEntity scenarioEntity;
    openScenario::Parameters parameters{{"Rate", 10.0}, {"Velocity", 27.7}};

    EXPECT_NO_THROW(ScenarioImporter::ImportLongitudinalElement(scenarioEntity, positionElement, parameters));

    ASSERT_THAT(scenarioEntity.spawnInfo.stochasticVelocity.has_value(), Eq(false));
    ASSERT_THAT(scenarioEntity.spawnInfo.velocity, Eq(27.7));

    ASSERT_THAT(scenarioEntity.spawnInfo.stochasticAcceleration.has_value(), Eq(false));
    ASSERT_THAT(scenarioEntity.spawnInfo.acceleration.has_value(), Eq(true));
    ASSERT_THAT(scenarioEntity.spawnInfo.acceleration.value(), Eq(10.0));
}

std::ostream& operator<<(std::ostream& os, const RouteElement& obj)
{
    return os
            << "road " << obj.roadId << " "
            << (obj.inOdDirection ? "in" : "against") << " OpenDrive direction";
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
    openScenario::Parameters parameters;

    ScenarioImporter::ImportRoutingElement(scenarioEntity, routingElement, parameters);

    auto route = scenarioEntity.spawnInfo.route;
    ASSERT_THAT(route.has_value(), Eq(true));
    ASSERT_THAT(route.value(), ElementsAre(RouteElement{"RoadId1", true}, RouteElement{"RoadId2", false}, RouteElement{"RoadId3", true}));
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

    openScenario::Parameters parameters;

    ASSERT_NO_THROW(catalogPath = ScenarioImporter::ImportCatalog("VehicleCatalog", catalogsElement, parameters));
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

    openScenario::Parameters parameters;

    ASSERT_NO_THROW(catalogPath = ScenarioImporter::ImportCatalog("PedestrianCatalog", catalogsElement, parameters));
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
    openScenario::Parameters parameters;

    EXPECT_THROW(ScenarioImporter::ImportStoryboard(storyboardRootElement, entities, &mockScenario, parameters), std::runtime_error);
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
                "				<Condition name=\"TestCondition\" delay=\"0.0\" edge=\"rising\">"
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
    openScenario::Parameters parameters;

    EXPECT_NO_THROW(ScenarioImporter::ImportStoryboard(storyboardRootElement, entities, &mockScenario, parameters));
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
    openScenario::Parameters parameters;

    EXPECT_THROW(ScenarioImporter::ImportStoryboard(storyboardRootElementConditionMissingName, entities, &mockScenario, parameters), std::runtime_error);
    EXPECT_THROW(ScenarioImporter::ImportStoryboard(storyboardRootElementConditionMissingDelay, entities, &mockScenario, parameters), std::runtime_error);
    EXPECT_THROW(ScenarioImporter::ImportStoryboard(storyboardRootElementConditionMissingEdge, entities, &mockScenario, parameters), std::runtime_error);
    EXPECT_THROW(ScenarioImporter::ImportStoryboard(storyboardRootElementConditionDelayNegative, entities, &mockScenario, parameters), std::runtime_error);
}

TEST(ScenarioImporter_UnitTests, ImportParameterDeclarationElement)
{
    QDomElement parameterDeclarationElement = documentRootFromString(
              "<ParameterDeclaration>"
                "<Parameter name=\"Parameter1\" type=\"string\" value=\"TestString\" />"
                "<Parameter name=\"Parameter2\" type=\"double\" value=\"10.0\" />"
                "<Parameter name=\"Parameter3\" type=\"integer\" value=\"2\" />"
              "</ParameterDeclaration>"
              );

    openScenario::Parameters parameters;

    ASSERT_NO_THROW(Importer::ImportParameterDeclarationElement(parameterDeclarationElement, parameters));
    EXPECT_THAT(parameters, SizeIs(3));
    EXPECT_THAT(std::get<std::string>(parameters.at("Parameter1")), Eq("TestString"));
    EXPECT_THAT(std::get<double>(parameters.at("Parameter2")), Eq(10.0));
    EXPECT_THAT(std::get<int>(parameters.at("Parameter3")), Eq(2));
}

TEST(ScenarioImporter_UnitTests, ParseAttributeWithPlainValue)
{
    QDomElement element = documentRootFromString(
                "<Element valueString=\"TestString\" valueDouble=\"10.0\" valueInt=\"5\" />"
              );

    openScenario::Parameters parameters;

    std::string valueString;
    double valueDouble;
    int valueInt;

    ASSERT_NO_THROW(valueString = ParseAttribute<std::string>(element, "valueString", parameters));
    ASSERT_THAT(valueString, Eq("TestString"));
    ASSERT_NO_THROW(valueDouble = ParseAttribute<double>(element, "valueDouble", parameters));
    ASSERT_THAT(valueDouble, Eq(10.0));
    ASSERT_NO_THROW(valueInt = ParseAttribute<int>(element, "valueInt", parameters));
    ASSERT_THAT(valueInt, Eq(5));
}

TEST(ScenarioImporter_UnitTests, ParseAttributeWithParameter)
{
    QDomElement element = documentRootFromString(
                 "<Element valueString=\"$ParameterString\" valueDouble=\"$ParameterDouble\" valueInt=\"$ParameterInt\" />"
              );

    std::string parameterString{"TestString"};
    double parameterDouble{10.0};
    int parameterInt{5};

    openScenario::Parameters parameters{{"ParameterString", parameterString}, {"ParameterDouble", parameterDouble}, {"ParameterInt", parameterInt}};

    std::string valueString;
    double valueDouble;
    int valueInt;

    ASSERT_NO_THROW(valueString = ParseAttribute<std::string>(element, "valueString", parameters));
    ASSERT_THAT(valueString, Eq(parameterString));
    ASSERT_NO_THROW(valueDouble = ParseAttribute<double>(element, "valueDouble", parameters));
    ASSERT_THAT(valueDouble, Eq(parameterDouble));
    ASSERT_NO_THROW(valueInt = ParseAttribute<int>(element, "valueInt", parameters));
    ASSERT_THAT(valueInt, Eq(parameterInt));
}

TEST(ScenarioImporter_UnitTests, ParseAttributeWithUndefinedAttribute_Throws)
{
    QDomElement element = documentRootFromString(
                 "<Element valueString=\"$ParameterString\" valueDouble=\"$ParameterDouble\" valueInt=\"$ParameterInt\" />"
              );

    openScenario::Parameters parameters{};

    ASSERT_THROW(ParseAttribute<double>(element, "valueUndefined", parameters), std::runtime_error);
}

TEST(ScenarioImporter_UnitTests, ParseAttributeWithUndefinedParameter_Throws)
{
    QDomElement element = documentRootFromString(
                 "<Element valueString=\"$ParameterString\" valueDouble=\"$ParameterDouble\" valueInt=\"$ParameterInt\" />"
              );

    openScenario::Parameters parameters{};

    ASSERT_THROW(ParseAttribute<double>(element, "valueString", parameters), std::runtime_error);
}

TEST(ScenarioImporter_UnitTests, ParseAttributeWithParameterWrongType_Throws)
{
    QDomElement element = documentRootFromString(
                 "<Element valueString=\"$ParameterString\" valueDouble=\"$ParameterDouble\" valueInt=\"$ParameterInt\" />"
              );

    std::string parameterString{"TestString"};
    double parameterDouble{10.0};
    int parameterInt{5};

    openScenario::Parameters parameters{{"ParameterString", parameterString}, {"ParameterDouble", parameterDouble}, {"ParameterInt", parameterInt}};

    ASSERT_THROW(ParseAttribute<double>(element, "valueString", parameters), std::runtime_error);
}
