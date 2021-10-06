/********************************************************************************
 * Copyright (c) 2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
/** \file  AgentParser.h
*	\brief The Parser generates all component tasks for an given agent
*	\details AgentParser generate Trigger and Update tasks.
*            All init tasks are stored to nonRecurring list and all recurring
*            tasks to Recurring list.
*/
//-----------------------------------------------------------------------------

#pragma once

#include <functional>
#include <list>

#include "agent.h"
#include "tasks.h"

namespace core::scheduling {

//-----------------------------------------------------------------------------
/** \brief convert agent to taskItems for Scheduling
*
* 	\ingroup opSimulation
*/
//-----------------------------------------------------------------------------

class AgentParser
{
private:
    std::vector<TaskItem> nonRecurringTasks;
    std::vector<TaskItem> recurringTasks;

    const int &currentTime;

public:
    AgentParser(const int &currentTime);

    /*!
    * \brief Parse
    *
    * \details function is called when spawning agent and provides tasks for each component of the agent
    *
    *
    * @param[in]     Agent    new agent
    */
    void Parse(const core::Agent &agent);

    std::vector<TaskItem> GetNonRecurringTasks();
    std::vector<TaskItem> GetRecurringTasks();
};

} // namespace openpass::scheduling
