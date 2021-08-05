/*********************************************************************
* Copyright (c) 2021 ITK Engineering GmbH
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

// Construct from ControlData
ControlSignal::ControlSignal(const ControlData &inData)
    : value{inData.steer, inData.throttle, inData.brakePedal, inData.brakeSuperpose}
{}

//-----------------------------------------------------------------------------
//! Converts signal to string
//-----------------------------------------------------------------------------
ControlSignal::operator std::string() const
{
    return std::string(value);
}

