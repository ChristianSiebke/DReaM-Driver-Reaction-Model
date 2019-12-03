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

//-----------------------------------------------------------------------------
//! @file  ScenarioImporter.h
//! @brief This file contains the importer of the scenario.
//-----------------------------------------------------------------------------

#pragma once

#include <string>
#include <QDomDocument>
#include "Interfaces/scenarioInterface.h"
#include "CoreFramework/CoreShare/parameters.h"
#include "Common/openScenarioDefinitions.h"

namespace Importer
{
const std::map<std::string, int> parameterTypes = {{"bool",    0 },
                                                   {"integer", 1 },
                                                   {"double",  2 },
                                                   {"string",  3 }};

class ScenarioImporter
{
public:    
    ScenarioImporter() = default;
    ScenarioImporter(const ScenarioImporter&) = delete;
    ScenarioImporter(ScenarioImporter&&) = delete;
    ScenarioImporter& operator=(const ScenarioImporter&) = delete;
    ScenarioImporter& operator=(ScenarioImporter&&) = delete;
    virtual ~ScenarioImporter() = default;

    static bool Import(const std::string &filename, ScenarioInterface *scenario);

    //Public for testing only
    static void ImportLongitudinalElement(ScenarioEntity& scenarioEntity, QDomElement firstChildOfActionElement);
    static void ImportPositionElement(ScenarioEntity& scenarioEntity, QDomElement firstChildOfActionElement);

    /*!
     * \brief Imports the storyboard from OpenSCENARIO DOM
     *
     * \param[in]   documentRoot    The DOM root of the scenario file
     * \param[in]   entities        Objects from 'Entities' tag
     * \param[out]  scenario        The storyboard is imported into this scenario
     */
    static void ImportStoryboard(QDomElement& documentRoot, std::vector<ScenarioEntity>& entities, ScenarioInterface* scenario);

    //Import EventDetectors and Manipulators
    static void ImportParameterElement(QDomElement& parameterElement, ParameterInterface *parameters);

    /*!
     * \brief Imports a catalog tag
     *
     * \param[in]   catalogName         Name of the catalog to import
     * \param[in]   catatalogsElement   DOM element holding all available catalogs
     *
     * \return      True on successful parsing, false otherwise.
     */
    static std::string ImportCatalog(const std::string& catalogName, QDomElement& catalogsElement);

private:
    /*!
     * \brief Imports and validates the internally used OpenSCENARIO schema version
     *
     * \param[in]   documentRoot    The DOM root of the scenario file
     *
     * \return  true if import succeeds and version matches, falso otherwise
     */
    static void ImportAndValidateVersion(QDomElement& documentRoot);

    /*!
     * \brief Imports catalogs from OpenSCENARIO DOM
     *
     * Currently, vehicle and pedestrian catalogs are imported.
     *
     * \param[in]   documentRoot    The DOM root of the scenario file
     * \param[out]  scenario        Catalogs are imported into this scenario
     */
    static void ImportCatalogs(QDomElement& documentRoot, ScenarioInterface* scenario);

    /*!
     * \brief Imports the roadnetwork information from OpenSCENARIO DOM
     *
     * Currently, the path to the OpenDRIVE scenery file is extracted.
     *
     * \param[in]   roadNetworkElement  DOM element of the roadnetwork tag
     * \param[out]  sceneryPath         File path to the scenery
     */
    static void ImportRoadNetwork(QDomElement& roadNetworkElement, std::string& sceneryPath);

    /*!
     * \brief Imports the init element of a OpenSCENARIO storyboard DOM
     *
     * \param[in]   initElement     The DOM of the storyboard init element
     * \param[in]   entities        Objects from 'Entities' tag
     * \param[out]  scenario        Init element data is imported into this scenario
     */
    static void ImportInitElement(QDomElement& initElement, std::vector<ScenarioEntity>& entities);

    /*!
     * \brief Imports the simulation end conditions
     * \param[in] storyboardElement The storyboard DOM element
     * \param[out] scenario End conditions data is imported into this scenario
     */
    static void ImportEndConditionsFromStoryboard(const QDomElement &storyboardElement, ScenarioInterface *scenario);

    /*!
     * \brief ParseConditionAttributes parses the attributes for a condition
     * 		  element
     *
     * \param[in] conditionElement
     * \param[out] name the value of the name attribute is parsed into this
     * 			   std::string
     * \param[out] delay the value of the delay attribute is parsed into this
     * 			   double
     * \param[out] edge the value of the edge attribute is parsed into this
     * 			   std::string
     *
     * \throw std::runtime_exception If name, delay, or edge is not specified,
     * 								 or if a negative delay is provided
     */
    static void ParseConditionAttributes(const QDomElement& conditionElement, std::string& name, double& delay, std::string& edge);

    /*!
     * \brief ParseSimulationTime parses the simulation time tag according to
     * 		  the OpenSCENARIO standard
     *
     * \param[in] byValueElement the byValueElement containing the
     * 			  SimulationTime element
     * \param[out] value the value of the value attribute of the SimulationTime
     * 			   element
     * \param[out] rule the value of the rule attribute of the SimulationTime
     * 			   element
     *
     * \throw std::runtime_exception If value or rule is not specified, or an
     * 								 invalid rule is provided
     */
    static void ParseSimulationTime(const QDomElement &byValueElement, double& value, std::string& rule);

    /*!
     * \brief Validates spawn parameters
     *
     * \param[in]   scenarioEntity      The scenario entity to validate
     *
     * Throws when validation fails.
     *
     * \throw std::runtime_exception    If s-coordinate or start time are not defined
     */
    static void ValidityCheckForSpawnParameters(const ScenarioEntity& scenarioEntity);

