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
//! @file  VehicleProfileItemModel.h
//! @ingroup agentConfigurationPlugin
//! @brief This class implements the functionality of VehicleProfileItemInterface.
//-----------------------------------------------------------------------------

#ifndef VEHICLEPROFILEITEMMODEL_H
#define VEHICLEPROFILEITEMMODEL_H

#include "openPASS-AgentConfiguration/VehicleProfileItemInterface.h"

//-----------------------------------------------------------------------------
//! @brief This class implements the functionality of VehicleProfileItemInterface.
//-----------------------------------------------------------------------------
class VehicleProfileItemModel : public VehicleProfileItemInterface
{

    Q_OBJECT

public:
    VehicleProfileItemModel(QObject *parent = nullptr);
    VehicleProfileItemModel(VehicleProfileItemInterface const * const vehicle, QObject *parent = nullptr);
    virtual ~VehicleProfileItemModel() override;

public:
    virtual Name getName() const override;

public:
    virtual ModelCatalogue getModelCatalogue() const override;
    virtual void setModelCatalogue(ModelCatalogue const & _modelCatalogue) override;

public:
    virtual Model getModel() const override;
    virtual void setModel(Model const & _model) override;

public:
    virtual SystemConfig getSystemConfig() const override;
    virtual void setSystemConfig( SystemConfig const & _systemConfig) override;

public:
    virtual SystemProfiles * getSystemProfiles() const override;

public:
    virtual Systems * getSystems(AlgoType const &_type) const override;


public:
    virtual VehicleComponents * getVehicleComponents() const override;
    virtual Sensors * getSensors() const override;


private:
    ModelCatalogue modelCatalogue;              //!< the string object storing the selected vehicle model catalogue
    Model model;                                //!< the string object storing the current vehicle model
    SystemConfig systemConfig;                  //!< the string object storing the selected system config file
    SystemProfiles * systemProfiles;            //!< the map registering mappings of systems onto probabilities using the algorithm type as a key identifier

private:
    VehicleComponents * const components;       //!< the map containing all vehicle component data
    Sensors * const sensors;                    //!< the map containing all sensor data
};

#endif // VEHICLEPROFILEITEMMODEL_H
