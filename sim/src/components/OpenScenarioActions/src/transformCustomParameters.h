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

#include "common/events/customParametersEvent.h"
#include "common/customParametersSignal.h"
#include "transformerBase.h"

namespace openScenario::transformation {

struct CustomParameters : public TransformerBase<CustomParameters, CustomParametersSignal, openpass::events::CustomParametersEvent>
{
    static std::shared_ptr<CustomParametersSignal> ConvertToSignal(const openpass::events::CustomParametersEvent &event, WorldInterface *, AgentInterface *, int);
    static inline bool registered = ActionTransformRepository::Register(Transform);
};

} // namespace openScenario::transformation
