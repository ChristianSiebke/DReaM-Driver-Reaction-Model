/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020, 2021 in-tech GmbH
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
#include "scenario.h"
#include "scenarioImporter.h"
#include "importer/importerCommon.h"
#include "directories.h"
#include "common/openPassUtils.h"
#include "scenarioImporterHelper.h"

namespace TAG = openpass::importer::xml::scenarioImporter::tag;
namespace ATTRIBUTE = openpass::importer::xml::scenarioImporter::attribute;
namespace RULE = openpass::importer::xml::openScenario::rule;
namespace DYNAMICSSHAPE = openpass::importer::xml::openScenario::dynamicsShape;
namespace DYNAMICSDIMENSION = openpass::importer::xml::openScenario::dynamicsDimension;

using Directories = openpass::core::Directories;

using namespace openScenario;

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

        if (SimulationCommon::GetFirstChildElement(documentRoot, TAG::parameterDeclarations, parameterDeclarationElement))
        {
            ImportParameterDeclarationElement(parameterDeclarationElement, parameters);
        }

        ImportAndValidateVersion(parameters);

        ImportRoadNetwork(documentRoot, scenario, parameters);

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
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(documentRoot, TAG::catalogLocations, catalogsElement),
                 documentRoot, "Tag " + std::string(TAG::catalogLocations) + " is missing.");

    const auto vehicleCatalogPath = ImportCatalog(TAG::vehicleCatalog, catalogsElement, parameters);
    scenario->SetVehicleCatalogPath(vehicleCatalogPath);

    const auto pedestrianCatalogPath = ImportCatalog(TAG::pedestrianCatalog, catalogsElement, parameters);
    scenario->SetPedestrianCatalogPath(pedestrianCatalogPath);

    auto trajectoryCatalogPath = ImportCatalog(TAG::trajectoryCatalog, catalogsElement, parameters);

    if (Directories::IsRelative(trajectoryCatalogPath))
    {
        trajectoryCatalogPath = Directories::Concat(path, trajectoryCatalogPath);
    }

    scenario->SetTrajectoryCatalogPath(trajectoryCatalogPath);
}

void ScenarioImporter::ImportRoadNetwork(QDomElement& documentRoot, ScenarioInterface *scenario, openScenario::Parameters& parameters)
{
    QDomElement roadNetworkElement;
    QDomElement logicsElement;
    QDomElement trafficSignalsElement;
    QDomElement trafficSignalControllerElement;

    ThrowIfFalse(SimulationCommon::GetFirstChildElement(documentRoot, TAG::roadNetwork, roadNetworkElement),
                 documentRoot, "Tag " + std::string(TAG::roadNetwork) + " is missing.");
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(roadNetworkElement, TAG::logicFile, logicsElement),
                 roadNetworkElement, "Tag " + std::string(TAG::logicFile) + " is missing.");
    scenario->SetSceneryPath(ParseAttribute<std::string>(logicsElement, ATTRIBUTE::filepath, parameters));

    if(SimulationCommon::GetFirstChildElement(roadNetworkElement, TAG::trafficSignals, trafficSignalsElement));
    {
        SimulationCommon::GetFirstChildElement(trafficSignalsElement, TAG::trafficSignalController, trafficSignalControllerElement);
        while (!trafficSignalControllerElement.isNull())
        {
            openScenario::TrafficSignalController controller;
            ImportTrafficSignalsElement(trafficSignalControllerElement, controller, parameters);
            scenario->AddTrafficSignalController(controller);

            trafficSignalControllerElement = trafficSignalControllerElement.nextSiblingElement(TAG::trafficSignalController);
        }
    }
}

