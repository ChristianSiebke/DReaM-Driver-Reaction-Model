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

#include <string>
#include "include/agentInterface.h"

enum class ConditionCategory
{
    Undefined = 0,
    ByEntity,
    ByValue
};

enum class Rule
{
    LessThan = 0,
    EqualTo,
    GreaterThan
};

class ConditionCommonBase
{
public:
    ConditionCommonBase(const ConditionCategory conditionCategory):
        conditionCategory(conditionCategory){}

    ConditionCommonBase(const ConditionCommonBase&) = default;
    ConditionCommonBase(ConditionCommonBase&&) = delete;
    ConditionCommonBase& operator=(const ConditionCommonBase&) = delete;
    ConditionCommonBase& operator=(ConditionCommonBase&&) = delete;

    virtual ~ConditionCommonBase() = default;

    ConditionCategory GetConditionCategory()
    {
        return conditionCategory;
    }

private:
    const ConditionCategory conditionCategory;
};
