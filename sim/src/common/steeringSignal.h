/********************************************************************************
 * Copyright (c) 2018 AMFD GmbH
 *               2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  SteeringSignal.h
//! @brief This file contains all functions for class
//! SteeringSignal
//!
//! This class contains all functionality of the module.
//-----------------------------------------------------------------------------

#pragma once

#include "include/modelInterface.h"

//-----------------------------------------------------------------------------
//! Signal class
//-----------------------------------------------------------------------------
class SteeringSignal : public ComponentStateSignalInterface
{
public:
    static constexpr char COMPONENTNAME[] = "SteeringSignal";

    //-----------------------------------------------------------------------------
    //! Constructor
    //-----------------------------------------------------------------------------
    SteeringSignal(ComponentState componentState,
                  double steeringWheelAngle) :
        ComponentStateSignalInterface (componentState),
        steeringWheelAngle(steeringWheelAngle)
    {}

    SteeringSignal(const SteeringSignal&) = default;
    SteeringSignal(SteeringSignal&&) = default;
    SteeringSignal& operator=(const SteeringSignal&) = default;
    SteeringSignal& operator=(SteeringSignal&&) = default;

    virtual ~SteeringSignal() = default;

    //-----------------------------------------------------------------------------
    //! Returns the content/payload of the signal as an std::string
    //!
    //! @return                       Content/payload of the signal as an std::string
    //-----------------------------------------------------------------------------
    virtual operator std::string() const
    {
        std::ostringstream stream;
        stream << COMPONENTNAME;
        return stream.str();
    }

    double steeringWheelAngle;
};