void ScenarioImporter::ImportTrafficSignalsElement(QDomElement &trafficSignalControllerElement, openScenario::TrafficSignalController &controller, Parameters &parameters)
{
    controller.name = ParseAttribute<std::string>(trafficSignalControllerElement, ATTRIBUTE::name, parameters);
    controller.delay = ParseOptionalAttribute<double>(trafficSignalControllerElement, ATTRIBUTE::delay, parameters).value_or(0.0);

    QDomElement phaseElement;

    SimulationCommon::GetFirstChildElement(trafficSignalControllerElement, TAG::phase, phaseElement);
    while (!phaseElement.isNull())
    {
        TrafficSignalControllerPhase phase;
        phase.name = ParseAttribute<std::string>(phaseElement, ATTRIBUTE::name, parameters);
        phase.duration = ParseAttribute<double>(phaseElement, ATTRIBUTE::duration, parameters);

        QDomElement trafficSignalStateElement;
        SimulationCommon::GetFirstChildElement(phaseElement, TAG::trafficSignalState, trafficSignalStateElement);

        while (!trafficSignalStateElement.isNull())
        {
            auto trafficSignalId = ParseAttribute<std::string>(trafficSignalStateElement, ATTRIBUTE::trafficSignalId, parameters);
            auto state = ParseAttribute<std::string>(trafficSignalStateElement, ATTRIBUTE::state, parameters);

            phase.states[trafficSignalId] = state;

            trafficSignalStateElement = trafficSignalStateElement.nextSiblingElement(TAG::trafficSignalState);
        }

        controller.phases.push_back(phase);

        phaseElement = phaseElement.nextSiblingElement(TAG::phase);
    }
}

void ScenarioImporter::ImportStoryboard(QDomElement& documentRoot, std::vector<ScenarioEntity>& entities,
                                        ScenarioInterface* scenario, openScenario::Parameters& parameters)
{
    QDomElement storyboardElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(documentRoot, TAG::storyboard, storyboardElement),
                 documentRoot, "Tag " + std::string(TAG::storyboard) + " is missing.");

    //Import Init
    QDomElement initElement;
    // for initial entity parameters we just use first child "Init" --> others will be ignore
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

void ScenarioImporter::ImportStoryElement(QDomElement &storyElement,
                                          const std::vector<ScenarioEntity> &entities,
                                          ScenarioInterface* scenario,
                                          openScenario::Parameters& parameters)
{
    QDomElement actElement;
    if(!SimulationCommon::GetFirstChildElement(storyElement, TAG::act, actElement))
    {
        return;
    }

    // Story name is mandatory unless story is empty tag
    std::string storyName{};
    ThrowIfFalse(SimulationCommon::ParseAttribute(storyElement, ATTRIBUTE::name, storyName),
                     storyElement, "Attribute " + std::string(ATTRIBUTE::name) + " is missing.");

    while (!actElement.isNull())
    {
         // Act name is mandatory
        std::string actName;
        ThrowIfFalse(SimulationCommon::ParseAttribute(actElement, ATTRIBUTE::name, actName),
                     actElement, "Attribute " + std::string(ATTRIBUTE::name) + " is missing.");

        QDomElement maneuverGroupElement;
        ThrowIfFalse(SimulationCommon::GetFirstChildElement(actElement, TAG::maneuverGroup, maneuverGroupElement),
                     actElement, std::string(TAG::act) + " requries " + TAG::maneuverGroup);

        while (!maneuverGroupElement.isNull())
        {
                std::string maneuverGroupName;
                ThrowIfFalse(SimulationCommon::ParseAttribute(maneuverGroupElement, ATTRIBUTE::name, maneuverGroupName),
                             maneuverGroupElement, "Attribute " + std::string(ATTRIBUTE::name) + " is missing.");
                int numberOfExecutions;
                numberOfExecutions = ParseAttribute<int>(maneuverGroupElement, ATTRIBUTE::maximumExecutionCount, parameters);
        
                    QDomElement actorsElement;
                    openScenario::ActorInformation actorInformation;
                    if (SimulationCommon::GetFirstChildElement(maneuverGroupElement, TAG::actors, actorsElement)) // just one actors per sequence
                    {
                        actorInformation = ImportActors(actorsElement, entities, parameters);
                    }

                    QDomElement maneuverElement;
                    if (SimulationCommon::GetFirstChildElement(maneuverGroupElement, TAG::maneuver, maneuverElement)) // just one maneuver per sequence
                    {
                        ImportManeuverElement(maneuverElement, entities, scenario, actorInformation,
                                              {storyName, actName, maneuverGroupName},
                                              numberOfExecutions, parameters);
                    }

                maneuverGroupElement = maneuverGroupElement.nextSiblingElement(TAG::maneuverGroup);
        }

        actElement = actElement.nextSiblingElement(TAG::act);
    }
}

