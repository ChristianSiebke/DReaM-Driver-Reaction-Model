/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*               2017, 2018 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include <memory>
#include <iostream>
#include <queue>
#include <QDomDocument>
#include <QDir>
#include <QFile>
#include <QFileInfo>

#include "eventDetectorImporter.h"
#include "manipulatorImporter.h"
#include "scenario.h"
#include "scenarioImporter.h"
#include "xmlParser.h"
#include "CoreFramework/CoreShare/log.h"
#include "CoreFramework/OpenPassSlave/framework/directories.h"

namespace TAG = openpass::importer::xml::scenarioImporter::tag;
namespace ATTRIBUTE = openpass::importer::xml::scenarioImporter::attribute;
using Directories = openpass::core::Directories;

namespace Importer {

bool ScenarioImporter::Import(const std::string& filename, ScenarioInterface* scenario)
{
    try
    {
        std::locale::global(std::locale("C"));

        QFile xmlFile(filename.c_str()); // automatic object will be closed on destruction
        ThrowIfFalse(xmlFile.open(QIODevice::ReadOnly), "Could not open scenario (" + filename + ")");

        QByteArray xmlData(xmlFile.readAll());
        QDomDocument document;
        QString errorMsg;
        int errorLine;
        ThrowIfFalse(document.setContent(xmlData, &errorMsg, &errorLine), "Invalid xml format (" + filename + ") in line " + std::to_string(errorLine) + ": " + errorMsg.toStdString());

        QDomElement documentRoot = document.documentElement();
        ThrowIfFalse(!documentRoot.isNull(), "Scenario xml has no document root");

        ImportAndValidateVersion(documentRoot);

        std::string sceneryPath;
        ImportRoadNetwork(documentRoot, sceneryPath);
        scenario->SetSceneryPath(sceneryPath);

        auto path = Directories::StripFile(filename);
        ImportCatalogs(documentRoot, scenario, path);

        std::vector<ScenarioEntity> entities;
        std::map<std::string, std::list<std::string>> groups;
        ImportEntities(documentRoot, entities, groups);

        ImportStoryboard(documentRoot, entities, scenario);
        CategorizeEntities(entities, groups, scenario);

        return true;
    }
    catch(std::runtime_error& e)
    {
        LOG_INTERN(LogLevel::Error) << "Scenario import failed: " + std::string(e.what());
        return false;
    }
}

void ScenarioImporter::ImportAndValidateVersion(QDomElement& documentRoot)
{
    openpass::parameter::Container scenarioParameters;

    QDomElement parameterDeclarationElement;

    if (SimulationCommon::GetFirstChildElement(documentRoot, TAG::parameterDeclaration, parameterDeclarationElement))
    {
        ImportParameterDeclarationElement(parameterDeclarationElement, scenarioParameters);
    }

    const auto version = openpass::parameter::Get<std::string>(scenarioParameters, "OP_OSC_SchemaVersion");
    ThrowIfFalse(version.has_value(), "Cannot determine scenario version");
    ThrowIfFalse(version.value() == supportedScenarioVersion,
                 "Scenario version not supported (" + version.value() + "). Supported version is " + supportedScenarioVersion);
}

void ScenarioImporter::ImportCatalogs(QDomElement& documentRoot, ScenarioInterface* scenario, const std::string& path)
{
    QDomElement catalogsElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(documentRoot, TAG::catalogs, catalogsElement),
                 documentRoot, "Tag " + std::string(TAG::catalogs) + " is missing.");

    const auto vehicleCatalogPath = ImportCatalog("VehicleCatalog", catalogsElement);
    scenario->SetVehicleCatalogPath(vehicleCatalogPath);

    const auto pedestrianCatalogPath = ImportCatalog("PedestrianCatalog", catalogsElement);
    scenario->SetPedestrianCatalogPath(pedestrianCatalogPath);

    auto trajectoryCatalogPath = ImportCatalog("TrajectoryCatalog", catalogsElement);

    if (Directories::IsRelative(trajectoryCatalogPath))
    {
        trajectoryCatalogPath = Directories::Concat(path, trajectoryCatalogPath);
    }

    scenario->SetTrajectoryCatalogPath(trajectoryCatalogPath);
}

