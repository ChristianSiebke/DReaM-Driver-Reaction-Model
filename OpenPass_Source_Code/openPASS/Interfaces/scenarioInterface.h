/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*               2016, 2017, 2018 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  ScenarioInterface.h
//! @brief This file contains the interface to translate the scenario between
//!        framework and world.
//-----------------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>

#include "Common/worldDefinitions.h"
#include "Common/eventDetectorDefinitions.h"
#include "Interfaces/scenarioActionInterface.h"

struct SpawnInfo
{
public:
    SpawnInfo() {}
    SpawnInfo(std::variant<openScenario::LanePosition, openScenario::WorldPosition> position,
                        double v,
                        double acceleration):
        position(position)
    {
        this->velocity = v;
        this->acceleration = acceleration;
    }

    std::variant<openScenario::LanePosition, openScenario::WorldPosition> position;

    std::optional<Route> route {std::nullopt};

    double velocity;
    std::optional<openScenario::StochasticAttribute> stochasticVelocity;

    std::optional<double> acceleration;
    std::optional<openScenario::StochasticAttribute> stochasticAcceleration;
};

/*!
 * \brief References an element inside a catalog
 */
struct CatalogReference
{
    std::string catalogName;    //!< Name of the catalog (currently used as filename reference)
    std::string entryName;      //!< Name of the element inside the catalog
};

/*!
 * \brief Represents an entity from the scenario
 */
struct ScenarioEntity
{
    std::string name;                               //! Name of the scenario object
    CatalogReference catalogReference;              //! Catalog reference information
    SpawnInfo spawnInfo;                            //! Initial spawn parameter information
    openScenario::Parameters assignedParameters;    //! Parameters assigned in the Catalog reference
};

//-----------------------------------------------------------------------------
//! Class representing a scenario as a list of roads.
//-----------------------------------------------------------------------------
class ScenarioInterface
{

public:
    ScenarioInterface() = default;
    ScenarioInterface(const ScenarioInterface&) = delete;
    ScenarioInterface(ScenarioInterface&&) = delete;
    ScenarioInterface& operator=(const ScenarioInterface&) = delete;
    ScenarioInterface& operator=(ScenarioInterface&&) = delete;
    virtual ~ScenarioInterface() = default;

    //-----------------------------------------------------------------------------
    //! \brief Retreives the path to the vehicle catalog file
    //!
    //! \return     Relative path to the vehicle catalog
    //-----------------------------------------------------------------------------
    virtual const std::string& GetVehicleCatalogPath() = 0;

    //-----------------------------------------------------------------------------
    //! \brief Sets the path to the vehicle catalog file
    //!
    //! \param[in]      catalogPath     Relative path to the vehicle catalog file
    //-----------------------------------------------------------------------------
    virtual void SetVehicleCatalogPath(const std::string& catalogPath) = 0;

    //-----------------------------------------------------------------------------
    //! \brief Retreives the path to the pedestrian catalog file
    //!
    //! \return     Relative path to the pedestrian catalog
    //-----------------------------------------------------------------------------
    virtual const std::string& GetPedestrianCatalogPath() = 0;

    //-----------------------------------------------------------------------------
    //! Sets the path to the pedestrian catalog file
    //!
    //! \param[in]      catalogPath     Relative path to the pedestrian catalog file
    //-----------------------------------------------------------------------------
    virtual void SetPedestrianCatalogPath(const std::string& catalogPath) = 0;

    //-----------------------------------------------------------------------------
    //! \brief Retreives the path to the trajectory catalog file
    //!
    //! The path can either be absolute or relative to the simulator executable
    //!
    //! \return     Path to the trajectory catalog file
    //-----------------------------------------------------------------------------
    virtual const std::string& GetTrajectoryCatalogPath() = 0;

    //-----------------------------------------------------------------------------
    //! Sets the path to the trajectory catalog file
    //!
    //! The path can either be absolute or relative to the simulator executable
    //!
    //! \param[in]      catalogPath     Path to the trajectory catalog file
    //-----------------------------------------------------------------------------
    virtual void SetTrajectoryCatalogPath(const std::string& catalogPath) = 0;

    //-----------------------------------------------------------------------------
    //! Retreives the path to the scenery file (OpenDRIVE)
    //!
    //! \return     Relative path to the scenery file
    //-----------------------------------------------------------------------------
    virtual const std::string& GetSceneryPath() = 0;

    //-----------------------------------------------------------------------------
    //! Sets the path to the scenery file (OpenDRIVE)
    //!
    //! \param[in]      sceneryPath     Relative path to the scenery file
    //-----------------------------------------------------------------------------
    virtual void SetSceneryPath(const std::string& sceneryPath) = 0;

    //-----------------------------------------------------------------------------
    //! Adds one scenario entity to the scenery entities of the scenario.
    //-----------------------------------------------------------------------------
    virtual void AddScenarioEntity(const ScenarioEntity& entity) = 0;

    //-----------------------------------------------------------------------------
    //! Adds groups to the scenario as defined by groupDefinitions - a map of
    //! group names to a list of group member entity names.
    //-----------------------------------------------------------------------------
    virtual void AddScenarioGroupsByEntityNames(const std::map<std::string, std::list<std::string>> &groupDefinitions) = 0;

    virtual const std::vector<ScenarioEntity>& GetEntities() const = 0;

    //-----------------------------------------------------------------------------
    //! Returns the ego entity of the scenario.
    //!
    //! @return                         ScenarioEntities of vehicles other than ego
    //-----------------------------------------------------------------------------
    virtual const std::vector<ScenarioEntity*> &GetScenarioEntities() = 0;

    //-----------------------------------------------------------------------------
    //! Returns the entity groups of the scenario.
    //!
    //! @return map of group names to vector of ScenarioEntities belonging to the
    //! 		group
    //-----------------------------------------------------------------------------
    virtual const std::map<std::string, std::vector<ScenarioEntity*>> &GetScenarioGroups() = 0;

    //-----------------------------------------------------------------------------
    //! Adds a event detector to the event detectors map.
    //-----------------------------------------------------------------------------
    virtual void AddConditionalEventDetector(const openScenario::ConditionalEventDetectorInformation &eventDetectorInformation) = 0;

    //-------------------------------------------------------------------------
    //! \brief AddAction Adds a shared_ptr to an action to the actions map
    //!
    //! \param[in] action a shared_ptr to an action
    //-------------------------------------------------------------------------
    virtual void AddAction(std::shared_ptr<ScenarioActionInterface> action) = 0;

    //-----------------------------------------------------------------------------
    //! Returns the event detector.
    //!
    //! @return                         list of event detector
    //-----------------------------------------------------------------------------
    virtual const std::vector<openScenario::ConditionalEventDetectorInformation>& GetEventDetectorInformations() = 0;

    //-------------------------------------------------------------------------
    //! \brief GetActions Returns the actions of the scenario
    //!
    //! \returns list of actions
    //-------------------------------------------------------------------------
    virtual std::vector<std::shared_ptr<ScenarioActionInterface>> GetActions() const = 0;

    //-------------------------------------------------------------------------
    //! \brief Returns the desired end time of the simulation.
    //! \returns the desired end time of the simulation.
    //-------------------------------------------------------------------------
    virtual int GetEndTime() const = 0;

    //-------------------------------------------------------------------------
    //! \brief Sets the desired end time of the simulation.
    //! \param[in] endTime The desired end time of the simulation.
    //-------------------------------------------------------------------------
    virtual void SetEndTime(const double endTime) = 0;
};
