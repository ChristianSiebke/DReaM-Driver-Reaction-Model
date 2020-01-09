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

namespace TAG = openpass::importer::xml::eventDetectorImporter::tag;
namespace ATTRIBUTE = openpass::importer::xml::eventDetectorImporter::attribute;

namespace Importer
{
    openScenario::ConditionalEventDetectorInformation EventDetectorImporter::ImportEventDetector(QDomElement& eventElement,
                                                                                                 const std::string& eventName,
                                                                                                 const int numberOfExecutions,
                                                                                                 const openScenario::ActorInformation& actorInformation,
                                                                                                 const std::vector<ScenarioEntity>& entities)
    {
        QDomElement startConditionsElement;
        ThrowIfFalse(SimulationCommon::GetFirstChildElement(eventElement, TAG::startConditions, startConditionsElement),
                      "Could not import EventDetector. Tag " + std::string(TAG::startConditions) + " missing.");

        QDomElement conditionGroupElement;
        ThrowIfFalse(SimulationCommon::GetFirstChildElement(startConditionsElement, TAG::conditionGroup, conditionGroupElement),
                      "Could not import EventDetector. Tag " + std::string(TAG::conditionGroup) + " missing.");

        QDomElement conditionElement;
        ThrowIfFalse(SimulationCommon::GetFirstChildElement(conditionGroupElement, TAG::condition, conditionElement),
                      "Could not import EventDetector. Tag " + std::string(TAG::condition) + " missing.");

        openScenario::ConditionCollection conditions{};

        while (!conditionElement.isNull())
        {
            conditions.emplace_back(ImportConditionElement(conditionElement, entities));
            conditionElement = conditionElement.nextSiblingElement(TAG::condition);
        }

        return openScenario::ConditionalEventDetectorInformation{actorInformation,
                                                                 numberOfExecutions,
                                                                 eventName,
                                                                 conditions};
    }

    openScenario::Condition EventDetectorImporter::ImportConditionElement(QDomElement& conditionElement,
                                                                          const std::vector<ScenarioEntity>& entities)
    {
        QDomElement byEntityElement;

        //Parse specific entity
        if (SimulationCommon::GetFirstChildElement(conditionElement, TAG::byEntity, byEntityElement))
        {
            return ImportByEntityElement(byEntityElement, entities);
        }

        QDomElement byValueElement;
        if (SimulationCommon::GetFirstChildElement(conditionElement, TAG::byValue, byValueElement))
        {
            return ImportConditionByValueElement(byValueElement);
        }

        LogErrorAndThrow("No valid Condition found.");
    }

    openScenario::Condition EventDetectorImporter::ImportByEntityElement(QDomElement byEntityElement,
                                                                         const std::vector<ScenarioEntity>& entities)
    {
        std::vector<std::string> triggeringEntities;

        QDomElement triggeringEntitiesElement;
        ThrowIfFalse(SimulationCommon::GetFirstChildElement(byEntityElement, TAG::triggeringEntities, triggeringEntitiesElement),
                      "Could not import by entity element. " + std::string(TAG::triggeringEntities) + " missing.");

        QDomElement entityElement;
        SimulationCommon::GetFirstChildElement(triggeringEntitiesElement, "Entity", entityElement);

        while (!entityElement.isNull())
        {
            std::string entityName;
            ThrowIfFalse(SimulationCommon::ParseAttributeString(entityElement, ATTRIBUTE::name, entityName),
                          "Could not import by entity element. Entity requires a " + std::string(ATTRIBUTE::name) + " attribute.");

            ThrowIfFalse(ContainsEntity(entities, entityName), "TriggeringEntity '" + entityName + "' not declared in 'Entities'");

            triggeringEntities.push_back(entityName);
            entityElement = entityElement.nextSiblingElement(TAG::entity);
        }

        QDomElement entityConditionElement;
        ThrowIfFalse(SimulationCommon::GetFirstChildElement(byEntityElement, TAG::entityCondition, entityConditionElement),
                      "Could not import by entity element. " + std::string(TAG::entityCondition) + " missing.");

        QDomElement reachPositionElement;
        if (SimulationCommon::GetFirstChildElement(entityConditionElement, "ReachPosition", reachPositionElement))
        {
            double tolerance;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(reachPositionElement, ATTRIBUTE::tolerance, tolerance),
                          "Could not import by entity element. Reach position requires a " + std::string(ATTRIBUTE::tolerance) + " attribute.");

            QDomElement positionElement;
            ThrowIfFalse(SimulationCommon::GetFirstChildElement(reachPositionElement, TAG::position, positionElement),
                          "Could not import by entity element. " + std::string(TAG::position) + " missing.");

            QDomElement roadElement;
            if(SimulationCommon::GetFirstChildElement(positionElement, TAG::road, roadElement))
            {
                double sCoordinate;
                ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadElement, ATTRIBUTE::s, sCoordinate),
                              "Could not import by entity element. Road requires a " + std::string(ATTRIBUTE::s) + " attribute.");

                std::string roadId;
                ThrowIfFalse(SimulationCommon::ParseAttributeString(roadElement, ATTRIBUTE::roadId, roadId),
                              "Could not import by entity element. Road requires a " + std::string(ATTRIBUTE::roadId) + " attribute.");

                // this return must occur across two lines to appropriately construct the std::variant
                auto condition = openScenario::ReachPositionRoadCondition(triggeringEntities,
                                                                          tolerance,
                                                                          sCoordinate,
                                                                          roadId);

