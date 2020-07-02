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

#include "Common/openScenarioDefinitions.h"
#include "Interfaces/worldInterface.h"

//! This class makes calculations for the OpenScenarioActions module
class OscActionsCalculation
{
public:
    OscActionsCalculation(const WorldInterface* world);

    //! Calculates a trajectory for a lane change with shape "sinusiodal"
    //! The result has constant speed in s
    //!
    //! \param deltaS           length of the trajectory in s
    //! \param deltaT           length of the trajectory in t
    //! \param deltaTime        time of the trajectory
    //! \param timeStep         delta time between two trajectory points
    //! \param startPosition    start point of the trajectory
    //! \param startTime        time of the first trajectory point
    //! \return     sinusiodal lane change trajectory in world coordinates
    openScenario::Trajectory CalculateSinusiodalLaneChange (double deltaS, double deltaT, double deltaTime, double timeStep, GlobalRoadPosition startPosition, double startTime) const;

private:
    const WorldInterface* world;
};
