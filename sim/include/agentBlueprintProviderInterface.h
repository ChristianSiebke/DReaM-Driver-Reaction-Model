/********************************************************************************
 * Copyright (c) 2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  AgentBlueprintProviderInterface.h
//! @brief This file contains the interface of the agentBlueprintProvider to interact
//!        with the framework.
//-----------------------------------------------------------------------------

#pragma once

#include <optional>
#include "common/openScenarioDefinitions.h"

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
    * @param agentProfileName    Name of AgentProfile to sample
    * \param assignedParameters  parameters assigned by a catalog reference if appropriate
    *
    * @return           Sampled AgentBlueprint if successful
    */
    virtual AgentBlueprint SampleAgent(const std::string& agentProfileName, const openScenario::Parameters& assignedParameters) const = 0;
};

