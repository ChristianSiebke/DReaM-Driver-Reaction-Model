/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
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

namespace TAG = openpass::importer::xml::scenarioImporter::tag;
namespace ATTRIBUTE = openpass::importer::xml::scenarioImporter::attribute;

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

        ImportCatalogs(documentRoot, scenario);

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

void ScenarioImporter::ImportCatalogs(QDomElement& documentRoot, ScenarioInterface* scenario)
{
    QDomElement catalogsElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(documentRoot, TAG::catalogs, catalogsElement),
                  "Could not import Catalogs. Tag " + std::string(TAG::catalogs) + " is missing.");

    try
    {
        const auto vehicleCatalogPath = ImportCatalog("VehicleCatalog", catalogsElement);
        scenario->SetVehicleCatalogPath(vehicleCatalogPath);
    }
    catch (const std::runtime_error &error)
    {
        LOG_INTERN(LogLevel::Error) << "Could not find VehicleCatalog in openScenario file." << error.what();
    }

    try
    {
        const auto pedestrianCatalogPath = ImportCatalog("PedestrianCatalog", catalogsElement);
        scenario->SetPedestrianCatalogPath(pedestrianCatalogPath);
    }
    catch (const std::runtime_error &error)
    {
        LOG_INTERN(LogLevel::Error) << "Could not find PedestrianCatalog in openScenario file." << error.what();
    }
}

void ScenarioImporter::ImportRoadNetwork(QDomElement& documentRoot, std::string& sceneryPath)
{
    QDomElement roadNetworkElement;
    QDomElement logicsElement;

    ThrowIfFalse(SimulationCommon::GetFirstChildElement(documentRoot, TAG::roadNetwork, roadNetworkElement),
                  "Could not import RoadNetwork. Tag " + std::string(TAG::roadNetwork) + " is missing.");
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(roadNetworkElement, TAG::logics, logicsElement),
                  "Could not import RoadNetwork. Tag " + std::string(TAG::logics) + " is missing.");
    ThrowIfFalse(SimulationCommon::ParseAttributeString(logicsElement, ATTRIBUTE::filePath, sceneryPath),
                  "Could not import RoadNetwork. Logics tag requires a " + std::string(ATTRIBUTE::filePath) + " attribute");
}

