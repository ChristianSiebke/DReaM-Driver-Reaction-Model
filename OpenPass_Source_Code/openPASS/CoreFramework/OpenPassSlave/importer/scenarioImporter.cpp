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
#include "CoreFramework/CoreShare/parameters.h"

// local helper macros
#define SCENARIOCHECKFALSE(element) \
    do { \
        if (!(element)) \
        { \
            throw std::runtime_error("Checkfalse in scenario importer failed"); \
        } \
    } \
    while (0);

namespace Importer {

bool ScenarioImporter::Import(const std::string& filename, ScenarioInterface* scenario)
{
    std::locale::global(std::locale("C"));

    QFile xmlFile(filename.c_str()); // automatic object will be closed on destruction
    if (!xmlFile.open(QIODevice::ReadOnly))
    {
        LOG_INTERN(LogLevel::Warning) << "an error occurred during scenario import";
        throw std::runtime_error("Could not open scenario (" + filename + ")");
    }

    QByteArray xmlData(xmlFile.readAll());
    QDomDocument document;
    QString errorMsg;
    int errorLine;
    if (!document.setContent(xmlData, &errorMsg, &errorLine))
    {
        LOG_INTERN(LogLevel::Warning) << "invalid xml format: " << filename;
        LOG_INTERN(LogLevel::Warning) << "in line " << errorLine << ": " << errorMsg.toStdString();
        throw std::runtime_error("Invalid xml format (" + filename + ")");
    }

    QDomElement documentRoot = document.documentElement();
    if (documentRoot.isNull())
    {
        throw std::runtime_error("Scenario xml has no document root");
    }

    if (!ImportAndValidateVersion(documentRoot))
    {
        return false;
    }

    std::string sceneryPath;
    ImportRoadNetwork(documentRoot, sceneryPath);
    scenario->SetSceneryPath(sceneryPath);

    ImportCatalogs(documentRoot, scenario);

    std::vector<ScenarioEntity> entities;
    std::map<std::string, std::list<std::string>> groups;
    if (!ImportEntities(documentRoot, entities, groups))
    {
        LOG_INTERN(LogLevel::Error) << "Unable to import entities from scenario file";
        return false;
    }

    ImportStoryboard(documentRoot, entities, scenario);
    CategorizeEntities(entities, groups, scenario);

    return true;
}

bool ScenarioImporter::ImportAndValidateVersion(QDomElement& documentRoot)
{
    SimulationCommon::Parameters scenarioParameters;

    QDomElement parameterDeclarationElement;

    if (SimulationCommon::GetFirstChildElement(documentRoot, "ParameterDeclaration", parameterDeclarationElement))
    {
        ImportParameterDeclarationElement(parameterDeclarationElement, &scenarioParameters);
    }

    const auto version = scenarioParameters.GetParametersString().find("OP_OSC_SchemaVersion");

    if (version == scenarioParameters.GetParametersString().cend())
    {
        LOG_INTERN(LogLevel::Error) << "Cannot determine scenario version";
        return false;
    }
    else
        if (version->second.compare(supportedScenarioVersion) != 0)
        {
            LOG_INTERN(LogLevel::Error) << "Scenario version not supported (" << version->second << "). Supported version is " <<
                                        supportedScenarioVersion;
            return false;
        }

    return true;
}

void ScenarioImporter::ImportCatalogs(QDomElement& documentRoot, ScenarioInterface* scenario)
{
    std::string catalogPath;

    QDomElement catalogsElement;
    SCENARIOCHECKFALSE(SimulationCommon::GetFirstChildElement(documentRoot, "Catalogs", catalogsElement));

    if (ImportCatalog(catalogPath, "VehicleCatalog", catalogsElement))
    {
        scenario->SetVehicleCatalogPath(catalogPath);
    }

    if (ImportCatalog(catalogPath, "PedestrianCatalog", catalogsElement))
    {
        scenario->SetPedestrianCatalogPath(catalogPath);
    }
}

void ScenarioImporter::ImportRoadNetwork(QDomElement& documentRoot, std::string& sceneryPath)
{
    QDomElement roadNetworkElement;
    QDomElement logicsElement;

    SCENARIOCHECKFALSE(SimulationCommon::GetFirstChildElement(documentRoot, "RoadNetwork", roadNetworkElement))
    SCENARIOCHECKFALSE(SimulationCommon::GetFirstChildElement(roadNetworkElement, "Logics", logicsElement))
    SCENARIOCHECKFALSE(SimulationCommon::ParseAttributeString(logicsElement, "filepath", sceneryPath));
}

void ScenarioImporter::ImportStoryboard(QDomElement& documentRoot, std::vector<ScenarioEntity>& entities,
                                        ScenarioInterface* scenario)
{
    QDomElement storyboardElement;
    SCENARIOCHECKFALSE(SimulationCommon::GetFirstChildElement(documentRoot, "Storyboard", storyboardElement))

    //Import Init
    QDomElement initElement;
    // for initial entitiy parameters we just use first child "Init" --> others will be ignore
    SCENARIOCHECKFALSE(SimulationCommon::GetFirstChildElement(storyboardElement, "Init", initElement))
    ImportInitElement(initElement, entities);

    // Import Story
    QDomElement storyElement;
    if (SimulationCommon::GetFirstChildElement(storyboardElement, "Story", storyElement))
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
    if (SimulationCommon::GetFirstChildElement(storyElement, "Act", actElement))
    {
        while (!actElement.isNull())
        {
            QDomElement seqElement;
            if (SimulationCommon::GetFirstChildElement(actElement, "Sequence", seqElement))
            {
                while (!seqElement.isNull())
                {
                    std::string sequenceName;
                    SCENARIOCHECKFALSE(SimulationCommon::ParseAttributeString(seqElement, "name", sequenceName));

                    int numberOfExecutions;
                    SCENARIOCHECKFALSE(SimulationCommon::ParseAttributeInt(seqElement, "numberOfExecutions", numberOfExecutions));

                    QDomElement actorsElement;
                    openScenario::ActorInformation actorInformation;
                    if (SimulationCommon::GetFirstChildElement(seqElement, "Actors", actorsElement)) // just one actors per sequence
                    {
                        actorInformation = ImportActors(actorsElement, entities);
                    }

                    QDomElement maneuverElement;
                    if (SimulationCommon::GetFirstChildElement(seqElement, "Maneuver", maneuverElement)) // just one maneuver per sequence
                    {
                        ImportManeuverElement(maneuverElement, entities, scenario, sequenceName, actorInformation, numberOfExecutions);
                    }

                    seqElement = seqElement.nextSiblingElement("Sequence");
                }
            }
            actElement = actElement.nextSiblingElement("Act");
        }
    }
}

openScenario::ActorInformation ScenarioImporter::ImportActors(QDomElement& actorsElement,
                                                              const std::vector<ScenarioEntity>& entities)
{
    openScenario::ActorInformation actorInformation;

    QDomElement entityElement;
    SimulationCommon::GetFirstChildElement(actorsElement, "Entity", entityElement);

    while (!entityElement.isNull())
    {
        std::string entityName;
        SCENARIOCHECKFALSE(SimulationCommon::ParseAttributeString(entityElement, "name", entityName))
        if (!ContainsEntity(entities, entityName))
        {
            throw std::runtime_error(std::string("Actor element references entity '") + entityName +
                                     "' which isn't declared in 'Entities'");
        }

        if (actorInformation.actors.has_value())
        {
            actorInformation.actors.value().push_back(entityName);
        }
        else
        {
            actorInformation.actors.emplace({entityName});
        }

        entityElement = entityElement.nextSiblingElement("Entity");
    }

    QDomElement byConditionElement;
    SimulationCommon::GetFirstChildElement(actorsElement, "ByCondition", byConditionElement);

    if(!byConditionElement.isNull())
    {
        std::string actor;
        SCENARIOCHECKFALSE(SimulationCommon::ParseAttributeString(byConditionElement, "actor", actor))
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
    SimulationCommon::GetFirstChildElement(maneuverElement, "ParameterDeclaration", parameterDeclarationElement);

    // handle event element
    QDomElement eventElement;
    SimulationCommon::GetFirstChildElement(maneuverElement, "Event", eventElement);
    while (!eventElement.isNull())
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

        eventElement = eventElement.nextSiblingElement("Event");
    }
}

void ScenarioImporter::ImportLongitudinalElement(ScenarioEntity& scenarioEntity, QDomElement firstChildOfActionElement)
{
    QDomElement speedElement;
    if (SimulationCommon::GetFirstChildElement(firstChildOfActionElement, "Speed", speedElement))
    {
        QDomElement dynamicsElement;
        if (SimulationCommon::GetFirstChildElement(speedElement, "Dynamics", dynamicsElement))
        {
            double rate;
            if (SimulationCommon::ParseAttributeDouble(dynamicsElement, "rate", rate))
            {
                scenarioEntity.spawnInfo.acceleration.value = rate;
            }
        }

        // Handle <Target> attributes
        QDomElement targetElement;
        if (SimulationCommon::GetFirstChildElement(speedElement, "Target", targetElement))
        {
            // Handle <Target> internal tags - currently ignoring <Relative> tags
            QDomElement absoluteElement;
            if (SimulationCommon::GetFirstChildElement(targetElement, "Absolute", absoluteElement))
            {
                double velocity;
                if (SimulationCommon::ParseAttributeDouble(absoluteElement, "value", velocity))
                {
                    scenarioEntity.spawnInfo.velocity.value = velocity;
                }
            }
        }

        // Parse stochastics if available
        QDomElement stochasticElement;
        SimulationCommon::GetFirstChildElement(speedElement, "Stochastics", stochasticElement);
        while (!stochasticElement.isNull())
        {
           SetStochasticsData(scenarioEntity, stochasticElement);
           stochasticElement = stochasticElement.nextSiblingElement("Stochastics");

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
    SCENARIOCHECKFALSE(SimulationCommon::GetFirstChildElement(firstChildOfActionElement, "Lane", laneElement))
    SCENARIOCHECKFALSE(SimulationCommon::ParseAttributeDouble(laneElement, "s", s));
    SCENARIOCHECKFALSE(SimulationCommon::ParseAttributeInt(laneElement, "laneId", laneId));
    SCENARIOCHECKFALSE(SimulationCommon::ParseAttributeString(laneElement, "roadId", roadId));

    SpawnInfo& spawnInfo = scenarioEntity.spawnInfo;
    spawnInfo.s.value = s;
    spawnInfo.TStart = 0; // always presimulation spawning
    spawnInfo.ILane = laneId;
    spawnInfo.roadId = roadId;

    bool offsetAvailable = SimulationCommon::ParseAttributeDouble(laneElement, "offset", offset);
    if (offsetAvailable)
    {
        spawnInfo.offset.value = offset;
    }

    QDomElement stochasticElement;
    SimulationCommon::GetFirstChildElement(laneElement, "Stochastics", stochasticElement);
    while (!stochasticElement.isNull())
    {
        SetStochasticsData(scenarioEntity, stochasticElement);
        stochasticElement = stochasticElement.nextSiblingElement("Stochastics");
    }

    QDomElement orientationElement;
    if (SimulationCommon::GetFirstChildElement(laneElement, "Orientation", orientationElement))
    {
        SetOrientationData(scenarioEntity, orientationElement);
    }
}

void ScenarioImporter::SetStochasticsDataHelper(SpawnAttribute& attribute, QDomElement& stochasticsElement)
{
    double stdDeviation;
    double lowerBound;
    double upperBound;

    SCENARIOCHECKFALSE(SimulationCommon::ParseAttributeDouble(stochasticsElement, "stdDeviation", stdDeviation));
    SCENARIOCHECKFALSE(SimulationCommon::ParseAttributeDouble(stochasticsElement, "lowerBound", lowerBound));
    SCENARIOCHECKFALSE(SimulationCommon::ParseAttributeDouble(stochasticsElement, "upperBound", upperBound));

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
    SCENARIOCHECKFALSE(SimulationCommon::ParseAttributeString(stochasticsElement, "value", type));

    if (type == "offset")
    {
        SetStochasticsDataHelper(spawnInfo.offset, stochasticsElement);
    }
    else
        if (type == "s")
        {
            SetStochasticsDataHelper(spawnInfo.s, stochasticsElement);
        }
        else
            if (type == "velocity")
            {
                SetStochasticsDataHelper(spawnInfo.velocity, stochasticsElement);
            }
            else
                if (type == "rate")
                {
                    SetStochasticsDataHelper(spawnInfo.acceleration, stochasticsElement);
                }
}

void ScenarioImporter::SetOrientationData(ScenarioEntity& scenarioEntity, QDomElement& orientationElement)
{
    std::string type;
    double heading;

    SCENARIOCHECKFALSE(SimulationCommon::ParseAttributeString(orientationElement, "type", type));
    if (type != "relative")
    {
        LOG_INTERN(LogLevel::Warning) << "Scenario Importer: only relative orientation is allowed.";
        throw std::runtime_error("Scenario Importer: only relative orientation is allowed.");
    }

    SCENARIOCHECKFALSE(SimulationCommon::ParseAttributeDouble(orientationElement, "h", heading));

    scenarioEntity.spawnInfo.heading = heading;
}

void ScenarioImporter::ImportPrivateElement(QDomElement& privateElement,
        std::vector<ScenarioEntity>& entities)
{
    std::string object;
    SCENARIOCHECKFALSE(SimulationCommon::ParseAttributeString(privateElement, "object", object));

    ScenarioEntity* scenarioEntity = GetEntityByName(entities, object);

    if (scenarioEntity == nullptr)
    {
        throw std::runtime_error(std::string("Action object '") + object + "' not declared in 'Entities'");
    }

    QDomElement actionElement;
    SimulationCommon::GetFirstChildElement(privateElement, "Action", actionElement);

    while (!actionElement.isNull())
    {
        QDomElement firstChildOfActionElement;
        if (SimulationCommon::GetFirstChildElement(actionElement, "Longitudinal", firstChildOfActionElement))
        {
            ImportLongitudinalElement(*scenarioEntity, firstChildOfActionElement);
        }
        else
            if (SimulationCommon::GetFirstChildElement(actionElement, "Position", firstChildOfActionElement))
            {
                ImportPositionElement(*scenarioEntity, firstChildOfActionElement);
            }

        actionElement = actionElement.nextSiblingElement("Action");
    }

    try
    {
        ValidityCheckForSpawnParameters(*scenarioEntity);
    }
    catch (const std::runtime_error& error)
    {
        LOG_INTERN(LogLevel::Warning) << "error in importInitElement: " << error.what();
        throw std::runtime_error("Import scenario xml failed.");
    }
    catch (...)
    {
        LOG_INTERN(LogLevel::Warning) << "An unexpected error occured.";
        throw std::runtime_error("Import scenario xml failed.");
    }
}

void ScenarioImporter::ImportPrivateElements(QDomElement& actionsElement, std::vector<ScenarioEntity>& entities)
{
    QDomElement privateElement;
    SimulationCommon::GetFirstChildElement(actionsElement, "Private", privateElement);

    while (!privateElement.isNull())
    {
        ImportPrivateElement(privateElement, entities);
        privateElement = privateElement.nextSiblingElement("Private");
    }
}

void ScenarioImporter::ImportInitElement(QDomElement& initElement, std::vector<ScenarioEntity>& entities)
{
    // for initial entitiy parameters we just use first child "Actions" --> others will be ignore
    QDomElement actionsElement;
    SimulationCommon::GetFirstChildElement(initElement, "Actions", actionsElement);

    ImportPrivateElements(actionsElement, entities);
}

void ScenarioImporter::ImportEndConditionsFromStoryboard(const QDomElement& storyboardElement, ScenarioInterface* scenario)
{
    bool endConditionProvided = false;

    QDomElement endConditionsElement;
    if (SimulationCommon::GetFirstChildElement(storyboardElement, "EndConditions", endConditionsElement))
    {
        QDomElement conditionGroupElement;
        if (SimulationCommon::GetFirstChildElement(endConditionsElement, "ConditionGroup", conditionGroupElement))
        {
            QDomElement conditionElement;
            if (SimulationCommon::GetFirstChildElement(conditionGroupElement, "Condition", conditionElement))
            {
                // these attributes are required by OpenSCENARIO standard, but have no impact on behaviour
                std::string conditionName;
                double conditionDelay;
                std::string conditionEdge;

                ParseConditionAttributes(conditionElement, conditionName, conditionDelay, conditionEdge);
                if (conditionDelay > 0.0)
                {
                    LOG_INTERN(LogLevel::Warning) << "Condition delay attribute not equal to zero not currently supported";
                    throw std::runtime_error("End Condition specifies unsupported delay value");
                }

                if (conditionEdge != "rising")
                {
                    LOG_INTERN(LogLevel::Warning) << "Condition edge attribute not equal to 'rising' not currently supported";
                    throw std::runtime_error("End Condition specifies unsupported edge value");
                }

                QDomElement byValueElement;
                if (SimulationCommon::GetFirstChildElement(conditionElement, "ByValue", byValueElement))
                {
                    double endTime;
                    std::string rule;

                    ParseSimulationTime(byValueElement, endTime, rule);
                    if (rule == "less_than" || rule == "equal_to")
                    {
                        LOG_INTERN(LogLevel::Warning) << "SimulationTime rule attribute value '" << rule << "' not supported; defaulting to 'greater_than'";
                        throw std::runtime_error("End Condition specifies unsupported rule");
                    }
                    scenario->SetEndTime(endTime);
                    endConditionProvided = true;
                }
            }
        }
    }

    if (!endConditionProvided)
    {
        // error message
        throw std::runtime_error("Scenario provides no EndConditions for the simulation");
    }
}

void ScenarioImporter::ParseConditionAttributes(const QDomElement& conditionElement, std::string& name, double& delay, std::string& edge)
{
    if (!SimulationCommon::ParseAttributeString(conditionElement, "name", name))
    {
        LOG_INTERN(LogLevel::Error) << "Condition name required";
        throw std::runtime_error("No name specified for condition");
    }

    if (!SimulationCommon::ParseAttributeDouble(conditionElement, "delay", delay))
    {
        LOG_INTERN(LogLevel::Error) << "Condition delay required";
        throw std::runtime_error("No delay attribute specified for condition " + name);
    }
    else if (delay < 0.0)
    {
        LOG_INTERN(LogLevel::Error) << "Negative condition delay not valid";
        throw std::runtime_error("Invalid delay value specified for condition " + name);
    }

    if (!SimulationCommon::ParseAttributeString(conditionElement, "edge", edge))
    {
        LOG_INTERN(LogLevel::Error) << "Condition edge required";
        throw std::runtime_error("No edge specified for condition " + name);
    }
}

void ScenarioImporter::ParseSimulationTime(const QDomElement& byValueElement, double& value, std::string& rule)
{
    QDomElement simulationTimeElement;
    if(SimulationCommon::GetFirstChildElement(byValueElement, "SimulationTime", simulationTimeElement))
    {
        if(SimulationCommon::ParseAttributeDouble(simulationTimeElement, "value", value))
        {
            if(SimulationCommon::ParseAttributeString(simulationTimeElement, "rule", rule))
            {
                if(!(rule == "greater_than" || rule == "less_than" || rule == "equal_to"))
                {
                    LOG_INTERN(LogLevel::Error) << "SimulationTime rule attribute value '" << rule << "' not valid";
                    throw std::runtime_error("Invalid rule specified for SimulationTime element");
                }
            }
        }
        else
        {
            LOG_INTERN(LogLevel::Error) << "SimulationTime value attribute not valid";
            throw std::runtime_error("Invalid SimulationTime element");
        }
    }
}

void ScenarioImporter::ValidityCheckForSpawnParameters(const ScenarioEntity& scenarioEntity)
{
    if (scenarioEntity.spawnInfo.s.value == -999 || scenarioEntity.spawnInfo.TStart == -999)
    {
        throw std::runtime_error("no position information available");
    }
}

void ScenarioImporter::ImportParameterElement(QDomElement& parameterElement, ParameterInterface *parameters)
{
    std::string parameterName;
    SCENARIOCHECKFALSE(SimulationCommon::ParseAttributeString(parameterElement, "name", parameterName));

    std::string parameterType;
    SCENARIOCHECKFALSE(SimulationCommon::ParseAttributeString(parameterElement, "type", parameterType));

    switch (parameterTypes.at(parameterType))
    {
        case 0:
            bool parameterValueBool;
            SCENARIOCHECKFALSE(SimulationCommon::ParseAttributeBool(parameterElement, "value", parameterValueBool));
            parameters->AddParameterBool(parameterName, parameterValueBool);
            break;

        case 1:
            int parameterValueInt;
            SCENARIOCHECKFALSE(SimulationCommon::ParseAttributeInt(parameterElement, "value", parameterValueInt));
            parameters->AddParameterInt(parameterName, parameterValueInt);
            break;

        case 2:
            double parameterValueDouble;
            SCENARIOCHECKFALSE(SimulationCommon::ParseAttributeDouble(parameterElement, "value", parameterValueDouble));
            parameters->AddParameterDouble(parameterName, parameterValueDouble);
            break;

        case 3:
            std::string parameterValueString;
            SCENARIOCHECKFALSE(SimulationCommon::ParseAttributeString(parameterElement, "value", parameterValueString));
            parameters->AddParameterString(parameterName, parameterValueString);
            break;
    }
}

void ScenarioImporter::ImportParameterDeclarationElement(QDomElement& parameterDeclarationElement,
        ParameterInterface* parameters)
{
    QDomElement parameterElement;
    if (SimulationCommon::GetFirstChildElement(parameterDeclarationElement, "Parameter", parameterElement))
    {
        while (!parameterElement.isNull())
        {
            ImportParameterElement(parameterElement, parameters);

            parameterElement = parameterElement.nextSiblingElement("Parameter");
        }
    }
}

bool ScenarioImporter::ImportCatalogDirectory(std::string& catalogPath, QDomElement& catalogElement)
{
    QDomElement directoryElement;
    SimulationCommon::GetFirstChildElement(catalogElement, "Directory", directoryElement);

    if (directoryElement.isNull())
    {
        LOG_INTERN(LogLevel::Warning) << "Catalog element directory tag missing or invalid";
        return false;
    }

    return SimulationCommon::ParseAttributeString(directoryElement, "path", catalogPath);
}

bool ScenarioImporter::ImportCatalog(std::string& catalogPath, const std::string& catalogName,
                                     QDomElement& catalogsElement)
{
    QDomElement catalogElement;
    SimulationCommon::GetFirstChildElement(catalogsElement, catalogName, catalogElement);

    if (catalogElement.isNull())
    {
        LOG_INTERN(LogLevel::Warning) << "No " << catalogName << " found in scenario file";
        return false;
    }

    return ImportCatalogDirectory(catalogPath, catalogElement);
}

bool ScenarioImporter::ImportEntities(QDomElement& documentRoot, std::vector<ScenarioEntity>& entities, std::map<std::string, std::list<std::string>> &groups)
{
    QDomElement entitiesElement;
    SCENARIOCHECKFALSE(SimulationCommon::GetFirstChildElement(documentRoot, "Entities", entitiesElement));

    if (entitiesElement.isNull())
    {
        LOG_INTERN(LogLevel::Error) << "'Entities' tag not found in scenario file";
        return false;
    }

    QDomElement entityElement;
    SCENARIOCHECKFALSE(SimulationCommon::GetFirstChildElement(entitiesElement, "Object", entityElement));

    while (!entityElement.isNull())
    {
        ScenarioEntity entity;

        if (!ImportEntity(entityElement, entity))
        {
            LOG_INTERN(LogLevel::Error) << "Unable to import entity object";
            return false;
        }

        entities.push_back(entity);
        entityElement = entityElement.nextSiblingElement("Object");
    }

    // Parse selection elements (there can be as few as zero and as many as one wants)
    if (!ImportSelectionElements(entitiesElement, groups))
    {
        LOG_INTERN(LogLevel::Error) << "Unable to import selection element";
        return false;
    }

    return true;
}

bool ScenarioImporter::ImportSelectionElements(QDomElement &entitiesElement, std::map<std::string, std::list<std::string>> &groups)
{
    QDomElement selectionElement;
    if(SimulationCommon::GetFirstChildElement(entitiesElement, "Selection", selectionElement))
    {
        std::string selectionName;
        QDomElement membersElement;
        std::list<std::string> members;
        while (!selectionElement.isNull())
        {
            if (!SimulationCommon::ParseAttributeString(selectionElement, "name", selectionName))
            {
                LOG_INTERN(LogLevel::Error) << "No name provided for selection";
                return false;
            }
            // parse members element (just one)
            if(!SimulationCommon::GetFirstChildElement(selectionElement, "Members", membersElement))
            {
                LOG_INTERN(LogLevel::Error) << "No members provided for selection: " << selectionName;
                return false;
            }

            ImportMembers(membersElement, members);

            groups.insert({selectionName, members});

            selectionElement = selectionElement.nextSiblingElement("Selection");
        }
    }

    if (groups.find("ScenarioAgents") == groups.cend())
    {
        LOG_INTERN(LogLevel::Info) << "ScenarioAgents selection is not defined. Adding empty one.";
        groups.insert({"ScenarioAgents", {}});
    }
    return true;
}

bool ScenarioImporter::ImportEntity(QDomElement& entityElement, ScenarioEntity& entity)
{
    if (!SimulationCommon::ParseAttributeString(entityElement, "name", entity.name))
    {
        LOG_INTERN(LogLevel::Error) << "Unable to parse 'name' tag of entity object";
        return false;
    }

    if (entity.name.size() == 0)
    {
        LOG_INTERN(LogLevel::Error) << "Length of entity object name has to be greater than 0";
        return false;
    }

    QDomElement catalogReferenceElement;

    if (!SimulationCommon::GetFirstChildElement(entityElement, "CatalogReference", catalogReferenceElement))
    {
        LOG_INTERN(LogLevel::Error) << "Unable to import entity catalog reference tag";
        return false;
    }

    if (!ImportEntityCatalogReference(catalogReferenceElement, entity))
    {
        LOG_INTERN(LogLevel::Error) << "Invalid catalog reference for entity '" << entity.name << "'";
        return false;
    }

    return true;
}

bool ScenarioImporter::ImportMembers(const QDomElement &membersElement, std::list<std::string> &members)
{
    QDomElement byEntityElement;
    if(SimulationCommon::GetFirstChildElement(membersElement, "ByEntity", byEntityElement))
    {
        std::string memberName;
        while(!byEntityElement.isNull())
        {
            SimulationCommon::ParseAttributeString(byEntityElement, "name", memberName);
            members.push_back(memberName);

            byEntityElement = byEntityElement.nextSiblingElement("ByEntity");
        }
    }

    return true;
}

bool ScenarioImporter::ImportEntityCatalogReference(QDomElement& catalogReferenceElement, ScenarioEntity& entity)
{
    if (!SimulationCommon::ParseAttributeString(catalogReferenceElement, "catalogName",
            entity.catalogReference.catalogName))
    {
        LOG_INTERN(LogLevel::Error) << "Unable to parse 'catalogName' attribute of catalog reference";
        return false;
    }

    if (entity.catalogReference.catalogName.size() == 0)
    {
        LOG_INTERN(LogLevel::Error) << "Length of 'catalogName' has to be greater than 0";
        return false;
    }

    if (!SimulationCommon::ParseAttributeString(catalogReferenceElement, "entryName", entity.catalogReference.entryName))
    {
        LOG_INTERN(LogLevel::Error) << "Unable to parse 'entryName' attribute of catalog reference";
        return false;
    }

    if (entity.catalogReference.entryName.size() == 0)
    {
        LOG_INTERN(LogLevel::Error) << "Length of 'entryName' has to be greater than 0";
        return false;
    }

    return true;
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
