/********************************************************************************
 * Copyright (c) 2017-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include <QDomDocument>
#include "include/scenarioInterface.h"
#include "common/eventDetectorDefinitions.h"
#include "importerLoggingHelper.h"
#include "oscImporterCommon.h"

namespace RULE = openpass::importer::xml::openScenario::rule;

namespace Importer
{

const std::map<std::string, openScenario::Rule> ruleConversionMap = {{RULE::greaterThan, openScenario::Rule::GreaterThan},
                                                                     {RULE::lessThan, openScenario::Rule::LessThan},
                                                                     {RULE::equalTo, openScenario::Rule::EqualTo}};

class EventDetectorImporter
{
public:
    EventDetectorImporter() = default;
    EventDetectorImporter(const EventDetectorImporter&) = delete;
    EventDetectorImporter(EventDetectorImporter&&) = delete;
    EventDetectorImporter& operator=(const EventDetectorImporter&) = delete;
    EventDetectorImporter& operator=(EventDetectorImporter&&) = delete;
    virtual ~EventDetectorImporter() = default;

    /*!
     * ------------------------------------------------------------------------
     * \brief ImportEventDetector parses information from an EventElement for
     *        instantiation of a ConditionalEventDetector.
     *
     * \param[in] eventElement       the element from which to parse
     *                               ConditionalEventDetector instantiation
     *                               information
     * \param[in] eventName          Name of the event used for identification
     *                               e.g. "MyStory/MyAct/MySequence/MyManeuver/MyEvent"
     * \param[in] numberOfExecutions the maximum number of times the
     *                               ConditionalEventDetector is to emit an
     *                               event
     * \param[in] actorInformation   information detailing what entities are the
     *                               actors to be targeted by the
     *                               ConditionalEventDetector's emitted events
     * \param[in] entities           entities defined in openScenario Entities
     *
     * \return a struct containing all information relevant for the
     *         instantiation of a ConditionalEventDetector
     * ------------------------------------------------------------------------
     */
    static openScenario::ConditionalEventDetectorInformation ImportEventDetector(QDomElement &eventElement,
                                                                                 const std::string &eventName,
                                                                                 const int numberOfExecutions,
                                                                                 const openScenario::ActorInformation &actorInformation,
                                                                                 const std::vector<ScenarioEntity>& entities,
                                                                                 openScenario::Parameters& parameters);

private:
    /*!
     * \brief Imports a condition element of a OpenSCENARIO storyboard DOM
     *
     * \param[in]   conditionElement   The DOM root of the condition element
     * \param[in]   entities           Objects from 'Entities' tag
     * \param[in]   parameters         Declared parameters
     */
    static openScenario::Condition ImportConditionElement(QDomElement& conditionElement,
                                                          const std::vector<ScenarioEntity>& entities,
                                                          openScenario::Parameters& parameters);

    /*!
     * \brief Imports a ByEntity condition element of a OpenSCENARIO storyboard DOM
     *
     * \param[in]   byEntityElement   The DOM root of the by-entity element
     * \param[in]   entities          Objects from 'Entities' tag
     * \param[in]   parameters         Declared parameters
     */
    static openScenario::Condition ImportByEntityElement(QDomElement byEntityElement,
                                                         const std::vector<ScenarioEntity>& entities,
                                                         openScenario::Parameters& parameters);

    /*!
     * \brief Imports a EntityCondition element of a OpenSCENARIO storyboard DOM
     *
     * \param[in]   byEntityCondition   The DOM root of the byEntityCondition element
     * \param[in]   triggeringEntities  Objects from 'Entities' tag
     *
     * return Condition
     */
    static openScenario::Condition ImportEntityCondition(QDomElement byEntityCondition,
                                                         std::vector<std::string> triggeringEntities);

    /*!
     * ------------------------------------------------------------------------
     * \brief ImportConditionByValueElement Imports a Condition ByValue element
     *        into a condition parameter interface.
     *
     * \param[in] byValueElement the ByValue element to parse for condition
     *            details.
     * \param[in] parameters   Declared parameters
     * \return      openScenario Condition
     * ------------------------------------------------------------------------
     */
    static openScenario::Condition ImportConditionByValueElement(QDomElement& byValueElement, openScenario::Parameters& parameters);

    /*!
     * \brief Tests, if a entity with a given name is included in the provided vector of scenario entities
     *
     * \param[in]   entities        Vector of entities to test against
     * \param[in]   entityName      Name of the entity to check for existence
     *
     * \return true, if entityName is included in entities, false otherwise.
     */
    static bool ContainsEntity(const std::vector<ScenarioEntity>& entities,
                               const std::string& entityName);
};

}; // Importer
