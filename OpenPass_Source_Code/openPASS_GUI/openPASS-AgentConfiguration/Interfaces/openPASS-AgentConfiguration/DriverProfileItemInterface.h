/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef DRIVERPROFILEITEMINTERFACE_H
#define DRIVERPROFILEITEMINTERFACE_H

#include "openPASS-AgentConfiguration/ParameterMapInterface.h"

#include <QObject>
#include <QString>
#include <QMap>

class DriverProfileItemInterface : public QObject
{
    Q_OBJECT

public:
    using Name = QString;
    using DriverConfig = QString;               // some file which specifies how many parameters a driver has...
    using Parameters = ParameterMapInterface;

public:
    DriverProfileItemInterface(QObject * const parent = nullptr) : QObject(parent) {}
    ~DriverProfileItemInterface() = default;

public:
    virtual Name getName() const = 0;

public:
    virtual DriverConfig getConfig() const = 0;
    virtual void setConfig(DriverConfig const &_driverConfig ) = 0;

public:
    virtual Parameters * getParameters() const = 0;

};

#endif // DRIVERPROFILEITEMINTERFACE_H
