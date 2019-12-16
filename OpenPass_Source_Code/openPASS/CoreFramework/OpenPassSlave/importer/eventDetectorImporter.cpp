/*******************************************************************************
* Copyright (c) 2017, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "eventDetectorImporter.h"
#include "xmlParser.h"

#define CHECKFALSE(element) \
    do { \
        if (!(element)) \
        { \
            throw std::runtime_error("Checkfalse in eventDetector importer failed"); \
        } \
    } \
    while (0);

namespace Importer
{
    openScenario::ConditionalEventDetectorInformation EventDetectorImporter::ImportEventDetector(QDomElement& eventElement,
                                                                                                 const std::string& sequenceName,
                                                                                                 const int numberOfExecutions,
                                                                                                 const openScenario::ActorInformation& actorInformation,
                                                                                                 const std::vector<ScenarioEntity>& entities)
    {
        QDomElement startConditionsElement;
        CHECKFALSE(SimulationCommon::GetFirstChildElement(eventElement, "StartConditions", startConditionsElement));

        QDomElement conditionGroupElement;
        CHECKFALSE(SimulationCommon::GetFirstChildElement(startConditionsElement, "ConditionGroup", conditionGroupElement));

        QDomElement conditionElement;
        CHECKFALSE(SimulationCommon::GetFirstChildElement(conditionGroupElement, "Condition", conditionElement));

        openScenario::ConditionCollection conditions{};

        while (!conditionElement.isNull())
        {
            conditions.emplace_back(ImportConditionElement(conditionElement, entities));
            conditionElement = conditionElement.nextSiblingElement("Condition");
        }

        return openScenario::ConditionalEventDetectorInformation{actorInformation,
                                                                 numberOfExecutions,
                                                                 sequenceName,
                                                                 conditions};
    }

    openScenario::Condition EventDetectorImporter::ImportConditionElement(QDomElement& conditionElement,
                                                                          const std::vector<ScenarioEntity>& entities)
    {
        QDomElement byEntityElement;
        //Parse specific entity
        if (SimulationCommon::GetFirstChildElement(conditionElement, "ByEntity", byEntityElement))
        {
            return ImportByEntityElement(byEntityElement, entities);
        }

        QDomElement byValueElement;
        if (SimulationCommon::GetFirstChildElement(conditionElement, "ByValue", byValueElement))
        {
            return ImportConditionByValueElement(byValueElement);
        }

        LOG_INTERN(LogLevel::Error) << "No valid Condition found";
        throw std::runtime_error("No valid Condition found");
    }

    openScenario::Condition EventDetectorImporter::ImportByEntityElement(QDomElement byEntityElement,
                                                                         const std::vector<ScenarioEntity>& entities)
    {
        std::vector<std::string> triggeringEntities;

        QDomElement triggeringEntitiesElement;
        CHECKFALSE(SimulationCommon::GetFirstChildElement(byEntityElement, "TriggeringEntities", triggeringEntitiesElement));

        QDomElement entityElement;
        SimulationCommon::GetFirstChildElement(triggeringEntitiesElement, "Entity", entityElement);

        while (!entityElement.isNull())
        {
            std::string entityName;
            CHECKFALSE(SimulationCommon::ParseAttributeString(entityElement, "name", entityName));

            if (!ContainsEntity(entities, entityName))
            {
                throw std::runtime_error(std::string("TriggeringEntity '") + entityName + "' not declared in 'Entities'");
            }

            triggeringEntities.push_back(entityName);
            entityElement = entityElement.nextSiblingElement("Entity");
        }

        QDomElement entityConditionElement;
        CHECKFALSE(SimulationCommon::GetFirstChildElement(byEntityElement, "EntityCondition", entityConditionElement));

        QDomElement reachPositionElement;
        if (SimulationCommon::GetFirstChildElement(entityConditionElement, "ReachPosition", reachPositionElement))
        {
            double tolerance;
            CHECKFALSE(SimulationCommon::ParseAttributeDouble(reachPositionElement, "tolerance", tolerance));

            QDomElement positionElement;
            CHECKFALSE(SimulationCommon::GetFirstChildElement(reachPositionElement, "Position", positionElement));

            QDomElement roadElement;
            if(SimulationCommon::GetFirstChildElement(positionElement, "Road", roadElement))
            {
                double sCoordinate;
                CHECKFALSE(SimulationCommon::ParseAttributeDouble(roadElement, "s", sCoordinate));

                std::string roadId;
                CHECKFALSE(SimulationCommon::ParseAttributeString(roadElement, "roadId", roadId));

                // this return must occur across two lines to appropriately construct the std::variant
                auto condition = openScenario::ReachPositionRoadCondition(triggeringEntities,
                                                                          tolerance,
                                                                          sCoordinate,
                                                                          roadId);

                return condition;
            }

            QDomElement relativeLaneElement;
            if(SimulationCommon::GetFirstChildElement(positionElement, "RelativeLane", relativeLaneElement))
            {
                std::string referenceEntityName;
                CHECKFALSE(SimulationCommon::ParseAttributeString(relativeLaneElement, "object", referenceEntityName));

                int deltaLane;
                CHECKFALSE(SimulationCommon::ParseAttributeInt(relativeLaneElement, "dLane", deltaLane));

                double deltaS;
                CHECKFALSE(SimulationCommon::ParseAttributeDouble(relativeLaneElement, "ds", deltaS));

                auto condition = openScenario::RelativeLaneCondition(triggeringEntities,
                                                                     referenceEntityName,
                                                                     deltaLane,
                                                                     deltaS,
                                                                     tolerance);

                return condition;
            }
        }

        QDomElement relativeSpeedElement;
        if (SimulationCommon::GetFirstChildElement(entityConditionElement, "RelativeSpeed", relativeSpeedElement))
        {
            std::string referenceEntityName;
            CHECKFALSE(SimulationCommon::ParseAttributeString(relativeSpeedElement, "entity", referenceEntityName));

            double tolerance;
            CHECKFALSE(SimulationCommon::ParseAttributeDouble(relativeSpeedElement, "value", tolerance));

            std::string ruleString;
            CHECKFALSE(SimulationCommon::ParseAttributeString(relativeSpeedElement, "rule", ruleString));

            auto condition = openScenario::RelativeSpeedCondition(triggeringEntities,
                                                                  referenceEntityName,
                                                                  tolerance,
                                                                  ruleConversionMap.at(ruleString));
            return condition;
        }

        QDomElement timeToCollisionElement;
        if (SimulationCommon::GetFirstChildElement(entityConditionElement, "TimeToCollision", timeToCollisionElement))
        {
            double targetTTC;
            CHECKFALSE(SimulationCommon::ParseAttributeDouble(timeToCollisionElement, "value", targetTTC));

            std::string ruleString;
            CHECKFALSE(SimulationCommon::ParseAttributeString(timeToCollisionElement, "rule", ruleString));

            QDomElement targetElement;
            CHECKFALSE(SimulationCommon::GetFirstChildElement(timeToCollisionElement, "Target", targetElement));

            QDomElement entityElement;
            CHECKFALSE(SimulationCommon::GetFirstChildElement(targetElement, "Entity", entityElement));

            std::string targetEntityName;
            CHECKFALSE(SimulationCommon::ParseAttributeString(entityElement, "name", targetEntityName));

            auto condition = openScenario::TimeToCollisionCondition(triggeringEntities,
                                                                    targetEntityName,
                                                                    targetTTC,
                                                                    ruleConversionMap.at(ruleString));

            return condition;
        }

        // if no element was parsed successfully
        LOG_INTERN(LogLevel::Error) << "No valid ByEntity Condition found";
        throw std::runtime_error("No valid ByEntity Condition found");
    }

    openScenario::Condition EventDetectorImporter::ImportConditionByValueElement(QDomElement& byValueElement)
    {
        QDomElement simulationTimeElement;
        if (SimulationCommon::GetFirstChildElement(byValueElement, "SimulationTime", simulationTimeElement))
        {
            double value;
            CHECKFALSE(SimulationCommon::ParseAttributeDouble(simulationTimeElement, "value", value));

            std::string ruleString;
            CHECKFALSE(SimulationCommon::ParseAttributeString(simulationTimeElement, "rule", ruleString));

            openScenario::Rule rule = ruleConversionMap.at(ruleString);

            // this return must occur across two lines to appropriately construct the std::variant
            openScenario::SimulationTimeCondition condition = openScenario::SimulationTimeCondition(rule,
                                                                                                    value);
            return condition;
        }
        else
        {
            LOG_INTERN(LogLevel::Error) << "No valid ByValue Condition found";
            throw std::runtime_error("No valid ByValue Condition found");
        }
    }

    bool EventDetectorImporter::ContainsEntity(const std::vector<ScenarioEntity>& entities,
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
} // Importer
