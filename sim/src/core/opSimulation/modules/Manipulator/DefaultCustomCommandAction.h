/********************************************************************************
 * Copyright (c) 2020-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

/// @file  DefaultCustomCommandAction.h

#pragma once

#include "ManipulatorCommonBase.h"
#include "common/openScenarioDefinitions.h"
#include "include/agentInterface.h"

/**
 * @brief   Triggers a CustomCommandActionEvent holding the whole command as payload
 * 
 * <CustomCommandAction>arbitrary string to send</CustomCommandAction>
 * 
 * @ingroup Manipulator
*/
class DefaultCustomCommandAction : public ManipulatorCommonBase
{
public:
    DefaultCustomCommandAction(WorldInterface *world,
                               core::EventNetworkInterface *eventNetwork,
                               const CallbackInterface *callbacks,
                               const openScenario::CustomCommandAction action,
                               const std::string &eventName);

    virtual void Trigger(int time) override;

private:
    EventContainer GetEvents() override;
    std::string command;
};
