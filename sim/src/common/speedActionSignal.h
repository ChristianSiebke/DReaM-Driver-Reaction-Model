/********************************************************************************
 * Copyright (c) 2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  speedActionSignal.h
//! @brief This file contains all functions for class SpeedActionSignal
//!
//! This class contains all functionality of the module.
//-----------------------------------------------------------------------------

#pragma once

#include "include/modelInterface.h"
#include "common/openScenarioDefinitions.h"

//-----------------------------------------------------------------------------
//! Signal class
//-----------------------------------------------------------------------------
class SpeedActionSignal : public ComponentStateSignalInterface
{
public:
    static constexpr char COMPONENTNAME[] = "SpeedActionSignal";

    //-----------------------------------------------------------------------------
    //! Constructor
    //-----------------------------------------------------------------------------
    SpeedActionSignal()
    {
        componentState = ComponentState::Disabled;
    }

    //-----------------------------------------------------------------------------
    //! Constructor
    //-----------------------------------------------------------------------------
    SpeedActionSignal(SpeedActionSignal &other) :
        SpeedActionSignal(other.componentState,
                          other.targetSpeed,
                          other.acceleration)
    {}

    //-----------------------------------------------------------------------------
    //! Constructor
    //-----------------------------------------------------------------------------
    SpeedActionSignal(ComponentState componentState,
                      const double targetSpeed,
                      const double acceleration) :
        ComponentStateSignalInterface(componentState),
        targetSpeed(targetSpeed),
        acceleration(acceleration)
    {}

    virtual ~SpeedActionSignal() override = default;

    //-----------------------------------------------------------------------------
    //! Returns the content/payload of the signal as an std::string
    //!
    //! @return     Content/payload of the signal as an std::string
    //-----------------------------------------------------------------------------
    virtual operator std::string() const
    {
        std::ostringstream stream;
        stream << COMPONENTNAME << std::endl;
        return stream.str();
    }

    double targetSpeed {};
    double acceleration {}; // Range [0..inf[
};

