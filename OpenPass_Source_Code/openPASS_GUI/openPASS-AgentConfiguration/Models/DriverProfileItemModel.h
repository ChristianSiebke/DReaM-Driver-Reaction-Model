/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef DRIVERPROFILEITEMMODEL_H
#define DRIVERPROFILEITEMMODEL_H

#include "openPASS-AgentConfiguration/DriverProfileItemInterface.h"

class DriverProfileItemModel : public DriverProfileItemInterface
{
    Q_OBJECT

public:
    DriverProfileItemModel(QObject * parent = nullptr);
    DriverProfileItemModel(DriverProfileItemInterface const * const driverProfile, QObject * parent = nullptr);
    ~DriverProfileItemModel() = default;

public:
    virtual Name getName() const override;

public:
    virtual DriverConfig getConfig() const override;
    virtual void setConfig(const DriverConfig &_driverConfig ) override;

public:
    virtual Parameters* getParameters() const override;


private:
    Parameters * const parameters;
    DriverConfig driverConfig;
};

#endif // DRIVERPROFILEITEMMODEL_H