openScenario::ActorInformation ScenarioImporter::ImportActors(QDomElement &actorsElement,
                                                              const std::vector<ScenarioEntity>& entities,
                                                              openScenario::Parameters& parameters)
{
    openScenario::ActorInformation actorInformation;

    QDomElement entityElement;
    SimulationCommon::GetFirstChildElement(actorsElement, TAG::entityRef, entityElement);

    while (!entityElement.isNull())
    {
        std::string entityRef = ParseAttribute<std::string>(entityElement, ATTRIBUTE::entityRef, parameters);
        ThrowIfFalse(ContainsEntity(entities, entityRef),
                     entityElement, "Element references entity '" + entityRef + "' which isn't declared in 'Entities'");

        if (actorInformation.actors.has_value())
        {
            actorInformation.actors.value().push_back(entityRef);
        }
        else
        {
            actorInformation.actors.emplace({entityRef});
        }

        entityElement = entityElement.nextSiblingElement(TAG::entityRef);
    }

    ThrowIfFalse(SimulationCommon::ParseAttributeBool(actorsElement, ATTRIBUTE::selectTriggeringEntities, actorInformation.actorIsTriggeringEntity),
                 actorsElement, "Attribute " + std::string(ATTRIBUTE::selectTriggeringEntities) + " is missing.");
    return actorInformation;
}


Action ScenarioImporter::ImportAction(QDomElement eventElement, openScenario::Parameters& parameters, const std::string catalogPath)
{
    Action action;

    QDomElement actionElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(eventElement, TAG::action, actionElement),
                  eventElement, "Tag " + std::string(TAG::action) + " is missing.");

    QDomElement actionTypeElement;
    if (SimulationCommon::GetFirstChildElement(actionElement, TAG::userDefinedAction, actionTypeElement))
    {
        action = ScenarioImporterHelper::ImportUserDefinedAction(actionTypeElement);
    }

    else if (SimulationCommon::GetFirstChildElement(actionElement, TAG::privateAction, actionTypeElement))
    {
        action = ScenarioImporterHelper::ImportPrivateAction(actionTypeElement,
                                                             parameters,
                                                             catalogPath);
    }

    else if (SimulationCommon::GetFirstChildElement(actionElement, TAG::globalAction, actionTypeElement))
    {
        action = ScenarioImporterHelper::ImportGlobalAction(actionTypeElement,
                                                            parameters);
    }
    else
    {
        LogErrorAndThrow("Invalid Action Type in OpenSCENARIO file");
    }

    return action;
}

void ScenarioImporter::ImportManeuverElement(QDomElement &maneuverElement,
                                             const std::vector<ScenarioEntity> &entities,
                                             ScenarioInterface *scenario,
                                             const openScenario::ActorInformation &actorInformation,
                                             const std::vector<std::string> &nameStack,
                                             const int numberOfExecutions,
                                             openScenario::Parameters& parameters)
{
    std::string maneuverName;
    ThrowIfFalse(SimulationCommon::ParseAttribute(maneuverElement, ATTRIBUTE::name, maneuverName),
                 maneuverElement, "Attribute " + std::string(ATTRIBUTE::name) + " is missing.");

    // handle event element
    QDomElement eventElement;
    SimulationCommon::GetFirstChildElement(maneuverElement, TAG::event, eventElement);

    while (!eventElement.isNull())
    {
        std::string eventName = ParseAttribute<std::string>(eventElement, ATTRIBUTE::name, parameters);

        std::vector<std::string> eventNameStack{nameStack};
        eventNameStack.emplace_back(maneuverName);
        eventNameStack.emplace_back(eventName);

        const auto stackedEventName = openpass::utils::vector::to_string(eventNameStack, "/");

        auto metaInfo = EventDetectorImporter::ImportEventDetector(eventElement, stackedEventName, numberOfExecutions, actorInformation, entities, parameters);
        scenario->AddConditionalEventDetector(metaInfo);

        const auto action = ImportAction(eventElement,
                                         parameters,
                                         scenario->GetTrajectoryCatalogPath());

        // Adjust action to hold eventname + action
        scenario->AddAction(action, stackedEventName);

        eventElement = eventElement.nextSiblingElement(TAG::event);
    }
}

