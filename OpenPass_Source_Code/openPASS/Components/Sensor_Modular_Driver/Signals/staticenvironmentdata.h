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
//! @brief provide Container Structures for the driver's cognition of the static-environment
//!
//-----------------------------------------------------------------------------
//! \addtogroup Algorithm_ModularDriver
//-----------------------------------------------------------------------------

#pragma once

#include <vector>
#include <string>
#include "globalDefinitions.h"
#include "signalInterface.h"
#include "egodata.h"
#include "../Components/Sensor_Driver/Signals/sensor_driverDefinitions.h"
#include "MentalModelLane.h"

class MentalModelLane;

enum ObjectTypes //Sensor Informationen related to object of type ...
{
    Unknown,
    TrafficSign,
    RoadMarking,
    Building,
    Plant
};

struct Dimension //dimension of the object related to the sensor coordinate system
{
    double lx;
    double ly;
    double lz;
};

struct TrafficSignInformation
{
    std::string GetName() const
    {
      return  "TrafficSignInformation";
    }
};

enum class PrecipitationType
{    
    None,
    Raindrops,
    IcePellets,
    Hail,
    Snowflakes,
    DiamondDust
};

struct Weather
{
    std::string GetName() const
    {
      return  "Weather";
    }

    int temperature; // Temp. in °C
    PrecipitationType precipitationType;
    int precipitation; // in l/m²
};

struct  Environment
{
    std::string GetName() const
    {
      return  "Environment";
    }

    Weather weather;
    Weekday weekday;
    double frictionCoeff;
};

struct StaticObject
{
    std::string GetName() const
    {
      return  "StaticObject";
    }

    ObjectType type;
    ObjectPosition pos_abs;
    Dimension dimension;
};

struct LaneInformation:LaneInformationGeometry
{
    //! Lane-id of regarded lane
    int laneid;
    //! representation of a lane in the cognitive processes
    std::vector<MentalModelLane> mentalModelLanes;
};

struct RoadGeometry
{
    //! Current maximum visibility distance as specified by the world
    double visibilityDistance {-999.0};
    //! Data about the lane to left (in driving direction) of the mainLane
    LaneInformation laneLeft;
    //! Data about the lane the where the middle of the front of the agent is (i.e. mainLane)
    LaneInformation laneEgo;
    //! Data about the lane to right (in driving direction) of the mainLane
    LaneInformation laneRight;
};

struct  StaticEnvironmentData
{
    std::string GetName() const
    {
      return  "StaticEnvironmentData";
    }

    int numberoflanes;
    std::vector<StaticObject> staticObjects;
    RoadGeometry roadGeometry;
    TrafficRuleInformation trafficRuleInformation;   
    Environment environment;
};
