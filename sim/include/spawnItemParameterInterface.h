/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  SpawnItemParameterInterface.h
//! @brief This file contains the interface of the spawn item parameter to interact
//!        with the framework.
//-----------------------------------------------------------------------------

#pragma once

#include <string>

#include "include/agentBlueprintInterface.h"

class WorldInterface;
//-----------------------------------------------------------------------------
//! This class represents the functionality of a spawn item (agent) which will
//! be spawned by a spawn point
//-----------------------------------------------------------------------------
class SpawnItemParameterInterface
{
public:
    SpawnItemParameterInterface() = default;
    SpawnItemParameterInterface(const SpawnItemParameterInterface&) = delete;
    SpawnItemParameterInterface(SpawnItemParameterInterface&&) = delete;
    SpawnItemParameterInterface& operator=(const SpawnItemParameterInterface&) = delete;
    SpawnItemParameterInterface& operator=(SpawnItemParameterInterface&&) = delete;
    virtual ~SpawnItemParameterInterface() = default;

    //-----------------------------------------------------------------------------
    //! Sets the x-coordinate of the agent to be spawned
    //!
    //! @param[in]     positionX    X-coordinate
    //-----------------------------------------------------------------------------
    virtual void SetPositionX(double positionX) = 0;

    //-----------------------------------------------------------------------------
    //! Sets the y-coordinate of the agent to be spawned
    //!
    //! @param[in]     positionY    Y-coordinate
    //-----------------------------------------------------------------------------
    virtual void SetPositionY(double positionY) = 0;

    //-----------------------------------------------------------------------------
    //! Sets the forward velocity of the agent to be spawned
    //!
    //! @param[in]     velocity    Forward velocity
    //-----------------------------------------------------------------------------
    virtual void SetVelocity(double velocity) = 0;

    //-----------------------------------------------------------------------------
    //! Sets the forward acceleration of the agent to be spawned
    //!
    //! @param[in]     acceleration    Forward acceleration
    //-----------------------------------------------------------------------------
    virtual void SetAcceleration(double acceleration) = 0;

    //-----------------------------------------------------------------------------
    //! Sets the gear of the agent to be spawned
    //!
    //! @param[in]     gear    current/calculated gear
    //-----------------------------------------------------------------------------
    virtual void SetGear(double gear) = 0;

    //-----------------------------------------------------------------------------
    //! Sets the yaw angle of the agent to be spawned
    //!
    //! @param[in]     yawAngle    Agent orientation (0 points to east)
    //-----------------------------------------------------------------------------
    virtual void SetYaw(double yawAngle) = 0;

    //-----------------------------------------------------------------------------
    //! Sets the next time when the agent will be spawned
    //!
    //! @param[in]     nextTimeOffset    Time offset counted from the current
    //!                                  scheduling time
    //-----------------------------------------------------------------------------
    virtual void SetNextTimeOffset(int nextTimeOffset) = 0;

    //-----------------------------------------------------------------------------
    //! Selectes the agent to be spawned next within the configured agent array
    //!
    //! @param[in]     index    Index within configured agent array
    //-----------------------------------------------------------------------------
    virtual void SetIndex(int index) = 0;

    //-----------------------------------------------------------------------------
    //! Sets the vehicle type of the agent to be spawned
    //!
    //! @param[in]     vehicleModel    vehicleModel of agent
    //-----------------------------------------------------------------------------
    virtual void SetVehicleModel(std::string vehicleModel) = 0;

    virtual double GetPositionX() const = 0;

    virtual double GetPositionY() const = 0;

    virtual double GetVelocity() const = 0;

    virtual double GetAcceleration() const = 0;

    virtual double GetYaw() const = 0;

    virtual std::string GetVehicleModel() const = 0;
};

