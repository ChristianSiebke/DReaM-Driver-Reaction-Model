/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include "common/events/trajectoryEvent.h"
#include "common/trajectorySignal.h"
#include "transformerBase.h"

namespace openScenario::transformation {

struct Trajectory : public TransformerBase<Trajectory, TrajectorySignal, openpass::events::TrajectoryEvent>
{
    static std::shared_ptr<TrajectorySignal> ConvertToSignal(const openpass::events::TrajectoryEvent &event, WorldInterface *, AgentInterface *, int)
    {
        return std::make_shared<TrajectorySignal>(ComponentState::Acting, event.trajectory);
    }
};

} // namespace openScenario::transformation
