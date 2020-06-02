/*******************************************************************************
* Copyright (c) 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! \file  CompCtrlToAgentCompSignal.h
//! \brief This file contains all functions for class CompCtrlToAgentCompSignal
//!
//! This class contains all functionality of the module.
//-----------------------------------------------------------------------------

#pragma once

#include <map>
#include <string>

#include "Interfaces/signalInterface.h"
#include "Common/globalDefinitions.h"

using Warnings = std::map<std::string, std::vector<ComponentWarningInformation>>;

/*!
 * --------------------------------------------------------------------------------
 * \brief The CompCtrlToAgentCompSignal class facilitates communication with the
 *        agent component, defining the max reachable state of the component as
 *        well as the states of other components connected to the
 *        ComponentController
 * --------------------------------------------------------------------------------
 */
class CompCtrlToAgentCompSignal : public SignalInterface
{
public:
    static constexpr char COMPONENTNAME[] = "CompCtrlToAgentCompSignal";
    //-----------------------------------------------------------------------------
    //! Constructor
    //-----------------------------------------------------------------------------
    CompCtrlToAgentCompSignal(const ComponentState &maxReachableState,
                              const std::map<std::string, std::pair<ComponentType, ComponentState>> &vehicleComponentStates):
        maxReachableState(maxReachableState),
        vehicleComponentStates(vehicleComponentStates)
    {}

    CompCtrlToAgentCompSignal() = delete;
    CompCtrlToAgentCompSignal(const CompCtrlToAgentCompSignal&) = delete;
    CompCtrlToAgentCompSignal(CompCtrlToAgentCompSignal&&) = delete;
    CompCtrlToAgentCompSignal& operator=(const CompCtrlToAgentCompSignal&) = delete;
    CompCtrlToAgentCompSignal& operator=(CompCtrlToAgentCompSignal&&) = delete;

    virtual ~CompCtrlToAgentCompSignal() = default;

    //-----------------------------------------------------------------------------
    //! Returns the content/payload of the signal as an std::string
    //!
    //! \return     Content/payload of the signal as an std::string
    //-----------------------------------------------------------------------------
    virtual operator std::string() const
    {
        return "";
    }

    /*!
     * \brief GetMaxReachableState returns the max reachable state of the target component as defined by the ComponentController
     *
     * \return The max reachable state of the target component as defined by the ComponentController as a ComponentState
     */
    ComponentState GetMaxReachableState() const {
        return maxReachableState;
    }

    /*!
     * \brief GetVehicleComponentStates returns a map linking vehicle component names to types and states
     *
     * \return A map of vehicle component names to pairs of types and states as std::map<std::string, std::pair<AdasType, ComponentState>>
     */
    const std::map<std::string, std::pair<ComponentType, ComponentState>>& GetVehicleComponentStates() const {
        return vehicleComponentStates;
    }

private:
    const ComponentState maxReachableState;     //!< Highest reachable state
    const std::map<std::string, std::pair<ComponentType, ComponentState>> vehicleComponentStates;   //!< Current state of all components
};

/*!
 * --------------------------------------------------------------------------------
 * \brief The CompCtrlToDriverCompSignal class facilitates communication with the
 *        driver component, allowing for warnings to be forwarded to the driver
 *        alongside standard agent component information.
 * --------------------------------------------------------------------------------
 */
class CompCtrlToDriverCompSignal : public CompCtrlToAgentCompSignal
{
public:
    static constexpr char COMPONENTNAME[] = "CompCtrlToDriverCompSignal";
    //-----------------------------------------------------------------------------
    //! Constructor
    //-----------------------------------------------------------------------------
    CompCtrlToDriverCompSignal(const ComponentState &maxReachableState,
                               const std::map<std::string, std::pair<ComponentType, ComponentState>> &vehicleComponentStates,
                               const Warnings& warnings):
        CompCtrlToAgentCompSignal(maxReachableState,
                                  vehicleComponentStates),
        warnings(warnings)
    {}

    CompCtrlToDriverCompSignal() = delete;
    CompCtrlToDriverCompSignal(const CompCtrlToDriverCompSignal&) = delete;
    CompCtrlToDriverCompSignal(CompCtrlToDriverCompSignal&&) = delete;
    CompCtrlToDriverCompSignal& operator=(const CompCtrlToDriverCompSignal&) = delete;
    CompCtrlToDriverCompSignal& operator=(CompCtrlToDriverCompSignal&&) = delete;
    virtual ~CompCtrlToDriverCompSignal() = default;

    /*!
     * \brief GetWarnings gets all warnings from the signal (if any exist), by component name
     *
     * \return all warnings for the driver known by the ComponentController (if any exist), by component name
     */
    const Warnings& GetWarnings() const
    {
        return warnings;
    }
private:
    const Warnings warnings; //!< Contains all warnings for the driver known by the ComponentController (if any exist) by component name
};