void ScenarioImporter::ImportStoryboard(QDomElement& documentRoot, std::vector<ScenarioEntity>& entities,
                                        ScenarioInterface* scenario)
{
    QDomElement storyboardElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(documentRoot, TAG::storyboard, storyboardElement),
                  "Could not import Storyboard. Tag " + std::string(TAG::storyboard) + " is missing.");

    //Import Init
    QDomElement initElement;
    // for initial entitiy parameters we just use first child "Init" --> others will be ignore
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(storyboardElement, TAG::init, initElement),
                  "Could not import Storyboard. Tag " + std::string(TAG::init) + " is missing.");
    ImportInitElement(initElement, entities);

    // Import Story
    QDomElement storyElement;
    if (SimulationCommon::GetFirstChildElement(storyboardElement, TAG::story, storyElement))
    {
        ImportStoryElement(storyElement, entities, scenario);
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
                    std::string sequenceName;
                    ThrowIfFalse(SimulationCommon::ParseAttributeString(seqElement, ATTRIBUTE::name, sequenceName),
                                  "Could not import Story. Sequence tag requires a " + std::string(ATTRIBUTE::name) + " attribute.");

                    int numberOfExecutions;
                    ThrowIfFalse(SimulationCommon::ParseAttributeInt(seqElement, ATTRIBUTE::numberOfExecutions, numberOfExecutions),
                                  "Could not import Story. Sequence tag requires a " + std::string(ATTRIBUTE::numberOfExecutions) + " attribute.");

                    QDomElement actorsElement;
                    openScenario::ActorInformation actorInformation;
                    if (SimulationCommon::GetFirstChildElement(seqElement, TAG::actors, actorsElement)) // just one actors per sequence
                    {
                        actorInformation = ImportActors(actorsElement, entities);
                    }

                    QDomElement maneuverElement;
                    if (SimulationCommon::GetFirstChildElement(seqElement, TAG::maneuver, maneuverElement)) // just one maneuver per sequence
                    {
                        ImportManeuverElement(maneuverElement, entities, scenario, sequenceName, actorInformation, numberOfExecutions);
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
                      "Could not import Scenario. Entity tag requires a " + std::string(ATTRIBUTE::name) + " attribute.");
        ThrowIfFalse(ContainsEntity(entities, entityName),
                     "Actor element references entity '" + entityName + "' which isn't declared in 'Entities'");

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
                      "Could not import Scenario. ByCondition tag requires a " + std::string(ATTRIBUTE::actor) + " attribute.");
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
                                             const std::string& sequenceName,
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
        try
        {
            openScenario::ConditionalEventDetectorInformation conditionalEventDetectorInformation = EventDetectorImporter::ImportEventDetector(eventElement,
                                                                                                                                               sequenceName,
                                                                                                                                               numberOfExecutions,
                                                                                                                                               actorInformation,
                                                                                                                                               entities);

            scenario->AddConditionalEventDetector(conditionalEventDetectorInformation);

            std::shared_ptr<ScenarioActionInterface> action = ManipulatorImporter::ImportManipulator(eventElement,
                                                                                                     sequenceName);
            scenario->AddAction(action);

            eventElement = eventElement.nextSiblingElement(TAG::event);
        }
        catch (const std::runtime_error &error)
        {
            LogErrorAndThrow("Could not import EventDetector and Manipulator pair." + std::string(error.what()));
        }
    }
}

void ScenarioImporter::ImportLongitudinalElement(ScenarioEntity& scenarioEntity, QDomElement firstChildOfActionElement)
{
    QDomElement speedElement;
    if (SimulationCommon::GetFirstChildElement(firstChildOfActionElement, TAG::speed, speedElement))
    {
        QDomElement dynamicsElement;
        if (SimulationCommon::GetFirstChildElement(speedElement, TAG::dynamics, dynamicsElement))
        {
            double rate;
            if (SimulationCommon::ParseAttributeDouble(dynamicsElement, ATTRIBUTE::rate, rate))
            {
                scenarioEntity.spawnInfo.acceleration.value = rate;
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
                    scenarioEntity.spawnInfo.velocity.value = velocity;
                }
            }
        }

        // Parse stochastics if available
        QDomElement stochasticElement;
        SimulationCommon::GetFirstChildElement(speedElement, TAG::stochastics, stochasticElement);
        while (!stochasticElement.isNull())
        {
           SetStochasticsData(scenarioEntity, stochasticElement);
           stochasticElement = stochasticElement.nextSiblingElement(TAG::stochastics);
        }
    }
}

void ScenarioImporter::ImportPositionElement(ScenarioEntity& scenarioEntity, QDomElement firstChildOfActionElement)
{
    std::string roadId = "";
    int laneId;
    double s;
    double offset;

    QDomElement laneElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(firstChildOfActionElement, TAG::lane, laneElement),
                  "Could not import Position. Tag " + std::string(TAG::lane) + " is missing.");
    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(laneElement, ATTRIBUTE::s, s),
                  "Could not import Position. Lane tag requires a " + std::string(ATTRIBUTE::s) + " attribute");
    ThrowIfFalse(SimulationCommon::ParseAttributeInt(laneElement, ATTRIBUTE::laneId, laneId),
                  "Could not import Position. Lane tag requires a " + std::string(ATTRIBUTE::laneId) + " attribute.");
    ThrowIfFalse(SimulationCommon::ParseAttributeString(laneElement, ATTRIBUTE::roadId, roadId),
                  "Could not import Position. Lane tag requires a " + std::string(ATTRIBUTE::roadId) + " attribute.");

    SpawnInfo& spawnInfo = scenarioEntity.spawnInfo;
    spawnInfo.s.value = s;
    spawnInfo.TStart = 0; // always presimulation spawning
    spawnInfo.ILane = laneId;
    spawnInfo.roadId = roadId;

    bool offsetAvailable = SimulationCommon::ParseAttributeDouble(laneElement, ATTRIBUTE::offset, offset);
    if (offsetAvailable)
    {
        spawnInfo.offset.value = offset;
    }

    QDomElement stochasticElement;
    SimulationCommon::GetFirstChildElement(laneElement, TAG::stochastics, stochasticElement);
    while (!stochasticElement.isNull())
    {
        SetStochasticsData(scenarioEntity, stochasticElement);
        stochasticElement = stochasticElement.nextSiblingElement(TAG::stochastics);
    }

    QDomElement orientationElement;
    if (SimulationCommon::GetFirstChildElement(laneElement, TAG::orientation, orientationElement))
    {
        SetOrientationData(scenarioEntity, orientationElement);
    }
}

