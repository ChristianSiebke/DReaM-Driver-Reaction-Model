/*******************************************************************************
* Copyright (c) 2020, 2021 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include "common/events/defaultCustomCommandActionEvent.h"
#include "common/stringSignal.h"
#include "transformerBase.h"

namespace openScenario::transformation {

struct DefaultCustomCommandAction : public TransformerBase<DefaultCustomCommandAction, StringSignal, openpass::events::DefaultCustomCommandActionEvent>
{
    static std::shared_ptr<StringSignal> ConvertToSignal(const openpass::events::DefaultCustomCommandActionEvent &event,
                                                         WorldInterface *,
                                                         AgentInterface *,
                                                         int);
};

} // namespace openScenario::transformation
