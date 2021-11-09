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
#include <map>
#include <string>
#include "include/agentTypeInterface.h"
#include "include/parameterInterface.h"

class SystemConfigInterface
{
public:
    SystemConfigInterface() = default;
    virtual ~SystemConfigInterface() = default;

    virtual std::map<int, std::shared_ptr< core::AgentTypeInterface>>& GetSystems() = 0;
    virtual void SetSystems(std::map<int, std::shared_ptr< core::AgentTypeInterface>> systems) = 0;
    virtual void AddModelParameters(std::shared_ptr<ParameterInterface> modelParameters) = 0;
};

