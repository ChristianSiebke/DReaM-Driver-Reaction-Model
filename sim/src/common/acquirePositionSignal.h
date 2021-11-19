/********************************************************************************
 * Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  acquirePositionSignal.h
//! @brief This file contains all functions for class
//! AcquirePositionSignal
//!
//! This class contains all functionality of the module.
//-----------------------------------------------------------------------------


#include <sstream>
#include <include/signalInterface.h>

#include "openScenarioDefinitions.h"
class AcquirePositionSignal : public ComponentStateSignalInterface
{
public:
    static constexpr char COMPONENTNAME[] = "AcquirePositionSignal";

    AcquirePositionSignal(){
        componentState = ComponentState::Disabled;
    };
    AcquirePositionSignal(ComponentState componentState, openScenario::Position position) :
        position(std::move(position))
    {
        this->componentState = componentState;
    };
    AcquirePositionSignal(const AcquirePositionSignal &) = delete;
    AcquirePositionSignal(AcquirePositionSignal &&) = delete;

    ~AcquirePositionSignal() override = default;

    AcquirePositionSignal& operator=(const AcquirePositionSignal&) = delete;
    AcquirePositionSignal& operator=(AcquirePositionSignal&&) = delete;

    openScenario::Position position;

    explicit operator std::string() const override {
        std::ostringstream stream{};
        stream << COMPONENTNAME << "\n"
               << "openScenario::Position output stream operator not implemented.";
        return stream.str();
    };
};

