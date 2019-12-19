/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*               2016, 2017, 2018 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  Scheduler.h
//! @brief This file contains the generic schedule managing component
//-----------------------------------------------------------------------------

#pragma once

#include <functional>
#include <memory>
#include "Interfaces/worldInterface.h"
#include "taskBuilder.h"
#include "schedulerTasks.h"

namespace SimulationSlave
{

class RunResult;
class EventNetworkInterface;
class SchedulePolicy;
class SpawnPointNetworkInterface;

//-----------------------------------------------------------------------------
/** \brief execute all tasks for an simulation run
* 	\details The scheduler triggers TaskBuilder to build up common tasks and
*           SchedulerTasks to manage sorting of all tasks. Each timestep all
*           given tasks are executed.
*
* 	\ingroup OpenPassSlave
*/
//-----------------------------------------------------------------------------
class Scheduler
{
public:
    static constexpr bool FAILURE { false };
    static constexpr bool SUCCESS { true };
    static constexpr int FRAMEWORK_UPDATE_RATE { 100 };

    Scheduler(WorldInterface &world,
              SpawnPointNetworkInterface &spawnPointNetwork,
              EventDetectorNetworkInterface &eventDetectorNetwork,
              ManipulatorNetworkInterface &manipulatorNetwork,
              ObservationNetworkInterface &observationNetwork);

    /*!
    * \brief Run
    *
    * \details execute all tasks for one simulation run
    *
    * @param[in]     startTime              simulation start
    * @param[in]     endTime                simulation end
    * @param[out]    runResult              RunResult
    * @param[in,out] EventNetwork           EventNetwork
    * @returns true if simulation ends withuot error
    */
    bool Run(int startTime,
             int endTime,
             RunResult &runResult,
             SimulationSlave::EventNetworkInterface &eventNetwork);

    /*!
    * \brief ScheduleAgentTasks
    *
    * \details schedule all tasks of an new agent
    *           e.g. for respawning
    *
    * @param[in]     taskList current task list
    * @param[in]     Agent    new agent
    */
    void ScheduleAgentTasks(SchedulerTasks& taskList, const Agent& agent);

private:    
    WorldInterface &world;
    SpawnPointNetworkInterface &spawnPointNetwork;
    EventDetectorNetworkInterface &eventDetectorNetwork;
    ManipulatorNetworkInterface &manipulatorNetwork;
    ObservationNetworkInterface &observationNetwork;

    int currentTime {0};

    /*!
    * \brief UpdateAgents
    *
    * \details schedule new agents and remove deleted ones
    *
    * @param[in]     taskList               current task list
    * @param[out]    WorldInterface         world
    */
    void UpdateAgents(SchedulerTasks &taskList, WorldInterface &world);

    /*!
    * \brief ExecuteTasks
    *
    * \details execute function of given task
    *
    *
    * @param[in]     tasks     execute function of given tasks
    * @return                  false, if a task reports error
    */
    template<typename T>
    bool ExecuteTasks(T tasks);
};

} // namespace SimulationSlave


