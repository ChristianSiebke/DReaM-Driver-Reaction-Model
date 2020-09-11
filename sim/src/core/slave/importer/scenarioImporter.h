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

//-----------------------------------------------------------------------------
//! @file  ScenarioImporter.h
//! @brief This file contains the importer of the scenario.
//-----------------------------------------------------------------------------

#pragma once

#include <string>
#include <QDomDocument>
#include "include/scenarioInterface.h"
#include "common/parameters.h"
#include "common/openScenarioDefinitions.h"
#include "oscImporterCommon.h"

namespace Importer
{

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
    /*!
     * \brief Imports an entity of a OpenSCENARIO Entities DOM
     *
     * \param[in]   entityElement   The DOM root of the entity element
     * \param[out]  entity          Entity element data is imported into this container
     * \param[in]   parameters      declared parameters
     */
    static void ImportEntity(QDomElement& entityElement, ScenarioEntity& entity, openScenario::Parameters& parameters);

    /*!
     * \brief Imports the storyboard from OpenSCENARIO DOM
     *
     * \param[in]   documentRoot    The DOM root of the scenario file
     * \param[in]   entities        Objects from 'Entities' tag
     * \param[out]  scenario        The storyboard is imported into this scenario
     * \param[in]   parameters      declared parameters
     */
    static void ImportStoryboard(QDomElement& documentRoot, std::vector<ScenarioEntity>& entities, ScenarioInterface* scenario, openScenario::Parameters& parameters);

    /*!
     * \brief Imports a catalog tag
     *
     * \param[in]   catalogName         Name of the catalog to import
     * \param[in]   catatalogsElement   DOM element holding all available catalogs
     * \param[in]   parameters          declared parameters
     *
     * \return      True on successful parsing, false otherwise.
     */
    static std::string ImportCatalog(const std::string& catalogName, QDomElement& catalogsElement, openScenario::Parameters& parameters);

private:
    /*!
     * \brief Imports and validates the internally used OpenSCENARIO schema version
     *
     * \param[in]   parameters      declared parameters
     *
     * \return  true if import succeeds and version matches, falso otherwise
     */
    static void ImportAndValidateVersion(openScenario::Parameters& parameters);

    /*!
     * \brief Imports catalogs from OpenSCENARIO DOM
     *
     * Currently, vehicle and pedestrian catalogs are imported.
     *
     * \param[in]   documentRoot    The DOM root of the scenario file
     * \param[out]  scenario        Catalogs are imported into this scenario
     * \param[in]   parameters      declared parameters
     */
    static void ImportCatalogs(QDomElement& documentRoot, ScenarioInterface* scenario, const std::string& filePath, openScenario::Parameters& parameters);

    /*!
     * \brief Imports the roadnetwork information from OpenSCENARIO DOM
     *
     * Currently, the path to the OpenDRIVE scenery file is extracted.
     *
     * \param[in]   roadNetworkElement  DOM element of the roadnetwork tag
     * \param[out]  sceneryPath         File path to the scenery
     * \param[in]   parameters          declared parameters
     */
    static void ImportRoadNetwork(QDomElement& roadNetworkElement, std::string& sceneryPath, openScenario::Parameters& parameters);

    /*!
     * \brief Imports the init element of a OpenSCENARIO storyboard DOM
     *
     * \param[in]   initElement     The DOM of the storyboard init element
     * \param[in]   entities        Objects from 'Entities' tag
     * \param[in]   parameters      declared parameters
     */
    static void ImportInitElement(QDomElement& initElement, std::vector<ScenarioEntity>& entities, openScenario::Parameters& parameters);

    /*!
     * \brief Imports the simulation end conditions
     * \param[in]   storyboardElement   The storyboard DOM element
     * \param[out]  scenario            End conditions data is imported into this scenario
     * \param[in]   parameters          declared parameters
     */
    static void ImportEndConditionsFromStoryboard(const QDomElement &storyboardElement, ScenarioInterface *scenario, openScenario::Parameters& parameters);

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
     * \param[in]   parameters      declared parameters
     *
     * \throw std::runtime_exception If name, delay, or edge is not specified,
     * 								 or if a negative delay is provided
     */
    static void ParseConditionAttributes(const QDomElement& conditionElement, std::string& name, double& delay, std::string& edge, openScenario::Parameters& parameters);

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
     * \param[in]   parameters      declared parameters
     *
     * \throw std::runtime_exception If value or rule is not specified, or an
     * 								 invalid rule is provided
     */
    static void ParseSimulationTime(const QDomElement &byValueElement, double& value, std::string& rule, openScenario::Parameters& parameters);

    /*!
     * \brief Imports the entities element of a OpenSCENARIO DOM
     *
     * \param[in]   documentRoot    The DOM root of the scenario file
     * \param[out]  entities        Entity element data is imported into this container
     * \param[in]   parameters      declared parameters
     */
    static void ImportEntities(QDomElement& documentRoot, std::vector<ScenarioEntity>& entities, std::map<std::string, std::list<std::string> > &groups, openScenario::Parameters& parameters);

    /*!
     * \brief Imports a group definition of a OpenSCENARIO Selection DOM
     *
     * \param[in]   selectionElement	The DOM node of the selection element
     * \param[out]  groups 				Groups element data is imported into this container
     * \param[in]   parameters      declared parameters
     */
    static void ImportSelectionElements(QDomElement &entitiesElement, std::map<std::string, std::list<std::string>> &groups, openScenario::Parameters& parameters);

