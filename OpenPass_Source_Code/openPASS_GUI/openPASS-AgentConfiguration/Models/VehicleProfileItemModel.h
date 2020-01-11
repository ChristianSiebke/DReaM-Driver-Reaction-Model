/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef VEHICLEPROFILEITEMMODEL_H
#define VEHICLEPROFILEITEMMODEL_H

#include "openPASS-AgentConfiguration/VehicleProfileItemInterface.h"

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
    virtual VehicleComponents * getVehicleComponents() const override;      // interferes with systemConfig
    virtual Sensors * getSensors() const override;                          // interferes with systemConfig


private:
    ModelCatalogue modelCatalogue;
    Model model;
    SystemConfig systemConfig;
    SystemProfiles * systemProfiles;

private:
    VehicleComponents * const components;       // interferes with systemConfig
    Sensors * const sensors;                    // interferes with systemConfig
};

#endif // VEHICLEPROFILEITEMMODEL_H