void ScenarioImporter::ImportRoadNetwork(QDomElement& documentRoot, std::string& sceneryPath)
{
    QDomElement roadNetworkElement;
    QDomElement logicsElement;

    ThrowIfFalse(SimulationCommon::GetFirstChildElement(documentRoot, TAG::roadNetwork, roadNetworkElement),
                 documentRoot, "Tag " + std::string(TAG::roadNetwork) + " is missing.");
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(roadNetworkElement, TAG::logics, logicsElement),
                 roadNetworkElement, "Tag " + std::string(TAG::logics) + " is missing.");
    ThrowIfFalse(SimulationCommon::ParseAttributeString(logicsElement, ATTRIBUTE::filePath, sceneryPath),
                 logicsElement, "Attribute " + std::string(ATTRIBUTE::filePath) + " is missing.");
}

void ScenarioImporter::ImportStoryboard(QDomElement& documentRoot, std::vector<ScenarioEntity>& entities,
                                        ScenarioInterface* scenario)
{
    QDomElement storyboardElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(documentRoot, TAG::storyboard, storyboardElement),
                 documentRoot, "Tag " + std::string(TAG::storyboard) + " is missing.");

    //Import Init
    QDomElement initElement;
    // for initial entitiy parameters we just use first child "Init" --> others will be ignore
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(storyboardElement, TAG::init, initElement),
                 storyboardElement, "Tag " + std::string(TAG::init) + " is missing.");
    ImportInitElement(initElement, entities);

    // Import Story
    QDomElement storyElement;

    SimulationCommon::GetFirstChildElement(storyboardElement, TAG::story, storyElement);

    while (!storyElement.isNull())
    {
        ImportStoryElement(storyElement, entities, scenario);
        storyElement = storyElement.nextSiblingElement(TAG::story);
    }

    // Import EndCondition
    ImportEndConditionsFromStoryboard(storyboardElement, scenario);
}

void ScenarioImporter::ImportStoryElement(QDomElement& storyElement,
                                          const std::vector<ScenarioEntity>& entities,
                                          ScenarioInterface* scenario)
{
    // none of these tags are urgently required
    QDomElement actElement;
    if (SimulationCommon::GetFirstChildElement(storyElement, TAG::act, actElement))
    {
        while (!actElement.isNull())
        {
            QDomElement seqElement;
            if (SimulationCommon::GetFirstChildElement(actElement, TAG::sequence, seqElement))
            {
                while (!seqElement.isNull())
                {
                    int numberOfExecutions;
                    ThrowIfFalse(SimulationCommon::ParseAttributeInt(seqElement, ATTRIBUTE::numberOfExecutions, numberOfExecutions),
                                 seqElement, "Attribute " + std::string(ATTRIBUTE::numberOfExecutions) + " is missing.");

                    QDomElement actorsElement;
                    openScenario::ActorInformation actorInformation;
                    if (SimulationCommon::GetFirstChildElement(seqElement, TAG::actors, actorsElement)) // just one actors per sequence
                    {
                        actorInformation = ImportActors(actorsElement, entities);
                    }

                    QDomElement maneuverElement;
                    if (SimulationCommon::GetFirstChildElement(seqElement, TAG::maneuver, maneuverElement)) // just one maneuver per sequence
                    {
                        ImportManeuverElement(maneuverElement, entities, scenario, actorInformation, numberOfExecutions);
                    }

                    seqElement = seqElement.nextSiblingElement(TAG::sequence);
                }
            }
            actElement = actElement.nextSiblingElement(TAG::act);
        }
    }
}

openScenario::ActorInformation ScenarioImporter::ImportActors(QDomElement& actorsElement,
                                                              const std::vector<ScenarioEntity>& entities)
{
    openScenario::ActorInformation actorInformation;

    QDomElement entityElement;
    SimulationCommon::GetFirstChildElement(actorsElement, TAG::entity, entityElement);

    while (!entityElement.isNull())
    {
        std::string entityName;
        ThrowIfFalse(SimulationCommon::ParseAttributeString(entityElement, ATTRIBUTE::name, entityName),
                     entityElement, "Attribute " + std::string(ATTRIBUTE::name) + " is missing.");
        ThrowIfFalse(ContainsEntity(entities, entityName),
                     entityElement, "Element references entity '" + entityName + "' which isn't declared in 'Entities'");

        if (actorInformation.actors.has_value())
        {
            actorInformation.actors.value().push_back(entityName);
        }
        else
        {
            actorInformation.actors.emplace({entityName});
        }

        entityElement = entityElement.nextSiblingElement(TAG::entity);
    }

    QDomElement byConditionElement;
    SimulationCommon::GetFirstChildElement(actorsElement, TAG::byCondition, byConditionElement);

    if(!byConditionElement.isNull())
    {
        std::string actor;
        ThrowIfFalse(SimulationCommon::ParseAttributeString(byConditionElement, ATTRIBUTE::actor, actor),
                     byConditionElement, "Attribute " + std::string(ATTRIBUTE::actor) + " is missing.");
        if(actor == "triggeringEntity")
        {
            actorInformation.triggeringAgentsAsActors.emplace(true);
        }
    }

    return actorInformation;
}

