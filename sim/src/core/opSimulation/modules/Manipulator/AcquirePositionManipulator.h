/********************************************************************************
 * Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/


#pragma once

#include <src/common/openScenarioDefinitions.h>

#include "ManipulatorCommonBase.h"

class AcquirePositionManipulator : public ManipulatorCommonBase
{
public:
    AcquirePositionManipulator(WorldInterface *world,
                               core::EventNetworkInterface *eventNetwork,
                               const CallbackInterface *callbacks,
                               const std::string &eventName,
                               openScenario::AcquirePositionAction);

    /*!
    * \brief Triggers the functionality of this class
    *
    * \details Trigger gets called each cycle timestep.
    * This function is repsonsible for creating events
    *
    * @param[in]     time    Current time.
    */
    void Trigger(int time) override;

private:
    openScenario::AcquirePositionAction acquirePositionAction;
    EventContainer GetEvents() override;
};

