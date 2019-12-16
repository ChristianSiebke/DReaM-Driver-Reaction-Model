/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  AgentConfigurationModel.h
//! @ingroup agentConfigurationPlugin
//! @brief This class implements the functionality of AgentConfigurationInterface.
//-----------------------------------------------------------------------------
#ifndef AGENTCONFIGURATIONMODEL_H
#define AGENTCONFIGURATIONMODEL_H

#include "openPASS-AgentConfiguration/AgentConfigurationInterface.h"

#include "Models/AgentMapModel.h"
#include "Models/VehicleProfileMapModel.h"
#include "Models/DriverProfileMapModel.h"
#include "Models/VehicleComponentProfileMapModel.h"
#include "Models/SensorProfileMapModel.h"

//-----------------------------------------------------------------------------
//! @brief This class implements the functionality of AgentConfigurationInterface.
//-----------------------------------------------------------------------------
class AgentConfigurationModel : public AgentConfigurationInterface
{
    Q_OBJECT

public:
    explicit AgentConfigurationModel(QObject *parent = nullptr);
    virtual ~AgentConfigurationModel() = default;

public:
    virtual AgentMapInterface * getAgents() const override;
    virtual VehicleProfileMapInterface * getVehicleProfiles() const override;
    virtual DriverProfileMapInterface * getDriverProfiles() const override;
    virtual VehicleComponentProfileMapInterface * getVehicleComponentProfiles() const override;
    virtual SensorProfileMapInterface * getSensorProfiles() const override;

public:
    virtual void clear() override;

public:
    AgentMapModel * const agents;                               //!< the pointer to the map of agent profiles
    VehicleProfileMapModel * const vehicleProfiles;             //!< the pointer to the map of vehicle profiles
    DriverProfileMapModel * const driverProfiles;               //!< the pointer to the map of driver profiles
    VehicleComponentProfileMapModel * const componentProfiles;  //!< the pointer to the map of vehicle component profiles (corresponding to algorithms of an ADAS)
    SensorProfileMapModel * const sensorProfiles;               //!< the pointer to the map of sensor profiles


};

#endif // AGENTCONFIGURATIONMODEL_H
