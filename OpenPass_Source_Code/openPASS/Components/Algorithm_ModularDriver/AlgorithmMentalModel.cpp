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
//! @file  AlgorithmMentalModel.cpp
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

//#include <cassert>
//#include <cmath>
//#include <array>
//#include <limits>
//#include <map>
#include <iostream>
#include "AlgorithmMentalModel.h"

MentalModel::MentalModel(StochasticsInterface *stochastics):
    _stochastic(stochastics)
{}

void MentalModel::SetMentalModel_Input(MentalModel_Input_BU *Input_BU, MentalModel_Input_TD *Input_TD)
{
    MM_Input_BU = Input_BU;
    MM_Input_TD = Input_TD;
}

void MentalModel::GetMentalModel_Output(MentalModel_Output *MM_Output)
{
    MM_Output->Ego = MM_Input_BU->IA_O->Ego;
    Update(*MM_Input_BU->IA_O->SurroundingMovingObjects, *MM_Input_BU->IA_O->EnvironmentInfo, 100);
    MM_Output->SurroundingMovingObjects = &_surrounding_Agents;
    MM_Output->EnvironmentInfo = MM_Input_BU->IA_O->EnvironmentInfo;
}

void MentalModel::Update(const std::list<SurroundingMovingObjectsData> &in_perceptionData,
                         const StaticEnvironmentData &_roadPerceptionData, int in_cycletime)
{    
    std::list<std::unique_ptr<AgentRepresentation>>::const_iterator mentalmodel_agent_iter = _surrounding_Agents.begin();
    while(mentalmodel_agent_iter != _surrounding_Agents.end()) {
        if ((*mentalmodel_agent_iter)->Get_internal_Data().GetState()->laneid == UINT64_MAX) {
         // agent had left the road network
           mentalmodel_agent_iter = _surrounding_Agents.erase(mentalmodel_agent_iter);
           continue;
        }
        (*mentalmodel_agent_iter)->LifeTimeTicker();
        
        if(!Forget( mentalmodel_agent_iter, in_perceptionData))
        {
            (*mentalmodel_agent_iter)->Extrapolate(_roadPerceptionData);
             mentalmodel_agent_iter++;
        }   
    }
    // "perception process"
     for( std::list<SurroundingMovingObjectsData>::const_reverse_iterator element_in_perceptionData = in_perceptionData.rbegin();
          element_in_perceptionData != in_perceptionData.rend(); element_in_perceptionData++)
     {
     std::unique_ptr<AgentRepresentation> ptr_Agent_Representation = std::make_unique<AgentRepresentation>(*element_in_perceptionData,in_cycletime);
    
    //add current fixated agent
     _surrounding_Agents.push_back(std::move( ptr_Agent_Representation));
     }

     Memory_capacity_exceeded();
}

bool MentalModel::Forget(std::list<std::unique_ptr<AgentRepresentation>>::const_iterator &mentalmodel_agent_iter, const std::list<SurroundingMovingObjectsData> &in_perceptionData)
{
    // memory time is exceeded? ("forget process")
    if((*mentalmodel_agent_iter)->Get_LifeTime() > memorytime)
    {
       mentalmodel_agent_iter = _surrounding_Agents.erase(std::find(_surrounding_Agents.begin(),_surrounding_Agents.end(), *mentalmodel_agent_iter));

       return true;
    }

  for( auto element_in_perceptionData : in_perceptionData)
  {

    //check whether an old agent representation of current visual fixated agent exist in Mental Modal Agents ("forget process")
    if((*mentalmodel_agent_iter)->Get_internal_Data().GetState()->id == element_in_perceptionData.GetState()->id)
    {
      // delete old angent representation
     mentalmodel_agent_iter = _surrounding_Agents.erase(std::find(_surrounding_Agents.begin(),_surrounding_Agents.end(), *mentalmodel_agent_iter));

      return true;
    }
  }

  return false;
}

void MentalModel::Memory_capacity_exceeded()
{
    // memory capacity is exceeded? ( "forget process")
    if(_surrounding_Agents.size() > number_of_agent_representations)
    {
        unsigned int number_of_deletions= _surrounding_Agents.size() - number_of_agent_representations ;

        auto range_end = _surrounding_Agents.begin();
        std::advance(range_end,number_of_deletions);

        _surrounding_Agents.erase(_surrounding_Agents.begin(),range_end );
    }
}