    /*!
     * \brief Imports a list of members of a OpenSCENARIO Members DOM
     *
     * \param[in]   membersElement  The DOM root of the members element
     * \param[out]  members			Members element data is imported into this container
     */
    static void ImportMembers(const QDomElement &membersElement, std::list<std::string> &members, openScenario::Parameters& parameters);

    /*!
     * \brief Imports a catalog reference of an entity of a OpenSCENARIO Entities DOM
     *
     * \param[in]   catalogReferenceElement   The DOM root of the catalog reference element
     * \param[out]  entity                    Catalog refrence data is imported into this container
     * \param[in]   parameters      declared parameters
     */
    static void ImportEntityCatalogReference(QDomElement& catalogReferenceElement, ScenarioEntity& entity, openScenario::Parameters& parameters);

    /*!
     * \brief Imports a story element of a OpenSCENARIO storyboard DOM
     *
     * \param[in]   storyElement   The DOM root of the catalog reference element
     * \param[in]   entities       Objects from 'Entities' tag
     * \param[out]  scenario       The story data is imported into this scenario
     * \param[in]   parameters      declared parameters
     */
    static void ImportStoryElement(QDomElement& storyElement, const std::vector<ScenarioEntity>& entities, ScenarioInterface *scenario, openScenario::Parameters& parameters);

    /*!
     * \brief Imports actors from of a OpenSCENARIO story DOM
     *
     * \param[in]   actorsElement   The DOM root of the actors element
     * \param[in]   entities        Objects from 'Entities' tag
     * \param[in]   parameters      declared parameters
     *
     * \return  Actor names referenced in the DOM
     */
    static openScenario::ActorInformation ImportActors(QDomElement& actorsElement,
                                                       const std::vector<ScenarioEntity>& entities,
                                                       openScenario::Parameters& parameters);

    /*!
     * \brief Imports a maneuvre element of a OpenSCENARIO storyboard DOM
     *
     * \param[in]   maneuverElement   The DOM root of the maneuver element
     * \param[in]   entities          Objects from 'Entities' tag
     * \param[out]  scenario          The maneuver data is imported into this scenario
     * \param[out]  actors            Actors from the maneuver are imported into this container
     * \param[in]   parameters        declared parameters
     */

    /*!
     * \brief ImportManeuverElement
     * \param maneuverElement    The DOM root of the maneuver element
     * \param entities           Objects from 'Entities' tag
     * \param scenario           The maneuver data is imported into this scenario
     * \param actorInformation   Information of actors from the maneuver
     * \param nameStack          List of names from tree structure e.g. {"myStory", "myAct", "myManeuver"}
     * \param numberOfExecutions Number of executions
     */
    static void ImportManeuverElement(QDomElement& maneuverElement,
                                      const std::vector<ScenarioEntity>& entities,
                                      ScenarioInterface *scenario,
                                      const openScenario::ActorInformation &actorInformation,
                                      const std::vector<std::string> &nameStack,
                                      const int numberOfExecutions,
                                      openScenario::Parameters& parameters);

    /*!
     * \brief Imports a orientation element of a OpenSCENARIO entity
     *
     * \param[in]   orientationElement  The DOM root of the orientation element
     * \param[in]   parameters          declared parameters
     * \return  orientation
     */
    static openScenario::Orientation ImportOrientation(QDomElement& orientationElement, openScenario::Parameters& parameters);

    /*!
     * \brief Imports a private element of a OpenSCENARIO Storyboard/Init/Actions DOM
     *
     * \param[in]      privateElement    The DOM root of the storyboard/init/action element
     * \param[inout]   entities          Objects from 'Entities' tag. Containing spawn information will be set by this method call.
     * \param[in]      parameters      declared parameters
     */
    static void ImportPrivateElement(QDomElement& privateElement, std::vector<ScenarioEntity>& entities, openScenario::Parameters& parameters);

    /*!
     * \brief Imports all private elements of a OpenSCENARIO actions DOM
     *
     * \param[in]      privateElement    The DOM root of the actions element
     * \param[inout]   entities          Objects from 'Entities' tag. Containing spawn information will be set by this method call.
     * \param[in]      parameters        declared parameters
     */
    static void ImportPrivateElements(QDomElement& actionsElement, std::vector<ScenarioEntity>& entities, openScenario::Parameters& parameters);

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

    /*!
     * \brief Imports an openScenario Lane-Position element
     *
     * \param[in]   positionElement    XML-Element containing position
     * \param[in]   parameters      declared parameters
     * \return      lanePosition
     */
    static openScenario::LanePosition ImportLanePosition(QDomElement positionElement, openScenario::Parameters& parameters);

    /*!
     * \brief Imports an openScenario World-Position element
     *
     * \param[in]   positionElement    XML-Element containing position
     * \param[in]   parameters      declared parameters
     * \return  worldPosition
     */
    static openScenario::WorldPosition ImportWorldPosition(QDomElement positionElement, openScenario::Parameters& parameters);

    /*!
     * \brief Imports an openScenario Action element
     *
     * \param[in]   eventElement        XML-Element containing Event
     * \param[in]   parameters          declared parameters
     * \param[in]   catalogPath         catalog path
     * \return  action
     */
    static openScenario::Action ImportAction(QDomElement eventElement, openScenario::Parameters& parameters, const std::string catalogPath);

    //! Currently supported "internal" OpenSCENARIO version
    static constexpr auto supportedScenarioVersion = "0.4.0";
};

} // namespace Importer
