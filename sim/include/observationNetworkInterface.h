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
/** \file  ObservationNetworkInterface.h
*   \brief This file provides the interface for the ObservationNetwork
*/
//-----------------------------------------------------------------------------

#pragma once

#include "include/eventNetworkInterface.h"
#include "include/stochasticsInterface.h"
#include "common/observationLibraryDefinitions.h"

namespace core {

class ObservationBinding;
class ObservationModule;
class RunResult;

//-----------------------------------------------------------------------------
/** \brief This class provides the interface for the ObservationNetwork
*
*   \ingroup ObservationNetwork */
//-----------------------------------------------------------------------------
class ObservationNetworkInterface
{
public:
    ObservationNetworkInterface() = default;
    ObservationNetworkInterface(const ObservationNetworkInterface&) = delete;
    ObservationNetworkInterface(ObservationNetworkInterface&&) = delete;
    ObservationNetworkInterface& operator=(const ObservationNetworkInterface&) = delete;
    ObservationNetworkInterface& operator=(ObservationNetworkInterface&&) = delete;
    virtual ~ObservationNetworkInterface() = default;

    //-----------------------------------------------------------------------------
    //! Instantiates observation module objects using the provided mapping and the
    //! stored observation binding by filling the observation network modules map
    //! with these objects and the ids of their observation instances priovided in
    //! the given map.
    //!
    //! \param observationInstances     Contains library path and parameters
    //! \param  stochastics             The stochastics object
    //! \param  world                   The world object
    //! \param eventNetwork             EventNetwork instance
    //! \param sceneryPath              path of the scenery file
    //! @return                             Flag if the instantiation was successful
    //-----------------------------------------------------------------------------
    virtual bool Instantiate(const ObservationInstanceCollection& observationInstances,
                             StochasticsInterface* stochastics,
                             WorldInterface* world,
                             EventNetworkInterface* eventNetwork,
                             const std::string& sceneryPath,
                             DataBufferReadInterface* dataBuffer) = 0;

    //-----------------------------------------------------------------------------
    //! Returns the observation module mapping.
    //!
    //! @return                             Observation module mapping
    //-----------------------------------------------------------------------------
    virtual const std::map<int, ObservationModule*>& GetObservationModules() = 0;

    //-----------------------------------------------------------------------------
    //! Inits the observation network by calling the OpSimulationPreHook function pointer
    //! of the observation library with each observation module implementation as
    //! parameter.
    //!
    //! @return                             Flag if the init was successful
    //-----------------------------------------------------------------------------
    virtual bool InitAll() = 0;

    //-----------------------------------------------------------------------------
    //! Inits the network run by calling the OpSimulationPreRunHook function
    //! pointer of the observation library with each observation module instance.
    //!
    //! @return                             Flag if the run init was successful
    //-----------------------------------------------------------------------------
    virtual bool InitRun() = 0;

    //-----------------------------------------------------------------------------
    //! Updates the time step by calling the OpSimulationUpdateHook function pointer of the
    //! observation library with each observation module instance and the provided
    //! parameters.
    //!
    //! @param[in]  time                    Current time
    //! @param[out] runResult               The results of the run
    //! @return                             Flag if the update was successful
    //-----------------------------------------------------------------------------
    virtual bool UpdateTimeStep(int time, RunResult& runResult) = 0;

    //-----------------------------------------------------------------------------
    //! Finalizes the run by calling the OpSimulationUpdateHook function pointer of the
    //! observation library with each observation module instance and teh provided
    //! result parameter.
    //!
    //! @param[in] runResult                The results of the run
    //! @return                             Flag if the finalize was successful
    //-----------------------------------------------------------------------------
    virtual bool FinalizeRun(const RunResult& result) = 0;

    //-----------------------------------------------------------------------------
    //! Finalizes the observation network by calling the OpSimulationManagerPostHook
    //! pointer of the observation library with each observation module instance
    //! (and its ID) in each network simulation.
    //!
    //! @return                             Flag if the finalize was successful
    //-----------------------------------------------------------------------------
    virtual bool FinalizeAll() = 0;

    //-----------------------------------------------------------------------------
    //! Clears the modules mapping and deletes the observation module instances
    //! within and also unloads the observation bindings.
    //-----------------------------------------------------------------------------
    virtual void Clear() = 0;

};

} //namespace core
