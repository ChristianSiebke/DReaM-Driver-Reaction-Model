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

#include "common/events/speedActionEvent.h"
#include "common/speedActionSignal.h"
#include "transformerBase.h"

namespace openScenario::transformation {

struct SpeedAction : public TransformerBase<SpeedAction, SpeedActionSignal, openpass::events::SpeedActionEvent>
{
    static std::shared_ptr<SpeedActionSignal> ConvertToSignal(const openpass::events::SpeedActionEvent &event,
                                                              WorldInterface *world,
                                                              AgentInterface *agent,
                                                              int cycleTime);
};

} // namespace openScenario::transformation