void ScenarioImporter::ImportManeuverElement(QDomElement& maneuverElement,
                                             const std::vector<ScenarioEntity>& entities,
                                             ScenarioInterface* scenario,
                                             const openScenario::ActorInformation &actorInformation,
                                             const int numberOfExecutions)
{
    // handle parameterdeclaration element(s) for event(s)
    QDomElement parameterDeclarationElement;
    SimulationCommon::GetFirstChildElement(maneuverElement, TAG::parameterDeclaration, parameterDeclarationElement);

    // handle event element
    QDomElement eventElement;
    SimulationCommon::GetFirstChildElement(maneuverElement, TAG::event, eventElement);

    while (!eventElement.isNull())
    {
        std::string eventName;
        ThrowIfFalse(SimulationCommon::ParseAttributeString(eventElement, ATTRIBUTE::name, eventName),
                     eventElement, "Attribute " + std::string(ATTRIBUTE::name) + " is missing.");

        openScenario::ConditionalEventDetectorInformation conditionalEventDetectorInformation = EventDetectorImporter::ImportEventDetector(eventElement,
                                                                                                                                           eventName,
                                                                                                                                           numberOfExecutions,
                                                                                                                                           actorInformation,
                                                                                                                                           entities);

        scenario->AddConditionalEventDetector(conditionalEventDetectorInformation);

        std::shared_ptr<ScenarioActionInterface> action = ManipulatorImporter::ImportManipulator(eventElement,
                                                                                                 eventName,
                                                                                                 scenario->GetTrajectoryCatalogPath());
        scenario->AddAction(action);

        eventElement = eventElement.nextSiblingElement(TAG::event);
    }
}

void ScenarioImporter::ImportLongitudinalElement(ScenarioEntity& scenarioEntity, QDomElement firstChildOfActionElement)
{
    SpawnInfo &spawnInfo = scenarioEntity.spawnInfo;

    QDomElement speedElement;
    if (SimulationCommon::GetFirstChildElement(firstChildOfActionElement, TAG::speed, speedElement))
    {
        QDomElement dynamicsElement;
        if (SimulationCommon::GetFirstChildElement(speedElement, TAG::dynamics, dynamicsElement))
        {
            double rate;
            if (SimulationCommon::ParseAttributeDouble(dynamicsElement, ATTRIBUTE::rate, rate))
            {
                spawnInfo.acceleration = rate;
            }
        }

        // Handle <Target> attributes
        QDomElement targetElement;
        if (SimulationCommon::GetFirstChildElement(speedElement, TAG::target, targetElement))
        {
            // Handle <Target> internal tags - currently ignoring <Relative> tags
            QDomElement absoluteElement;
            if (SimulationCommon::GetFirstChildElement(targetElement, TAG::absolute, absoluteElement))
            {
                double velocity;
                if (SimulationCommon::ParseAttributeDouble(absoluteElement, ATTRIBUTE::value, velocity))
                {
                    spawnInfo.velocity = velocity;
                }
            }
        }

        // Parse stochastics if available
        QDomElement stochasticElement;
        SimulationCommon::GetFirstChildElement(speedElement, TAG::stochastics, stochasticElement);
        while (!stochasticElement.isNull())
        {
            const auto& [attributeName, stochasticInformation] = ImportStochastics(stochasticElement);
            if (attributeName == "velocity")
            {
                spawnInfo.stochasticVelocity = stochasticInformation;
                spawnInfo.stochasticVelocity.value().mean = spawnInfo.velocity;
            }
            else if (attributeName == "rate")
            {
                ThrowIfFalse(spawnInfo.acceleration.has_value(), stochasticElement, "Rate attribute is requried in order to use stochastic rates.");
                spawnInfo.stochasticAcceleration = stochasticInformation;
                spawnInfo.stochasticAcceleration.value().mean = spawnInfo.acceleration.value();
            }

           stochasticElement = stochasticElement.nextSiblingElement(TAG::stochastics);
        }
    }
}

