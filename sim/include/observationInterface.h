/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020, 2021 in-tech GmbH
*               2016, 2017, 2018 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  ObservationInterface.h
//! @brief This file contains the interface of the observation modules to
//!        interact with the framework.
//-----------------------------------------------------------------------------

#pragma once

#include <string>
#include <map>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <tuple>
#include "include/callbackInterface.h"
#include "include/eventInterface.h"
#include "include/runResultInterface.h"

class CallbackInterface;
class DataBufferReadInterface;
class ParameterInterface;
class StochasticsInterface;
class WorldInterface;

//! Interface which has to be provided by observation modules
class ObservationInterface
{
public:
    ObservationInterface() = default;
    ObservationInterface(StochasticsInterface* stochastics,
                         WorldInterface* world,
                         const ParameterInterface* parameters,
                         const CallbackInterface* callbacks,
                         [[maybe_unused]] DataBufferReadInterface* dataBuffer) :
        stochastics(stochastics),
        world(world),
        parameters(parameters),
        callbacks(callbacks)
    {}
    ObservationInterface(const ObservationInterface&) = delete;
    ObservationInterface(ObservationInterface&&) = delete;
    ObservationInterface& operator=(const ObservationInterface&) = delete;
    ObservationInterface& operator=(ObservationInterface&&) = delete;
    virtual ~ObservationInterface() = default;

    //-----------------------------------------------------------------------------
    //! Called by framework in opSimulationManager before each simulation run starts
    //-----------------------------------------------------------------------------
    virtual void OpSimulationManagerPreHook() = 0; // currently not implemented

    //-----------------------------------------------------------------------------
    //! Called by framework in opSimulationManager after each simulation run ends
    //!
    //! @param[in]     filename      Name of file containing the simulation run results from the simulation
    //-----------------------------------------------------------------------------
    virtual void OpSimulationManagerPostHook(const std::string& filename) = 0; // currently not implemented

    //-----------------------------------------------------------------------------
    //! Called by framework in simulation before all simulation runs start
    //!
    //-----------------------------------------------------------------------------
    virtual void OpSimulationPreHook() = 0;

    //-----------------------------------------------------------------------------
    //! Called by framework in simulation before each simulation run starts be stored
    //-----------------------------------------------------------------------------
    virtual void OpSimulationPreRunHook() = 0;

    //-----------------------------------------------------------------------------
    //! Called by framework in simulation at each time step.
    //! Observation module can indicate end of simulation run here.
    //!
    //! @param[in]     time          Current scheduling time
    //! @param[in,out] runResult     Reference to run result
    //-----------------------------------------------------------------------------
    virtual void OpSimulationUpdateHook(int time, RunResultInterface& runResult) = 0;

    //-----------------------------------------------------------------------------
    //! Called by framework in simulation after each simulation run ends.
    //! Observation module can observe the current simulation run here.
    //!
    //! @param[in]     runResult     Reference to run result
    //-----------------------------------------------------------------------------
    virtual void OpSimulationPostRunHook(const RunResultInterface& runResult) = 0;

    //-----------------------------------------------------------------------------
    //! Called by framework in simulation after all simulation runs end.
    //-----------------------------------------------------------------------------
    virtual void OpSimulationPostHook() = 0;

    //-----------------------------------------------------------------------------
    //! Called by framework in simulation after all simulation runs end to transfer
    //! the observation module results to the opSimulationManager.
    //!
    //! @return                      File to be transferred
    //-----------------------------------------------------------------------------
    virtual const std::string OpSimulationResultFile() = 0;

protected:
    //-----------------------------------------------------------------------------
    //! Retrieves the stochastics functionality
    //!
    //! @return                      Stochastics functionality of framework
    //-----------------------------------------------------------------------------
    StochasticsInterface* GetStochastics() const
    {
        return stochastics;
    }

    //-----------------------------------------------------------------------------
    //! Retrieves the world
    //!
    //! @return                      World (scenery and agents)
    //-----------------------------------------------------------------------------
    WorldInterface* GetWorld() const
    {
        return world;
    }

    //-----------------------------------------------------------------------------
    //! Retrieves the configuration parameters of the observation module
    //!
    //! @return                      Configuration parameters
    //-----------------------------------------------------------------------------
    const ParameterInterface* GetParameters() const
    {
        return parameters;
    }

    //-----------------------------------------------------------------------------
    //! Provides callback to LOG() macro
    //!
    //! @param[in]     logLevel      Importance of log
    //! @param[in]     file          Name of file where log is called
    //! @param[in]     line          Line within file where log is called
    //! @param[in]     message       Message to log
    //-----------------------------------------------------------------------------
    void Log(CbkLogLevel logLevel,
             const char* file,
             int line,
             const std::string& message)
    {
        if (callbacks)
        {
            callbacks->Log(logLevel,
                           file,
                           line,
                           message);
        }
    }

private:
    // Access to the following members is provided by the corresponding member
    // functions.
    StochasticsInterface* stochastics;                      //!< References stochastics functionality of the framework
    WorldInterface* world;                                  //!< References the world of the framework
    const ParameterInterface* parameters;                   //!< References the configuration parameters
    const CallbackInterface* callbacks;                     //!< References the callback functions of the framework
};


