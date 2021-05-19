/*******************************************************************************
* Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include <src/common/acquirePositionSignal.h>
#include <src/common/events/acquirePositionEvent.h>

#include "transformerBase.h"

namespace openScenario::transformation {

struct AcquirePosition : public TransformerBase<AcquirePosition, AcquirePositionSignal, openpass::events::AcquirePositionEvent>
{
    static std::shared_ptr<AcquirePositionSignal> ConvertToSignal(const openpass::events::AcquirePositionEvent &event,
                                                                  WorldInterface *world,
                                                                  AgentInterface *agent,
                                                                  int cycleTime);
};

} // namespace openScenario::transformation
