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
        QString errorMsg{};
        int errorLine{};
        ThrowIfFalse(document.setContent(xmlData, &errorMsg, &errorLine), "Invalid xml format (" + filename + ") in line " + std::to_string(errorLine) + ": " + errorMsg.toStdString());

        QDomElement documentRoot = document.documentElement();
        ThrowIfFalse(!documentRoot.isNull(), "Scenario xml has no document root");

        openScenario::Parameters parameters;

        QDomElement parameterDeclarationElement;

        if (SimulationCommon::GetFirstChildElement(documentRoot, TAG::parameterDeclaration, parameterDeclarationElement))
        {
            ImportParameterDeclarationElement(parameterDeclarationElement, parameters);
        }

        ImportAndValidateVersion(parameters);

        std::string sceneryPath;
        ImportRoadNetwork(documentRoot, sceneryPath, parameters);
        scenario->SetSceneryPath(sceneryPath);

        auto path = Directories::StripFile(filename);
        ImportCatalogs(documentRoot, scenario, path, parameters);

        std::vector<ScenarioEntity> entities;
        std::map<std::string, std::list<std::string>> groups;
        ImportEntities(documentRoot, entities, groups, parameters);

        ImportStoryboard(documentRoot, entities, scenario, parameters);
        CategorizeEntities(entities, groups, scenario);

        return true;
    }
    catch(std::runtime_error& e)
    {
        LOG_INTERN(LogLevel::Error) << "Scenario import failed: " + std::string(e.what());
        return false;
    }
}

void ScenarioImporter::ImportAndValidateVersion(openScenario::Parameters& parameters)
{
    const std::string VERSION_ATTRIBUTE{"OP_OSC_SchemaVersion"};
    ThrowIfFalse(parameters.count(VERSION_ATTRIBUTE), "Cannot determine scenario version");
    const auto version = std::get<std::string>(parameters.at(VERSION_ATTRIBUTE));
    ThrowIfFalse(version == supportedScenarioVersion,
                 "Scenario version not supported (" + version + "). Supported version is " + supportedScenarioVersion);
}

void ScenarioImporter::ImportCatalogs(QDomElement& documentRoot, ScenarioInterface* scenario, const std::string& path, openScenario::Parameters& parameters)
{
    QDomElement catalogsElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(documentRoot, TAG::catalogs, catalogsElement),
                 documentRoot, "Tag " + std::string(TAG::catalogs) + " is missing.");

    const auto vehicleCatalogPath = ImportCatalog("VehicleCatalog", catalogsElement, parameters);
    scenario->SetVehicleCatalogPath(vehicleCatalogPath);

    const auto pedestrianCatalogPath = ImportCatalog("PedestrianCatalog", catalogsElement, parameters);
    scenario->SetPedestrianCatalogPath(pedestrianCatalogPath);

    auto trajectoryCatalogPath = ImportCatalog("TrajectoryCatalog", catalogsElement, parameters);

    if (Directories::IsRelative(trajectoryCatalogPath))
    {
        trajectoryCatalogPath = Directories::Concat(path, trajectoryCatalogPath);
    }

    scenario->SetTrajectoryCatalogPath(trajectoryCatalogPath);
}

void ScenarioImporter::ImportRoadNetwork(QDomElement& documentRoot, std::string& sceneryPath, openScenario::Parameters& parameters)
{
    QDomElement roadNetworkElement;
    QDomElement logicsElement;

    ThrowIfFalse(SimulationCommon::GetFirstChildElement(documentRoot, TAG::roadNetwork, roadNetworkElement),
                 documentRoot, "Tag " + std::string(TAG::roadNetwork) + " is missing.");
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(roadNetworkElement, TAG::logics, logicsElement),
                 roadNetworkElement, "Tag " + std::string(TAG::logics) + " is missing.");
    sceneryPath = ParseAttribute<std::string>(logicsElement, ATTRIBUTE::filePath, parameters);
}

