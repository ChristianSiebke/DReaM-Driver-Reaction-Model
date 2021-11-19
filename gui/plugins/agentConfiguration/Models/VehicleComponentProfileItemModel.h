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
//! @file  VehicleComponentProfileItemModel.h
//! @ingroup agentConfigurationPlugin
//! @brief This class implements the functionality of VehicleComponentProfileItemInterface.
//-----------------------------------------------------------------------------

#ifndef VEHICLECOMPONENTPROFILEITEMMODEL_H
#define VEHICLECOMPONENTPROFILEITEMMODEL_H

#include "openPASS-AgentConfiguration/VehicleComponentProfileItemInterface.h"

//-----------------------------------------------------------------------------
//! @brief This class implements the functionality of VehicleComponentProfileItemInterface.
//-----------------------------------------------------------------------------
class VehicleComponentProfileItemModel : public VehicleComponentProfileItemInterface
{
    Q_OBJECT

public:
    VehicleComponentProfileItemModel(QObject *parent = nullptr);
    ~VehicleComponentProfileItemModel() override = default;

public:
    virtual Parameters *getParameters() const override;

public:
    virtual Name getName() const override;
    virtual Type getType() const override;

private:
    Parameters *const parameters; //!< the pointer to the parameter map of this profile
};

#endif // VEHICLECOMPONENTPROFILEITEMMODEL_H
