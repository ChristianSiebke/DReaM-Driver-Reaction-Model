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

#include "Common/Events/laneChangeEvent.h"
#include "Common/trajectorySignal.h"
#include "transformerBase.h"

namespace openScenario::transformation {

struct LaneChange : public TransformerBase<LaneChange, TrajectorySignal, openpass::events::LaneChangeEvent>
{
    static std::shared_ptr<TrajectorySignal> ConvertToSignal(const openpass::events::LaneChangeEvent &event,
                                                             WorldInterface *world,
                                                             AgentInterface *agent,
                                                             int cycleTime);
};

} // namespace openScenario::transformation