void ScenarioImporter::ImportStoryboard(QDomElement& documentRoot, std::vector<ScenarioEntity>& entities,
                                        ScenarioInterface* scenario, openScenario::Parameters& parameters)
{
    QDomElement storyboardElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(documentRoot, TAG::storyboard, storyboardElement),
                 documentRoot, "Tag " + std::string(TAG::storyboard) + " is missing.");

    //Import Init
    QDomElement initElement;
    // for initial entitiy parameters we just use first child "Init" --> others will be ignore
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(storyboardElement, TAG::init, initElement),
                 storyboardElement, "Tag " + std::string(TAG::init) + " is missing.");
    ImportInitElement(initElement, entities, parameters);

    // Import Story
    QDomElement storyElement;

    SimulationCommon::GetFirstChildElement(storyboardElement, TAG::story, storyElement);

    while (!storyElement.isNull())
    {
        ImportStoryElement(storyElement, entities, scenario, parameters);
        storyElement = storyElement.nextSiblingElement(TAG::story);
    }

    // Import EndCondition
    ImportEndConditionsFromStoryboard(storyboardElement, scenario, parameters);
}

void ScenarioImporter::ImportStoryElement(QDomElement& storyElement,
                                          const std::vector<ScenarioEntity>& entities,
                                          ScenarioInterface* scenario,
                                          openScenario::Parameters& parameters)
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
                    numberOfExecutions = ParseAttribute<int>(seqElement, ATTRIBUTE::numberOfExecutions, parameters);

                    QDomElement actorsElement;
                    openScenario::ActorInformation actorInformation;
                    if (SimulationCommon::GetFirstChildElement(seqElement, TAG::actors, actorsElement)) // just one actors per sequence
                    {
                        actorInformation = ImportActors(actorsElement, entities, parameters);
                    }

                    QDomElement maneuverElement;
                    if (SimulationCommon::GetFirstChildElement(seqElement, TAG::maneuver, maneuverElement)) // just one maneuver per sequence
                    {
                        ImportManeuverElement(maneuverElement, entities, scenario, actorInformation, numberOfExecutions, parameters);
                    }

                    seqElement = seqElement.nextSiblingElement(TAG::sequence);
                }
            }
            actElement = actElement.nextSiblingElement(TAG::act);
        }
    }
}

