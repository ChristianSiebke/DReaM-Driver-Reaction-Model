/*********************************************************************
* Copyright (c) 2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

//-----------------------------------------------------------------------------
//! @file  controlData.h
//! @brief This file contains signals
//!
//-----------------------------------------------------------------------------

#pragma once

#include <vector>
#include <string>

//-----------------------------------------------------------------------------
//! Signal class containing throttle, steer, brakePedal and 4x brakeSuperpose signals
//-----------------------------------------------------------------------------

class ControlData
{
public:
    // Construct from std::vector
    ControlData(double steer = 0.0, double throttle = 0.0, double brakePedal = 0.0, std::vector<double> inValue = {0.0, 0.0, 0.0, 0.0})
        : steer{steer}, throttle{throttle}, brakePedal{brakePedal}, brakeSuperpose{inValue}
    {}
    ControlData(const ControlData &inSignal)
        : steer{inSignal.steer}, throttle{inSignal.throttle}, brakePedal{inSignal.brakePedal}, brakeSuperpose{inSignal.brakeSuperpose}
    {}
    // Construct from SignalVector by moving
    ControlData(ControlData &&inSignal)
        : steer{std::move(inSignal.steer)},
          throttle{std::move(inSignal.throttle)},
          brakePedal{std::move(inSignal.brakePedal)},
          brakeSuperpose{std::move(inSignal.brakeSuperpose)}
    {}

    // Assign from SignalVector by copy
    ControlData &operator=(const ControlData &inSignal)
    {
        steer = inSignal.steer;
        throttle = inSignal.throttle;
        brakePedal = inSignal.brakePedal;
        if (brakeSuperpose.size() != inSignal.brakeSuperpose.size())
        {
            brakeSuperpose.resize(inSignal.brakeSuperpose.size());
        }
        brakeSuperpose = inSignal.brakeSuperpose;
        return *this;
    }
    // Assign from SignalVector by moving
    ControlData &operator=(ControlData &&inSignal)
    {
        steer = std::move(inSignal.steer);
        throttle = std::move(inSignal.throttle);
        brakePedal = std::move(inSignal.brakePedal);
        brakeSuperpose = std::move(inSignal.brakeSuperpose);
        return *this;
    }

    virtual ~ControlData() = default;

    //-----------------------------------------------------------------------------
    //! Converts signal to string
    //-----------------------------------------------------------------------------
    operator std::string() const
    {
        std::string signalString;
        signalString += std::to_string(steer);
        signalString += "_";
        signalString += std::to_string(throttle);
        signalString += "_";
        for (auto elem : brakeSuperpose)
        {
            signalString += std::to_string(elem);
            signalString += "_";
        }
        signalString.erase(signalString.end());
        return signalString;
    }

    //
    double steer {0.0};
    double throttle {0.0};
    double brakePedal {0.0};
    std::vector<double> brakeSuperpose {0.0, 0.0, 0.0, 0.0};
};