    /*!
     * \brief Imports the entities element of a OpenSCENARIO DOM
     *
     * \param[in]   documentRoot    The DOM root of the scenario file
     * \param[out]  entities        Entity element data is imported into this container
     */
    static void ImportEntities(QDomElement& documentRoot, std::vector<ScenarioEntity>& entities, std::map<std::string, std::list<std::string> > &groups);

    /*!
     * \brief Imports an entity of a OpenSCENARIO Entities DOM
     *
     * \param[in]   entityElement   The DOM root of the entity element
     * \param[out]  entity          Entity element data is imported into this container
     */
    static void ImportEntity(QDomElement& entityElement, ScenarioEntity& entity);

    /*!
     * \brief Imports a group definition of a OpenSCENARIO Selection DOM
     *
     * \param[in]   selectionElement	The DOM node of the selection element
     * \param[out]  groups 				Groups element data is imported into this container
     */
    static void ImportSelectionElements(QDomElement &entitiesElement, std::map<std::string, std::list<std::string>> &groups);

    /*!
     * \brief Imports a list of members of a OpenSCENARIO Members DOM
     *
     * \param[in]   membersElement  The DOM root of the members element
     * \param[out]  members			Members element data is imported into this container
     */
    static void ImportMembers(const QDomElement &membersElement, std::list<std::string> &members);

    /*!
     * \brief Imports a catalog reference of an entity of a OpenSCENARIO Entities DOM
     *
     * \param[in]   catalogReferenceElement   The DOM root of the catalog reference element
     * \param[out]  entity                    Catalog refrence data is imported into this container
     */
    static void ImportEntityCatalogReference(QDomElement& catalogReferenceElement, ScenarioEntity& entity);

    /*!
     * \brief Imports a story element of a OpenSCENARIO storyboard DOM
     *
     * \param[in]   storyElement   The DOM root of the catalog reference element
     * \param[in]   entities       Objects from 'Entities' tag
     * \param[out]  scenario       The story data is imported into this scenario
     */
    static void ImportStoryElement(QDomElement& storyElement, const std::vector<ScenarioEntity>& entities, ScenarioInterface *scenario);

    /*!
     * \brief Imports actors from of a OpenSCENARIO story DOM
     *
     * \param[in]   actorsElement   The DOM root of the actors element
     * \param[in]   entities        Objects from 'Entities' tag
     *
     * \return  Actor names referenced in the DOM
     */
    static openScenario::ActorInformation ImportActors(QDomElement& actorsElement,
                                                       const std::vector<ScenarioEntity>& entities);

    /*!
     * \brief Imports a maneuvre element of a OpenSCENARIO storyboard DOM
     *
     * \param[in]   maneuverElement   The DOM root of the maneuver element
     * \param[in]   entities          Objects from 'Entities' tag
     * \param[out]  scenario          The maneuver data is imported into this scenario
     * \param[out]  actors            Actors from the maneuver are imported into this container
     */
    static void ImportManeuverElement(QDomElement& maneuverElement, const std::vector<ScenarioEntity>& entities, ScenarioInterface *scenario, const std::string& sequenceName, const openScenario::ActorInformation &actorInformation, const int numberOfExecutions);

    static void SetStochasticsDataHelper(SpawnAttribute &attribute, QDomElement& stochasticsElement);
    static void SetStochasticsData(ScenarioEntity& scenarioEntity, QDomElement& stochasticsElement);
    static void SetOrientationData(ScenarioEntity& scenarioEntity, QDomElement& orientationElement);

    /*!
     * \brief Imports a private element of a OpenSCENARIO Storyboard/Init/Actions DOM
     *
     * \param[in]      privateElement    The DOM root of the storyboard/init/action element
     * \param[inout]   entities          Objects from 'Entities' tag. Containing spawn information will be set by this method call.
     */
    static void ImportPrivateElement(QDomElement& privateElement, std::vector<ScenarioEntity>& entities);

    /*!
     * \brief Imports all private elements of a OpenSCENARIO actions DOM
     *
     * \param[in]      privateElement    The DOM root of the actions element
     * \param[inout]   entities          Objects from 'Entities' tag. Containing spawn information will be set by this method call.
     * \param[out]     scenario          The private element data is imported into this scenario
     */
    static void ImportPrivateElements(QDomElement& actionsElement, std::vector<ScenarioEntity>& entities);

    static void ImportParameterDeclarationElement(QDomElement& parameterDeclarationElement, ParameterInterface *parameters);

    /*!
     * \brief Tests, if a entity with a given name is included in the provided vector of scenario entities
     *
     * \param[in] entities Vector of entities to test against
     * \param entityName Name of the entity to check for existence
     *
     * \return true, if entityName is included in entities, false otherwise
     */
    static bool ContainsEntity(const std::vector<ScenarioEntity>& entities,
                               const std::string& entityName);
    /*!
     * \brief Returns the entity object contained in a vector of entities, selected by its name
     *
     * \param[in]   entities        Vector of entities to search
     * \param[in]   entityName      Name of entity to look up
     *
     * \return      Pointer to found entity if name exists, nullptr otherwise
     */
    static ScenarioEntity* GetEntityByName(std::vector<ScenarioEntity>& entities, const std::string& entityName);

    /*!
     * \brief Categorizes the provided entities and adds them to the scenario either as scanario entity or ego entity.
     *
     * \param[in]   entities    Entities to check for being Ego or of other type
     * \param[out]  scenario    Ego and Scenario entities are added to this scenario
     */
    static void CategorizeEntities(const std::vector<ScenarioEntity>& entities, const std::map<std::string, std::list<std::string> > &groups, ScenarioInterface* scenario);

    //! Currently supported "internal" OpenSCENARIO version
    static constexpr auto supportedScenarioVersion = "0.3.0";
};

} // namespace Importer