openScenario::ActorInformation ScenarioImporter::ImportActors(QDomElement& actorsElement,
                                                              const std::vector<ScenarioEntity>& entities,
                                                              openScenario::Parameters& parameters)
{
    openScenario::ActorInformation actorInformation;

    QDomElement entityElement;
    SimulationCommon::GetFirstChildElement(actorsElement, TAG::entity, entityElement);

    while (!entityElement.isNull())
    {
        std::string entityName = ParseAttribute<std::string>(entityElement, ATTRIBUTE::name, parameters);
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
        std::string actor = ParseAttribute<std::string>(byConditionElement, ATTRIBUTE::actor, parameters);
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
                                             const int numberOfExecutions,
                                             openScenario::Parameters& parameters)
{
    // handle parameterdeclaration element(s) for event(s)
    QDomElement parameterDeclarationElement;
    SimulationCommon::GetFirstChildElement(maneuverElement, TAG::parameterDeclaration, parameterDeclarationElement);

    // handle event element
    QDomElement eventElement;
    SimulationCommon::GetFirstChildElement(maneuverElement, TAG::event, eventElement);

    while (!eventElement.isNull())
    {
        std::string eventName = ParseAttribute<std::string>(eventElement, ATTRIBUTE::name, parameters);

        openScenario::ConditionalEventDetectorInformation conditionalEventDetectorInformation = EventDetectorImporter::ImportEventDetector(eventElement,
                                                                                                                                           eventName,
                                                                                                                                           numberOfExecutions,
                                                                                                                                           actorInformation,
                                                                                                                                           entities,
                                                                                                                                           parameters);

        scenario->AddConditionalEventDetector(conditionalEventDetectorInformation);

        std::shared_ptr<ScenarioActionInterface> action = ManipulatorImporter::ImportManipulator(eventElement,
                                                                                                 eventName,
                                                                                                 scenario->GetTrajectoryCatalogPath(),
                                                                                                 parameters);
        scenario->AddAction(action);

        eventElement = eventElement.nextSiblingElement(TAG::event);
    }
}

void ScenarioImporter::ImportLongitudinalElement(ScenarioEntity& scenarioEntity, QDomElement firstChildOfActionElement, openScenario::Parameters& parameters)
{
    SpawnInfo &spawnInfo = scenarioEntity.spawnInfo;

    QDomElement speedElement;
    if (SimulationCommon::GetFirstChildElement(firstChildOfActionElement, TAG::speed, speedElement))
    {
        QDomElement dynamicsElement;
        if (SimulationCommon::GetFirstChildElement(speedElement, TAG::dynamics, dynamicsElement))
        {
            if (SimulationCommon::HasAttribute(dynamicsElement, ATTRIBUTE::rate))
            {
                double rate = ParseAttribute<double>(dynamicsElement, ATTRIBUTE::rate, parameters);
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
                if (SimulationCommon::HasAttribute(absoluteElement, ATTRIBUTE::value))
                {
                    double velocity = ParseAttribute<double>(absoluteElement, ATTRIBUTE::value, parameters);
                    spawnInfo.velocity = velocity;
                }
            }
        }

        // Parse stochastics if available
        QDomElement stochasticElement;
        SimulationCommon::GetFirstChildElement(speedElement, TAG::stochastics, stochasticElement);
        while (!stochasticElement.isNull())
        {
            const auto& [attributeName, stochasticInformation] = ImportStochastics(stochasticElement, parameters);
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

openScenario::LanePosition ScenarioImporter::ImportLanePosition(QDomElement positionElement, openScenario::Parameters& parameters)
{
    openScenario::LanePosition lanePosition;
    lanePosition.s = ParseAttribute<double>(positionElement, ATTRIBUTE::s, parameters);
    lanePosition.laneId = ParseAttribute<int>(positionElement, ATTRIBUTE::laneId, parameters);
    lanePosition.roadId = ParseAttribute<std::string>(positionElement, ATTRIBUTE::roadId, parameters);

    if(SimulationCommon::HasAttribute(positionElement, ATTRIBUTE::offset))
    {
        lanePosition.offset = ParseAttribute<double>(positionElement, ATTRIBUTE::offset, parameters);
    }

    //Parse optional stochastics
    QDomElement stochasticElement;
    SimulationCommon::GetFirstChildElement(positionElement, TAG::stochastics, stochasticElement);
    while (!stochasticElement.isNull())
    {
        const auto& stochasticInformation = ImportStochastics(stochasticElement, parameters);

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
        lanePosition.orientation = ImportOrientation(orientationElement, parameters);
    }

    return lanePosition;
}

openScenario::WorldPosition ScenarioImporter::ImportWorldPosition(QDomElement positionElement, openScenario::Parameters& parameters)
{
    openScenario::WorldPosition worldPosition;
    worldPosition.x = ParseAttribute<double>(positionElement, ATTRIBUTE::x, parameters);
    worldPosition.y= ParseAttribute<double>(positionElement, ATTRIBUTE::y, parameters);

    if(SimulationCommon::HasAttribute(positionElement, ATTRIBUTE::h))
    {
        worldPosition.heading = ParseAttribute<double>(positionElement, ATTRIBUTE::h, parameters);
    }

    return worldPosition;
}

void ScenarioImporter::ImportPositionElement(ScenarioEntity& scenarioEntity, QDomElement firstChildOfActionElement, openScenario::Parameters& parameters)
{
    QDomElement positionElement;
    if(SimulationCommon::GetFirstChildElement(firstChildOfActionElement, TAG::lane, positionElement))
    {
        scenarioEntity.spawnInfo.position = ImportLanePosition(positionElement, parameters);
    }
    else
    {
        ThrowIfFalse(SimulationCommon::GetFirstChildElement(firstChildOfActionElement, TAG::world, positionElement),
                  firstChildOfActionElement, "OSCPosition type not supported. Currently only World and Lane are supported!");
        scenarioEntity.spawnInfo.position = ImportWorldPosition(positionElement, parameters);
    }
}

void ScenarioImporter::ImportRoutingElement(ScenarioEntity& scenarioEntity, QDomElement firstChildOfActionElement, openScenario::Parameters& parameters)
{
    std::vector<RouteElement> route;

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
        std::string roadId = ParseAttribute<std::string>(roadElement, ATTRIBUTE::roadId, parameters);
        double t = ParseAttribute<double>(roadElement, ATTRIBUTE::t, parameters);
        bool inRoadDirection = (t <= 0);
        route.push_back({roadId, inRoadDirection});

        waypointElement = waypointElement.nextSiblingElement(TAG::waypoint);
    }
    scenarioEntity.spawnInfo.route = route;
}

std::pair<std::string, openScenario::StochasticAttribute> ScenarioImporter::ImportStochastics(QDomElement& stochasticsElement, openScenario::Parameters& parameters)
{
    std::string attributeName = ParseAttribute<std::string>(stochasticsElement, ATTRIBUTE::value, parameters);

    openScenario::StochasticAttribute stochasticAttribute;
    stochasticAttribute.stdDeviation = ParseAttribute<double>(stochasticsElement, ATTRIBUTE::stdDeviation, parameters);
    stochasticAttribute.lowerBoundary = ParseAttribute<double>(stochasticsElement, ATTRIBUTE::lowerBound, parameters);
    stochasticAttribute.upperBoundary = ParseAttribute<double>(stochasticsElement, ATTRIBUTE::upperBound, parameters);

    return {attributeName, stochasticAttribute};
}

openScenario::Orientation ScenarioImporter::ImportOrientation(QDomElement& orientationElement, openScenario::Parameters& parameters)
{
    openScenario::Orientation orientation;

    std::string type = ParseAttribute<std::string>(orientationElement, ATTRIBUTE::type, parameters);
    ThrowIfFalse(type == "relative", orientationElement, "Scenario Importer: only relative orientation is allowed.");
    orientation.type = openScenario::OrientationType::Relative;

    double heading = ParseAttribute<double>(orientationElement, ATTRIBUTE::h, parameters);
    orientation.h = heading;

    return orientation;
}

void ScenarioImporter::ImportPrivateElement(QDomElement& privateElement,
        std::vector<ScenarioEntity>& entities, openScenario::Parameters& parameters)
{
    std::string object = ParseAttribute<std::string>(privateElement, ATTRIBUTE::object, parameters);

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
            ImportLongitudinalElement(*scenarioEntity, firstChildOfActionElement, parameters);
        }
        else if (SimulationCommon::GetFirstChildElement(actionElement, TAG::position, firstChildOfActionElement))
        {
            ImportPositionElement(*scenarioEntity, firstChildOfActionElement, parameters);
        }
        else if (SimulationCommon::GetFirstChildElement(actionElement, TAG::routing, firstChildOfActionElement))
        {
            ImportRoutingElement(*scenarioEntity, firstChildOfActionElement, parameters);
        }

        actionElement = actionElement.nextSiblingElement(TAG::action);
    }
}

void ScenarioImporter::ImportPrivateElements(QDomElement& actionsElement, std::vector<ScenarioEntity>& entities, openScenario::Parameters& parameters)
{
    QDomElement privateElement;
    SimulationCommon::GetFirstChildElement(actionsElement, TAG::Private, privateElement);

    while (!privateElement.isNull())
    {
        ImportPrivateElement(privateElement, entities, parameters);
        privateElement = privateElement.nextSiblingElement(TAG::Private);
    }
}

void ScenarioImporter::ImportInitElement(QDomElement& initElement, std::vector<ScenarioEntity>& entities, openScenario::Parameters& parameters)
{
    // for initial entitiy parameters we just use first child "Actions" --> others will be ignore
    QDomElement actionsElement;
    SimulationCommon::GetFirstChildElement(initElement, TAG::actions, actionsElement);

    ImportPrivateElements(actionsElement, entities, parameters);
}

void ScenarioImporter::ImportEndConditionsFromStoryboard(const QDomElement& storyboardElement, ScenarioInterface* scenario, openScenario::Parameters& parameters)
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

                ParseConditionAttributes(conditionElement, conditionName, conditionDelay, conditionEdge, parameters);
                ThrowIfFalse(conditionDelay == 0.0, conditionElement,
                             "End Condition specifies unsupported delay value. Condition delay attribute not equal to zero not currently supported.");

                ThrowIfFalse(conditionEdge == "rising", conditionElement,
                             "End Condition specifies unsupported edge value. Condition edge attribute not equal to 'rising' not currently supported.");

                QDomElement byValueElement;
                if (SimulationCommon::GetFirstChildElement(conditionElement, TAG::byValue, byValueElement))
                {
                    double endTime;
                    std::string rule;

                    ParseSimulationTime(byValueElement, endTime, rule, parameters);
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

void ScenarioImporter::ParseConditionAttributes(const QDomElement& conditionElement, std::string& name, double& delay, std::string& edge, openScenario::Parameters& parameters)
{
    name = ParseAttribute<std::string>(conditionElement, ATTRIBUTE::name, parameters);
    delay = ParseAttribute<double>(conditionElement, ATTRIBUTE::delay, parameters);
    ThrowIfFalse(delay >= 0.0,
                 conditionElement, "Invalid delay value specified for condition");
    edge = ParseAttribute<std::string>(conditionElement, ATTRIBUTE::edge, parameters);
}

void ScenarioImporter::ParseSimulationTime(const QDomElement& byValueElement, double& value, std::string& rule, openScenario::Parameters& parameters)
{
    QDomElement simulationTimeElement;
    if(SimulationCommon::GetFirstChildElement(byValueElement, "SimulationTime", simulationTimeElement))
    {
        value = ParseAttribute<double>(simulationTimeElement, ATTRIBUTE::value, parameters);

        rule = ParseAttribute<std::string>(simulationTimeElement, ATTRIBUTE::rule, parameters);
        {
            ThrowIfFalse((rule == "greater_than"
                            || rule == "less_than"
                            || rule == "equal_to"),
                         simulationTimeElement, "Simulation rule attribute value '" + rule + "' not valid");
        }
    }
}

std::string ScenarioImporter::ImportCatalog(const std::string& catalogName,
                                            QDomElement& catalogsElement,
                                            openScenario::Parameters& parameters)
{
    QDomElement catalogElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(catalogsElement, catalogName, catalogElement),
                catalogsElement, "Tag " + catalogName + " is missing.");

    QDomElement directoryElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(catalogElement, TAG::directory, directoryElement),
                 catalogElement, "Tag " + std::string(TAG::directory) + " is missing.");

    std::string catalogPath = ParseAttribute<std::string>(directoryElement, ATTRIBUTE::path, parameters);

    return catalogPath;
}

