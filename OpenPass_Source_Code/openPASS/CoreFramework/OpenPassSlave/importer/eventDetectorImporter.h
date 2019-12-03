﻿/*******************************************************************************
* Copyright (c) 2017, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include <QDomDocument>
#include "Interfaces/scenarioInterface.h"
#include "Common/eventDetectorDefinitions.h"
#include "importerLoggingHelper.h"

namespace Importer
{

const std::map<std::string, openScenario::Rule> ruleConversionMap = {{"greater_than", openScenario::Rule::GreaterThan},
                                                                     {"less_than", openScenario::Rule::LessThan},
                                                                     {"equal_to", openScenario::Rule::EqualTo}};

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
     * \param[in] eventElement the element from which to parse
     *                         ConditionalEventDetector instantiation
     *                         information
     * \param[in] sequenceName the name of the sequence to which the
     *                         ConditionalEventDetector will belong
     * \param[in] numberOfExecutions the maximum number of times the
     *                               ConditionalEventDetector is to emit an
     *                               event
     * \param[in] actorInformation information detailing what entities are the
     *                             actors to be targeted by the
     *                             ConditionalEventDetector's emitted events
     * \param[in] entities the entities explicitly defined in the openScenario
     *                     file's Entities element
     *
     * \return a struct containing all information relevant for the
     *         instantiation of a ConditionalEventDetector
     * ------------------------------------------------------------------------
     */
    static openScenario::ConditionalEventDetectorInformation ImportEventDetector(QDomElement& eventElement,
                                                                                 const std::string &sequenceName,
                                                                                 const int numberOfExecutions,
                                                                                 const openScenario::ActorInformation& actorInformation,
                                                                                 const std::vector<ScenarioEntity>& entities);
private:
    /*!
     * \brief Imports a condition element of a OpenSCENARIO storyboard DOM
     *
     * \param[in]   conditionElement   The DOM root of the condition element
     * \param[in]   entities           Objects from 'Entities' tag
     * \param[out]  scenario           The relevant event detector data is imported into this scenario
     * \param[in]   seqName            Name of the containing sequence
     */
    static openScenario::Condition ImportConditionElement(QDomElement& conditionElement,
                                                                          const std::vector<ScenarioEntity>& entities);

    /*!
     * \brief Imports a ByEntity condition element of a OpenSCENARIO storyboard DOM
     *
     * \param[in]   byEntityElement   The DOM root of the by-entity element
     * \param[in]   entities          Objects from 'Entities' tag
     * \param[out]  eventDetectorParameters   Triggering entity names are stored here
     */
    static openScenario::Condition ImportByEntityElement(QDomElement byEntityElement,
                                                         const std::vector<ScenarioEntity>& entities);

    /*!
     * ------------------------------------------------------------------------
     * \brief ImportConditionByValueElement Imports a Condition ByValue element
     *        into a condition parameter interface.
     *
     * \param[in] byValueElement the ByValue element to parse for condition
     *            details.
     * \param[out] conditionParameters If successfully parsed, the ByValue
     *             element's condition details are imported into this object.
     * ------------------------------------------------------------------------
     */
    static openScenario::Condition ImportConditionByValueElement(QDomElement& byValueElement);

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
