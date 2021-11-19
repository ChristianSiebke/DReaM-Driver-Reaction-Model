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
//! @file  VehicleProfileMapModel.h
//! @ingroup agentConfigurationPlugin
//! @brief This class implements the functionality of VehicleProfileMapInterface.
//-----------------------------------------------------------------------------

#ifndef VEHICLEPROFILEMAPMODEL_H
#define VEHICLEPROFILEMAPMODEL_H

#include "openPASS-AgentConfiguration/VehicleProfileMapInterface.h"

//-----------------------------------------------------------------------------
//! @brief This class implements the functionality of VehicleProfileMapInterface.
//-----------------------------------------------------------------------------
class VehicleProfileMapModel : public VehicleProfileMapInterface
{
    Q_OBJECT
public:
    explicit VehicleProfileMapModel(QObject *parent = nullptr);
    virtual ~VehicleProfileMapModel() override = default;

public:
    virtual VehicleProfileMapInterface::Iterator begin() override;
    virtual VehicleProfileMapInterface::ConstIterator constBegin() const override;

public:
    virtual VehicleProfileMapInterface::Iterator end() override;
    virtual VehicleProfileMapInterface::ConstIterator constEnd() const override;

public:
    virtual bool add(VehicleProfileMapInterface::Name const &name) override;
    virtual bool add(VehicleProfileMapInterface::Name const &name,
                     VehicleProfileMapInterface::Item const *const item) override;

public:
    virtual bool remove(VehicleProfileMapInterface::Name const &name) override;

public:
    virtual VehicleProfileMapInterface::Item *getItem(VehicleProfileMapInterface::Name const &name) const override;

public:
    virtual void setName(VehicleProfileMapInterface::Item *const item,
                         VehicleProfileMapInterface::Name const &name) override;

    virtual VehicleProfileMapInterface::Name getName(VehicleProfileMapInterface::Item *const item) const override;

public:
    virtual bool contains(VehicleProfileMapInterface::Name const &name) const override;
    virtual int count() const override;

public:
    virtual QStringList keys() const override;
    virtual QList<Item *> values() const override;

private:
    VehicleProfileMapInterface::Map profiles;
};

#endif // VEHICLEPROFILEMAPMODEL_H
