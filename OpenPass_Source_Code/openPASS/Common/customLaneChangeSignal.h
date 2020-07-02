/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*               2020 BMW AG
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  CustomLaneChangeSignal.h
//! @brief This file contains all functions for class CustomLaneChangeSignal
//!
//! This class contains all functionality of the module.
//-----------------------------------------------------------------------------

#pragma once

#include "Interfaces/modelInterface.h"

//-----------------------------------------------------------------------------
//! Signal class
//-----------------------------------------------------------------------------
class CustomLaneChangeSignal : public ComponentStateSignalInterface
{
public:
    static constexpr char COMPONENTNAME[] = "'CustomLaneChangeSignal";


    //-----------------------------------------------------------------------------
    //! Constructor
    //-----------------------------------------------------------------------------
    CustomLaneChangeSignal()
    {
        componentState = ComponentState::Disabled;
    }

    //-----------------------------------------------------------------------------
    //! Constructor
    //-----------------------------------------------------------------------------
    CustomLaneChangeSignal(CustomLaneChangeSignal &other) :
        CustomLaneChangeSignal(other.componentState,
                         other.deltaLaneId)

    {

    }

    //-----------------------------------------------------------------------------
    //! Constructor
    //-----------------------------------------------------------------------------
    CustomLaneChangeSignal(ComponentState componentState,
                     int deltaLaneId) :
        deltaLaneId(deltaLaneId)
    {
        this->componentState = componentState;
    }

    CustomLaneChangeSignal(const CustomLaneChangeSignal&) = delete;
    CustomLaneChangeSignal(CustomLaneChangeSignal&&) = delete;
    CustomLaneChangeSignal& operator=(const CustomLaneChangeSignal&) = delete;
    CustomLaneChangeSignal& operator=(CustomLaneChangeSignal&&) = delete;

    virtual ~CustomLaneChangeSignal()

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