openScenario::LanePosition ScenarioImporter::ImportLanePosition(QDomElement positionElement)
{
    openScenario::LanePosition lanePosition;
    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(positionElement, ATTRIBUTE::s, lanePosition.s),
                 positionElement, "Attribute " + std::string(ATTRIBUTE::s) + " is missing");
    ThrowIfFalse(SimulationCommon::ParseAttributeInt(positionElement, ATTRIBUTE::laneId, lanePosition.laneId),
                 positionElement, "Attribute " + std::string(ATTRIBUTE::laneId) + " is missing.");
    ThrowIfFalse(SimulationCommon::ParseAttributeString(positionElement, ATTRIBUTE::roadId, lanePosition.roadId),
                 positionElement, "Attribute " + std::string(ATTRIBUTE::roadId) + " is missing.");

    double offset{};
    if(SimulationCommon::ParseAttributeDouble(positionElement, ATTRIBUTE::offset, offset))
    {
        lanePosition.offset = offset;
    }

    //Parse optional stochastics
    QDomElement stochasticElement;
    SimulationCommon::GetFirstChildElement(positionElement, TAG::stochastics, stochasticElement);
    while (!stochasticElement.isNull())
    {
        const auto& stochasticInformation = ImportStochastics(stochasticElement);

        if (stochasticInformation.first == "offset")
        {
            ThrowIfFalse(lanePosition.offset.has_value(), stochasticElement, "The offset attribute is required in order to use stochastic offsets.");

            lanePosition.stochasticOffset = stochasticInformation.second;
            lanePosition.stochasticOffset->mean = lanePosition.offset.value();
        }
        else if (stochasticInformation.first == "s")
        {
            lanePosition.stochasticS = stochasticInformation.second;
            lanePosition.stochasticS->mean = lanePosition.s;
        }
        stochasticElement = stochasticElement.nextSiblingElement(TAG::stochastics);
    }

    QDomElement orientationElement;
    if(SimulationCommon::GetFirstChildElement(positionElement, TAG::orientation, orientationElement))
    {
        lanePosition.orientation = ImportOrientation(orientationElement);
    }

    return lanePosition;
}

openScenario::WorldPosition ScenarioImporter::ImportWorldPosition(QDomElement positionElement)
{
    openScenario::WorldPosition worldPosition;
    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(positionElement, ATTRIBUTE::x, worldPosition.x),
                 positionElement, "Attribute " + std::string(ATTRIBUTE::x) + " is missing");
    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(positionElement, ATTRIBUTE::y, worldPosition.y),
                 positionElement, "Attribute " + std::string(ATTRIBUTE::y) + " is missing.");

    double heading{};
    if(SimulationCommon::ParseAttributeDouble(positionElement, ATTRIBUTE::h, heading))
    {
        worldPosition.heading = heading;
    }

    return worldPosition;
}

void ScenarioImporter::ImportPositionElement(ScenarioEntity& scenarioEntity, QDomElement firstChildOfActionElement)
{
    QDomElement positionElement;
    if(SimulationCommon::GetFirstChildElement(firstChildOfActionElement, TAG::lane, positionElement))
    {
        scenarioEntity.spawnInfo.position = ImportLanePosition(positionElement);
    }
    else
    {
        ThrowIfFalse(SimulationCommon::GetFirstChildElement(firstChildOfActionElement, TAG::world, positionElement),
                  firstChildOfActionElement, "OSCPosition type not supported. Currently only World and Lane are supported!");
        scenarioEntity.spawnInfo.position = ImportWorldPosition(positionElement);
    }
}

void ScenarioImporter::ImportRoutingElement(ScenarioEntity& scenarioEntity, QDomElement firstChildOfActionElement)
{
    std::vector<RouteElement> roads;
    size_t hash{0};

    QDomElement followRouteElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(firstChildOfActionElement, TAG::followRoute, followRouteElement),
                 firstChildOfActionElement, "Tag " + std::string(TAG::followRoute) + " is missing.");
    QDomElement routeElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(followRouteElement, TAG::route, routeElement),
                 followRouteElement, "Tag " + std::string(TAG::route) + " is missing.");
    QDomElement waypointElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(routeElement, TAG::waypoint, waypointElement),
                 routeElement, "Tag " + std::string(TAG::waypoint) + " is missing.");
    while (!waypointElement.isNull())
    {
        QDomElement positionElement;
        ThrowIfFalse(SimulationCommon::GetFirstChildElement(waypointElement, TAG::position, positionElement),
                     waypointElement, "Tag " + std::string(TAG::position) + " is missing.");
        QDomElement roadElement;
        ThrowIfFalse(SimulationCommon::GetFirstChildElement(positionElement, TAG::road, roadElement),
                     positionElement, "Tag " + std::string(TAG::road) + " is missing.");
        std::string roadId;
        ThrowIfFalse(SimulationCommon::ParseAttributeString(roadElement, ATTRIBUTE::roadId, roadId),
                     roadElement, "Attribute " + std::string(ATTRIBUTE::roadId) + " is missing.");
        double t;
        ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadElement, ATTRIBUTE::t, t),
                     roadElement, "Attribute " + std::string(ATTRIBUTE::t) + " is missing.");
        bool inRoadDirection = (t <= 0);
        roads.push_back({roadId, inRoadDirection});
        hash = hash ^ (std::hash<std::string>{}(roadId) << 1);

        waypointElement = waypointElement.nextSiblingElement(TAG::waypoint);
    }
    scenarioEntity.spawnInfo.route = Route{roads, {}, hash};
}

