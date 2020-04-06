/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*               2016, 2017 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#define OBSERVATION_LOG_VALUE_UNDEFINED ""

#include <string>
#include <unordered_map>
#include <vector>

//! Defines the cyclic coloumn names associated with each logging group
const std::unordered_map<std::string, std::vector<std::string>> LOGGINGGROUP_DEFINITIONS
{
    { "RoadPosition", {
            "AgentInFront",
            "Lane",
            "PositionRoute",
            "Road",
            "TCoordinate"
        }
    },
    { "RoadPositionExtended", {
            "SecondaryLanes"
        }
    },
    { "Trace", {
            "XPosition",
            "YPosition",
            "YawAngle"
        }
    },
    { "Sensor", {
        }
    },
    { "SensorExtended", {
            "Sensor0_DetectedAgents",
            "Sensor1_DetectedAgents",
            "Sensor2_DetectedAgents",
            "Sensor3_DetectedAgents",
            "Sensor4_DetectedAgents",
            "Sensor5_DetectedAgents",
            "Sensor6_DetectedAgents",
            "Sensor7_DetectedAgents",
            "Sensor8_DetectedAgents",
            "Sensor9_DetectedAgents",
            "Sensor0_VisibleAgents",
            "Sensor1_VisibleAgents",
            "Sensor2_VisibleAgents",
            "Sensor3_VisibleAgents",
            "Sensor4_VisibleAgents",
            "Sensor5_VisibleAgents",
            "Sensor6_VisibleAgents",
            "Sensor7_VisibleAgents",
            "Sensor8_VisibleAgents",
            "Sensor9_VisibleAgents"
        }
    },
    { "Vehicle", {
            "AccelerationPedalPosition",
            "BrakePedalPosition",
            "EngineMoment",
            "Gear",
            "SteeringAngle",
            "TotalDistanceTraveled",
            "YawRate",
        }
    },
    { "Visualization", {
            "AccelerationEgo",
            "BrakeLight",
            "IndicatorState",
            "LightStatus",
            "VelocityEgo"
        }
    }
};

const std::string AgentCategoryStrings[] =
{
    "Ego",
    "Scenario",
    "Common"
};
