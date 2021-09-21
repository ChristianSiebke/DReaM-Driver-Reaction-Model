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

class ReachPositionCondition : public ConditionCommonBase
{
public:
    ReachPositionCondition(const ParameterInterface &parameters);
    virtual ~ReachPositionCondition() = default;

    bool IsMet(const AgentInterface *agent) const;

    ReachPositionCondition() = delete;
    ReachPositionCondition(const ReachPositionCondition&) = default;
    ReachPositionCondition(ReachPositionCondition&&) = delete;
    ReachPositionCondition& operator=(const ReachPositionCondition&) = delete;
    ReachPositionCondition& operator=(ReachPositionCondition&&) = delete;

private:
    double tolerance;
    double targetSCoordinate;
    std::string targetRoadId;
};
