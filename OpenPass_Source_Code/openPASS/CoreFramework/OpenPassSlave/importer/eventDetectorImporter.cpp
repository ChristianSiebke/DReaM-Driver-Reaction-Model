/*******************************************************************************
* Copyright (c) 2017, 2019, 2020 in-tech GmbH
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
                                                                                                 const std::vector<ScenarioEntity>& entities,
                                                                                                 openScenario::Parameters& parameters)
    {
        QDomElement startConditionsElement;
        ThrowIfFalse(SimulationCommon::GetFirstChildElement(eventElement, TAG::startConditions, startConditionsElement),
                     eventElement, "Tag " + std::string(TAG::startConditions) + " missing.");

        QDomElement conditionGroupElement;
        ThrowIfFalse(SimulationCommon::GetFirstChildElement(startConditionsElement, TAG::conditionGroup, conditionGroupElement),
                     startConditionsElement, "Tag " + std::string(TAG::conditionGroup) + " missing.");

        QDomElement conditionElement;
        ThrowIfFalse(SimulationCommon::GetFirstChildElement(conditionGroupElement, TAG::condition, conditionElement),
                     conditionGroupElement, "Tag " + std::string(TAG::condition) + " missing.");

        openScenario::ConditionCollection conditions{};

        while (!conditionElement.isNull())
        {
            conditions.emplace_back(ImportConditionElement(conditionElement, entities, parameters));
            conditionElement = conditionElement.nextSiblingElement(TAG::condition);
        }

        return openScenario::ConditionalEventDetectorInformation{actorInformation,
                                                                 numberOfExecutions,
                                                                 eventName,
                                                                 conditions};
    }

    openScenario::Condition EventDetectorImporter::ImportConditionElement(QDomElement& conditionElement,
                                                                          const std::vector<ScenarioEntity>& entities,
                                                                          openScenario::Parameters& parameters)
    {
        QDomElement byEntityElement;

        //Parse specific entity
        if (SimulationCommon::GetFirstChildElement(conditionElement, TAG::byEntity, byEntityElement))
        {
            return ImportByEntityElement(byEntityElement, entities, parameters);
        }

        QDomElement byValueElement;
        if (SimulationCommon::GetFirstChildElement(conditionElement, TAG::byValue, byValueElement))
        {
            return ImportConditionByValueElement(byValueElement, parameters);
        }

        LogErrorAndThrow("No valid Condition found.");
    }

    openScenario::Condition EventDetectorImporter::ImportByEntityElement(QDomElement byEntityElement,
                                                                         const std::vector<ScenarioEntity>& entities,
                                                                         openScenario::Parameters& parameters)
    {
        std::vector<std::string> triggeringEntities;

        QDomElement triggeringEntitiesElement;
        ThrowIfFalse(SimulationCommon::GetFirstChildElement(byEntityElement, TAG::triggeringEntities, triggeringEntitiesElement),
                     byEntityElement, "Tag " + std::string(TAG::triggeringEntities) + " is missing.");

        QDomElement entityElement;
        SimulationCommon::GetFirstChildElement(triggeringEntitiesElement, "Entity", entityElement);

        while (!entityElement.isNull())
        {
            std::string entityName = ParseAttribute<std::string>(entityElement, ATTRIBUTE::name, parameters);

            ThrowIfFalse(ContainsEntity(entities, entityName),
                         entityElement, "TriggeringEntity '" + entityName + "' not declared in 'Entities'");

            triggeringEntities.push_back(entityName);
            entityElement = entityElement.nextSiblingElement(TAG::entity);
        }

        QDomElement entityConditionElement;
        ThrowIfFalse(SimulationCommon::GetFirstChildElement(byEntityElement, TAG::entityCondition, entityConditionElement),
                     byEntityElement, "Tag " + std::string(TAG::entityCondition) + " is missing.");

        QDomElement reachPositionElement;
        if (SimulationCommon::GetFirstChildElement(entityConditionElement, "ReachPosition", reachPositionElement))
        {
            double tolerance = ParseAttribute<double>(reachPositionElement, ATTRIBUTE::tolerance, parameters);

            QDomElement positionElement;
            ThrowIfFalse(SimulationCommon::GetFirstChildElement(reachPositionElement, TAG::position, positionElement),
                         reachPositionElement, "Tag " + std::string(TAG::position) + " is missing.");

            QDomElement roadElement;
            if(SimulationCommon::GetFirstChildElement(positionElement, TAG::road, roadElement))
            {
                double sCoordinate = ParseAttribute<double>(roadElement, ATTRIBUTE::s, parameters);

                std::string roadId = ParseAttribute<std::string>(roadElement, ATTRIBUTE::roadId, parameters);

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
                std::string referenceEntityName = ParseAttribute<std::string>(relativeLaneElement, ATTRIBUTE::object, parameters);

                int deltaLane = ParseAttribute<int>(relativeLaneElement, ATTRIBUTE::dLane, parameters);

                double deltaS = ParseAttribute<double>(relativeLaneElement, ATTRIBUTE::ds, parameters);

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
            std::string referenceEntityName = ParseAttribute<std::string>(relativeSpeedElement, ATTRIBUTE::entity, parameters);

            double value = ParseAttribute<double>(relativeSpeedElement, ATTRIBUTE::value, parameters);

            std::string ruleString = ParseAttribute<std::string>(relativeSpeedElement, ATTRIBUTE::rule, parameters);

            auto condition = openScenario::RelativeSpeedCondition(triggeringEntities,
                                                                  referenceEntityName,
                                                                  value,
                                                                  ruleConversionMap.at(ruleString));
            return condition;
        }

        QDomElement timeToCollisionElement;
        if (SimulationCommon::GetFirstChildElement(entityConditionElement, TAG::timeToCollision, timeToCollisionElement))
        {
            double targetTTC = ParseAttribute<double>(timeToCollisionElement, ATTRIBUTE::value, parameters);

            std::string ruleString = ParseAttribute<std::string>(timeToCollisionElement, ATTRIBUTE::rule, parameters);

            QDomElement targetElement;
            ThrowIfFalse(SimulationCommon::GetFirstChildElement(timeToCollisionElement, TAG::target, targetElement),
                         timeToCollisionElement, "Tag " + std::string(TAG::target) + " is missing.");

            QDomElement entityElement;
            ThrowIfFalse(SimulationCommon::GetFirstChildElement(targetElement, TAG::entity, entityElement),
                         targetElement, "Tag " + std::string(TAG::entity) + " is missing.");

            std::string targetEntityName = ParseAttribute<std::string>(entityElement, ATTRIBUTE::name, parameters);

            auto condition = openScenario::TimeToCollisionCondition(triggeringEntities,
                                                                    targetEntityName,
                                                                    targetTTC,
                                                                    ruleConversionMap.at(ruleString));

            return condition;
        }

        QDomElement timeHeadwayElement;
        if (SimulationCommon::GetFirstChildElement(entityConditionElement, TAG::timeHeadway, timeHeadwayElement))
        {
            std::string targetEntityName = ParseAttribute<std::string>(timeHeadwayElement, ATTRIBUTE::entity, parameters);

            double targetTHW = ParseAttribute<double>(timeHeadwayElement, ATTRIBUTE::value, parameters);

            bool freeSpace = ParseAttribute<bool>(timeHeadwayElement, ATTRIBUTE::freespace, parameters);

            std::string ruleString = ParseAttribute<std::string>(timeHeadwayElement, ATTRIBUTE::rule, parameters);

            bool alongRoute = ParseAttribute<bool>(timeHeadwayElement, ATTRIBUTE::alongRoute, parameters);
            ThrowIfFalse(alongRoute, timeHeadwayElement, "Attribute alongRoute=\"false\" in TimeHeadway condition is currently not supported.");

            auto condition = openScenario::TimeHeadwayCondition(triggeringEntities,
                                                                targetEntityName,
                                                                targetTHW,
                                                                freeSpace,
                                                                ruleConversionMap.at(ruleString));

            return condition;
        }

        // if no element was parsed successfully
        LogErrorAndThrow("No valid ByEntity Condition found.");
    }

    openScenario::Condition EventDetectorImporter::ImportConditionByValueElement(QDomElement& byValueElement, openScenario::Parameters& parameters)
    {
        QDomElement simulationTimeElement;
        ThrowIfFalse(SimulationCommon::GetFirstChildElement(byValueElement, TAG::simulationTime, simulationTimeElement),
                     byValueElement, "Tag " + std::string(TAG::simulationTime) + " is missing.");

        double value = ParseAttribute<double>(simulationTimeElement, ATTRIBUTE::value, parameters);

        std::string ruleString = ParseAttribute<std::string>(simulationTimeElement, ATTRIBUTE::rule, parameters);

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
