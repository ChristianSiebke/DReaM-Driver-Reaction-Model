/********************************************************************************
 * Copyright (c) 2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  SchedulerTasks.h
//! @brief This file contains the generic schedule managing component
//-----------------------------------------------------------------------------

#pragma once

#include <list>
#include <set>

#include "tasks.h"

namespace core::scheduling {

//-----------------------------------------------------------------------------
/** \brief managing timing of all given tasks
* 	\details The SchedulerTasks class sorts tasks for each phase (bbootstrap,
*           common, nonrecurring, recurring, finalize recurring, finalize)
*           Returns all tasks for given timestamp.
*
* 	\ingroup opSimulation
*/
//-----------------------------------------------------------------------------

class SchedulerTasks
{
public:
    SchedulerTasks(std::vector<TaskItem> bootstrapTasks,
                   std::vector<TaskItem> spawningTasks,
                   std::vector<TaskItem> preAgentTasks,
                   std::vector<TaskItem> synchronizeTasks,
                   std::vector<TaskItem> finalizeTasks,
                   int scheduledTimestampsInterval);

    /*!
    * \brief ScheduleNewRecurringTasks
    *
    * \details insert given tasks to intern recurring tasks
    *
    * @param[in]     list of TaskItems      new tasks
    */
    void ScheduleNewRecurringTasks(std::vector<TaskItem> newTasks);

    /*!
    * \brief ScheduleNewNonRecurringTasks
    *
    * \details insert given tasks to intern nonrecurring tasks
    *
    * @param[in]     list of TaskItems      new tasks
    */
    void ScheduleNewNonRecurringTasks(std::vector<TaskItem> newTasks);

    /*!
    * \brief DeleteAgentTasks
    *
    * \details filter all tasks to remove tasks of given agent id
    *
    * @param[in]     list of int    agent ids to remove from tasks
    */
    void DeleteAgentTasks(std::vector<int> &agents);

    /*!
    * \brief DeleteAgentTasks
    *
    * \details filter all tasks to remove tasks of given agent id
    *
    * @param[in]     int      agentId
    */
    void DeleteAgentTasks(int agentId);

    /*!
    * \brief GetNextTimestamp
    *
    * \details calculates with scheduledTimestamps next timestamp
    *
    * @param[in]     int      timestamp
    * @return    int      next timestamp
    */
    int GetNextTimestamp(int timestamp);

    /*!
    * \brief GetTasks
    *
    * @param[in]     int                timestamp
    * @return    list of TaskItems  all tasks for given timestamp
    */
    std::vector<TaskItem> GetTasks(int timestamp);

    /*!
    * \brief GetCommonTasks
    *
    * @param[in]     int                timestamp
    * @return    list of TaskItems  all common tasks for given timestamp
    */
    std::vector<TaskItem> GetSpawningTasks(int timestamp);

    std::vector<TaskItem> GetPreAgentTasks(int timestamp);

    /*!
    * \brief ConsumeNonRecurringTasks
    *
    * \details calls PullNonRecurringTasks
    *
    * @param[in]     int                timestamp
    * @return    list of TaskItems  all init tasks for given timestamp
    */
    std::vector<TaskItem> ConsumeNonRecurringAgentTasks(int timestamp);

    /*!
    * \brief PullNonRecurringTasks
    *
    * \details clear all init tasks after returning them
    *
    * @param[in]     int                timestamp
    * @param[out]    list of TaskItems  all init tasks for given timestamp
    */
    void PullNonRecurringTasks(int timestamp, std::vector<TaskItem> &currentTasks);

    /*!
    * \brief GetRecurringTasks
    *
    * @param[in]    int                timestamp
    * @return       list of TaskItems  all recurring tasks for given timestamp
    */
    std::vector<TaskItem> GetRecurringAgentTasks(int timestamp);

    std::vector<TaskItem> GetSynchronizeTasks(int timestamp);

    /*!
    * \brief GetBootstrapTasks
    *
    * @param[in]    int                timestamp
    * @return       list of TaskItems  all bootstrap tasks for given timestamp
    */
    std::multiset<TaskItem> GetBootstrapTasks();

    /*!
    * \brief GetFinalizeTasks
    *
    * @param[in]    int                timestamp
    * @return       list of TaskItems  all finalize tasks for given timestamp
    */
    std::multiset<TaskItem> GetFinalizeTasks();

    std::set<int> scheduledTimestamps;

    Tasks bootstrapTasks;
    Tasks spawningTasks;
    Tasks preAgentTasks;
    Tasks nonRecurringAgentTasks;
    Tasks recurringAgentTasks;
    Tasks synchronizeTasks;
    Tasks finalizeTasks;

private:
    /*!
    * \brief CreateNewScheduledTimestamps
    *
    * \details renew scheduled timestamps
    */
    void CreateNewScheduledTimestamps();

    /*!
    * \brief GetTasks
    *
    * @param[in]     int                timestamp
    * @param[in]     tasks              tasks to filter by current timestamp
    * @param[out]    list of TaskItems  filtered tasks
    */
    void GetTasks(int timestamp, std::multiset<TaskItem> &tasks, std::vector<TaskItem> &currentTasks);

    /*!
    * \brief UpdateScheduledTimestamps
    *
    * \details update scheduled timestamps with new schedule times
    *
    * @param[in]     int     cycletime of task
    * @param[in]     int     delay/offset of task
    */
    void UpdateScheduledTimestamps(int cycleTime, int delay);

    /*!
    * \brief UpdateScheduledTimestamps
    *
    * \details call UpdateScheduledTimestamps for each task
    *
    * @param[out]     tasks     new scheduled tasks
    */
    void UpdateScheduledTimestamps(std::multiset<TaskItem> &tasks);

    /*!
    * \brief ScheduleNewTasks
    *
    * \details call UpdateScheduledTimestamps for each task
    *
    * @param[out]     tasks     new scheduled tasks
    */
    void ScheduleNewTasks(Tasks &tasks, std::vector<TaskItem> newTasks);

    /*!
    * \brief ClearNonrecurringTasks
    *
    * \details clears non recurring tasks
    */
    void ClearNonrecurringTasks();

    /*!
    * \brief ExpandUpperBoundary
    *
    * \details if reaching upper limit of currently scheduled timestamps
    *           next timestamps have to be calculated
    *
    * @param[in]     int     ctimestamp
    */
    void ExpandUpperBoundary(int timestamp);

    int scheduledTimestampsInterval;
    int upperBoundOfScheduledTimestamps;
    int lowerBoundOfScheduledTimestamps;
};

} // namespace openpass::scheduling