std::pair<std::string, openScenario::StochasticAttribute> ScenarioImporter::ImportStochastics(QDomElement& stochasticsElement)
{
    std::string attributeName;
    ThrowIfFalse(SimulationCommon::ParseAttributeString(stochasticsElement, ATTRIBUTE::value, attributeName),
                 stochasticsElement, "Attribute " + std::string(ATTRIBUTE::value) + " is missing.");

    openScenario::StochasticAttribute stochasticAttribute;
    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(stochasticsElement, ATTRIBUTE::stdDeviation, stochasticAttribute.stdDeviation),
                 stochasticsElement, "Attribute " + std::string(ATTRIBUTE::stdDeviation) + " is missing.");
    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(stochasticsElement, ATTRIBUTE::lowerBound, stochasticAttribute.lowerBoundary),
                 stochasticsElement, "Attribute " + std::string(ATTRIBUTE::lowerBound) + " is missing.");
    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(stochasticsElement, ATTRIBUTE::upperBound, stochasticAttribute.upperBoundary),
                 stochasticsElement, "Attribute " + std::string(ATTRIBUTE::upperBound) + " is missing.");

    return {attributeName, stochasticAttribute};
}

openScenario::Orientation ScenarioImporter::ImportOrientation(QDomElement& orientationElement)
{
    openScenario::Orientation orientation;

    std::string type;
    ThrowIfFalse(SimulationCommon::ParseAttributeString(orientationElement, ATTRIBUTE::type, type),
                 orientationElement, "Attribute " + std::string(ATTRIBUTE::type) + " is missing.");
    ThrowIfFalse(type == "relative", orientationElement, "Scenario Importer: only relative orientation is allowed.");
    orientation.type = openScenario::OrientationType::Relative;

    double heading;
    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(orientationElement, ATTRIBUTE::h, heading),
                 orientationElement, "Attribute " + std::string(ATTRIBUTE::h) + " is missing.");
    orientation.h = heading;

    return orientation;
}

void ScenarioImporter::ImportPrivateElement(QDomElement& privateElement,
        std::vector<ScenarioEntity>& entities)
{
    std::string object;
    ThrowIfFalse(SimulationCommon::ParseAttributeString(privateElement, ATTRIBUTE::object, object),
                 privateElement, "Attribute " + std::string(ATTRIBUTE::object) + " is missing.");

    ScenarioEntity* scenarioEntity = GetEntityByName(entities, object);

    ThrowIfFalse(scenarioEntity, privateElement,
                 (std::string("Action object '") + object + "' not declared in 'Entities'"));

    QDomElement actionElement;
    SimulationCommon::GetFirstChildElement(privateElement, TAG::action, actionElement);

    while (!actionElement.isNull())
    {
        QDomElement firstChildOfActionElement;
        if (SimulationCommon::GetFirstChildElement(actionElement, TAG::longitudinal, firstChildOfActionElement))
        {
            ImportLongitudinalElement(*scenarioEntity, firstChildOfActionElement);
        }
        else if (SimulationCommon::GetFirstChildElement(actionElement, TAG::position, firstChildOfActionElement))
        {
            ImportPositionElement(*scenarioEntity, firstChildOfActionElement);
        }
        else if (SimulationCommon::GetFirstChildElement(actionElement, TAG::routing, firstChildOfActionElement))
        {
            ImportRoutingElement(*scenarioEntity, firstChildOfActionElement);
        }

        actionElement = actionElement.nextSiblingElement(TAG::action);
    }
}

