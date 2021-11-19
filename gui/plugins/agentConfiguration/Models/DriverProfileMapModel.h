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
//! @file  DriverProfileMapModel.h
//! @ingroup agentConfigurationPlugin
//! @brief This class implements the functionality of DriverProfileMapInterface.
//-----------------------------------------------------------------------------

#ifndef DRIVERPROFILEMAPMODEL_H
#define DRIVERPROFILEMAPMODEL_H

#include "openPASS-AgentConfiguration/DriverProfileMapInterface.h"

//-----------------------------------------------------------------------------
//! @brief This class implements the functionality of DriverProfileMapInterface.
//-----------------------------------------------------------------------------
class DriverProfileMapModel : public DriverProfileMapInterface
{
    Q_OBJECT

public:
    DriverProfileMapModel(QObject *const parent = nullptr);
    virtual ~DriverProfileMapModel() override = default;

public:
    virtual DriverProfileMapInterface::Iterator begin() override;
    virtual DriverProfileMapInterface::ConstIterator constBegin() const override;

public:
    virtual DriverProfileMapInterface::Iterator end() override;
    virtual DriverProfileMapInterface::ConstIterator constEnd() const override;

public:
    virtual bool add(DriverProfileMapInterface::Name const &name) override;
    virtual bool add(DriverProfileMapInterface::Name const &name,
                     DriverProfileMapInterface::Item const *const driver) override;

public:
    virtual bool remove(DriverProfileMapInterface::Name const &name) override;

public:
    virtual DriverProfileMapInterface::Item *getItem(DriverProfileMapInterface::Name const &name) const override;

public:
    virtual void setName(DriverProfileMapInterface::Item *const item,
                         DriverProfileMapInterface::Name const &name) override;

    virtual DriverProfileMapInterface::Name getName(DriverProfileMapInterface::Item *const item) const override;

public:
    virtual bool contains(DriverProfileMapInterface::Name const &name) const override;
    virtual int count() const override;

public:
    virtual QStringList keys() const override;
    virtual QList<Item *> values() const override;

private:
    Map driverProfiles; //!< the map (container) for registering driver profiles
};

#endif // DRIVERPROFILEMAPMODEL_H
