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
//! @file  SensorItemModel.h
//! @ingroup agentConfigurationPlugin
//! @brief This class implements the functionality of SensorItemInterface.
//-----------------------------------------------------------------------------

#ifndef SENSORITEMMODEL_H
#define SENSORITEMMODEL_H

#include "openPASS-AgentConfiguration/SensorItemInterface.h"

//-----------------------------------------------------------------------------
//! @brief This class implements the functionality of SensorItemInterface.
//-----------------------------------------------------------------------------
class SensorItemModel : public SensorItemInterface
{
public:
    SensorItemModel(QObject *parent = nullptr);
    virtual ~SensorItemModel() override = default;

public:
    virtual SensorPosition getSensorPosition() const override;
    virtual void setSensorPosition(SensorPosition const &_position) override;

public:
    virtual Profile *getSensorProfile() const override;
    virtual void setSensorProfile(Profile *const _profile) override;

public:
    virtual ID getID() const override;

private:
    SensorPosition position;                //!< the structure for storing sensor positioning data
    Profile *profile;                       //!< the pointer to the sensor's profile
    static SensorPosition default_position; //!< the default position structure
};

#endif // SENSORITEMMODEL_H
