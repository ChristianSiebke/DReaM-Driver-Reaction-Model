﻿/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

/*!
 * \addtogroup Dynamics_TrajectoryFollower
 * @{
 * \brief makes an agent strictly follow a predefined path
 *
 * The path is defined in a file that is loaded in the importer into a map.
 * The module simply reads the coordinates defined in the map every step and forces the agent to move to that position.
 * The vehicle is in this mode very simplified (there is no driver, no logic or any intelligence). It's basically a "playback" of the trajectory.
 * @} */

#pragma once

#include "trajectoryFollowerCommonBase.h"

struct DynamicsInformation
{
    const RoadPosition previousPosition;
    const Common::Vector2d direction;
    const double deltaHeading;
    const double velocity;
    const double acceleration;
};

/*!
 * \brief Makes an agent strictly follow a predefined path.
 *
 * \ingroup Dynamics_TrajectoryFollower
 */
class RoadCoordinateTrajectoryFollower : public TrajectoryFollowerCommonBase
{
public:
    RoadCoordinateTrajectoryFollower(
            std::string componentName,
            bool isInit,
            int priority,
            int offsetTime,
            int responseTime,
            int cycleTime,
            StochasticsInterface *stochastics,
            WorldInterface *world,
            const ParameterInterface *parameters,
            const std::map<int, ObservationInterface*> *observations,
            const CallbackInterface *callbacks,
            AgentInterface *agent,
            TrajectoryInterface *trajectory,
            SimulationSlave::EventNetworkInterface * const eventNetwork);

    RoadCoordinateTrajectoryFollower(const RoadCoordinateTrajectoryFollower&) = delete;
    RoadCoordinateTrajectoryFollower(RoadCoordinateTrajectoryFollower&&) = delete;
    RoadCoordinateTrajectoryFollower& operator=(const RoadCoordinateTrajectoryFollower&) = delete;
    RoadCoordinateTrajectoryFollower& operator=(RoadCoordinateTrajectoryFollower&&) = delete;
    virtual ~RoadCoordinateTrajectoryFollower() override = default;

    /*!
    * \brief Process data within component.
    *
    * Function is called by framework when the scheduler calls the trigger task
    * of this component
    *
    * @param[in]     time           Current scheduling time
    */
    virtual void CalculateNextTimestep(int time) override;

private:
    void Init();

    void TriggerWithActiveAccelerationInput();
    void TriggerWithInactiveAccelerationInput();

    DynamicsInformation PrepareDynamicsInformationWithoutExternalAccelaration(const RoadPosition previousPosition,
                                                   const RoadPosition nextPosition,
                                                   const double additionalDistance);

    void UpdateDynamics(const DynamicsInformation dynamicsInformation);

    RoadCoordinateTrajectory roadTrajectory;
    const bool isRelativeTrajectory;

    RoadCoordinateTrajectory::iterator previousTrajectoryIterator {};
    RoadCoordinateTrajectory::iterator nextTrajectoryIterator {};

    double percentageTraveledBetweenCoordinates {0};
    RoadPosition lastRoadPosition {};
    RoadPosition startPosition {};
    int startLaneId {};
};


