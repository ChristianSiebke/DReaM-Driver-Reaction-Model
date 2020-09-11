/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
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
