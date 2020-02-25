/******************************************************************************
* Copyright (c) 2019 TU Dresden
* Copyright (c) 2019 AMFD GmbH
*
* This program and the accompanying materials are made available under the
* terms of the Eclipse Public License 2.0 which is available at
* https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*****************************************************************************/

//-----------------------------------------------------------------------------
//! @file  AlgorithmMentalModel.h
//! @author  Christian Siebke
//! @author  Konstantin Blenz
//! @author  Christian Gärber
//! @author  Vincent   Adam
//! @date    Tue, 03.12.2019
//! @brief provide data and calculate driver's extrapolation of the environment.
//!
//! This class contains the data calculations.
//! Singleton such that data calculations is generated only once and not seperately
//! for each instance.
//!
//-----------------------------------------------------------------------------

#pragma once

//#include <vector>
#include <ContainerStructures.h>

//! \addtogroup Algorithm_ModularDriver
//! \ingroup AlgorithmMentalModel

class MentalModel
{
public:

    MentalModel(StochasticsInterface *stochastics); // avoid access of constructor of singleton outside clas

    void SetMentalModel_Input(MentalModel_Input_BU *Input_BU, MentalModel_Input_TD *Input_TD);
    void GetMentalModel_Output(MentalModel_Output *MM_Output);

    void Update(const std::list<SurroundingMovingObjectsData> &_inperceptionData, const StaticEnvironmentData &_roadPerceptionData,int in_cycletime);

    /*!
    * \brief Identify agents which need to be updated.
    *
    * Identify agents who wasn't fixated since a choosen time.
    *
    * @param[in]   time              time since Agents werent't visually fixed.
    * @param[out]  Agents            agents which need an visual update
    */

    std::vector<int>  Agents_which_need_update(double time);

    const std::list <std::unique_ptr <AgentRepresentation>> &GetSurroundingAgents() const
    {
         return   _surrounding_Agents;
    }


private:

    MentalModel_Input_BU *MM_Input_BU;
    MentalModel_Input_TD *MM_Input_TD;
    StochasticsInterface *_stochastic;

    /*!
    \brief "forget-process of the human"
     *
     * agents that have not been fixated since a long time will be deleted (forgotten)
     * memory time is exceeded
     *
     * @param[in] mentalmodel_agent  iterator of the MentalModal_Agents
     * @param[in] in_perceptionData   perceived information of fixed agent
     */

    bool Forget(std::list<std::unique_ptr<AgentRepresentation>>::const_iterator &mentalmodel_agent_iter, const std::list<SurroundingMovingObjectsData> &in_perceptionData);

     /*!
     * \brief "human memory capacity "
     * memory capacity is exeded
     *
     * If the agent capacity is exeeded the oldest agents are deleted.
     *
     */
     void Memory_capacity_exceeded ();

 // mental model memory

     //! contains all  surrouded representation agents of the MentalModel
     std::list <std::unique_ptr<AgentRepresentation>> _surrounding_Agents;


 // memory variables
     //! maximum memory capacity of agent representations
     unsigned int number_of_agent_representations = 10;

     //! time, till agent representation will be forgotten, not yet representative [ in ms]
     int memorytime  = 10000;  //ms



};


