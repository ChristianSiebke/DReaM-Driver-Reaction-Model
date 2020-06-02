/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*               2018 AMFD GmbH
*               2016, 2017 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  ALongitudinalDriverToDLongitudinalSignal.h
//! @brief This file contains all functions for class
//! ALongitudinalDriverToDLongitudinalSignal
//!
//! This class contains all functionality of the signal.
//-----------------------------------------------------------------------------
#pragma once

#include "Interfaces/modelInterface.h"

class LongitudinalSignal: public ComponentStateSignalInterface
{
public:
    static constexpr char COMPONENTNAME[] = "LongitudinalSignal";

    LongitudinalSignal() : ComponentStateSignalInterface{ComponentState::Disabled}
    {}

    LongitudinalSignal(
            ComponentState componentState,
            double accPedalPos,
            double brakePedalPos,
            int gear
            ) :
        ComponentStateSignalInterface{componentState},
        accPedalPos{accPedalPos},
        brakePedalPos{brakePedalPos},
        gear{gear}
    {}

    LongitudinalSignal(const LongitudinalSignal&) = default;
    LongitudinalSignal(LongitudinalSignal&&) = default;
    LongitudinalSignal& operator=(const LongitudinalSignal&) = default;
    LongitudinalSignal& operator=(LongitudinalSignal&&) = default;

    virtual ~LongitudinalSignal() = default;

    double accPedalPos {};
    double brakePedalPos {};
    int gear {};

    //-----------------------------------------------------------------------------
    //! Returns the content/payload of the signal as an std::string
    //!
    //! @return     Content/payload of the signal as an std::string
    //-----------------------------------------------------------------------------
    virtual operator std::string() const
    {
        std::ostringstream stream;
        stream << COMPONENTNAME << '\n';
        stream << "accPedalPos:   " << accPedalPos << '\n';
        stream << "brakePedalPos: " << brakePedalPos << '\n';
        stream << "gear:          " << gear << '\n';
        return stream.str();
    }
};



