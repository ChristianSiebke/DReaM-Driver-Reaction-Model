/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*               2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/*!
* \file  ObservationLogImplementation.h
* \brief Adds the RunStatistic information to the simulation output.
* \details  Writes the RunStatistic information into the simulation output.
*           Also manages the stop reasons of the simulation.
*/
//-----------------------------------------------------------------------------

#pragma once

#include <set>
#include <string>
#include <tuple>

#include <QFile>
#include <QTextStream>

#include "Common/runtimeInformation.h"
#include "Interfaces/observationInterface.h"

#include "observationCyclics.h"
#include "observationFileHandler.h"
#include "runStatistic.h"

namespace SimulationSlave {
class EventNetworkInterface;
}

class DataStoreReadInterface;

//-----------------------------------------------------------------------------
/** \brief This class adds the RunStatistic information to the simulation output.
*   \details This class inherits the ObservationLogGeneric which creates the basic simulation output
*            and adds the RunStatistic information to the output.
*            This class also manages the stop reasons of the simulation.
*
*   \ingroup ObservationLog
*/
//-----------------------------------------------------------------------------
class ObservationLogImplementation : ObservationInterface
{
public:
    const std::string COMPONENTNAME = "ObservationLog";

    ObservationLogImplementation(SimulationSlave::EventNetworkInterface* eventNetwork,
                                   StochasticsInterface* stochastics,
                                   WorldInterface* world,
                                   const ParameterInterface* parameters,
                                   const CallbackInterface* callbacks,
                                   DataStoreReadInterface* dataStore);
    ObservationLogImplementation(const ObservationLogImplementation&) = delete;
    ObservationLogImplementation(ObservationLogImplementation&&) = delete;
    ObservationLogImplementation& operator=(const ObservationLogImplementation&) = delete;
    ObservationLogImplementation& operator=(ObservationLogImplementation&&) = delete;
    virtual ~ObservationLogImplementation() override = default;

    virtual void SlavePreHook() override;
    virtual void SlavePreRunHook() override;
    virtual void SlavePostRunHook(const RunResultInterface& runResult) override;
    virtual void SlaveUpdateHook(int, RunResultInterface&) override {}
    virtual void MasterPreHook() override {}
    virtual void MasterPostHook(const std::string&) override {}
    virtual void SlavePostHook() override;


    virtual const std::string SlaveResultFile() override
    {
        return "";
    }

private:
    const openpass::common::RuntimeInformation& runtimeInformation;
    SimulationSlave::EventNetworkInterface* eventNetwork;
    DataStoreReadInterface* dataStore;
    ObservationFileHandler fileHandler;
    ObservationCyclics cyclics;
    RunStatistic runStatistic = RunStatistic(-1);
    std::vector<std::string> selectedColumns;
    std::vector<std::pair<std::string,std::string>> selectedRegexColumns;
};


