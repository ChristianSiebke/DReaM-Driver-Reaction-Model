/********************************************************************************
 * Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "transformAcquirePosition.h"

std::shared_ptr<AcquirePositionSignal>
openScenario::transformation::AcquirePosition::ConvertToSignal(const openpass::events::AcquirePositionEvent &event,
                                                               WorldInterface *world,
                                                               AgentInterface *agent,
                                                               int cycleTime)
{
    return std::make_shared<AcquirePositionSignal>(ComponentState::Acting, event.position);
}
