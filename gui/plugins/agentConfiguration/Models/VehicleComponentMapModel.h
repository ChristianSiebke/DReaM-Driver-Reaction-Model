/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  VehicleComponentMapModel.h
//! @ingroup agentConfigurationPlugin
//! @brief This class implements the functionality of VehicleComponentMapInterface.
//-----------------------------------------------------------------------------

#ifndef VEHICLECOMPONENTMAPMODEL_H
#define VEHICLECOMPONENTMAPMODEL_H

#include "openPASS-AgentConfiguration/VehicleComponentMapInterface.h"

//-----------------------------------------------------------------------------
//! @brief This class implements the functionality of VehicleComponentMapInterface.
//-----------------------------------------------------------------------------
class VehicleComponentMapModel : public VehicleComponentMapInterface
{
    Q_OBJECT

public:
    VehicleComponentMapModel(QObject *parent = nullptr);
    ~VehicleComponentMapModel() = default;

public:
    virtual VehicleComponentMapInterface::Iterator begin() override;
    virtual VehicleComponentMapInterface::ConstIterator constBegin() const override;

public:
    virtual VehicleComponentMapInterface::Iterator end() override;
    virtual VehicleComponentMapInterface::ConstIterator constEnd() const override;

public:
    virtual bool add(VehicleComponentMapInterface::Type const &type) override;

public:
    virtual bool remove(VehicleComponentMapInterface::Type const &type) override;

public:
    virtual VehicleComponentMapInterface::Item *getItem(VehicleComponentMapInterface::Type const &type) const override;

public:
    virtual void setType(VehicleComponentMapInterface::Item *const item,
                         VehicleComponentMapInterface::Type const &type) override;

    virtual VehicleComponentMapInterface::Type getType(VehicleComponentMapInterface::Item *const item) const override;

public:
    virtual bool contains(VehicleComponentMapInterface::Type const &type) const override;
    virtual int count() const override;

public:
    virtual void clear() override;

public:
    virtual QStringList keys() const override;
    virtual QList<Item *> values() const override;

private:
    VehicleComponentMapInterface::Map components; //!< the map (container) for registering vehicle components
};

#endif // VEHICLECOMPONENTMAPMODEL_H
