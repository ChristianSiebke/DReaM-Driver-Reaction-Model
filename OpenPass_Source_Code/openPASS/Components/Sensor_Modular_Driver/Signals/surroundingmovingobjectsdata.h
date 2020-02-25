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
//! @file  staticenvironmentdata.h
//! @author  Konstantin Blenz
//! @date    Tue, 03.12.2019
//! @brief provide Container Structures for the driver's cognition of the moving-environment
//!
//-----------------------------------------------------------------------------
//! \addtogroup Algorithm_ModularDriver
//-----------------------------------------------------------------------------

#pragma once

#include <vector>
#include <string>
#include "globalDefinitions.h"
#include "egodata.h"

struct Properties //dimension of the object related to the sensor coordinate system
{
    double lx {-999};
    double ly {-999};
    double lz {-999};
    double weight {-999};
    double heightCOG {-999};
    double wheelbase {-999};
    double momentInertiaRoll {-999};
    double momentInertiaPitch {-999};
    double momentInertiaYaw {-999};
    double frictionCoeff {-999};
    double trackWidth {-999};
    double distanceReftoLeadingEdge {-999};
};


enum RelationType
{
    NONE = 0,
    Leader = 1,
    Follower = 2,
    LeaderRight = 3,
    LeaderLeft = 4,
    FollowerRight = 5,
    FollowerLeft = 6
};

struct State_Moving : State
{
    double velocity_long {-999};
    double velocity_lat {-999};
    double acceleration_long {-999};
    double acceleration_lat = 0;
};

class SurroundingMovingObjectsData
{
public:
    SurroundingMovingObjectsData(AgentVehicleType VehicleType,
                                 State_Moving State,
                                 Properties Properties):
        VehicleType(VehicleType),
        State(State),
        Properties(Properties)
    {
    }

    std::string GetName() const
    {
      return  "SurroundingMovingObjectsData";
    }
    AgentVehicleType *GetVehicleType()
    {
        return &VehicleType;
    }
    State_Moving *GetState()
    {
        return &State;
    }
    Properties *GetProperties()
    {
        return &Properties;
    }
    void SetVehicleType(AgentVehicleType VehicleType)
    {
        this->VehicleType = VehicleType;
    }
    void SetState(State_Moving State)
    {
        this->State = State;
    }
    void SetProperties(Properties Properties)
    {
        this->Properties = Properties;
    }
    void SetDistanceToEgo(double DistanceToEgo)
    {
        this->DistanceToEgo = DistanceToEgo;
    }
    double const *GetDistanceToEgo()
    {
        return &DistanceToEgo;
    }
private:

    AgentVehicleType VehicleType;
    State_Moving State;
    Properties Properties;

    double DistanceToEgo;
};


