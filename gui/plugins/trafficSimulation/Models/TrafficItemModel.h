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
//! @file  TrafficItemModel.h
//! @ingroup trafficSimulationPlugin
//! @brief The TrafficItemModel class implements functionality of the
//!        TrafficItemInterface class.
//-----------------------------------------------------------------------------
#ifndef TRAFFICITEMMODEL_H
#define TRAFFICITEMMODEL_H

#include "openPASS-TrafficSimulation/TrafficItemInterface.h"

//-----------------------------------------------------------------------------
//! @brief The TrafficItemModel class implements functionality of the
//!        TrafficItemInterface class.
//-----------------------------------------------------------------------------
class TrafficItemModel : public TrafficItemInterface
{
    Q_OBJECT

public:
    explicit TrafficItemModel(QObject *parent = 0);
    ~TrafficItemModel() = default;

public:
    virtual void setValue(Value const &_value) override;
    virtual Value getValue() const override;

public:
    virtual void setProbability(Probability const &_probability) override;
    virtual Probability getProbability() const override;

public:
    Value value;             //!< the current value of the traffic parameter
    Probability probability; //!< the selection probability of the traffic parameter
};

#endif // TRAFFICITEMMODEL_H
