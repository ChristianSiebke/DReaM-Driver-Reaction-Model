/*******************************************************************************
* Copyright (c) 2020, 2021 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "transformDefaultCustomCommandAction.h"

namespace openScenario::transformation {

std::shared_ptr<StringSignal> DefaultCustomCommandAction::ConvertToSignal(const openpass::events::DefaultCustomCommandActionEvent &event, WorldInterface *, AgentInterface *, int)
{
    return std::make_shared<StringSignal>(ComponentState::Acting, event.command);
}

} // namespace openScenario::transformation
