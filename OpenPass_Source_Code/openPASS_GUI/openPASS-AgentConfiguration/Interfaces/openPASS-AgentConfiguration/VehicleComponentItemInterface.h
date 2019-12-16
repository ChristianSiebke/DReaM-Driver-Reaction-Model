/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef VEHICLECOMPONENTITEMINTERFACE_H
#define VEHICLECOMPONENTITEMINTERFACE_H

#include "openPASS-AgentConfiguration/VehicleComponentProfileItemInterface.h"

#include <QObject>
#include <QMap>

class VehicleComponentItemInterface: public QObject
{

    Q_OBJECT

public:
    using Type = QString;
    using Profile = VehicleComponentProfileItemInterface;
    using Probability = double;

public:
    using Profiles = QMap<Profile *, Probability>;
    using SensorLinks = QMap<unsigned int, QString>;

public:
    VehicleComponentItemInterface(QObject * const parent = nullptr) : QObject(parent){}
    virtual ~VehicleComponentItemInterface() = default;

public:
    virtual Profiles * getProfiles() const = 0;
    virtual SensorLinks * getSensorLinks() const = 0;

public:
    virtual Type getType() const = 0;

};

#endif // VEHICLECOMPONENTITEMINTERFACE_H
