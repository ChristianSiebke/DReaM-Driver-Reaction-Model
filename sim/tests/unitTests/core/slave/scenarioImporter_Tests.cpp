/*******************************************************************************
* Copyright (c) 2019, 2020, 2021 in-tech GmbH
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
#include "scenarioImporterHelper.h"
#include "fakeScenario.h"

using namespace Configuration;
using namespace Importer;

using ::testing::Eq;
using ::testing::DoubleEq;
using ::testing::StrEq;
using ::testing::DontCare;
using ::testing::ElementsAre;
using ::testing::SizeIs;
using ::testing::_;

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
    QDomElement rootElement = documentRootFromString(
                                      "<root>"
                                          "<Position>"
                                                "<LanePosition roadId=\"RoadId1\" s=\"1470.0\" laneId=\"-4\" offset=\"0.5\" > "
                                                    "<Stochastics value=\"s\"  stdDeviation =\"5\" lowerBound = \"95\" upperBound=\"105\"/>"
                                                    "<Stochastics value=\"offset\" stdDeviation =\"4\" lowerBound = \"44\" upperBound=\"54\"/>"
                                                "</LanePosition>"
                                           "</Position>"
                                      "</root>"
              );

    openScenario::Parameters parameters;

    openScenario::LanePosition lanePosition;
    EXPECT_NO_THROW(lanePosition = std::get<openScenario::LanePosition>(openScenario::ScenarioImporterHelper::ImportPosition(rootElement, parameters)));

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
    QDomElement rootElement = documentRootFromString(
                                      "<root>"
                                          "<Position>"
                                             "<LanePosition roadId=\"RoadId1\" s=\"1470.0\" laneId=\"-4\" offset=\"0.5\" > "
                                                "<Orientation type=\"relative\" h=\"1.57\"/>"
                                             "</LanePosition>"
                                          "</Position>"
                                      "</root>"
              );

    openScenario::Parameters parameters;

    openScenario::LanePosition lanePosition;
    EXPECT_NO_THROW(lanePosition = std::get<openScenario::LanePosition>(openScenario::ScenarioImporterHelper::ImportPosition(rootElement, parameters)));

    ASSERT_DOUBLE_EQ(lanePosition.s, 1470.0);
    ASSERT_EQ(lanePosition.laneId, -4);
    ASSERT_DOUBLE_EQ(lanePosition.offset.value(),0.5);

    ASSERT_DOUBLE_EQ(lanePosition.orientation.value().h.value(), 1.57);
}


TEST(ScenarioImporter_UnitTests, ImportPositionElementWorld)
{
    QDomElement rootElement = documentRootFromString(
                                      "<root>"
                                          "<Position>"
                                                "<WorldPosition x=\"10.0\" y=\"-4.0\" h=\"0.5\" /> "
                                          "</Position>"
                                      "</root>"
              );
    QDomElement positionElement = documentRootFromString(
              "<Position>"
                    "<World x=\"10.0\" y=\"-4.0\" h=\"0.5\" /> "
               "</Position>"
              );

    openScenario::Parameters parameters;

    openScenario::WorldPosition worldPosition;
    EXPECT_NO_THROW(worldPosition = std::get<openScenario::WorldPosition>(openScenario::ScenarioImporterHelper::ImportPosition(rootElement, parameters)));

    ASSERT_THAT(worldPosition.x, Eq(10.0));
    ASSERT_THAT(worldPosition.y, Eq(-4));
    ASSERT_THAT(worldPosition.h.has_value(), Eq(true));
    ASSERT_THAT(worldPosition.h.value(), Eq(0.5));
}

TEST(ScenarioImporter_UnitTests, ImportPositionElementRelativeWorldPosition)
{
    QDomElement rootElement = documentRootFromString(
        R"(<root><Position><RelativeWorldPosition dx="10.0" dy="-4.0" entityRef="ref"><Orientation type="relative" h="1.57"></RelativeWorldPosition></Position></root>)");

    openScenario::Parameters parameters;

    openScenario::RelativeWorldPosition relativeWorldPosition;
    EXPECT_NO_THROW(relativeWorldPosition = std::get<openScenario::RelativeWorldPosition>(openScenario::ScenarioImporterHelper::ImportPosition(rootElement, parameters)));
    ASSERT_EQ(relativeWorldPosition.entityRef, "ref");
    ASSERT_EQ(relativeWorldPosition.dx, 10.0);
    ASSERT_EQ(relativeWorldPosition.dy, -4);
    ASSERT_FALSE(relativeWorldPosition.dz.has_value());
    ASSERT_TRUE(relativeWorldPosition.orientation.has_value());
}

TEST(ScenarioImporter_UnitTests, ImportPositionElementRelativeObjectPosition)
{
    QDomElement rootElement = documentRootFromString(
        R"(<root><Position><RelativeObjectPosition dx="10.0" dy="-4.0" entityRef="ref"><Orientation type="relative" h="1.57"></RelativeObjectPosition></Position></root>)");

    openScenario::Parameters parameters;

    openScenario::RelativeObjectPosition relativeObjectPosition;
    EXPECT_NO_THROW(relativeObjectPosition = std::get<openScenario::RelativeObjectPosition>(openScenario::ScenarioImporterHelper::ImportPosition(rootElement, parameters)));
    ASSERT_EQ(relativeObjectPosition.entityRef, "ref");
    ASSERT_EQ(relativeObjectPosition.dx, 10.0);
    ASSERT_EQ(relativeObjectPosition.dy, -4);
    ASSERT_FALSE(relativeObjectPosition.dz.has_value());
    ASSERT_TRUE(relativeObjectPosition.orientation.has_value());
}

TEST(ScenarioImporter_UnitTests, ImportSpeedAction)
{
    QDomElement rootElement = documentRootFromString(
                                "<root>"
                                        "<LongitudinalAction>"
                                            "<SpeedAction>"
                                                "<SpeedActionDynamics value=\"10.0\" dynamicsDimension=\"rate\" dynamicsShape=\"linear\" />"
                                                "<SpeedActionTarget>"
                                                    "<AbsoluteTargetSpeed value=\"27.7\" />"
                                                "</SpeedActionTarget>"
                                            "</SpeedAction>"
                                        "</Longitudinal>"
                                "</root>"
    );

    openScenario::Parameters parameters;

    openScenario::Action action;
    EXPECT_NO_THROW(action = openScenario::ScenarioImporterHelper::ImportPrivateAction(rootElement, parameters));
    openScenario::SpeedAction speedAction;
    EXPECT_NO_THROW(speedAction = std::get<openScenario::SpeedAction>(std::get<openScenario::LongitudinalAction>(std::get<openScenario::PrivateAction>(action))));

    ASSERT_THAT(speedAction.stochasticValue.has_value(), Eq(false));
    ASSERT_THAT(std::get<openScenario::AbsoluteTargetSpeed>(speedAction.target).value, DoubleEq(27.7));

    ASSERT_THAT(speedAction.stochasticDynamics.has_value(), Eq(false));
    ASSERT_THAT(speedAction.transitionDynamics.value, DoubleEq(10.0));
    ASSERT_THAT(speedAction.transitionDynamics.shape, Eq(openScenario::Shape::Linear));
    ASSERT_THAT(speedAction.transitionDynamics.dimension, Eq("rate"));
}

TEST(ScenarioImporter_UnitTests, ImportLongitudinalActionWithStochastics)
{
    QDomElement rootElement = documentRootFromString(
                                "<root>"
                                        "<LongitudinalAction>"
                                            "<SpeedAction>"
                                                "<SpeedActionDynamics value=\"10.0\" dynamicsDimension=\"rate\" dynamicsShape=\"linear\" />"
                                                "<SpeedActionTarget>"
                                                    "<AbsoluteTargetSpeed value=\"27.7\" />"
                                                "</SpeedActionTarget>"
                                                "<Stochastics value=\"velocity\"  stdDeviation =\"3\" lowerBound = \"12\" upperBound=\"40.0\"/>"
                                                "<Stochastics value=\"rate\" stdDeviation =\"4\" lowerBound = \"0\" upperBound=\"4\"/>"
                                            "</SpeedAction>"
                                        "</Longitudinal>"
                                "</root>"
    );

    openScenario::Parameters parameters;

    openScenario::Action action;
    EXPECT_NO_THROW(action = openScenario::ScenarioImporterHelper::ImportPrivateAction(rootElement, parameters));
    openScenario::SpeedAction speedAction;
    EXPECT_NO_THROW(speedAction = std::get<openScenario::SpeedAction>(std::get<openScenario::LongitudinalAction>(std::get<openScenario::PrivateAction>(action))));

    ASSERT_THAT(speedAction.stochasticValue.has_value(), Eq(true));
    const auto& velocityAttribute = speedAction.stochasticValue.value();
    ASSERT_DOUBLE_EQ(velocityAttribute.mean, 27.7);
    ASSERT_DOUBLE_EQ(velocityAttribute.stdDeviation, 3.0);
    ASSERT_DOUBLE_EQ(velocityAttribute.lowerBoundary, 12.0);
    ASSERT_DOUBLE_EQ(velocityAttribute.upperBoundary, 40.0);

    ASSERT_THAT(speedAction.stochasticDynamics.has_value(), Eq(true));
    const auto& rateAttribute = speedAction.stochasticDynamics.value();
    ASSERT_DOUBLE_EQ(rateAttribute.mean, 10.0);
    ASSERT_DOUBLE_EQ(rateAttribute.stdDeviation, 4.0);
    ASSERT_DOUBLE_EQ(rateAttribute.lowerBoundary, 0.0);
    ASSERT_DOUBLE_EQ(rateAttribute.upperBoundary, 4.0);

}

TEST(ScenarioImporter_UnitTests, ImportLongitudinalWithParameterDeclaration)
{
    QDomElement rootElement = documentRootFromString(
                                "<root>"
                                        "<LongitudinalAction>"
                                            "<SpeedAction>"
                                                "<SpeedActionDynamics value=\"$Rate\" dynamicsDimension=\"rate\" dynamicsShape=\"linear\" />"
                                                "<SpeedActionTarget>"
                                                    "<AbsoluteTargetSpeed value=\"$Velocity\" />"
                                                "</SpeedActionTarget>"
                                            "</SpeedAction>"
                                        "</Longitudinal>"
                                "</root>"
    );

    openScenario::Parameters parameters{{"Rate", 10.0}, {"Velocity", 27.7}};

    EXPECT_NO_THROW(openScenario::ScenarioImporterHelper::ImportPrivateAction(rootElement, parameters));

    openScenario::Action action;
    EXPECT_NO_THROW(action = openScenario::ScenarioImporterHelper::ImportPrivateAction(rootElement, parameters));
    openScenario::SpeedAction speedAction;
    EXPECT_NO_THROW(speedAction = std::get<openScenario::SpeedAction>(std::get<openScenario::LongitudinalAction>(std::get<openScenario::PrivateAction>(action))));

    ASSERT_THAT(speedAction.stochasticValue.has_value(), Eq(false));
    ASSERT_THAT(std::get<openScenario::AbsoluteTargetSpeed>(speedAction.target).value, DoubleEq(27.7));

    ASSERT_THAT(speedAction.stochasticDynamics.has_value(), Eq(false));
    ASSERT_THAT(speedAction.transitionDynamics.value, DoubleEq(10.0));
    ASSERT_THAT(speedAction.transitionDynamics.shape, Eq(openScenario::Shape::Linear));
    ASSERT_THAT(speedAction.transitionDynamics.dimension, Eq("rate"));
}

TEST(ScenarioImporter_UnitTests, ImportAssignRoutingAction)
{
    QDomElement rootElement = documentRootFromString(
                                     "<root>"
                                          "<RoutingAction>"
                                                "<AssignRouteAction>"
                                                    "<Route>"
                                                        "<Waypoint>"
                                                            "<Position>"
                                                                "<RoadPosition roadId=\"RoadId1\" s=\"0\" t=\"-1.0\" />"
                                                            "</Position>"
                                                        "</Waypoint>"
                                                        "<Waypoint>"
                                                            "<Position>"
                                                                "<RoadPosition roadId=\"RoadId2\" s=\"0\" t=\"1.0\" />"
                                                            "</Position>"
                                                        "</Waypoint>"
                                                        "<Waypoint>"
                                                            "<Position>"
                                                                "<RoadPosition roadId=\"RoadId3\" s=\"0\" t=\"-1.0\" />"
                                                            "</Position>"
                                                        "</Waypoint>"
                                                   "</Route>"
                                                "</AssignRouteAction>"
                                           "</RoutingAction>"
                                     "</root>"
              );

    openScenario::Parameters parameters;

    EXPECT_NO_THROW(openScenario::ScenarioImporterHelper::ImportPrivateAction(rootElement, parameters));

    openScenario::Action action;
    EXPECT_NO_THROW(action = openScenario::ScenarioImporterHelper::ImportPrivateAction(rootElement, parameters));
    openScenario::AssignRouteAction assignRouteAction;
    EXPECT_NO_THROW(assignRouteAction = std::get<openScenario::AssignRouteAction>(std::get<openScenario::RoutingAction>(std::get<openScenario::PrivateAction>(action))));

    ASSERT_THAT(assignRouteAction, SizeIs(3));
    ASSERT_THAT(assignRouteAction[0].roadId, Eq("RoadId1"));
    ASSERT_THAT(assignRouteAction[0].t, DoubleEq(-1));
    ASSERT_THAT(assignRouteAction[1].roadId, Eq("RoadId2"));
    ASSERT_THAT(assignRouteAction[1].t, DoubleEq(1));
    ASSERT_THAT(assignRouteAction[2].roadId, Eq("RoadId3"));
    ASSERT_THAT(assignRouteAction[2].t, DoubleEq(-1));
}

TEST(ScenarioImporter_UnitTests, ImportAcquirePositionAction)
{
    QDomElement rootElement = documentRootFromString(
        R"(<root><RoutingAction><AcquirePositionAction><Position><WorldPosition x="76.17" y="5.625" z="0" h="0.0" p="0" r="0"/></Position></AcquirePositionAction></RoutingAction></root>)");

    openScenario::Parameters parameters;

    openScenario::Action action;
    EXPECT_NO_THROW(action = openScenario::ScenarioImporterHelper::ImportPrivateAction(rootElement, parameters));
    EXPECT_NO_THROW(
        std::get<openScenario::AcquirePositionAction>(
            std::get<openScenario::RoutingAction>(
                std::get<openScenario::PrivateAction>(action))));
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
                "		<StopTrigger>"
                "			<ConditionGroup>"
                "				<Condition name=\"TestCondition\" delay=\"0.0\" conditionEdge=\"rising\">"
                "					<ByValueCondition>"
                "						<SimulationTimeCondition value=\"3.000\" rule=\"greaterThan\" />"
                "					</ByValueCondition>"
                "				</Condition>"
                "			</ConditionGroup>"
                "		</StopTrigger>"
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
                "		<StopTrigger>"
                "			<ConditionGroup>"
                "				<Condition delay=\"0.0\" edge=\"conditionEdge\">"
                "					<ByValueCondition>"
                "						<SimulationTimeCondition value=\"3.000\" rule=\"greaterThan\" />"
                "					</ByValueCondition>"
                "				</Condition>"
                "			</ConditionGroup>"
                "		</StopTrigger>"
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
                "		<StopTrigger>"
                "			<ConditionGroup>"
                "				<Condition name=\"\" edge=\"rising\">"
                "					<ByValueCondition>"
                "						<SimulationTimeCondition value=\"3.000\" rule=\"greaterThan\" />"
                "					</ByValueCondition>"
                "				</Condition>"
                "			</ConditionGroup>"
                "		</StopTrigger>"
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
                "		<StopTrigger>"
                "			<ConditionGroup>"
                "				<Condition name=\"\" delay=\"0.0\">"
                "					<ByValueCondition>"
                "						<SimulationTimeCondition value=\"3.000\" rule=\"greaterThan\" />"
                "					</ByValueCondition>"
                "				</Condition>"
                "			</ConditionGroup>"
                "		</StopTrigger>"
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
                "		<StopTrigger>"
                "			<ConditionGroup>"
                "				<Condition name=\"\" delay=\"-1.0\" edge=\"rising\">"
                "					<ByValueCondition>"
                "						<SimulationTimeCondition value=\"3.000\" rule=\"greaterThan\" />"
                "					</ByValueCondition>"
                "				</Condition>"
                "			</ConditionGroup>"
                "		</StopTrigger>"
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
              "<ParameterDeclarations>"
                "<ParameterDeclaration name=\"Parameter1\" parameterType=\"string\" value=\"TestString\" />"
                "<ParameterDeclaration name=\"Parameter2\" parameterType=\"double\" value=\"10.0\" />"
                "<ParameterDeclaration name=\"Parameter3\" parameterType=\"integer\" value=\"2\" />"
              "</ParameterDeclarations>"
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

TEST(ScenarioImporter_UnitTests, ImportRoadNetworkWithTrafficLSignalController)
{
    QDomElement entityElement = documentRootFromString(
        "<root>"
         "<RoadNetwork>"
            "<LogicFile filepath=\"SceneryConfiguration.xodr\"/>"
            "<SceneGraphFile filepath=\"\"/>"
            "<TrafficSignals>"
                "<TrafficSignalController name=\"ControllerA\" delay=\"1.0\">"
                    "<Phase name=\"Phase1\" duration=\"20\">"
                        "<TrafficSignalState trafficSignalId=\"100\" state=\"red\"/>"
                        "<TrafficSignalState trafficSignalId=\"101\" state=\"green\"/>"
                    "</Phase>"
                    "<Phase name=\"Phase2\" duration=\"3\">"
                        "<TrafficSignalState trafficSignalId=\"100\" state=\"red yellow\"/>"
                        "<TrafficSignalState trafficSignalId=\"101\" state=\"yellow\"/>"
                    "</Phase>"
                "</TrafficSignalController>"
            "</TrafficSignals>"
          "</RoadNetwork>"
        "</root>"
    );

    FakeScenario scenario;
    EXPECT_CALL(scenario, SetSceneryPath("SceneryConfiguration.xodr"));
    std::vector<openScenario::TrafficSignalController> controllers;
    ON_CALL(scenario, AddTrafficSignalController(_)).WillByDefault(
                [&](const openScenario::TrafficSignalController& controller){controllers.push_back(controller);});

    openScenario::Parameters parameters;

    EXPECT_NO_THROW(ScenarioImporter::ImportRoadNetwork(entityElement, &scenario, parameters));

    EXPECT_THAT(controllers, SizeIs(1));
    const auto controller = controllers.front();
    EXPECT_THAT(controller.name, Eq("ControllerA"));
    EXPECT_THAT(controller.delay, Eq(1));
    EXPECT_THAT(controller.phases, SizeIs(2));
    EXPECT_THAT(controller.phases[0].name, Eq("Phase1"));
    EXPECT_THAT(controller.phases[0].duration, Eq(20));
    EXPECT_THAT(controller.phases[0].states, ElementsAre(std::make_pair("100", "red"), std::make_pair("101", "green")));
    EXPECT_THAT(controller.phases[1].name, Eq("Phase2"));
    EXPECT_THAT(controller.phases[1].duration, Eq(3));
    EXPECT_THAT(controller.phases[1].states, ElementsAre(std::make_pair("100", "red yellow"), std::make_pair("101", "yellow")));
}


TEST(ScenarioImporter_UnitTests, ImportEnvironmentAction)
{
    QDomElement element = documentRootFromString(
                "<root>"
                 "<EnvironmentAction>"
                    "<Environment>"
                        "<Weather cloudState=\"free\">"
                            "<Sun intensity=\"1.0\" azimuth=\"1.1\" elevation=\"1.3\" />"
                            "<Fog visualRange=\"20.0\" />"
                            "<Precipitation intensity=\"0.5\" precipitationType=\"rain\" />"
                        "</Weather>"
                    "</Environment>"
                 "</EnvironmentAction>"
                "</root>"
              );

    openScenario::Parameters parameters;

    openScenario::EnvironmentAction result;
    EXPECT_NO_THROW(result = std::get<openScenario::EnvironmentAction>(openScenario::ScenarioImporterHelper::ImportGlobalAction(element, parameters)));
    EXPECT_THAT(result.weather.cloudState, Eq(openScenario::Weather::CloudState::free));
    EXPECT_THAT(result.weather.sun.intensity, Eq(1.0));
    EXPECT_THAT(result.weather.sun.azimuth, Eq(1.1));
    EXPECT_THAT(result.weather.sun.elevation, Eq(1.3));
    EXPECT_THAT(result.weather.fog.visualRange, Eq(20.0));
    EXPECT_THAT(result.weather.precipitation.intensity, Eq(0.5));
    EXPECT_THAT(result.weather.precipitation.type, Eq(openScenario::Precipitation::Type::rain));
}
