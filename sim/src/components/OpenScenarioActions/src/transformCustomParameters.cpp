/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/
#include "transformCustomParameters.h"

namespace openScenario::transformation {

std::shared_ptr<CustomParametersSignal> CustomParameters::ConvertToSignal(const openpass::events::CustomParametersEvent &event, WorldInterface *, AgentInterface *, int)
{
    return std::make_shared<CustomParametersSignal>(ComponentState::Acting, event.parameters);
}

} // namespace openScenario::transformation
