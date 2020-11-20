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
//! @file  CustomParametersSignal.h
//! @brief Transport a vector of strings
//-----------------------------------------------------------------------------

#pragma once

#include "include/modelInterface.h"

//! @brief Transport a vector of strings
class CustomParametersSignal : public ComponentStateSignalInterface
{
public:
    static constexpr char COMPONENTNAME[] = "'CustomParametersSignal";

    CustomParametersSignal() :
        parameters{}
    {
        componentState = ComponentState::Disabled;
    }

    CustomParametersSignal(CustomParametersSignal &other) :
        CustomParametersSignal(other.componentState, other.parameters)
    {
    }

    CustomParametersSignal(ComponentState componentState, std::vector<std::string> parameters) :
        parameters(parameters)
    {
        this->componentState = componentState;
    }

    virtual ~CustomParametersSignal() = default;

    //! @brief Conversion method for printing
    //! @return Payload of the signal as string
    virtual operator std::string() const
    {
        std::ostringstream sstream;
        sstream << COMPONENTNAME << " ";
        for (const auto parameter : parameters)
        {
            sstream << parameter << " ";
        }
        return sstream.str();
    }

    const std::vector<std::string> parameters;
};
