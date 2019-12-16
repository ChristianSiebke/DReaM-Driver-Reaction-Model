/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef SENSORPROFILEITEMINTERFACE_H
#define SENSORPROFILEITEMINTERFACE_H

#include "openPASS-AgentConfiguration/ParameterMapInterface.h"

#include <QObject>
#include <QPair>

class SensorProfileItemInterface: public QObject
{

    Q_OBJECT

public:
    using Name = QString;
    using Type = QString;
    using ID = QPair<Name, Type>;
    using Parameters = ParameterMapInterface;

public:
    SensorProfileItemInterface(QObject * const parent = nullptr) : QObject(parent){}
    virtual ~SensorProfileItemInterface() = default;

public:
    virtual Parameters * getParameters() const = 0;

public:
    virtual ID getID() const = 0;
    virtual Name getName() const = 0;
    virtual Type getType() const = 0;

};

#endif // SENSORPROFILEITEMINTERFACE_H
