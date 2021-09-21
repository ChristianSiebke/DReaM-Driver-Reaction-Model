/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** \file  EventNetworkInterface.h
*	\brief This file provides the interface for the EventNetwork
*/
//-----------------------------------------------------------------------------

#pragma once

#include "include/eventInterface.h"
#include "include/runResultInterface.h"

using SharedEvent = std::shared_ptr<EventInterface>;
using EventContainer = std::vector<SharedEvent>;

namespace core {

//-----------------------------------------------------------------------------
/** \brief This class provides the interface for the EventNetwork
*
* 	\ingroup EventNetwork */
//-----------------------------------------------------------------------------
class EventNetworkInterface
{
public:
    virtual ~EventNetworkInterface() = default;

    virtual EventContainer GetEvents(const EventDefinitions::EventCategory eventCategory) = 0;
    virtual void InsertEvent(SharedEvent event) = 0;
    virtual void Clear() = 0;
    virtual void AddCollision(const int agentId) = 0;
    virtual void Initialize(RunResultInterface *runResult) = 0;
    virtual void InsertTrigger(const std::string &identifier, std::unique_ptr<EventInterface> event) = 0;
    virtual std::vector<EventInterface const *> GetTrigger(const std::string &identifier) const = 0;
};

} //namespace core
