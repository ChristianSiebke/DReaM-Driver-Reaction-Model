/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef VEHICLECOMPONENTPROFILEITEMMODEL_H
#define VEHICLECOMPONENTPROFILEITEMMODEL_H

#include "openPASS-AgentConfiguration/VehicleComponentProfileItemInterface.h"

class VehicleComponentProfileItemModel : public VehicleComponentProfileItemInterface
{

    Q_OBJECT

public:
    VehicleComponentProfileItemModel(QObject *parent = nullptr);
    ~VehicleComponentProfileItemModel() override = default;

public:
    virtual Parameters * getParameters() const override;

public:
    virtual Name getName() const override;
    virtual Type getType() const override;

private:
    Parameters * const parameters;
};

#endif // VEHICLECOMPONENTPROFILEITEMMODEL_H
