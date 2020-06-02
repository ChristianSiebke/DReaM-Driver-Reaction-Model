/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  laneChangeSignal.h
//! @brief This file contains all functions for class
//! LaneChangeSignal
//!
//! This class contains all functionality of the module.
//-----------------------------------------------------------------------------

#pragma once

#include "Interfaces/modelInterface.h"

//-----------------------------------------------------------------------------
//! Signal class
//-----------------------------------------------------------------------------
class LaneChangeSignal : public ComponentStateSignalInterface
{
public:
    static constexpr char COMPONENTNAME[] = "LaneChangeSignal";


    //-----------------------------------------------------------------------------
    //! Constructor
    //-----------------------------------------------------------------------------
    LaneChangeSignal()
    {
        componentState = ComponentState::Disabled;
    }

    //-----------------------------------------------------------------------------
    //! Constructor
    //-----------------------------------------------------------------------------
    LaneChangeSignal(LaneChangeSignal &other) :
        LaneChangeSignal(other.componentState,
                         other.deltaLaneId)

    {

    }

    //-----------------------------------------------------------------------------
    //! Constructor
    //-----------------------------------------------------------------------------
    LaneChangeSignal(ComponentState componentState,
                     int deltaLaneId) :
        deltaLaneId(deltaLaneId)
    {
        this->componentState = componentState;
    }

    LaneChangeSignal(const LaneChangeSignal&) = delete;
    LaneChangeSignal(LaneChangeSignal&&) = delete;
    LaneChangeSignal& operator=(const LaneChangeSignal&) = delete;
    LaneChangeSignal& operator=(LaneChangeSignal&&) = delete;

    virtual ~LaneChangeSignal()

    {}

    //-----------------------------------------------------------------------------
    //! Returns the content/payload of the signal as an std::string
    //!
    //! @return                       Content/payload of the signal as an std::string
    //-----------------------------------------------------------------------------
    virtual operator std::string() const
    {
        std::ostringstream stream;
        stream << COMPONENTNAME <<" "
               << deltaLaneId << " lane(s)";
        return stream.str();
    }

    const int deltaLaneId{0};
};

