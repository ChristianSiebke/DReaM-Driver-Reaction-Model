/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** \file  TaskBuilder.h
*	\brief This file contains the logic of unchangeable tasks
*	\details bootstrap, common and finalize tasks have their fix order
*           The taskBuilder generates task lists and fill them with necessary tasks
*/
//-----------------------------------------------------------------------------

#include <functional>
#include <list>
#include <vector>

#include "include/spawnPointNetworkInterface.h"
#include "include/worldInterface.h"
#include "eventDetectorNetwork.h"
#include "manipulatorNetwork.h"
#include "observationNetwork.h"
#include "runResult.h"
#include "tasks.h"

namespace openpass::scheduling {
//-----------------------------------------------------------------------------
/** \brief interface of taskBuilder
*
* 	\ingroup OpenPassSlave
*/
//-----------------------------------------------------------------------------

class TaskBuilderInterface
{
public:
    virtual std::list<TaskItem> CreateBootstrapTasks() = 0;
    virtual std::list<TaskItem> CreateSpawningTasks() = 0;
    virtual std::list<TaskItem> CreateSynchronizeTasks() = 0;
    virtual std::list<TaskItem> CreateFinalizeTasks() = 0;
    virtual std::list<TaskItem> CreatePreAgentTasks() = 0;
};

//-----------------------------------------------------------------------------
/** \brief handles fix tasks of scheduler
*
* 	\ingroup OpenPassSlave
*/
//-----------------------------------------------------------------------------

class TaskBuilder : public TaskBuilderInterface
{
private:
    const int &currentTime;
    SimulationSlave::RunResult &runResult;
    const int frameworkUpdateRate;
    const int ScheduleAtEachCycle = 0;

    WorldInterface *const world;
    SimulationSlave::SpawnPointNetworkInterface *const spawnPointNetwork;
    SimulationSlave::ObservationNetworkInterface *const observationNetwork;
    SimulationSlave::EventDetectorNetworkInterface *const eventDetectorNetwork;
    SimulationSlave::ManipulatorNetworkInterface *const manipulatorNetwork;

    std::list<TaskItem> eventDetectorTasks;
    std::list<TaskItem> manipulatorTasks;

    /*!
    * \brief BuildEventDetectorTasks
    *
    * \details In constructor all eventDetectors are read out and
    *           stored in eventDetector Tasks to fill them later in
    *           tasks.
    */
    void BuildEventDetectorTasks();
    /*!
    * \brief BuildManipulatorTasks
    *
    * \details In constructor all manipulators are read out and
    *           stored in eventDetector Tasks to fill them later in
    *           tasks.
    */
    void BuildManipulatorTasks();

public:
    TaskBuilder(const int &currentTime,
                SimulationSlave::RunResult &runResult,
                const int frameworkUpdateRate,
                WorldInterface *const world,
                SimulationSlave::SpawnPointNetworkInterface *const spawnPointNetwork,
                SimulationSlave::ObservationNetworkInterface *const observationNetwork,
                SimulationSlave::EventDetectorNetworkInterface *const eventDetectorNetwork,
                SimulationSlave::ManipulatorNetworkInterface *const manipulatorNetwork);

    virtual ~TaskBuilder() = default;
    /*!
    * \brief CreateBootstrapTasks
    *
    * \details Creates tasks with all tasks of bootstrap phase.
    *          Bootstrap tasks can not be changed.
    *
    * @return	tasks   final tasks of bootstrap phase
    */
    std::list<TaskItem> CreateBootstrapTasks() override;

    /*!
    * \brief CreateCommonTasks
    *
    * \details Creates tasks with all tasks of common phase.
    *          Common tasks can not be changed.
    *
    * @return	tasks   final tasks of common phase
    */
    std::list<TaskItem> CreateSpawningTasks() override;

    std::list<TaskItem> CreatePreAgentTasks() override;

    /*!
    * \brief CreateFinalizeRecurringTasks
    *
    * \details Creates tasks with all tasks of finalizeRecurring phase.
    *          FinalizeRecurring tasks can not be changed.
    *
    * @return	tasks   final tasks of finalizeRecurring phase
    */
    std::list<TaskItem> CreateSynchronizeTasks() override;

    /*!
    * \brief CreateFinalizeTasks
    *
    * \details Creates tasks with all tasks of finalize phase.
    *          Finalize tasks can not be changed.
    *
    * @return	tasks   final tasks of finalize phase
    */
    std::list<TaskItem> CreateFinalizeTasks() override;
};

} // namespace openpass::scheduling
