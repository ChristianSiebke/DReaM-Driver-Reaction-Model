/*******************************************************************************
* Copyright (c) 2020 BMW AG
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  gazeFollowerSignal.h
//! @brief This file contains all functions for class GazeFollowerSignal
//!
//! This class contains all functionality of the module.
//-----------------------------------------------------------------------------

#pragma once

#include "Interfaces/modelInterface.h"

//-----------------------------------------------------------------------------
//! Signal class
//-----------------------------------------------------------------------------
class GazeFollowerSignal : public ComponentStateSignalInterface
{
public:
    static constexpr char COMPONENTNAME[] = "GazeFollowerSignal";

    //-----------------------------------------------------------------------------
    //! Constructor
    //-----------------------------------------------------------------------------
    GazeFollowerSignal()
    {
        componentState = ComponentState::Disabled;
    }

    //-----------------------------------------------------------------------------
    //! Constructor
    //-----------------------------------------------------------------------------
    GazeFollowerSignal(GazeFollowerSignal &other) :
        GazeFollowerSignal(other.componentState,
                           other.gazeActivityState,
                           other.gazeFileName)
    {

    }

    //-----------------------------------------------------------------------------
    //! Constructor
    //-----------------------------------------------------------------------------
    GazeFollowerSignal(ComponentState componentState,
                       std::string gazeActivityState,
                       std::string gazeFileName) :
        gazeActivityState(gazeActivityState),
        gazeFileName(gazeFileName)
    {
        this->componentState = componentState;
    }

    GazeFollowerSignal(const GazeFollowerSignal&) = delete;
    GazeFollowerSignal(GazeFollowerSignal&&) = delete;
    GazeFollowerSignal& operator=(const GazeFollowerSignal&) = delete;
    GazeFollowerSignal& operator=(GazeFollowerSignal&&) = delete;

    virtual ~GazeFollowerSignal()

    {}

    //-----------------------------------------------------------------------------
    //! Returns the content/payload of the signal as an std::string
    //!
    //! @return     Content/payload of the signal as an std::string
    //-----------------------------------------------------------------------------
    virtual operator std::string() const
    {
        std::ostringstream stream;
        stream << COMPONENTNAME << std::endl;
        stream << "gazeActivityState: " << gazeActivityState << std::endl;
        stream << "gazeFileName: " << gazeFileName << std::endl;

        return stream.str();
    }

    const std::string gazeActivityState;
    const std::string gazeFileName;
};

