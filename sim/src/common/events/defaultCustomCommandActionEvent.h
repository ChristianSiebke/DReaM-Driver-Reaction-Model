/********************************************************************************
 * Copyright (c) 2020-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include "common/events/basicEvent.h"
#include "include/signalInterface.h"

namespace openpass::events {

/// @brief   Relay for information from the DefaultCustomCommandAction implementation
/// @ingroup Event
class DefaultCustomCommandActionEvent : public OpenScenarioEvent
{
public:
    static constexpr char TOPIC[]{"OpenSCENARIO/UserDefinedAction/CustomCommandAction/DefaultCustomCommandAction"};

    DefaultCustomCommandActionEvent(int time, const std::string eventName, std::string source, const int agentId, std::string command) :
        OpenScenarioEvent{time, eventName, source, {}, {{agentId}}},
        command{std::move(command)}
    {
    }

    const std::string command;
};

} // namespace openpass::events
