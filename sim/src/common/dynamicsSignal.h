/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*               2018 AMFD GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  DynamicsSignal.h
//! @brief This file contains all functions for class
//! DynamicsSignal
//!
//! This class contains all functionality of the module.
//-----------------------------------------------------------------------------

#pragma once

#include "include/modelInterface.h"

//-----------------------------------------------------------------------------
//! Signal class
//-----------------------------------------------------------------------------
class DynamicsSignal : public ComponentStateSignalInterface
{
public:
    static constexpr char COMPONENTNAME[] = "DynamicsSignal";

    DynamicsSignal() : ComponentStateSignalInterface{ComponentState::Disabled} {}
    DynamicsSignal(ComponentState componentState) : ComponentStateSignalInterface{componentState}{}

    DynamicsSignal(ComponentState componentState,
                  double acceleration,
                  double velocity,
                  double positionX,
                  double positionY,
                  double yaw,
                  double yawRate,
                  double steeringWheelAngle,
                  double centripetalAcceleration,
                  double travelDistance) :
        ComponentStateSignalInterface{componentState},
        acceleration(acceleration),
        velocity(velocity),
        positionX(positionX),
        positionY(positionY),
        yaw(yaw),
        yawRate(yawRate),
        steeringWheelAngle(steeringWheelAngle),
        centripetalAcceleration(centripetalAcceleration),
        travelDistance(travelDistance)
    {}

    DynamicsSignal(DynamicsSignal &other) = default;
    DynamicsSignal(const DynamicsSignal&) = default;
    DynamicsSignal(DynamicsSignal&&) = default;
    DynamicsSignal& operator=(const DynamicsSignal&) = default;
    DynamicsSignal& operator=(DynamicsSignal&&) = default;
    virtual ~DynamicsSignal() = default;

    //-----------------------------------------------------------------------------
    //! Returns the content/payload of the signal as an std::string
    //!
    //! @return                       Content/payload of the signal as an std::string
    //-----------------------------------------------------------------------------
    virtual operator std::string() const
    {
        std::ostringstream stream;
        stream << COMPONENTNAME << std::endl;
        stream << "acceleration: " << acceleration << std::endl;
        stream << "velocity: " << velocity << std::endl;
        stream << "positionX: " << positionX << std::endl;
        stream << "positionY: " << positionY << std::endl;
        stream << "yaw: " << yaw << std::endl;
        stream << "yawRate: " << yawRate << std::endl;
        stream << "steeringWheelAngle: " << steeringWheelAngle << std::endl;
        stream << "centripetalAcceleration: " << centripetalAcceleration << std::endl;
        stream << "travelDistance: " << travelDistance << std::endl;
        return stream.str();
    }

    double acceleration = 0.0;
    double velocity = 0.0;
    double positionX = 0.0;
    double positionY = 0.0;
    double yaw = 0.0;
    double yawRate = 0.0;
    double steeringWheelAngle = 0.0;
    double centripetalAcceleration = 0.0;
    double travelDistance = 0.0;
};

