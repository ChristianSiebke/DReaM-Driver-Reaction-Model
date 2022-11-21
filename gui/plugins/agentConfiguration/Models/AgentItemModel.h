/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  AgentItemModel.h
//! @ingroup agentConfigurationPlugin
//! @brief This class implements the functionality of AgentItemInterface.
//-----------------------------------------------------------------------------

#ifndef AGENTITEMMODEL_H
#define AGENTITEMMODEL_H

#include "openPASS-AgentConfiguration/AgentItemInterface.h"

//-----------------------------------------------------------------------------
//! @file  AgentItemModel.h
//! @ingroup agentConfigurationPlugin
//! @brief This class implements the functionality of AgentItemInterface.
//-----------------------------------------------------------------------------
class AgentItemModel : public AgentItemInterface
{
public:
    explicit AgentItemModel(QObject *parent = nullptr);
    AgentItemModel(AgentItemInterface const *const agent, QObject *parent = nullptr);
    virtual ~AgentItemModel() override;

public:
    virtual AgentItemInterface::Name getName() const override;

public:
    virtual Type getType() const override;
    virtual void setType(const Type &_type) override;

public:
    virtual VehicleProfiles *getVehicleProfiles() const override;
    virtual DriverProfiles *getDriverProfiles() const override;

public:
    virtual VehicleModel getVehicleModel() const override;
    virtual void setVehicleModel(VehicleModel const &_vehicleModel) override;

public:
    virtual VehicleModelCatalogue getVehicleModelCatalogue() const override;
    virtual void setVehicleModelCatalogue(VehicleModelCatalogue const &_vehicleModelCatalogue) override;

public:
    virtual SystemID getSystemID() const override;
    virtual void setSystemID(SystemID const &_systemID) override;

public:
    virtual SystemTitle getSystemTitle() const override;
    virtual void setSystemTitle(SystemTitle const &_systemTitle) override;

public:
    virtual SystemConfig getSystemConfig() const override;
    virtual void setSystemConfig(SystemConfig const &_systemConfig) override;

public:
    virtual System *getSystem() override;

private:
    VehicleProfiles *const vehicleProfiles;      //!< the pointer to the vehicle profiles (VehicleProfileMapInterface)
    DriverProfiles *const driverProfiles;        //!< the pointer to the driver profiles (DriverProfileMapInterface)
    Type type;                                   //!< the agent profiles type (i.e. "dynamic" or "static")
    SystemConfig systemConfig;                   //!< the system config XML file
    System system;                               //!< the system (if static agent configuration is chosen)
    VehicleModelCatalogue vehicleModelCatalogue; //!< the vehicle model catalogue
    VehicleModel vehicleModel;                   //!< the vehicle model
};

#endif // AGENTITEMMODEL_H
