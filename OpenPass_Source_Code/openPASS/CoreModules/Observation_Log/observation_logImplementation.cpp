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
/** \file  observation_LogImplementation */
//-----------------------------------------------------------------------------

#include <unordered_map>
#include <variant>
#include <vector>

#include "Common/commonTools.h"
#include "Common/observationTypes.h"
#include "Common/openPassUtils.h"

#include "Interfaces/dataStoreInterface.h"
#include "Interfaces/stochasticsInterface.h"
#include "Interfaces/parameterInterface.h"
#include "Interfaces/worldInterface.h"

#include "observation_logImplementation.h"

#include "observationCyclics.h"
#include "runStatisticCalculation.h"
#include "runStatistic.h"

ObservationLogImplementation::ObservationLogImplementation(
        SimulationSlave::EventNetworkInterface* eventNetwork,
        StochasticsInterface* stochastics,
        WorldInterface* world,
        const ParameterInterface* parameters,
        const CallbackInterface* callbacks,
        DataStoreReadInterface* dataStore) :
    ObservationInterface(stochastics,
                         world,
                         parameters,
                         callbacks,
                         dataStore),
    runtimeInformation(parameters->GetRuntimeInformation()),
    eventNetwork(eventNetwork),
    dataStore(dataStore),
    fileHandler{*dataStore}
{
}

void ObservationLogImplementation::SlavePreHook()
{
    std::string filename{"simulationOutput.xml"};

    try
    {
        filename = GetParameters()->GetParametersString().at("OutputFilename");
    }
    catch (const std::out_of_range&)
    {
        LOG(CbkLogLevel::Warning, "Using default output filename: " + filename);
    }

    try
    {
        fileHandler.SetCsvOutput(GetParameters()->GetParametersBool().at("LoggingCyclicsToCsv"));
    }
    catch (const std::out_of_range&)
    {
        std::string msg = "Mandatory config parameter 'LoggingCyclicsToCsv' is missing";
        LOG(CbkLogLevel::Error, msg);
        throw std::runtime_error(msg);
    }

    try
    {
        for (const auto& loggingGroup : GetParameters()->GetParametersStringVector().at("LoggingGroups"))
        {
            try
            {
                const auto& groupColumns = LOGGINGGROUP_DEFINITIONS.at(loggingGroup);
                selectedColumns.insert(groupColumns.begin(), groupColumns.end());
            }
            catch (const std::out_of_range&)
            {
                std::string msg = "Unsupported LoggingGroup '" + loggingGroup + "'";
                LOG(CbkLogLevel::Error, msg);
                throw std::runtime_error(msg);
            }
        }
    }
    catch (const std::out_of_range&)
    {
        const auto& traceColumns = LOGGINGGROUP_DEFINITIONS.at("Trace");
        selectedColumns.insert(traceColumns.begin(), traceColumns.end());

        LOG(CbkLogLevel::Warning, "No LoggingGroups configured. Defaulting to 'Trace'");
    }

    fileHandler.SetOutputLocation(runtimeInformation.directories.output, filename);
    fileHandler.SetSceneryFile(std::get<std::string>(dataStore->GetStatic("SceneryFile").at(0)));
    fileHandler.WriteStartOfFile(runtimeInformation.versions.framework.str());
}

void ObservationLogImplementation::SlavePreRunHook()
{
    runStatistic = RunStatistic(GetStochastics()->GetRandomSeed());
    cyclics.Clear();
}

void ObservationLogImplementation::SlavePostRunHook(const RunResultInterface& runResult)
{
    RunStatisticCalculation::DetermineEgoCollision(runStatistic, runResult, GetWorld());
    runStatistic.VisibilityDistance = GetWorld()->GetVisibilityDistance();

    const auto dsCyclics = dataStore->GetCyclic(std::nullopt, std::nullopt, "*");

    for (const CyclicRow& dsCyclic : *dsCyclics)
    {
        std::stringstream entityStr;
        entityStr << std::setw(2) << std::setfill('0') << dsCyclic.entityId;

        std::visit(openpass::utils::FlatParameter::to_string([this, &dsCyclic, &entityStr](const std::string& valueStr)
        {
            if (selectedColumns.find(dsCyclic.key) != selectedColumns.end())
            {
                cyclics.Insert(dsCyclic.timestamp, entityStr.str() + ":" + dsCyclic.key, valueStr);
            }
        }), dsCyclic.value);
    }

    fileHandler.WriteRun(runResult, runStatistic, cyclics);
}

void ObservationLogImplementation::SlavePostHook()
{
    fileHandler.WriteEndOfFile();
}
