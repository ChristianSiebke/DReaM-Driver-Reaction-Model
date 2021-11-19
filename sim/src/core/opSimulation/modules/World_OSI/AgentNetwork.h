/********************************************************************************
 * Copyright (c) 2016-2018 ITK Engineering GmbH
 *               2017-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  AgentNetwork.h
//! @brief This file contains the representation of the agent network during a
//!        simulation run.
//-----------------------------------------------------------------------------


#pragma once

#include <functional>
#include <tuple>
#include <algorithm>
#include <utility>
#include <map>
#include "common/openPassTypes.h"
#include "include/agentInterface.h"
#include "AgentAdapter.h"

class WorldImplementation;

using Publisher = std::function<void(openpass::type::EntityId id, openpass::type::FlatParameterKey key, openpass::type::FlatParameterValue value)>;

/*!
* \brief network of agents
*
* This class stores all agents in a network. It is used to synchronize the update of all
* values of all agents.
*/
class AgentNetwork final
{
public:
    AgentNetwork(WorldImplementation *world, const CallbackInterface *callbacks);
    ~AgentNetwork();

    /*!
     * \brief Add an agent to the agent network
     *
     * All added agents must have unique ids.
     * Otherwise the method throws.
     *
     * \param[in] agent     agent reference
     */
    void AddAgent(AgentInterface *agent);

    /*!
     * \brief Clear
     * Clear map of agent network
     */
    void Clear();

    /*!
     * \brief QueueAgentUpdate
     * This function is used to store operations on the agents in a list.
     * At the end of each time step all queued operations will be executed.
     *
     * \param[in] func      function which is to stored to be executed later
     * \param[in] val       value for the function
     */
    void QueueAgentUpdate(std::function<void()> func);

    /*!
     * \brief QueueAgentRemove
     *
     * This function queues agents in a list that will be removed during the next syncronization.
     * \param agent agent which shall be removed
     */
    void QueueAgentRemove(const AgentInterface *agent);

    /*! Removes an agent from the network
     *
     * \param agent agent which shall be removed
     */
    void RemoveAgent(const AgentInterface *agent);

    /*!
         * \brief Publishes the general observations about current agents
         *
         * \param Publish call
         */
    void PublishGlobalData(Publisher publish);

    /*!
     * \brief SyncGlobalData
     * This function is called after each timestep and executes all update function
     * and removes all agents in the remove list.
     */
    void SyncGlobalData();

    /*!
     * \brief GetAgent
     *
     * Retrieves specific agent by id
     * \param[in] id        Agent id
     * \return              Agent reference
     */
    AgentInterface *GetAgent(int id) const;

    /*!
     * \brief GetAgents
     * Retrieves all agents that currently exist
     *
     * \return              Mapping of ids to agents
     */
    virtual const std::map<int, AgentInterface *> &GetAgents() const;

    /*!
     * \brief GetRemovedAgents
     * Retrieves agents that were removed during the simulation run
     *
     * \return              List of agent references
     */
    virtual const AgentInterfaces &GetRemovedAgents() const;

    /*!
     * \brief GetRemovedAgents
     * Retrieves agents that were removed during the previous timestep and clears the list
     *
     * \return              List of agent references
     */
    virtual const AgentInterfaces GetRemovedAgentsInPreviousTimestep();

protected:
    //-----------------------------------------------------------------------------
    //! Provides callback to LOG() macro
    //!
    //! @param[in]     logLevel    Importance of log
    //! @param[in]     file        Name of file where log is called
    //! @param[in]     line        Line within file where log is called
    //! @param[in]     message     Message to log
    //-----------------------------------------------------------------------------
    void Log(CbkLogLevel logLevel,
             const char *file,
             int line,
             const std::string &message)
    {
        if(callbacks)
        {
            callbacks->Log(logLevel,
                           file,
                           line,
                           message);
        }
    }

private:    
    WorldImplementation *world;
    std::map<int, AgentInterface*> agents;
    AgentInterfaces removedAgents;
    std::vector<std::function<void()>> updateQueue;
    AgentInterfaces removeQueue;
    AgentInterfaces removedAgentsPrevious;

    const CallbackInterface *callbacks;
};


