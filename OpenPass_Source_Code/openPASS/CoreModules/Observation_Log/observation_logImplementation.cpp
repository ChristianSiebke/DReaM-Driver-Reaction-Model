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
/** \file  ObservationLogImplementation */
//-----------------------------------------------------------------------------

#include <cassert>
#include <sstream>
#include <QDir>

#include "Interfaces/stochasticsInterface.h"
#include "Interfaces/worldInterface.h"
#include "Interfaces/parameterInterface.h"
#include "observation_logImplementation.h"
#include "runStatisticCalculation.h"

ObservationLogImplementation::ObservationLogImplementation(SimulationSlave::EventNetworkInterface* eventNetwork,
        StochasticsInterface* stochastics,
        WorldInterface* world,
        const ParameterInterface* parameters,
        const CallbackInterface* callbacks) :
    ObservationInterface(stochastics,
                         world,
                         parameters,
                         callbacks),
    runtimeInformation(parameters->GetRuntimeInformation()),
    eventNetwork(eventNetwork)
{
    // read parameters
    try
    {
        fileHandler.SetSceneryFile(parameters->GetParametersString().at("SceneryFile"));
        fileHandler.SetCsvOutput(parameters->GetParametersBool().at("LoggingCyclicsToCsv"));
        auto loggingGroupsfromConfig = parameters->GetParametersStringVector().at("LoggingGroups");
        for (auto loggingGroup : loggingGroupsfromConfig)
        {
            if (loggingGroup == "Trace")
            {
                loggingGroups.push_back(LoggingGroup::Trace);
                continue;
            }
            if (loggingGroup == "Visualization")
            {
                loggingGroups.push_back(LoggingGroup::Visualization);
                continue;
            }
            if (loggingGroup == "RoadPosition")
            {
                loggingGroups.push_back(LoggingGroup::RoadPosition);
                continue;
            }
            if (loggingGroup == "RoadPositionExtended")
            {
                loggingGroups.push_back(LoggingGroup::RoadPositionExtended);
                continue;
            }
            if (loggingGroup == "Vehicle")
            {
                loggingGroups.push_back(LoggingGroup::Vehicle);
                continue;
            }
            if (loggingGroup == "Sensor")
            {
                loggingGroups.push_back(LoggingGroup::Sensor);
                continue;
            }
            if (loggingGroup == "SensorExtended")
            {
                loggingGroups.push_back(LoggingGroup::SensorExtended);
                continue;
            }
            if (loggingGroup == "Driver")
            {
                loggingGroups.push_back(LoggingGroup::Driver);
                continue;
            }

            const std::string msg = "There is no logging group named " + loggingGroup;
            LOG(CbkLogLevel::Error, msg);
            throw std::runtime_error(msg);
        }
    }
    catch (...)
    {
        const std::string msg = COMPONENTNAME + " could not init parameters";
        LOG(CbkLogLevel::Error, msg);
        throw std::runtime_error(msg);
    }
}

//-----------------------------------------------------------------------------
//! \brief Logs a key/value pair
//!
//! @param[in]     time      current time
//! @param[in]     agentId   agent identifier
//! @param[in]     group     LoggingGroup the key/value pair should be assigned to
//! @param[in]     key       Key of the value to log
//! @param[in]     value     Value to log
//-----------------------------------------------------------------------------
void ObservationLogImplementation::Insert(int time,
        int agentId,
        LoggingGroup group,
        const std::string& key,
        const std::string& value)
{
    if (std::find(loggingGroups.cbegin(), loggingGroups.cend(), group) == loggingGroups.cend())
    {
        //ignore the value, because the specified group isn't logged
        return;
    }
    std::string extendedKey = (agentId < 10 ? "0" : "") + std::to_string(agentId) + ":" + key;
    cyclics.Insert(time, extendedKey, value);
}

//-----------------------------------------------------------------------------
//! \brief Logs an event
//!
//! @param[in]     event     Shared pointer to the event to log
//-----------------------------------------------------------------------------
void ObservationLogImplementation::InsertEvent(std::shared_ptr<EventInterface> event)
{
    eventNetwork->InsertEvent(event);
}

void ObservationLogImplementation::SlavePreHook()
{
    fileHandler.SetOutputDir(runtimeInformation.directories.output);
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
    RunStatisticCalculation::CalculateTotalDistanceTraveled(runStatistic, GetWorld());

    fileHandler.WriteRun(runResult, runStatistic, cyclics, GetWorld(), eventNetwork);
}

void ObservationLogImplementation::SlavePostHook()
{
    fileHandler.WriteEndOfFile();
}
