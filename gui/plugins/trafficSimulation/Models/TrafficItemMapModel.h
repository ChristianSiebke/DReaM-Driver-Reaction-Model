/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  TrafficItemMapModel.h
//! @ingroup trafficSimulationPlugin
//! @brief The TrafficItemMapModel class implements functionality of the
//!       TrafficItemMapInterface class.
//-----------------------------------------------------------------------------

#ifndef TRAFFICITEMMAPMODEL_H
#define TRAFFICITEMMAPMODEL_H

#include "TrafficItemModel.h"
#include "openPASS-TrafficSimulation/TrafficItemMapInterface.h"

//-----------------------------------------------------------------------------
//! @brief The TrafficItemMapModel class implements functionality of the
//!       TrafficItemMapInterface class.
//-----------------------------------------------------------------------------
class TrafficItemMapModel : public TrafficItemMapInterface
{
    Q_OBJECT

public:
    explicit TrafficItemMapModel(QObject *parent = 0);
    ~TrafficItemMapModel() = default;

public:
    virtual TrafficItemMapInterface::Iterator begin() override;
    virtual TrafficItemMapInterface::ConstIterator constBegin() const override;

public:
    virtual TrafficItemMapInterface::Iterator end() override;
    virtual TrafficItemMapInterface::ConstIterator constEnd() const override;

public:
    virtual bool add(TrafficItemMapInterface::ID const &id,
                     TrafficItemInterface::Probability const &probability,
                     TrafficItemInterface::Value const &value) override;

    virtual bool add(TrafficItemMapInterface::Item *const item) override;

public:
    virtual bool remove(TrafficItemMapInterface::ID const &id) override;
    virtual bool remove(TrafficItemMapInterface::Item *const item) override;

public:
    virtual TrafficItemMapInterface::Item *getItem(TrafficItemMapInterface::ID const &id) const override;
    virtual TrafficItemMapInterface::ID getID(TrafficItemMapInterface::Item *const item) const override;

public:
    virtual TrafficItemMapInterface::ID generateID() const;
    virtual bool contains(TrafficItemMapInterface::ID const &id) const override;
    virtual int count() const override;

public:
    virtual void clear() override;

public:
    virtual QList<TrafficItemMapInterface::ID> keys() const override;
    virtual QList<TrafficItemMapInterface::Item *> values() const override;

public:
    TrafficItemMapInterface::Map map; //!< the map of traffic items
};

#endif // TRAFFICITEMMAPMODEL_H
