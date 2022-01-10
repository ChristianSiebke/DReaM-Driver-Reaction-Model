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
//! @file  EnvironmentItemModel.h
//! @ingroup trafficSimulationPlugin
//! @brief The EnvironmentItemModel class implements the functionality of the
//!        EnvironmentItemInterface class.
//-----------------------------------------------------------------------------

#ifndef ENVIRONMENTITEMMODEL_H
#define ENVIRONMENTITEMMODEL_H

#include "openPASS-TrafficSimulation/EnvironmentItemInterface.h"

//-----------------------------------------------------------------------------
//! @brief The EnvironmentItemModel class implements the functionality of the
//!        EnvironmentItemInterface class.
//-----------------------------------------------------------------------------
class EnvironmentItemModel : public EnvironmentItemInterface
{
    Q_OBJECT

public:
    explicit EnvironmentItemModel(Value value,
                                  Probability probability,
                                  QObject *parent = nullptr);

    ~EnvironmentItemModel() = default;

public:
    virtual void setValue(const Value &_value) override;
    virtual Value getValue() const override;

public:
    virtual void setProbability(const Probability &_probability) override;
    virtual Probability getProbability() const override;

private:
    Probability probability; //!< the current selection probability
    Value value;             //!< the current value
};

#endif // ENVIRONMENTITEMMODEL_H
