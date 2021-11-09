/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2021 in-tech GmbH
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
#include "include/dataBufferInterface.h"
#include "include/eventDetectorNetworkInterface.h"
#include "include/manipulatorNetworkInterface.h"
#include "include/observationNetworkInterface.h"
#include "runResult.h"
#include "tasks.h"

namespace core::scheduling {
//-----------------------------------------------------------------------------
/** \brief interface of taskBuilder
*
* 	\ingroup opSimulation
*/
//-----------------------------------------------------------------------------

class TaskBuilderInterface
{
public:
    virtual std::vector<TaskItem> CreateBootstrapTasks() = 0;
    virtual std::vector<TaskItem> CreateSpawningTasks() = 0;
    virtual std::vector<TaskItem> CreateSynchronizeTasks() = 0;
    virtual std::vector<TaskItem> CreateFinalizeTasks() = 0;
    virtual std::vector<TaskItem> CreatePreAgentTasks() = 0;
};

//-----------------------------------------------------------------------------
/** \brief handles fix tasks of scheduler
*
* 	\ingroup opSimulation
*/
//-----------------------------------------------------------------------------

class TaskBuilder : public TaskBuilderInterface
{
private:
    const int &currentTime;
    core::RunResult &runResult;
    const int frameworkUpdateRate;
    const int ScheduleAtEachCycle = 0;

    WorldInterface *const world;
    core::SpawnPointNetworkInterface *const spawnPointNetwork;
    core::ObservationNetworkInterface *const observationNetwork;
    core::EventDetectorNetworkInterface *const eventDetectorNetwork;
    core::ManipulatorNetworkInterface *const manipulatorNetwork;
    DataBufferInterface *const dataInterface;

    std::vector<TaskItem> eventDetectorTasks;
    std::vector<TaskItem> manipulatorTasks;

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
                core::RunResult &runResult,
                const int frameworkUpdateRate,
                WorldInterface *const world,
                core::SpawnPointNetworkInterface *const spawnPointNetwork,
                core::ObservationNetworkInterface *const observationNetwork,
                core::EventDetectorNetworkInterface *const eventDetectorNetwork,
                core::ManipulatorNetworkInterface *const manipulatorNetwork,
                DataBufferInterface *const dataInterface);

    virtual ~TaskBuilder() = default;
    /*!
    * \brief CreateBootstrapTasks
    *
    * \details Creates tasks with all tasks of bootstrap phase.
    *          Bootstrap tasks can not be changed.
    *
    * @return	tasks   final tasks of bootstrap phase
    */
    std::vector<TaskItem> CreateBootstrapTasks() override;

    /*!
    * \brief CreateCommonTasks
    *
    * \details Creates tasks with all tasks of common phase.
    *          Common tasks can not be changed.
    *
    * @return	tasks   final tasks of common phase
    */
    std::vector<TaskItem> CreateSpawningTasks() override;

    std::vector<TaskItem> CreatePreAgentTasks() override;

    /*!
    * \brief CreateFinalizeRecurringTasks
    *
    * \details Creates tasks with all tasks of finalizeRecurring phase.
    *          FinalizeRecurring tasks can not be changed.
    *
    * @return	tasks   final tasks of finalizeRecurring phase
    */
    std::vector<TaskItem> CreateSynchronizeTasks() override;

    /*!
    * \brief CreateFinalizeTasks
    *
    * \details Creates tasks with all tasks of finalize phase.
    *          Finalize tasks can not be changed.
    *
    * @return	tasks   final tasks of finalize phase
    */
    std::vector<TaskItem> CreateFinalizeTasks() override;
};

} // namespace openpass::scheduling
