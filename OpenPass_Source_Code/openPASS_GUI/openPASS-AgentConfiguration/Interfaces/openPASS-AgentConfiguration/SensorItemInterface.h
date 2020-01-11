/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef SENSORITEMINTERFACE_H
#define SENSORITEMINTERFACE_H

#include "openPASS-AgentConfiguration/SensorProfileItemInterface.h"

#include <QObject>


struct SensorPosition
{
    QString Name;

    double roll;
    double yaw;
    double pitch;

    double height;
    double lateral;
    double longitudinal;
};

class SensorItemInterface: public QObject
{

    Q_OBJECT

public:
    using ID = unsigned int;
    using Profile = SensorProfileItemInterface;

public:
    SensorItemInterface(QObject * const parent = nullptr) : QObject(parent){}
    virtual ~SensorItemInterface() = default;

public:
    virtual SensorPosition getSensorPosition() const =0;
    virtual void setSensorPosition(SensorPosition const & position) =0;

public:
    virtual Profile * getSensorProfile() const =0;
    virtual void setSensorProfile(Profile  * const _profile) =0;

public:
    virtual ID getID() const =0;

};

#endif // SENSORITEMINTERFACE_H
