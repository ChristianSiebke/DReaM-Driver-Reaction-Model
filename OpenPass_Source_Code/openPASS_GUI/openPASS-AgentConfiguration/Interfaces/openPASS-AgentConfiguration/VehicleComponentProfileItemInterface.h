/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef VEHICLECOMPONENTPROFILEITEMINTERFACE_H
#define VEHICLECOMPONENTPROFILEITEMINTERFACE_H

#include "openPASS-AgentConfiguration/ParameterMapInterface.h"

#include <QObject>
#include <QMap>

class VehicleComponentProfileItemInterface: public QObject
{

    Q_OBJECT

public:
    using Name = QString;
    using Type = QString;
    using Parameters = ParameterMapInterface;

public:
    VehicleComponentProfileItemInterface(QObject * const parent = nullptr) : QObject(parent){}
    virtual ~VehicleComponentProfileItemInterface() = default;

public:
    virtual Parameters * getParameters() const = 0;

public:
    virtual Name getName() const = 0;
    virtual Type getType() const = 0;
};

#endif // VEHICLECOMPONENTPROFILEITEMINTERFACE_H
