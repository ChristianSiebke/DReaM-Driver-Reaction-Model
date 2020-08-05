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
//! @file  SensorProfileItemModel.h
//! @ingroup agentConfigurationPlugin
//! @brief This class implements the functionality of SensorProfileItemInterface.
//-----------------------------------------------------------------------------

#ifndef SENSORPROFILEITEMMODEL_H
#define SENSORPROFILEITEMMODEL_H

#include "openPASS-AgentConfiguration/SensorProfileItemInterface.h"

//-----------------------------------------------------------------------------
//! @brief This class implements the functionality of SensorProfileItemInterface.
//-----------------------------------------------------------------------------
class SensorProfileItemModel : public SensorProfileItemInterface
{

    Q_OBJECT

public:
    SensorProfileItemModel(QObject *parent = nullptr);
    ~SensorProfileItemModel() = default;

public:
    virtual Parameters * getParameters() const override;


public:
    virtual ID getID() const override;
    virtual Name getName() const override;
    virtual Type getType() const override;

private:
    Parameters * const parameters;      //!< pointer to the parameter map of the sensor profile

};

#endif // SENSORPROFILEITEMMODEL_H
