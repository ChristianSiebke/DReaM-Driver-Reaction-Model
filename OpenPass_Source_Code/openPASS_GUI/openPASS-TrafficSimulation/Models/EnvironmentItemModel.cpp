/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "EnvironmentItemModel.h"

EnvironmentItemModel::EnvironmentItemModel(Value value,
                                           Probability probability,
                                           QObject *parent)
    : EnvironmentItemInterface(parent)
    , probability(probability)
    , value(value)
{

}

void EnvironmentItemModel::setValue(const Value &_value)
{
    value = _value;
    Q_EMIT edited();
}


EnvironmentItemInterface::Value EnvironmentItemModel::getValue() const
{
    return value;
}


void EnvironmentItemModel::setProbability(const Probability &_probability)
{
    probability = _probability;
    Q_EMIT edited();
}


EnvironmentItemInterface::Probability EnvironmentItemModel::getProbability() const
{
    return probability;
}
