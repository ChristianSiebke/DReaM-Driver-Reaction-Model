/********************************************************************************
 * Copyright (c) 2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  rollSignal.h
//! @brief This file contains all functions for class
//! RollSignal
//!
//! This class contains all functionality of the signal.
//-----------------------------------------------------------------------------
#pragma once

#include "include/modelInterface.h"

class RollSignal: public ComponentStateSignalInterface
{
public:
    const std::string COMPONENTNAME = "RollSignal";

    //-----------------------------------------------------------------------------
    //! Constructor
    //-----------------------------------------------------------------------------
    RollSignal()
    {
        componentState = ComponentState::Disabled;
    }

    //-----------------------------------------------------------------------------
    //! Constructor
    //-----------------------------------------------------------------------------
    RollSignal(
            ComponentState componentState,
            double rollAngle
            ):
        rollAngle(rollAngle)
    {
        this->componentState = componentState;
    }

    RollSignal(const RollSignal&) = default;
    RollSignal(RollSignal&&) = default;
    RollSignal& operator=(const RollSignal&) = default;
    RollSignal& operator=(RollSignal&&) = default;

    virtual ~RollSignal() = default;

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

    double rollAngle {0.};

};
