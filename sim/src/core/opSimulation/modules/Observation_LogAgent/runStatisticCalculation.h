/********************************************************************************
 * Copyright (c) 2020 ITK Engineering GmbH
 *               2019-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include "runStatistic.h"

class WorldInterface;

/*!
 * \brief The RunStatisticCalculation class provides helper functions for run statistic analysis
 */
class RunStatisticCalculation
{
public:
    /*!
     * \brief Determines whether the ego had a collision and sets the related flag in the RunStatistic
     *
     * \param[out] runStatistic      Result is stored here
     * \param[in]  runResult         Structure to retrieve collisions from
     * \param[in]  world             Pointer to the world
     */
    static void DetermineEgoCollision(RunStatistic& runStatistic, const RunResultInterface& runResult, WorldInterface* world);
};
