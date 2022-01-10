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
//! @file  trajectorySignal.h
//! @brief This file contains all functions for class
//! TrajectorySignal
//!
//! This class contains all functionality of the module.
//-----------------------------------------------------------------------------

#pragma once

#include "include/modelInterface.h"
#include "common/openScenarioDefinitions.h"

//-----------------------------------------------------------------------------
//! Signal class
//-----------------------------------------------------------------------------
class TrajectorySignal : public ComponentStateSignalInterface
{
public:
    static constexpr char COMPONENTNAME[] = "TrajectorySignal";


    //-----------------------------------------------------------------------------
    //! Constructor
    //-----------------------------------------------------------------------------
    TrajectorySignal()
    {
        componentState = ComponentState::Disabled;
    }

    //-----------------------------------------------------------------------------
    //! Constructor
    //-----------------------------------------------------------------------------
    TrajectorySignal(TrajectorySignal &other) :
        TrajectorySignal(other.componentState,
                      other.trajectory)

    {

    }

    //-----------------------------------------------------------------------------
    //! Constructor
    //-----------------------------------------------------------------------------
    TrajectorySignal(ComponentState componentState,
                     openScenario::Trajectory trajectory) :
        trajectory(trajectory)
    {
        this->componentState = componentState;
    }

    TrajectorySignal(const TrajectorySignal&) = delete;
    TrajectorySignal(TrajectorySignal&&) = delete;
    TrajectorySignal& operator=(const TrajectorySignal&) = delete;
    TrajectorySignal& operator=(TrajectorySignal&&) = delete;

    virtual ~TrajectorySignal()

    {}

    //-----------------------------------------------------------------------------
    //! Returns the content/payload of the signal as an std::string
    //!
    //! @return                       Content/payload of the signal as an std::string
    //-----------------------------------------------------------------------------
    virtual operator std::string() const
    {
        std::ostringstream stream;
        stream << COMPONENTNAME << "\n"
               << trajectory;
        return stream.str();
    }

    openScenario::Trajectory trajectory;
};