void ScenarioImporter::ImportPrivateElements(QDomElement& actionsElement, std::vector<ScenarioEntity>& entities)
{
    QDomElement privateElement;
    SimulationCommon::GetFirstChildElement(actionsElement, TAG::Private, privateElement);

    while (!privateElement.isNull())
    {
        ImportPrivateElement(privateElement, entities);
        privateElement = privateElement.nextSiblingElement(TAG::Private);
    }
}

void ScenarioImporter::ImportInitElement(QDomElement& initElement, std::vector<ScenarioEntity>& entities)
{
    // for initial entitiy parameters we just use first child "Actions" --> others will be ignore
    QDomElement actionsElement;
    SimulationCommon::GetFirstChildElement(initElement, TAG::actions, actionsElement);

    ImportPrivateElements(actionsElement, entities);
}

void ScenarioImporter::ImportEndConditionsFromStoryboard(const QDomElement& storyboardElement, ScenarioInterface* scenario)
{
    bool endConditionProvided = false;

    QDomElement endConditionsElement;
    if (SimulationCommon::GetFirstChildElement(storyboardElement, TAG::endConditions, endConditionsElement))
    {
        QDomElement conditionGroupElement;
        if (SimulationCommon::GetFirstChildElement(endConditionsElement, TAG::conditionGroup, conditionGroupElement))
        {
            QDomElement conditionElement;
            if (SimulationCommon::GetFirstChildElement(conditionGroupElement, TAG::condition, conditionElement))
            {
                // these attributes are required by OpenSCENARIO standard, but have no impact on behaviour
                std::string conditionName;
                double conditionDelay;
                std::string conditionEdge;

                ParseConditionAttributes(conditionElement, conditionName, conditionDelay, conditionEdge);
                ThrowIfFalse(conditionDelay == 0.0, conditionElement,
                             "End Condition specifies unsupported delay value. Condition delay attribute not equal to zero not currently supported.");

                ThrowIfFalse(conditionEdge == "rising", conditionElement,
                             "End Condition specifies unsupported edge value. Condition edge attribute not equal to 'rising' not currently supported.");

                QDomElement byValueElement;
                if (SimulationCommon::GetFirstChildElement(conditionElement, TAG::byValue, byValueElement))
                {
                    double endTime;
                    std::string rule;

                    ParseSimulationTime(byValueElement, endTime, rule);
                    ThrowIfFalse(rule == "greater_than", byValueElement,
                                 "End Condition specifies unsupported rule. SimulationTime rule attribute value '" + rule + "' not supported; defaulting to 'greater_than'.");

                    scenario->SetEndTime(endTime);
                    endConditionProvided = true;
                }
            }
        }
    }

    ThrowIfFalse(endConditionProvided, "Scenario provides no EndConditions for the simulation");
}

void ScenarioImporter::ParseConditionAttributes(const QDomElement& conditionElement, std::string& name, double& delay, std::string& edge)
{
    ThrowIfFalse(SimulationCommon::ParseAttributeString(conditionElement, ATTRIBUTE::name, name),
                 conditionElement, "Attribute " + std::string(ATTRIBUTE::name) + " is missing.");
    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(conditionElement, ATTRIBUTE::delay, delay),
                 conditionElement, "Attribute " + std::string(ATTRIBUTE::delay) + " is missing.");
    ThrowIfFalse(delay >= 0.0,
                 conditionElement, "Invalid delay value specified for condition");
    ThrowIfFalse(SimulationCommon::ParseAttributeString(conditionElement, ATTRIBUTE::edge, edge),
                 conditionElement, "Attribute " + std::string(ATTRIBUTE::edge) + " is missing.");
}

void ScenarioImporter::ParseSimulationTime(const QDomElement& byValueElement, double& value, std::string& rule)
{
    QDomElement simulationTimeElement;
    if(SimulationCommon::GetFirstChildElement(byValueElement, "SimulationTime", simulationTimeElement))
    {
        ThrowIfFalse(SimulationCommon::ParseAttributeDouble(simulationTimeElement, ATTRIBUTE::value, value),
                     simulationTimeElement, "Attribute " + std::string(ATTRIBUTE::value) + " is missing.");

        if(SimulationCommon::ParseAttributeString(simulationTimeElement, ATTRIBUTE::rule, rule))
        {
            ThrowIfFalse((rule == "greater_than"
                            || rule == "less_than"
                            || rule == "equal_to"),
                         simulationTimeElement, "Simulation rule attribute value '" + rule + "' not valid");
        }
    }
}

