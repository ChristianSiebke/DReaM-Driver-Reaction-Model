/*********************************************************************
* Copyright (c) 2017, 2018 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

//-----------------------------------------------------------------------------
//! @file  default_Prio.h
//! @brief This file contains default priorities for PCM components.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
//! Enumeration class defining default priorities of PCM components
//-----------------------------------------------------------------------------
enum class PCMdefaultPrio
{
    Algorithm_RoutePlanner = 199, //199
    Algorithm_Selector = 100, //100
    Algorithm_ECU = 101, //101
    Algorithm_Switch = 102, //102
    Algorithm_Trajectory = 150, //105
    Dynamics_Collision = 0, //0
    Dynamics = 3, //1
    EgoSensor = 203, //203
    Init_Agent = 500, //500
    Sensor_Collision = 201, //201
    Sensor_OSIRoad = 205, //205
    Sensor_OSIAgent = 204 //204
};
