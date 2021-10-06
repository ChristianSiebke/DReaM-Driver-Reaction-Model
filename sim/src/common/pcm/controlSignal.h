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
//! @file  controlSignal.h
//! @brief This file contains signals
//!
//-----------------------------------------------------------------------------

#pragma once

#include "include/modelInterface.h"
#include "controlData.h"

//-----------------------------------------------------------------------------
//! Signal class containing the control data
//-----------------------------------------------------------------------------

class ControlSignal : public SignalInterface
{
public:
    // Construct from ControlData
    ControlSignal(const ControlData &inData);
    // Construct from SignalVector by copy
    ControlSignal(const ControlSignal &inSignal) = default;
    // Construct from SignalVector by moving
    ControlSignal(ControlSignal &&inSignal) = default;

    // Assign from SignalVector by copy
    ControlSignal &operator=(const ControlSignal &inSignal) = default;
    // Assign from SignalVector by moving
    ControlSignal &operator=(ControlSignal &&inSignal) = default;

    virtual ~ControlSignal() = default;

    //-----------------------------------------------------------------------------
    //! Converts signal to string
    //-----------------------------------------------------------------------------
    virtual operator std::string() const;

    //control data contains throttle, steer and 4x brake signals
    ControlData value;
};
