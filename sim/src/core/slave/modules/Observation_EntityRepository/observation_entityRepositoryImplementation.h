/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
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
#include <regex>

#include <QFile>
#include <QTextStream>

#include "common/runtimeInformation.h"
#include "include/observationInterface.h"
#include "include/dataStoreInterface.h"

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
class ObservationEntityRepositoryImplementation : ObservationInterface
{
public:
    const std::string COMPONENTNAME = "ObservationLog";

    ObservationEntityRepositoryImplementation(SimulationSlave::EventNetworkInterface* eventNetwork,
                                   StochasticsInterface* stochastics,
                                   WorldInterface* world,
                                   const ParameterInterface* parameters,
                                   const CallbackInterface* callbacks,
                                   DataStoreReadInterface* dataStore);
    ObservationEntityRepositoryImplementation(const ObservationEntityRepositoryImplementation&) = delete;
    ObservationEntityRepositoryImplementation(ObservationEntityRepositoryImplementation&&) = delete;
    ObservationEntityRepositoryImplementation& operator=(const ObservationEntityRepositoryImplementation&) = delete;
    ObservationEntityRepositoryImplementation& operator=(ObservationEntityRepositoryImplementation&&) = delete;
    virtual ~ObservationEntityRepositoryImplementation() override = default;

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
    void WriteEntities(AcyclicRowRefs& entities, QTextStream& stream);

    DataStoreReadInterface* dataStore;

    int runNumber = 0; //!< run number

    QString directory;
    QString filenamePrefix{"Repository"};

    bool writePersitent{true};
    bool persistentInSeparateFile{false};
};