void ScenarioImporter::ImportParameterElement(QDomElement& parameterElement, openpass::parameter::Container& parameters)
{
    std::string parameterName;
    ThrowIfFalse(SimulationCommon::ParseAttributeString(parameterElement, ATTRIBUTE::name, parameterName),
                 parameterElement, "Attribute " + std::string(ATTRIBUTE::name) + " is missing.");

    std::string parameterType;
    ThrowIfFalse(SimulationCommon::ParseAttributeString(parameterElement, ATTRIBUTE::type, parameterType),
                 parameterElement, "Attribute " + std::string(ATTRIBUTE::type) + " is missing.");

    switch (parameterTypes.at(parameterType))
    {
        case 0:
            bool parameterValueBool;
            ThrowIfFalse(SimulationCommon::ParseAttributeBool(parameterElement, ATTRIBUTE::value, parameterValueBool),
                         parameterElement, "Attribute " + std::string(ATTRIBUTE::value) + " is missing.");
            parameters.emplace_back(parameterName, parameterValueBool);
            break;

        case 1:
            int parameterValueInt;
            ThrowIfFalse(SimulationCommon::ParseAttributeInt(parameterElement, ATTRIBUTE::value, parameterValueInt),
                         parameterElement, "Attribute " + std::string(ATTRIBUTE::value) + " is missing.");
            parameters.emplace_back(parameterName, parameterValueInt);
            break;

        case 2:
            double parameterValueDouble;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(parameterElement, ATTRIBUTE::value, parameterValueDouble),
                         parameterElement, "Attribute " + std::string(ATTRIBUTE::value) + " is missing.");
            parameters.emplace_back(parameterName, parameterValueDouble);
            break;

        case 3:
            std::string parameterValueString;
            ThrowIfFalse(SimulationCommon::ParseAttributeString(parameterElement, ATTRIBUTE::value, parameterValueString),
                         parameterElement, "Attribute " + std::string(ATTRIBUTE::value) + " is missing.");
            parameters.emplace_back(parameterName, parameterValueString);
            break;
    }
}

void ScenarioImporter::ImportParameterDeclarationElement(QDomElement& parameterDeclarationElement,
        openpass::parameter::Container& parameters)
{
    QDomElement parameterElement;
    if (SimulationCommon::GetFirstChildElement(parameterDeclarationElement, TAG::parameter, parameterElement))
    {
        while (!parameterElement.isNull())
        {
            ImportParameterElement(parameterElement, parameters);
            parameterElement = parameterElement.nextSiblingElement(TAG::parameter);
        }
    }
}

std::string ScenarioImporter::ImportCatalog(const std::string& catalogName,
                                            QDomElement& catalogsElement)
{
    QDomElement catalogElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(catalogsElement, catalogName, catalogElement),
                catalogsElement, "Tag " + catalogName + " is missing.");

    QDomElement directoryElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(catalogElement, TAG::directory, directoryElement),
                 catalogElement, "Tag " + std::string(TAG::directory) + " is missing.");

    std::string catalogPath;
    ThrowIfFalse(SimulationCommon::ParseAttributeString(directoryElement, ATTRIBUTE::path, catalogPath),
                 directoryElement, "Attribute " + std::string(ATTRIBUTE::path) + " is missing.");

    return catalogPath;
}

void ScenarioImporter::ImportEntities(QDomElement& documentRoot, std::vector<ScenarioEntity>& entities, std::map<std::string, std::list<std::string>> &groups)
{
    QDomElement entitiesElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(documentRoot, TAG::entities, entitiesElement),
                 documentRoot, "Tag " + std::string(TAG::entities) + " is missing.");

    QDomElement entityElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(entitiesElement, TAG::object, entityElement),
                 entitiesElement, "Tag " + std::string(TAG::object) + " is missing.");

    while (!entityElement.isNull())
    {
        ScenarioEntity entity;

        ImportEntity(entityElement, entity);

        entities.push_back(entity);
        entityElement = entityElement.nextSiblingElement(TAG::object);
    }

    // Parse selection elements (there can be as few as zero and as many as one wants)
    ImportSelectionElements(entitiesElement, groups);
}