void ScenarioImporter::SetStochasticsDataHelper(SpawnAttribute& attribute, QDomElement& stochasticsElement)
{
    double stdDeviation;
    double lowerBound;
    double upperBound;

    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(stochasticsElement, ATTRIBUTE::stdDeviation, stdDeviation),
                  "Could not set stochastics data helper. Stochastics tag requires a " + std::string(ATTRIBUTE::stdDeviation) + " attribute.");
    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(stochasticsElement, ATTRIBUTE::lowerBound, lowerBound),
                       "Could not set stochastics data helper. Stochastics tag requires a " + std::string(ATTRIBUTE::lowerBound) + " attribute.");
    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(stochasticsElement, ATTRIBUTE::upperBound, upperBound),
                  "Could not set stochastics data helper. Stochastics tag requires a " + std::string(ATTRIBUTE::upperBound) + " attribute.");

    attribute.mean = attribute.value;
    attribute.isStochastic = true;
    attribute.stdDeviation = stdDeviation;
    attribute.lowerBoundary = lowerBound;
    attribute.upperBoundary = upperBound;
}

void ScenarioImporter::SetStochasticsData(ScenarioEntity& scenarioEntity, QDomElement& stochasticsElement)
{
    SpawnInfo& spawnInfo = scenarioEntity.spawnInfo;

    std::string type;
    ThrowIfFalse(SimulationCommon::ParseAttributeString(stochasticsElement, ATTRIBUTE::value, type),
                  "Could not set stochastics data. Stochastics tag requires a " + std::string(ATTRIBUTE::value) + " attribute.");

    if (type == "offset")
    {
        SetStochasticsDataHelper(spawnInfo.offset, stochasticsElement);
    }
    else if (type == "s")
    {
        SetStochasticsDataHelper(spawnInfo.s, stochasticsElement);
    }
    else if (type == "velocity")
    {
        SetStochasticsDataHelper(spawnInfo.velocity, stochasticsElement);
    }
    else if (type == "rate")
    {
        SetStochasticsDataHelper(spawnInfo.acceleration, stochasticsElement);
    }
}

void ScenarioImporter::SetOrientationData(ScenarioEntity& scenarioEntity, QDomElement& orientationElement)
{
    std::string type;
    double heading;

    ThrowIfFalse(SimulationCommon::ParseAttributeString(orientationElement, ATTRIBUTE::type, type),
                  "Could not set orientation data. Orientation tag requires a " + std::string(ATTRIBUTE::type) + " attribute.");
    ThrowIfFalse(type == "relative", "Scenario Importer: only relative orientation is allowed.");

    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(orientationElement, ATTRIBUTE::heading, heading),
                  "Could not set orientation data. Orientation tag requires a " + std::string(ATTRIBUTE::heading) + " attribute.");

    scenarioEntity.spawnInfo.heading = heading;
}

