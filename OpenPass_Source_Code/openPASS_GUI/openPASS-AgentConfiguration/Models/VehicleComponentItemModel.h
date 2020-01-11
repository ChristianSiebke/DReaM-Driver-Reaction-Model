/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef VEHICLECOMPONENTITEMMODEL_H
#define VEHICLECOMPONENTITEMMODEL_H

#include "openPASS-AgentConfiguration/VehicleComponentItemInterface.h"

class VehicleComponentItemModel : public VehicleComponentItemInterface
{
    Q_OBJECT

public:
    VehicleComponentItemModel(QObject *parent = nullptr);
    virtual ~VehicleComponentItemModel() override;

public:
    virtual Profiles * getProfiles() const override;
    virtual SensorLinks * getSensorLinks() const override;

public:
    virtual Type getType() const override;

private:
    Profiles * const profiles;
    SensorLinks * const sensorLinks;

};

#endif // VEHICLECOMPONENTITEMMODEL_H
