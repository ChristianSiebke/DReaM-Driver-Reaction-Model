/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef AGENTITEMINTERFACE_H
#define AGENTITEMINTERFACE_H

#include <QObject>
#include <QMap>

class AgentItemInterface : public QObject
{
    Q_OBJECT

public:
    using Name = QString;
    using Type = QString;
    using Probability = double;

public:
    using SystemConfig = QString;
    using SystemID = unsigned int;
    using SystemTitle = QString;
    using System = QPair<SystemID, SystemTitle>;
    using VehicleModelCatalogue = QString;
    using VehicleModel = QString;

public:
    using VehicleProfile = QString;
    using VehicleProfiles = QMap<VehicleProfile, Probability>;

public:
    using DriverProfile = QString;
    using DriverProfiles = QMap<DriverProfile, Probability>;

public:
    explicit AgentItemInterface (QObject * parent = nullptr)
                               : QObject(parent){}
    virtual ~AgentItemInterface() = default;

public:
    virtual Name getName() const = 0;

public:
    virtual Type getType() const = 0;
    virtual void setType(const Type &_type) = 0;

public:
    virtual VehicleProfiles * getVehicleProfiles() const = 0;
    virtual DriverProfiles * getDriverProfiles() const = 0;

public:
    virtual VehicleModel getVehicleModel() const = 0;
    virtual void setVehicleModel(VehicleModel const & _vehicleModel) = 0;

public:
    virtual VehicleModelCatalogue getVehicleModelCatalogue() const = 0;
    virtual void setVehicleModelCatalogue(VehicleModelCatalogue const & _vehicleModelCatalogue) = 0;

public:
    virtual SystemID getSystemID() const = 0;
    virtual void setSystemID(SystemID const & _systemID) = 0;

public:
    virtual SystemTitle getSystemTitle() const = 0;
    virtual void setSystemTitle(SystemTitle const & _systemTitle) = 0;


public:
    virtual SystemConfig getSystemConfig() const = 0;
    virtual void setSystemConfig(SystemConfig const & _systemConfig) = 0;

public:
    virtual System * getSystem() = 0;
};

#endif // AGENTITEMINTERFACE_H
