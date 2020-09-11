/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

namespace openpass::scheduling {

class Scheduler;

/// \brief Allows core components to access the current schedule time without need for a direct reference
/// \note The scheduler holds a manipulate reference to the current time provided by this instance
///
/// \code{.cpp}
/// auto currentTime = openpass::scheduling::TimeKeeper::Now();
/// \endcode
class TimeKeeper
{
    friend class Scheduler;

protected:
    static inline int time{-1}; //!< Global time for the simulator

public:
    /// \brief Get the current simulation time
    /// \return Current simulation time
    static int Now() noexcept
    {
        return time;
    }
};

} // namespace openpass::scheduling