void ScenarioImporter::ImportEntities(QDomElement& documentRoot, std::vector<ScenarioEntity>& entities, std::map<std::string, std::list<std::string>> &groups, openScenario::Parameters& parameters)
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

        ImportEntity(entityElement, entity, parameters);

        entities.push_back(entity);
        entityElement = entityElement.nextSiblingElement(TAG::object);
    }

    // Parse selection elements (there can be as few as zero and as many as one wants)
    ImportSelectionElements(entitiesElement, groups, parameters);
}

void ScenarioImporter::ImportSelectionElements(QDomElement &entitiesElement, std::map<std::string, std::list<std::string>> &groups, openScenario::Parameters& parameters)
{
    QDomElement selectionElement;
    if(SimulationCommon::GetFirstChildElement(entitiesElement, TAG::selection, selectionElement))
    {
        QDomElement membersElement;
        std::list<std::string> members;
        while (!selectionElement.isNull())
        {
            std::string selectionName = ParseAttribute<std::string>(selectionElement, ATTRIBUTE::name, parameters);
            ThrowIfFalse(SimulationCommon::GetFirstChildElement(selectionElement, TAG::members, membersElement),
                         selectionElement, "Tag " + std::string(TAG::members) + " is missing.");

            ImportMembers(membersElement, members, parameters);
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

void ScenarioImporter::ImportEntity(QDomElement& entityElement, ScenarioEntity& entity, openScenario::Parameters& parameters)
{
    entity.name = ParseAttribute<std::string>(entityElement, ATTRIBUTE::name, parameters);
    ThrowIfFalse(entity.name.size() > 0,
                 entityElement, "Length of entity object name has to be greater than 0");

    QDomElement catalogReferenceElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(entityElement, TAG::catalogReference, catalogReferenceElement),
                 entityElement, "Tag " + std::string(TAG::catalogReference) + " is missing.");
    ImportEntityCatalogReference(catalogReferenceElement, entity, parameters);
}

void ScenarioImporter::ImportMembers(const QDomElement &membersElement, std::list<std::string> &members, openScenario::Parameters& parameters)
{
    QDomElement byEntityElement;
    if(SimulationCommon::GetFirstChildElement(membersElement, TAG::byEntity, byEntityElement))
    {
        while(!byEntityElement.isNull())
        {
            std::string memberName = ParseAttribute<std::string>(byEntityElement, ATTRIBUTE::name, parameters);
            members.push_back(memberName);

            byEntityElement = byEntityElement.nextSiblingElement(TAG::byEntity);
        }
    }
}

void ScenarioImporter::ImportEntityCatalogReference(QDomElement& catalogReferenceElement, ScenarioEntity& entity, openScenario::Parameters& parameters)
{
    entity.catalogReference.catalogName = ParseAttribute<std::string>(catalogReferenceElement, ATTRIBUTE::catalogName, parameters);
    ThrowIfFalse(entity.catalogReference.catalogName.size() > 0,
                 catalogReferenceElement, "Length of 'catalogName' has to be greater than 0");
    entity.catalogReference.entryName = ParseAttribute<std::string>(catalogReferenceElement, ATTRIBUTE::entryName, parameters);
    ThrowIfFalse(entity.catalogReference.entryName.size() > 0,
                 catalogReferenceElement, "Length of 'entryName' has to be greater than 0");
    QDomElement parameterAssignmentsElement;
    if(SimulationCommon::GetFirstChildElement(catalogReferenceElement, TAG::parameterAssignments, parameterAssignmentsElement))
    {
        QDomElement parameterAssignmentElement;
        SimulationCommon::GetFirstChildElement(parameterAssignmentsElement, TAG::parameterAssignment, parameterAssignmentElement);
        while (!parameterAssignmentElement.isNull())
        {
            auto name = ParseAttribute<std::string>(parameterAssignmentElement, ATTRIBUTE::parameterRef, parameters);
            auto value = ParseAttribute<std::string>(parameterAssignmentElement, ATTRIBUTE::value, parameters);
            entity.assignedParameters.insert({name, value});
            parameterAssignmentElement = parameterAssignmentElement.nextSiblingElement(TAG::parameterAssignment);
        }
    }
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
