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
//! @file  egodata.h
//! @author  Konstantin Blenz
//! @date    Tue, 03.12.2019
//! @brief provide Container Structures for the driver's cognition of ego-informations
//!
//-----------------------------------------------------------------------------
//! \addtogroup Algorithm_ModularDriver
//-----------------------------------------------------------------------------

#pragma once

#include <vector>
#include <string>
#include <set>
#include <list>
#include "globalDefinitions.h"
#include "worldDefinitions.h"

struct state //absolute kinematic values in the global coordinate system
{
    bool hornSwitch = false;
    bool headLightSwitch = false;
    bool highBeamLightSwitch = false;
    bool flasherSwitch = false;
    bool brakeLight = false;

    int id {-999};
    int roadid {-999};
    int laneid {-999};
    int drivingdirection {-999};
    int spawntime {-999};

    double velocity_x {-999};
    double velocity_y {-999};
    double velocity_abs {-999};
    double acceleration_abs_inertial {-999};
    double yaw_velocity {-999};
    double yaw_acceleration {-999};

    IndicatorState indicatorstate = IndicatorState::IndicatorState_Off;
    std::set<int> secondarycoveredlanes = {};
    Position pos;
    RoadPosition roadPos;
};

struct state_Ego
{
    int currentGear {-999};
    double velocity_long {-999};
    double velocity_lat {-999};
    double acceleration_long {-999};
    double acceleration_lat {-999};
    double steeringWheelAngle {-999};
    double accelPedal {-999};
    double brakePedal {-999};
    double engineSpeed {-999};
    double distance_to_end_of_lane {-999};
};

struct Color //color specified in the RGB color space
{
    double r {-999};
    double g {-999};
    double b {-999};
};

struct driverInformation
{
    // Ego-Vehicle Limits
    double *v_y_Max;

    // Ego-Driver Information
    double *MinGap;
    double *v_Wish;
    double *thw_Wish;
    double *commonSpeedLimit_Violation;
};

class  egoData
{
public:
    egoData()
    {
    }

    std::string GetName() const
    {
        return  "egoData*";
    }

    AgentVehicleType *GetVehicleType()
    {
        return &VehicleType;
    }
    state_Ego *GetState_Ego()
    {
        return &State_Ego;
    }
    state *GetState()
    {
        return &State;
    }
    void SetVehicleType(AgentVehicleType VehicleType)
    {
        this->VehicleType = VehicleType;
    }
    void SetState_Ego(state_Ego State_Ego)
    {
        this->State_Ego = State_Ego;
    }
    void SetState(state State)
    {
        this->State = State;
    }
    void SetDriverInformation(driverInformation DriverInformation)
    {
        this->DriverInformation = DriverInformation;
    }
    driverInformation *GetDriverInformation()
    {
        return &DriverInformation;
    }

private:

    AgentVehicleType VehicleType;
    state_Ego State_Ego;
    state State;

    driverInformation DriverInformation {};
};