                return condition;
            }

            QDomElement relativeLaneElement;
            if(SimulationCommon::GetFirstChildElement(positionElement, TAG::relativeLane, relativeLaneElement))
            {
                std::string referenceEntityName;
                ThrowIfFalse(SimulationCommon::ParseAttributeString(relativeLaneElement, ATTRIBUTE::object, referenceEntityName),
                              "Could not import by entity element. RelativeLane requires a " + std::string(ATTRIBUTE::object) + " attribute.");

                int deltaLane;
                ThrowIfFalse(SimulationCommon::ParseAttributeInt(relativeLaneElement, ATTRIBUTE::dLane, deltaLane),
                              "Could not import by entity element. RelativeLane requires a " + std::string(ATTRIBUTE::dLane) + " attribute.");

                double deltaS;
                ThrowIfFalse(SimulationCommon::ParseAttributeDouble(relativeLaneElement, ATTRIBUTE::ds, deltaS),
                              "Could not import by entity element. RelativeLane requires a " + std::string(ATTRIBUTE::ds) + " attribute.");

                auto condition = openScenario::RelativeLaneCondition(triggeringEntities,
                                                                     referenceEntityName,
                                                                     deltaLane,
                                                                     deltaS,
                                                                     tolerance);

                return condition;
            }
        }

        QDomElement relativeSpeedElement;
        if (SimulationCommon::GetFirstChildElement(entityConditionElement, TAG::relativeSpeed, relativeSpeedElement))
        {
            std::string referenceEntityName;
            ThrowIfFalse(SimulationCommon::ParseAttributeString(relativeSpeedElement, ATTRIBUTE::entity, referenceEntityName),
                          "Could not import by entity element. RelativeSpeed requires a " + std::string(ATTRIBUTE::entity) + " attribute.");

            double tolerance;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(relativeSpeedElement, ATTRIBUTE::value, tolerance),
                          "Could not import by entity element. RelativeSpeed requires a " + std::string(ATTRIBUTE::value) + " attribute.");

            std::string ruleString;
            ThrowIfFalse(SimulationCommon::ParseAttributeString(relativeSpeedElement, ATTRIBUTE::rule, ruleString),
                          "Could not import by entity element. RelativeSpeed requires a " + std::string(ATTRIBUTE::rule) + " attribute.");

            auto condition = openScenario::RelativeSpeedCondition(triggeringEntities,
                                                                  referenceEntityName,
                                                                  tolerance,
                                                                  ruleConversionMap.at(ruleString));
            return condition;
        }

        QDomElement timeToCollisionElement;
        if (SimulationCommon::GetFirstChildElement(entityConditionElement, TAG::timeToCollision, timeToCollisionElement))
        {
            double targetTTC;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(timeToCollisionElement, ATTRIBUTE::value, targetTTC),
                          "Could not import by entity element. TimeToCollision requires a " + std::string(ATTRIBUTE::value) + " attribute.");

            std::string ruleString;
            ThrowIfFalse(SimulationCommon::ParseAttributeString(timeToCollisionElement, ATTRIBUTE::rule, ruleString),
                          "Could not import by entity element. TimeToCollision requires a " + std::string(ATTRIBUTE::rule) + " attribute.");

            QDomElement targetElement;
            ThrowIfFalse(SimulationCommon::GetFirstChildElement(timeToCollisionElement, TAG::target, targetElement),
                          "Could not import by entity element. " + std::string(TAG::target) + " missing.");

            QDomElement entityElement;
            ThrowIfFalse(SimulationCommon::GetFirstChildElement(targetElement, TAG::entity, entityElement),
                          "Could not import by entity element. " + std::string(TAG::entity) + " missing.");

            std::string targetEntityName;
            ThrowIfFalse(SimulationCommon::ParseAttributeString(entityElement, ATTRIBUTE::name, targetEntityName),
                          "Could not import by entity element. TimeToCollision requires a " + std::string(ATTRIBUTE::name) + " attribute.");

            auto condition = openScenario::TimeToCollisionCondition(triggeringEntities,
                                                                    targetEntityName,
                                                                    targetTTC,
                                                                    ruleConversionMap.at(ruleString));

            return condition;
        }

        // if no element was parsed successfully
        LogErrorAndThrow("No valid ByEntity Condition found.");
    }

    openScenario::Condition EventDetectorImporter::ImportConditionByValueElement(QDomElement& byValueElement)
    {
        QDomElement simulationTimeElement;
        ThrowIfFalse(SimulationCommon::GetFirstChildElement(byValueElement, TAG::simulationTime, simulationTimeElement), "No valid ByValue Condition found.");

        double value;
        ThrowIfFalse(SimulationCommon::ParseAttributeDouble(simulationTimeElement, ATTRIBUTE::value, value),
                      "Could not import by value element. SimulationTime requires a " + std::string(ATTRIBUTE::value) + " attribute.");

        std::string ruleString;
        ThrowIfFalse(SimulationCommon::ParseAttributeString(simulationTimeElement, ATTRIBUTE::rule, ruleString),
                      "Could not import by value element. SimulationTime requires a " + std::string(ATTRIBUTE::rule) + " attribute.");

        openScenario::Rule rule = ruleConversionMap.at(ruleString);

        // this return must occur across two lines to appropriately construct the std::variant
        openScenario::SimulationTimeCondition condition = openScenario::SimulationTimeCondition(rule,
                                                                                                value);
        return condition;
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