void ScenarioImporter::ImportInitElement(QDomElement& initElement, std::vector<ScenarioEntity>& entities, openScenario::Parameters& parameters)
{
    // for initial entity parameters we just use first child "Actions" --> others will be ignore
    QDomElement actionsElement;
    SimulationCommon::GetFirstChildElement(initElement, TAG::actions, actionsElement);

    QDomElement privateElement;
    SimulationCommon::GetFirstChildElement(actionsElement, TAG::Private, privateElement);

    while (!privateElement.isNull())
    {
        std::string entityRef = ParseAttribute<std::string>(privateElement, ATTRIBUTE::entityRef, parameters);

        ScenarioEntity* scenarioEntity = GetEntityByName(entities, entityRef);

        ThrowIfFalse(scenarioEntity, privateElement,
                     (std::string("Action object '") + entityRef + "' not declared in 'Entities'"));

        QDomElement privateActionElement;
        SimulationCommon::GetFirstChildElement(privateElement, TAG::privateAction, privateActionElement);
        while (!privateActionElement.isNull())
        {
            const auto &action = ScenarioImporterHelper::ImportPrivateAction(privateActionElement, parameters);
            if (std::holds_alternative<LongitudinalAction>(action))
            {
                const auto &longitudinalAction = std::get<LongitudinalAction>(action);
                if (std::holds_alternative<SpeedAction>(longitudinalAction))
                {
                    const auto &speedAction = std::get<SpeedAction>(longitudinalAction);
                    scenarioEntity->spawnInfo.acceleration = speedAction.transitionDynamics.value;

                    if (std::holds_alternative<AbsoluteTargetSpeed>(speedAction.target))
                    {
                        scenarioEntity->spawnInfo.velocity = std::get<AbsoluteTargetSpeed>(speedAction.target).value;
                    }
                    else
                    {
                        LogErrorAndThrow("Simulator only supports AbsoluteTargetSpeed for Init SpeedAction.");
                    }

                    scenarioEntity->spawnInfo.stochasticVelocity = speedAction.stochasticValue;
                    scenarioEntity->spawnInfo.stochasticAcceleration = speedAction.stochasticDynamics;
                }
                else
                {
                    LogErrorAndThrow("Simulator only supports SpeedAction for Init LongitudinalAction.");
                }
            }
            else if (std::holds_alternative<TeleportAction>(action))
            {
                scenarioEntity->spawnInfo.position = std::get<TeleportAction>(action);
            }
            else if (std::holds_alternative<RoutingAction>(action))
            {
                const auto &routingAction = std::get<RoutingAction>(action);
                if (std::holds_alternative<AssignRouteAction>(routingAction))
                {
                    std::vector<RouteElement> route;

                    for (const auto& roadPosition : std::get<AssignRouteAction>(routingAction))
                    {
                        bool inRoadDirection = roadPosition.t <= 0;
                        route.push_back({roadPosition.roadId, inRoadDirection});
                    }

                    scenarioEntity->spawnInfo.route = route;
                }
                else
                {
                    LogErrorAndThrow("Simulator only supports AssignRouteAction for Init RoutingAction.");
                }
            }
            else if (std::holds_alternative<VisibilityAction>(action))
            {
                const auto &visibilityAction = std::get<VisibilityAction>(action);
                scenarioEntity->spawnInfo.spawning = visibilityAction.traffic;
            }
            else
            {
                LOG_INTERN(LogLevel::Error) << "Unsupported PrivateAction in openScenario Init.";
            }

            privateActionElement = privateActionElement.nextSiblingElement(TAG::privateAction);
        }

        privateElement = privateElement.nextSiblingElement(TAG::Private);
    }
}

