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
//! @file  controlSignal.h
//! @brief This file contains signals
//!
//-----------------------------------------------------------------------------

#include "controlSignal.h"

//-----------------------------------------------------------------------------
//! Signal class containing throttle, steer and 4x brake signals
//-----------------------------------------------------------------------------

// Construct from std::vector
ControlSignal::ControlSignal(const ControlData &inData)
    : value{inData.steer, inData.throttle, inData.brakePedal, inData.brakeSuperpose}
{}
// Construct from SignalVector by copy
ControlSignal::ControlSignal(const ControlSignal &inSignal)
    : SignalInterface{}, value{inSignal.value}
{}
// Construct from SignalVector by moving
ControlSignal::ControlSignal(ControlSignal &&inSignal)
    : value{std::move(inSignal.value)}
{}

// Assign from SignalVector by copy
ControlSignal &ControlSignal::operator=(const ControlSignal &inSignal)
{
    value = inSignal.value;
    return *this;
}
// Assign from SignalVector by moving
ControlSignal &ControlSignal::operator=(ControlSignal &&inSignal)
{
    value = std::move(inSignal.value);
    return *this;
}

//-----------------------------------------------------------------------------
//! Converts signal to string
//-----------------------------------------------------------------------------
ControlSignal::operator std::string() const
{
    return std::string(value);
}

