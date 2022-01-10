/********************************************************************************
 * Copyright (c) 2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  controlData.h
//! @brief This file contains signals
//!
//-----------------------------------------------------------------------------

#pragma once

#include <array>
#include <string>
#include <vector>

//-----------------------------------------------------------------------------
//! Signal class containing throttle, steer, brakePedal and 4x brakeSuperpose signals
//-----------------------------------------------------------------------------

class ControlData
{
public:
    /// Constructor
    /// @param[in]  steer           steer angle
    /// @param[in]  throttle        throttle
    /// @param[in]  brakePedal      position of the brake pedal
    /// @param[in]  brakeSuperpose  brake superpose
    ControlData(double steer = 0.0, double throttle = 0.0, double brakePedal = 0.0, std::array<double, 4> brakeSuperpose = {0.0, 0.0, 0.0, 0.0}) :
        steer{steer},
        throttle{throttle},
        brakePedal{brakePedal},
        brakeSuperpose{brakeSuperpose}
    {
    }

    // removing operators
    ControlData(const ControlData &) = default;
    ControlData(ControlData &&l) = default;
    ControlData &operator=(const ControlData &l) = default;
    ControlData &operator=(ControlData &&) = default;

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
    std::array<double, 4> brakeSuperpose{0.0, 0.0, 0.0, 0.0};
};
