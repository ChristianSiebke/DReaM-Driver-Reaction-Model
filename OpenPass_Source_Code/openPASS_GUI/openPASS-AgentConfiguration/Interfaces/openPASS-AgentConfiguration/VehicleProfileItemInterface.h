/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef VEHICLEPROFILEITEMINTERFACE_H
#define VEHICLEPROFILEITEMINTERFACE_H

#include "openPASS-AgentConfiguration/VehicleComponentMapInterface.h"
#include "openPASS-AgentConfiguration/SensorMapInterface.h"

#include <QObject>
#include <QMap>

class VehicleProfileItemInterface : public QObject
{
    Q_OBJECT

public:
    using Name = QString;
    using Probability = double;

    using Model = QString;
    using ModelCatalogue = QString;

    using SystemConfig = QString;
    using SystemID = unsigned int;
    using SystemTitle = QString;
    using AlgoType = QString;

    using System = QPair <SystemID , SystemTitle>;
    using Systems = QMap <System, Probability>;
    using SystemProfiles = QMap <AlgoType, Systems*>;

public:
    using VehicleComponents = VehicleComponentMapInterface;             // interferes with systemConfig
    using Sensors = SensorMapInterface;                                 // interferes with systemConfig

public:
    VehicleProfileItemInterface(QObject * const parent = nullptr) : QObject(parent) {}
    virtual ~VehicleProfileItemInterface() = default;

public:
    virtual Name getName() const = 0;

public:
    virtual ModelCatalogue getModelCatalogue() const = 0;
    virtual void setModelCatalogue(ModelCatalogue const & _modelCatalogue) = 0;

public:
    virtual Model getModel() const = 0;
    virtual void setModel(Model const & _model) = 0;

public:
    virtual SystemConfig getSystemConfig() const = 0;
    virtual void setSystemConfig( SystemConfig const & _systemConfig) = 0;

public:
    virtual SystemProfiles * getSystemProfiles()  const =0;

public:
    virtual Systems * getSystems(AlgoType const &_type) const = 0;

public:
    virtual VehicleComponents * getVehicleComponents() const = 0;           // interferes with systemConfig
    virtual Sensors * getSensors() const = 0;                               // interferes with systemConfig

};

#endif // VEHICLEPROFILEITEMINTERFACE_H
