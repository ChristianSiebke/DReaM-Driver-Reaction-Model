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
//! @file  DriverProfileItemModel.h
//! @ingroup agentConfigurationPlugin
//! @brief This class implements the functionality of DriverProfileItemInterface.
//-----------------------------------------------------------------------------

#ifndef DRIVERPROFILEITEMMODEL_H
#define DRIVERPROFILEITEMMODEL_H

#include "openPASS-AgentConfiguration/DriverProfileItemInterface.h"

//-----------------------------------------------------------------------------
//! @brief This class implements the functionality of DriverProfileItemInterface.
//-----------------------------------------------------------------------------
class DriverProfileItemModel : public DriverProfileItemInterface
{
    Q_OBJECT

public:
    DriverProfileItemModel(QObject *parent = nullptr);
    DriverProfileItemModel(DriverProfileItemInterface const *const driverProfile, QObject *parent = nullptr);
    virtual ~DriverProfileItemModel() override = default;

public:
    virtual Name getName() const override;

public:
    virtual DriverConfig getConfig() const override;
    virtual void setConfig(const DriverConfig &_driverConfig) override;

public:
    virtual Parameters *getParameters() const override;

private:
    Parameters *const parameters; //!< instance of ParameterMapInterface for storing parameters
    DriverConfig driverConfig;    //!< string object for storing the driver configuration file (so far, not used!)
};

#endif // DRIVERPROFILEITEMMODEL_H