void ScenarioImporter::ImportPrivateElement(QDomElement& privateElement,
        std::vector<ScenarioEntity>& entities)
{
    std::string object;
    ThrowIfFalse(SimulationCommon::ParseAttributeString(privateElement, ATTRIBUTE::object, object),
                  "Could not import Private. Private tag requires a " + std::string(ATTRIBUTE::object) + " attribute.");

    ScenarioEntity* scenarioEntity = GetEntityByName(entities, object);

    ThrowIfFalse(scenarioEntity,
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

        actionElement = actionElement.nextSiblingElement(TAG::action);
    }

    try
    {
        ValidityCheckForSpawnParameters(*scenarioEntity);
    }
    catch (const std::runtime_error& error)
    {
        LogErrorAndThrow("Import scenario xml failed. Error in importInitElement: " + std::string(error.what()));
    }
    catch (...)
    {
        LogErrorAndThrow("Import scenario xml failed. An unexpected error occured.");
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
                ThrowIfFalse(conditionDelay == 0.0,
                             "End Condition specifies unsupported delay value. Condition delay attribute not equal to zero not currently supported.");

                ThrowIfFalse(conditionEdge == "rising",
                             "End Condition specifies unsupported edge value. Condition edge attribute not equal to 'rising' not currently supported.");

                QDomElement byValueElement;
                if (SimulationCommon::GetFirstChildElement(conditionElement, TAG::byValue, byValueElement))
                {
                    double endTime;
                    std::string rule;

                    ParseSimulationTime(byValueElement, endTime, rule);
                    ThrowIfFalse(rule == "greater_than",
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
                 "No name specified for condition.");
    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(conditionElement, ATTRIBUTE::delay, delay),
                 "No delay attribute specified for condition " + name);
    ThrowIfFalse(delay >= 0.0,
                 "Invalid delay value specified for condition " + name);
    ThrowIfFalse(SimulationCommon::ParseAttributeString(conditionElement, ATTRIBUTE::edge, edge),
                 "No edge specified for condition " + name);
}

void ScenarioImporter::ParseSimulationTime(const QDomElement& byValueElement, double& value, std::string& rule)
{
    QDomElement simulationTimeElement;
    if(SimulationCommon::GetFirstChildElement(byValueElement, "SimulationTime", simulationTimeElement))
    {
        ThrowIfFalse(SimulationCommon::ParseAttributeDouble(simulationTimeElement, ATTRIBUTE::value, value), "SimulationTime value attribute not valid.");

        if(SimulationCommon::ParseAttributeString(simulationTimeElement, ATTRIBUTE::rule, rule))
        {
            ThrowIfFalse((rule == "greater_than"
                            || rule == "less_than"
                            || rule == "equal_to"),
                         "Simulation rule attribute value '" + rule + "' not valid");
        }
    }
}

void ScenarioImporter::ValidityCheckForSpawnParameters(const ScenarioEntity& scenarioEntity)
{
    ThrowIfFalse((scenarioEntity.spawnInfo.s.value != -999
                    && scenarioEntity.spawnInfo.TStart != -999),
                 "no position information available.");
}

void ScenarioImporter::ImportParameterElement(QDomElement& parameterElement, openpass::parameter::Container& parameters)
{
    std::string parameterName;
    ThrowIfFalse(SimulationCommon::ParseAttributeString(parameterElement, ATTRIBUTE::name, parameterName),
                  "Could not import Parameter. Parameter tag requires a " + std::string(ATTRIBUTE::name) + " attribute.");

    std::string parameterType;
    ThrowIfFalse(SimulationCommon::ParseAttributeString(parameterElement, ATTRIBUTE::type, parameterType),
                  "Could not import Parameter. Paramter tag requires a " + std::string(ATTRIBUTE::type) + " attribute.");

    switch (parameterTypes.at(parameterType))
    {
        case 0:
            bool parameterValueBool;
            ThrowIfFalse(SimulationCommon::ParseAttributeBool(parameterElement, ATTRIBUTE::value, parameterValueBool),
                          "Could not import Parameter. Paramter tag requires a " + std::string(ATTRIBUTE::value) + " attribute.");
            parameters.emplace_back(parameterName, parameterValueBool);
            break;

        case 1:
            int parameterValueInt;
            ThrowIfFalse(SimulationCommon::ParseAttributeInt(parameterElement, ATTRIBUTE::value, parameterValueInt),
                          "Could not import Parameter. Paramter tag requires a " + std::string(ATTRIBUTE::value) + " attribute.");
            parameters.emplace_back(parameterName, parameterValueInt);
            break;

        case 2:
            double parameterValueDouble;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(parameterElement, ATTRIBUTE::value, parameterValueDouble),
                          "Could not import Parameter. Paramter tag requires a " + std::string(ATTRIBUTE::value) + " attribute.");
            parameters.emplace_back(parameterName, parameterValueDouble);
            break;

        case 3:
            std::string parameterValueString;
            ThrowIfFalse(SimulationCommon::ParseAttributeString(parameterElement, ATTRIBUTE::value, parameterValueString),
                          "Could not import Parameter. Paramter tag requires a " + std::string(ATTRIBUTE::value) + " attribute.");
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
    SimulationCommon::GetFirstChildElement(catalogsElement, catalogName, catalogElement);
    ThrowIfFalse(!catalogElement.isNull(),
                 "No " + catalogName + " found in scenario file.");

    QDomElement directoryElement;
    SimulationCommon::GetFirstChildElement(catalogElement, TAG::directory, directoryElement);
    ThrowIfFalse(!directoryElement.isNull(),
                 "Catalog element directory tag missing or invalid.");

    std::string catalogPath;
    ThrowIfFalse(SimulationCommon::ParseAttributeString(directoryElement, ATTRIBUTE::path, catalogPath),
                 "Could not import catalog. Attribute path is missing.");

    return catalogPath;
}

void ScenarioImporter::ImportEntities(QDomElement& documentRoot, std::vector<ScenarioEntity>& entities, std::map<std::string, std::list<std::string>> &groups)
{
    QDomElement entitiesElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(documentRoot, TAG::entities, entitiesElement),
                  "Could not import Entities. Tag " + std::string(TAG::entities) + " is missing.");

    ThrowIfFalse(!entitiesElement.isNull(),
                 std::string(TAG::entities) + " tag not found in scenario file.");

    QDomElement entityElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(entitiesElement, TAG::object, entityElement),
                  "Could not import Entities. Tag " + std::string(TAG::object) + " is missing.");

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
            ThrowIfFalse(SimulationCommon::ParseAttributeString(selectionElement, ATTRIBUTE::name, selectionName), "No name provided for selection");
            ThrowIfFalse(SimulationCommon::GetFirstChildElement(selectionElement, TAG::members, membersElement), "No members provided for selection: " + selectionName);

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
                 "Unable to parse 'name' tag of entity object");
    ThrowIfFalse(entity.name.size() > 0,
                 "Length of entity object name has to be greater than 0");

    QDomElement catalogReferenceElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(entityElement, TAG::catalogReference, catalogReferenceElement),
                 "Unable to import entity catalog reference tag");
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
                 "Unable to parse 'catalogName attribute of catalog reference");
    ThrowIfFalse(entity.catalogReference.catalogName.size() > 0,
                 "Length of 'catalogName' has to be greater than 0");
    ThrowIfFalse(SimulationCommon::ParseAttributeString(catalogReferenceElement, ATTRIBUTE::entryName, entity.catalogReference.entryName),
                 "Unable to parse 'entryName' attribute of catalog reference");
    ThrowIfFalse(entity.catalogReference.entryName.size() > 0,
                 "Length of 'entryName' has to be greater than 0");
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
        if (entity.name == "Ego")
        {
            scenario->SetEgoEntity(entity);
        }
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