void ScenarioImporter::ImportSelectionElements(QDomElement &entitiesElement, std::map<std::string, std::list<std::string>> &groups)
{
    QDomElement selectionElement;
    if(SimulationCommon::GetFirstChildElement(entitiesElement, TAG::selection, selectionElement))
    {
        std::string selectionName;
        QDomElement membersElement;
        std::list<std::string> members;
        while (!selectionElement.isNull())
        {
            ThrowIfFalse(SimulationCommon::ParseAttributeString(selectionElement, ATTRIBUTE::name, selectionName),
                         selectionElement, "Attribute " + std::string(ATTRIBUTE::name) + " is missing.");
            ThrowIfFalse(SimulationCommon::GetFirstChildElement(selectionElement, TAG::members, membersElement),
                         selectionElement, "Tag " + std::string(TAG::members) + " is missing.");

            ImportMembers(membersElement, members);
            groups.insert({selectionName, members});

            selectionElement = selectionElement.nextSiblingElement(TAG::selection);
        }
    }

    if (groups.find("ScenarioAgents") == groups.cend())
    {
        LOG_INTERN(LogLevel::Info) << "ScenarioAgents selection is not defined. Adding empty one.";
        groups.insert({"ScenarioAgents", {}});
    }
}

void ScenarioImporter::ImportEntity(QDomElement& entityElement, ScenarioEntity& entity)
{
    ThrowIfFalse(SimulationCommon::ParseAttributeString(entityElement, ATTRIBUTE::name, entity.name),
                 entityElement, "Attribute " + std::string(ATTRIBUTE::name) + " is missing.");
    ThrowIfFalse(entity.name.size() > 0,
                 entityElement, "Length of entity object name has to be greater than 0");

    QDomElement catalogReferenceElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(entityElement, TAG::catalogReference, catalogReferenceElement),
                 entityElement, "Tag " + std::string(TAG::catalogReference) + " is missing.");
    ImportEntityCatalogReference(catalogReferenceElement, entity);
}

void ScenarioImporter::ImportMembers(const QDomElement &membersElement, std::list<std::string> &members)
{
    QDomElement byEntityElement;
    if(SimulationCommon::GetFirstChildElement(membersElement, TAG::byEntity, byEntityElement))
    {
        std::string memberName;
        while(!byEntityElement.isNull())
        {
            SimulationCommon::ParseAttributeString(byEntityElement, ATTRIBUTE::name, memberName);
            members.push_back(memberName);

            byEntityElement = byEntityElement.nextSiblingElement(TAG::byEntity);
        }
    }
}

void ScenarioImporter::ImportEntityCatalogReference(QDomElement& catalogReferenceElement, ScenarioEntity& entity)
{
    ThrowIfFalse(SimulationCommon::ParseAttributeString(catalogReferenceElement, ATTRIBUTE::catalogName, entity.catalogReference.catalogName),
                 catalogReferenceElement, "Attribute " + std::string(ATTRIBUTE::catalogName) + " is missing.");
    ThrowIfFalse(entity.catalogReference.catalogName.size() > 0,
                 catalogReferenceElement, "Length of 'catalogName' has to be greater than 0");
    ThrowIfFalse(SimulationCommon::ParseAttributeString(catalogReferenceElement, ATTRIBUTE::entryName, entity.catalogReference.entryName),
                 catalogReferenceElement, "Attribute " + std::string(ATTRIBUTE::entryName) + " is missing.");
    ThrowIfFalse(entity.catalogReference.entryName.size() > 0,
                 catalogReferenceElement, "Length of 'entryName' has to be greater than 0");
}

ScenarioEntity* ScenarioImporter::GetEntityByName(std::vector<ScenarioEntity>& entities, const std::string& entityName)
{
    auto entitiesFound = std::find_if(entities.begin(),
                                      entities.end(),
                                      [entityName](ScenarioEntity & elem)
    {
        return elem.name == entityName;
    });

    if (entitiesFound != entities.end())
    {
        return &(*entitiesFound);
    }

    return nullptr;
}

void ScenarioImporter::CategorizeEntities(const std::vector<ScenarioEntity>& entities, const std::map<std::string, std::list<std::string>> &groups, ScenarioInterface* scenario)
{
    for (const auto& entity : entities)
    {
        scenario->AddScenarioEntity(entity);
    }

    scenario->AddScenarioGroupsByEntityNames(groups);
}

bool ScenarioImporter::ContainsEntity(const std::vector<ScenarioEntity>& entities,
                                      const std::string& entityName)
{
    auto entitiesFound = std::find_if(entities.cbegin(),
                                      entities.cend(),
                                      [entityName](const ScenarioEntity& elem)
    {
        return elem.name == entityName;
    });

    return entitiesFound != entities.cend();
}

} //namespace Importer
