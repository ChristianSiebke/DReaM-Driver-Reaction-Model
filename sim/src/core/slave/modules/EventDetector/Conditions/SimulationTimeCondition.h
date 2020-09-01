/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/
#pragma once

#include "ConditionCommonBase.h"
#include "include/parameterInterface.h"

class SimulationTimeCondition : public ConditionCommonBase
{
public:
    SimulationTimeCondition(const ParameterInterface &parameters);
    virtual ~SimulationTimeCondition() override = default;

    bool IsMet(const int value) const;

    SimulationTimeCondition() = delete;
    SimulationTimeCondition(const SimulationTimeCondition&) = default;
    SimulationTimeCondition(SimulationTimeCondition&&) = delete;
    SimulationTimeCondition& operator=(const SimulationTimeCondition&) = delete;
    SimulationTimeCondition& operator=(SimulationTimeCondition&&) = delete;

private:
    Rule rule;
    int targetValue;
};