void ScenarioImporter::ImportEndConditionsFromStoryboard(const QDomElement& storyboardElement, ScenarioInterface* scenario, openScenario::Parameters& parameters)
{
    bool endConditionProvided = false;

    QDomElement stopTriggerElement;
    if (SimulationCommon::GetFirstChildElement(storyboardElement, TAG::stopTrigger, stopTriggerElement))
    {
        QDomElement conditionGroupElement;
        if (SimulationCommon::GetFirstChildElement(stopTriggerElement, TAG::conditionGroup, conditionGroupElement))
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
                             "Stop Trigger specifies unsupported delay value. Condition delay attribute not equal to zero not currently supported.");

                ThrowIfFalse(conditionEdge == "rising", conditionElement,
                             "Stop Trigger specifies unsupported edge value. Condition edge attribute not equal to 'rising' not currently supported.");

                QDomElement byValueElement;
                if (SimulationCommon::GetFirstChildElement(conditionElement, TAG::byValueCondition, byValueElement))
                {
                    double endTime;
                    std::string rule;

                    ParseSimulationTime(byValueElement, endTime, rule, parameters);
                    ThrowIfFalse(rule == RULE::greaterThan, byValueElement,
                                 "End Condition specifies unsupported rule. SimulationTime rule attribute value '" + rule + "' not supported; defaulting to 'greater_than'.");

                    scenario->SetEndTime(endTime);
                    endConditionProvided = true;
                }
            }
        }
    }

    ThrowIfFalse(endConditionProvided, "Scenario provides no StopTrigger for the simulation");
}

void ScenarioImporter::ParseConditionAttributes(const QDomElement& conditionElement, std::string& name, double& delay, std::string& edge, openScenario::Parameters& parameters)
{
    name = ParseAttribute<std::string>(conditionElement, ATTRIBUTE::name, parameters);
    delay = ParseAttribute<double>(conditionElement, ATTRIBUTE::delay, parameters);
    ThrowIfFalse(delay >= 0.0,
                 conditionElement, "Invalid delay value specified for condition");
    edge = ParseAttribute<std::string>(conditionElement, ATTRIBUTE::conditionEdge, parameters);
}

void ScenarioImporter::ParseSimulationTime(const QDomElement& byValueElement, double& value, std::string& rule, openScenario::Parameters& parameters)
{
    QDomElement simulationTimeElement;
    if(SimulationCommon::GetFirstChildElement(byValueElement, "SimulationTimeCondition", simulationTimeElement))
    {
        value = ParseAttribute<double>(simulationTimeElement, ATTRIBUTE::value, parameters);

        rule = ParseAttribute<std::string>(simulationTimeElement, ATTRIBUTE::rule, parameters);
        {
            ThrowIfFalse((rule == RULE::greaterThan
                            || rule == RULE::lessThan
                            || rule == RULE::equalTo),
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
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(entitiesElement, TAG::scenarioObject, entityElement),
                 entitiesElement, "Tag " + std::string(TAG::scenarioObject) + " is missing.");

    while (!entityElement.isNull())
    {
        ScenarioEntity entity;

        ImportEntity(entityElement, entity, parameters);

        entities.push_back(entity);
        entityElement = entityElement.nextSiblingElement(TAG::scenarioObject);
    }

    // Parse selection elements (there can be as few as zero and as many as one wants)
    ImportSelectionElements(entitiesElement, groups, parameters);
}

void ScenarioImporter::ImportSelectionElements(QDomElement &entitiesElement, std::map<std::string, std::list<std::string>> &groups, openScenario::Parameters& parameters)
{
    QDomElement selectionElement;
    if(SimulationCommon::GetFirstChildElement(entitiesElement, TAG::entitySelection, selectionElement))
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

            selectionElement = selectionElement.nextSiblingElement(TAG::entitySelection);
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
    if(SimulationCommon::GetFirstChildElement(membersElement, TAG::entityRef, byEntityElement))
    {
        while(!byEntityElement.isNull())
        {
            std::string memberName = ParseAttribute<std::string>(byEntityElement, ATTRIBUTE::entityRef, parameters);
            members.push_back(memberName);

            byEntityElement = byEntityElement.nextSiblingElement(TAG::entityRef);
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
                                      [entityName](const ScenarioEntity &elem) {
                                          return elem.name == entityName;
                                      });

    return entitiesFound != entities.cend();
}

} //namespace Importer
