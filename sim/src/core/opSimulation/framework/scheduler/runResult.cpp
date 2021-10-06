/********************************************************************************
 * Copyright (c) 2016-2018 ITK Engineering GmbH
 *               2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include <algorithm>
#include <utility>

#include "common/log.h"
#include "runResult.h"
//-----------------------------------------------------------------------------
/** \file  RunResult.cpp */
//-----------------------------------------------------------------------------

namespace core
{

void RunResult::AddCollisionId(const int agentId)
{
    //SetCollision
    result |= maskCollision;

    //Insert new collision ids
    if(std::find(collisionIds.begin(), collisionIds.end(), agentId) == collisionIds.end())
    {
        collisionIds.push_back(agentId);
    }
}

} // namespace core
