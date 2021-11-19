/********************************************************************************
 * Copyright (c) 2017-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
//-----------------------------------------------------------------------------
//! @file  Scenario.h
//! @brief This file contains all information for the choosen scenario
//-----------------------------------------------------------------------------

#pragma once

#include <string>
#include "common/globalDefinitions.h"
#include "common/eventDetectorDefinitions.h"
#include "common/opExport.h"
#include "include/scenarioInterface.h"
#include "sceneryDynamics.h"

namespace Configuration {

class SIMULATIONCOREEXPORT Scenario : public ScenarioInterface
{
public:
    Scenario() = default;
    Scenario(const Scenario&) = delete;
    Scenario(Scenario&&) = delete;
    Scenario& operator=(const Scenario&) = delete;
    Scenario& operator=(Scenario&&) = delete;
    virtual ~Scenario() override = default;

    /*!
     * \brief Getter for vehicle catalog path
     *
     * \return The relative file path to the OpenSCENARIO vehicle catalog in use
     */
    const std::string& GetVehicleCatalogPath() override;

    /*!
     * \brief Setter for vehicle catalog path
     *
     * \param[in]   catalogPath     Relative file path to the OpenSCENARIO vehicle catalog to use
     */
    void SetVehicleCatalogPath(const std::string& catalogPath) override;

    /*!
     * \brief Getter for pedestrian catalog path
     *
     * \return The relative file path to the OpenSCENARIO pedestrian catalog in use
     */
    const std::string& GetPedestrianCatalogPath() override;

    /*!
     * \brief Setter for pedestrian catalog path
     *
     * \param[in]   catalogPath     Relative file path to the OpenSCENARIO pedestrian catalog to use
     */
    void SetPedestrianCatalogPath(const std::string& catalogPath) override;

    //-----------------------------------------------------------------------------
    //! \brief Retreives the path to the trajectory catalog file
    //!
    //! \return     Relative path to the trajectory catalog
    //-----------------------------------------------------------------------------
    const std::string& GetTrajectoryCatalogPath() override;

    //-----------------------------------------------------------------------------
    //! Sets the path to the trajectory catalog file
    //!
    //! \param[in]      catalogPath     Relative path to the trajectory catalog file
    //-----------------------------------------------------------------------------
    void SetTrajectoryCatalogPath(const std::string& catalogPath) override;

    /*!
     * \brief Getter for scenery path
     *
     * \return The relative file path to the OpenDRIVE scenery in use
     */
    const std::string& GetSceneryPath() override;

    /*!
     * \brief Setter for scenery path
     *
     * \param[in]   sceneryPath     Relative file path to the OpenDRIVE scenery to use
     */
    void SetSceneryPath(const std::string& sceneryPath) override;

    const SceneryDynamicsInterface& GetSceneryDynamics() override;

    void AddTrafficSignalController (const openScenario::TrafficSignalController& controller) override;

    void AddScenarioEntity(const ScenarioEntity& entity) override;
    void AddScenarioGroupsByEntityNames(const std::map<std::string, std::vector<std::string>> &groupDefinitions) override;

    const std::vector<ScenarioEntity> &GetEntities() const override;
    const std::vector<ScenarioEntity*> &GetScenarioEntities() override;
    const std::map<std::string, std::vector<ScenarioEntity*>> &GetScenarioGroups() override;

    void AddConditionalEventDetector(const openScenario::ConditionalEventDetectorInformation &eventDetectorInformation) override;
    void AddAction(const openScenario::Action action, const std::string eventName) override;

    std::string GetEventDetectorLibraryName();
    const std::vector<openScenario::ConditionalEventDetectorInformation>& GetEventDetectorInformations() override;

    std::string GetManipulatorLibraryName();
    std::vector<openScenario::ManipulatorInformation> GetActions() const override;

    int GetEndTime() const override;
    void SetEndTime(const double endTime) override;

    void SetEnvironment(const openScenario::EnvironmentAction &environment) override;

private:
    ScenarioEntity egoEntity;
    std::vector<ScenarioEntity> entities;
    std::map<std::string, std::vector<ScenarioEntity*>> scenarioGroups;

    std::vector<openScenario::ConditionalEventDetectorInformation> eventDetectorInformations;
    std::vector<openScenario::ManipulatorInformation> actions;
    SceneryDynamics sceneryDynamics;

    std::string vehicleCatalogPath;     //!< The path of the vehicle catalog (relative to Scenario.xosc)
    std::string pedestrianCatalogPath;  //!< The path of the pedestrian catalog (relative to Scenario.xosc)
    std::string trajectoryCatalogPath;  //!< The path of the trajectory catalog (relative to Scenario.xosc)
    std::string sceneryPath;            //!< The path of the scenery file (relative to Scenario.xosc)

    double endTimeInSeconds; //!< The simulationTime at which the simulation should end
};


} // namespace core


