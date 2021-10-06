/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "TrafficItemModel.h"

TrafficItemModel::TrafficItemModel(QObject *parent) :
    TrafficItemInterface(parent)
{
}

void TrafficItemModel::setValue(const Value &_value)
{
    value = _value;

    Q_EMIT edited();
}

TrafficItemInterface::Value TrafficItemModel::getValue() const
{
    return value;
}

void TrafficItemModel::setProbability(const Probability &_probability)
{
    probability = _probability;

    Q_EMIT edited();
}

TrafficItemInterface::Probability TrafficItemModel::getProbability() const
{
    return probability;
}
