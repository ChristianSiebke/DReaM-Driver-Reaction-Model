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
//! @file    agent_representation.h
//! @author  Christian Siebke
//! @author  Konstantin Blenz
//! @date    Tue, 03.12.2019
//! @brief provide container structure of agents seen by the driver
//!
//-----------------------------------------------------------------------------
//! \addtogroup Algorithm_ModularDriver
//-----------------------------------------------------------------------------

# pragma once
 
#include "ContainerStructures.h"
 
class AgentRepresentation
{
public:
 
    AgentRepresentation(SurroundingMovingObjectsData in_perceptionData, int in_cycletime) :
       internal_Data(*in_perceptionData.GetVehicleType(),
                     *in_perceptionData.GetState(),
                     *in_perceptionData.GetProperties()),
                     cycletime{in_cycletime}
    {
        lifetime = 0 ;
    }
 
    AgentRepresentation(const AgentRepresentation&) = delete;
    AgentRepresentation(AgentRepresentation&&) = delete;
    AgentRepresentation& operator=(const AgentRepresentation&) = delete;
    AgentRepresentation& operator=(AgentRepresentation&&) = delete;
    ~AgentRepresentation() = default;
 
    //! Extrapolate the internal_Data of an agent if he is not fixated
 
    void Extrapolate(const StaticEnvironmentData & _roadPerceptionData);
 
    void PositionAgentOnLanes(int laneid, std::map<int, const std::vector<MentalModelLane>*>*Lanes, const StaticEnvironmentData &_roadPerceptionData);

    void ExtrapolateKinematic();

    void ExtrapolateAlongLane(const std::vector<MentalModelLane> *lane);
    
    void LinkPosToRoadPos(int laneid, std::map<int, const std::vector<MentalModelLane*>> lanes);

    double LinearInterpolation(double a, double sa, double b, double sb, double s)
    {
        double div = (b-a)/(sb-sa);
        double c = div*s + (-(div)*sa + a);
        return c;
    }

    //! increase the life time of the agent representation by one cycletime
    void LifeTimeTicker() {
       lifetime += Get_cycletime();
    }
 
    //*********Get-functions****//
 
    SurroundingMovingObjectsData Get_internal_Data() const {
        return internal_Data;
    }
 
    int Get_cycletime() {
        return cycletime;
    }
 
    int Get_LifeTime() {
        return lifetime ;
    }

 
private:  
    //! the internal information of the agent representation
    SurroundingMovingObjectsData internal_Data;
 
    //!  represents the time since the agent representation is in the MentalModel (since the agent_representation exists)
    int lifetime;
 
    //! cycletime of the component MentalModel
    int cycletime;
 
};
