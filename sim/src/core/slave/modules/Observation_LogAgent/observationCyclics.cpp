/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*               2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** \file  ObservationCyclics.cpp */
//-----------------------------------------------------------------------------

#include "observationCyclics.h"
#include <QStringList>

std::string ObservationCyclics::GetHeader()
{
    std::string header;
    for (auto it = samples.begin(); it != samples.end(); ++it)
    {
        if(it != samples.begin())
        {
            header += ", ";
        }

        header += it->first;
    }
    return header;
}

std::string ObservationCyclics::GetAgentHeader()
{
    std::string header;
    std::string columnHeader;
    for(std::map<std::string, std::vector<std::string>>::iterator it = samples.begin(); it != samples.end(); ++it)
    {
        columnHeader = it->first;
        if(columnHeader.at(1) == '1')
        {
            return header;
        }
        if(it != samples.begin())
        {
            header += ";";
        }
        header += columnHeader.erase(0, 3);
    }
    return header;
}

std::string ObservationCyclics::GetSamplesLine(std::uint32_t timeStepNumber)
{
    std::string sampleLine;
    for (auto it = samples.begin(); it != samples.end(); ++it)
    {
        const std::vector<std::string>& values = it->second;

        if (it != samples.begin())
        {
            sampleLine += ", ";
        }

        // not all channels are sampled until end of simulation time
        if (timeStepNumber < values.size())
        {
            sampleLine += values.at(timeStepNumber);
        }
    }

    return sampleLine;
}

std::vector<std::string> ObservationCyclics::GetAgentSamplesLine(std::uint32_t timeStepNumber)
{
    std::vector<std::string> agentSamplesLines;
    std::string sampleLine {};
    std::string agentId {"00"};
    std::string newAgentId {"00"};

    for (std::map<std::string, std::vector<std::string>>::iterator it = samples.begin(); it != samples.end(); ++it)
    {
        size_t pos = it->first.find(":");
        newAgentId = it->first.substr(0, pos);
        if(newAgentId != agentId)
        {
            agentId = newAgentId;
            agentSamplesLines.push_back(sampleLine);
            sampleLine.clear();
        }
        const std::vector<std::string>& values = it->second;

        if (!sampleLine.empty())
        {
            sampleLine += ";";
        }

        // not all channels are sampled until end of simulation time
        if (timeStepNumber < values.size())
        {
            sampleLine += values.at(timeStepNumber);
        }
    }

    return agentSamplesLines;
}

std::vector<int> ObservationCyclics::FilterSamplesLine(QString samplesLine, QString agentId)
{
    std::vector<int> indexAgent;
    if (!samplesLine.contains(agentId))
    {
        return indexAgent;
    }
    QStringList values = samplesLine.split(",");
    indexAgent.resize(values.size());
    for (int i=0; i< values.size(); ++i)
    {
        if (values[i].contains(agentId))
        {
            indexAgent[i] = 1;
        } else {
            indexAgent[i] = 0;
        }
    }
    return indexAgent;
}

QString ObservationCyclics::GetSamplesLineByAgent(QString samplesLine, std::vector<int> index)
{
    QString indexAgent;
    QStringList values = samplesLine.split(",");

    for (int i=0; i< values.size(); ++i)
    {
        if (index[i])
        {
            indexAgent += values[i] + ";";
        }
    }
    indexAgent.chop(1);
    return indexAgent;
}

void ObservationCyclics::Clear()
{
    timeSteps.clear();
    samples.clear();
}

//-----------------------------------------------------------------------------
//! Called by Observation Log Implementation
//! to log all Sample values in output.xml
//-----------------------------------------------------------------------------
void ObservationCyclics::Insert(int time, const std::string &key, const std::string &value)
{
    timeSteps.insert(timeSteps.end(), time);

    auto it = samples.find(key);

    if (it == samples.end())
    {
        auto [newIt, newElement] = samples.emplace(key, timeSteps.size() - 1);
        newIt->second.push_back(value);
    }
    else
    {
        // fill up skipped time steps (e.g. another agent has been instantiated in between -> inserted new time step in scheduling)
        it->second.resize(timeSteps.size() - 1);
        it->second.push_back(value);
    }
}
