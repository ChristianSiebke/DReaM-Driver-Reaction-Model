/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  AgentBlueprintProviderInterface.h
//! @brief This file contains the interface of the agentBlueprintProvider to interact
//!        with the framework.
//-----------------------------------------------------------------------------

#pragma once

#include <optional>

class AgentBlueprint;
class AgentBlueprintProviderInterface
{
public:
    virtual ~AgentBlueprintProviderInterface() = default;

    /*!
    * \brief Samples an entire agent
    *
    * \details Samples an entired agent from a given SystemConfig or from a dynamically built agent
    *
    *
    * @param[in]        agentProfileName    Name of AgentProfile to sample
    *
    * @return           Sampled AgentBlueprint if successful
    */
    virtual AgentBlueprint SampleAgent(const std::string& agentProfileName) const = 0;
};

