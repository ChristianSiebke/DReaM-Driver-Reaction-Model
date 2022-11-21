/********************************************************************************
 * Copyright (c) 2020-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  StringSignal.h
//! @brief Transport a vector of strings
//-----------------------------------------------------------------------------

#pragma once

#include "include/modelInterface.h"

//! @brief Transport a vector of strings
class StringSignal : public ComponentStateSignalInterface
{
public:
    static constexpr char COMPONENTNAME[]{"StringSignal"};

    StringSignal() :
        payload{}
    {
        componentState = ComponentState::Disabled;
    }

    StringSignal(StringSignal &other) :
        StringSignal(other.componentState, other.payload)
    {
    }

    StringSignal(ComponentState componentState, const std::string payload) :
        payload{std::move(payload)}
    {
        this->componentState = componentState;
    }

    virtual ~StringSignal() = default;

    //! @brief Conversion method for printing
    //! @return Payload of the signal as string
    virtual operator std::string() const
    {
        return payload;
    }

    const std::string payload;
};
