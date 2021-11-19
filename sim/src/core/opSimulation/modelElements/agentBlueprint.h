/********************************************************************************
 * Copyright (c) 2016 ITK Engineering GmbH
 *               2017-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
/** \file  AgentBlueprint.h
*   \brief This file stores all information which is necessary to instantiate an agent
*/
//-----------------------------------------------------------------------------

#pragma once

#include "agentType.h"
#include "include/agentBlueprintInterface.h"
#include "spawnItemParameter.h"

//-----------------------------------------------------------------------------
/** \brief This class stores all information which is required to instantiate an agent.
*   \details The information stored include physical spawn variable as well as components the agent consists out of.
*
*/
//-----------------------------------------------------------------------------
class AgentBlueprint : public AgentBlueprintInterface
{
public:
    AgentBlueprint();
    virtual ~AgentBlueprint() override = default;

    /*!
    * \brief Sets the vehicle component profile names
    *
    * @param[in]     vehicleComponentProfileNames
    */
    virtual void SetVehicleComponentProfileNames(VehicleComponentProfileNames vehicleComponentProfileNames) override;

    /*!
    * \brief Sets the agent category
    *
    * @param[in]     agentCategory
    */
    virtual void SetAgentCategory(AgentCategory agentCategory) override;

    /*!
    * \brief Sets the agent profile name
    *
    * @param[in]     agentProfileName
    */
    virtual void SetAgentProfileName(std::string agentProfileName) override;

    /*!
    * \brief Sets the vehicle profile name
    *
    * @param[in]     vehicleProfileName
    */
    virtual void SetVehicleProfileName(std::string vehicleProfileName) override;

    /*!
    * \brief Sets the vehicle model name
    *
    * @param[in]     vehicleModelName
    */
    virtual void SetVehicleModelName(std::string vehicleModelName) override;

    /*!
    * \brief Sets the vehicle model parameter
    *
    * @param[in]     vehicleModelParameters
    */
    virtual void SetVehicleModelParameters(VehicleModelParameters vehicleModelParameters) override;

    /*!
    * \brief Sets the driver profile name
    *
    * @param[in]     driverProfileName
    */
    virtual void SetDriverProfileName(std::string driverProfileName) override;

    /*!
    * \brief Sets the spawn parameter
    *
    * @param[in]     spawnParameter
    */
    virtual void SetSpawnParameter(SpawnParameter spawnParameter) override;

    /*!
    * \brief Sets the minimum speed goal
    *
    * @param[in]     speedGoalMin
    */
    virtual void SetSpeedGoalMin(double speedGoalMin) override;

    /*!
    * \brief Sets the object name
    *
    * @param[in]     objectName
    */
    virtual void SetObjectName(std::string objectName) override;

    /*!
    * \brief Adds a sensor to the vehicle model parameters
    *
    * @param[in]     name           Name of the sensor
    * @param[in]     parameters     Parameters of the sensor
    */
    virtual void AddSensor(openpass::sensors::Parameter parameters) override;

    /*!
    * \brief Returns the agent category
    *
    * @return     agentCategory
    */
    virtual AgentCategory               GetAgentCategory() const override;

    /*!
    * \brief Returns the agent profile name
    *
    * @return     agentProfileName
    */
    virtual std::string                 GetAgentProfileName() const override;

    /*!
    * \brief Returns the vehicle profile name
    *
    * @return     vehicleProfileName
    */
    virtual std::string                 GetVehicleProfileName() const override;

    /*!
    * \brief Returns the vehicle model name
    *
    * @return     vehicleModelName
    */
    virtual std::string                 GetVehicleModelName() const override;

    /*!
    * \brief Returns the driver profile name
    *
    * @return     driverProfileName
    */
    virtual std::string                 GetDriverProfileName() const override;

    /*!
    * \brief Returns the object name
    *
    * @return     objectName
    */
    virtual std::string                 GetObjectName() const override;

    /*!
    * \brief Returns the vehicle model parameter
    *
    * @return     vehicleModelParameters
    */
    virtual VehicleModelParameters       GetVehicleModelParameters() const override;

    /*!
    * \brief Returns the sensor parameter
    *
    * @return     sensorParameter
    */
    virtual openpass::sensors::Parameters GetSensorParameters() const override;

    /*!
    * \brief Returns the vehicle components profile names
    *
    * @return     vehicleComponentProfileNames
    */
    virtual VehicleComponentProfileNames            GetVehicleComponentProfileNames() const override;

    virtual void SetAgentType(std::shared_ptr<core::AgentTypeInterface> agentType) override;

    /*!
    * \brief Returns the agent type as pointer
    *
    * @return     agentType
    */
    virtual core::AgentTypeInterface& GetAgentType() const override;

    /*!
    * \brief Returns the spawn parameter as reference
    *
    * @return     spawnParameter
    */
    virtual SpawnParameter&             GetSpawnParameter() override;

    /*!
    * \brief Returns the spawn parameter as reference
    *
    * @return     spawnParameter
    */
    virtual const SpawnParameter&       GetSpawnParameter() const override;

    /*!
    * \brief Returns the minimum speed goal
    *
    * @return     speedGoalMin
    */
    virtual double                      GetSpeedGoalMin() const override;

private:
    AgentCategory agentCategory {AgentCategory::Common};
    std::string agentProfileName = "";
    std::string vehicleProfileName = "";
    std::string vehicleModelName = "";
    std::string driverProfileName = "";
    std::string objectName = "";
    VehicleComponentProfileNames vehicleComponentProfileNames;

    SpawnParameter spawnParameter;
    VehicleModelParameters vehicleModelParameters;
    openpass::sensors::Parameters sensorParameters;

    std::shared_ptr<core::AgentTypeInterface> agentType {nullptr};
    double speedGoalMin = 30.0 / 3.6;
};


