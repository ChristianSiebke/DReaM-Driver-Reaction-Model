/********************************************************************************
 * Copyright (c) 2017-2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "pcm_agent.h"

PCM_Agent::PCM_Agent(int id):
    id(id)
{}

int PCM_Agent::GetId() const
{
    return id;
}
