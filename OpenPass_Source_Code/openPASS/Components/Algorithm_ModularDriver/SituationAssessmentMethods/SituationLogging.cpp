/******************************************************************************
* Copyright (c) 2019 TU Dresden
* Copyright (c) 2019 AMFD GmbH
*
* This program and the accompanying materials are made available under the
* terms of the Eclipse Public License 2.0 which is available at
* https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*****************************************************************************/

//-----------------------------------------------------------------------------
//! @file  SituationLogging.h
//! @author  Konstantin Blenz
//! @date    Tue, 03.12.2019
//! @brief provide data to the observer for logging of criticality values
//!
//-----------------------------------------------------------------------------
//! \addtogroup Algorithm_SituationAssessment
//-----------------------------------------------------------------------------

#include "SituationLogging.h"
#include "ContainerStructures.h"
#include "SituationCalculation.h"

#include <memory>
#include <qglobal.h>

SituationLogging::SituationLogging(int cycleTime, SituationCalculation *situationCalculation) :
    situationCalculation(situationCalculation)
{
}

void SituationLogging::CheckForLoggingStatesAndLog(int time, std::string pov)
{
    if (log_ttc)
    {
        std::string key = pov + "ttc";
        std::vector<int> AgentsWithTtc;
        std::vector<double> ttcs = situationCalculation->CalculateTTC(&AgentsWithTtc);
        LoggRelationValues(&AgentsWithTtc, ttcs, key, time);
    }
    if (log_ettc)
    {
        std::string key = pov + "ettc";
        std::vector<int> AgentsWithEttc;
        std::vector<double> ettcs = situationCalculation->CalculateETTC(&AgentsWithEttc);
        LoggRelationValues(&AgentsWithEttc, ettcs, key, time);
    }
    if (log_thw)
    {
        std::string key = pov + "thw";
        std::vector<int> AgentsWithThw;
        std::vector<double> thws = situationCalculation->CalculateTHW(&AgentsWithThw);
        LoggRelationValues(&AgentsWithThw, thws, key, time);
    }
    if (log_speeding)
    {
        double speedLimit = situationCalculation->CheckSignsForRelevantSpeedLimit().GetCurrentSpeedLimit();
        double violationDifference = situationCalculation->CheckForSpeedLimitViolation(speedLimit);
        if (violationDifference > 0)
        {
            std::string keylog = pov + "Speeding";
            LoggDoubleValue(keylog, violationDifference, time);
        }
    }
}

void SituationLogging::LoggDoubleValue(std::string keylog, double value, int time)
{
    observer->Insert(time,
                     situationCalculation->GetEgoId(),
                     LoggingGroup::Driver,
                     keylog,
                     std::to_string(value));
}

void SituationLogging::LoggRelationValues(const std::vector<int> *agents, std::vector<double> values, std::string key, int time)
{
    if (agents->size()>0)
    {
        int egoid = situationCalculation->GetEgoId();
        uint32_t i = 0;
        for (std::vector<int>::const_iterator it = agents->begin() ; it != agents->end(); ++it)
        {
            int agid = *it;
            std::string keylog = key + (agid < 10 ? "0" : "") + (agid < 100 && agid >= 10 ? "0" : "") + std::to_string(agid);
            const double value = values[i];
            const std::string valuestring = std::to_string(value);
            observer->Insert(time,
                             egoid,
                             LoggingGroup::Driver,
                             keylog,
                             valuestring);
            i++;
        }
    }
}

bool SituationLogging::Initialize(std::vector<std::string> loggingGroups, ObservationInterface *observation)
{
    observer = observation;

        for (std::vector<std::string>::iterator it = loggingGroups.begin() ; it != loggingGroups.end(); it++)
            {
                if (*it == "ttc")
                {
                    log_ttc = true;
                }
                if (*it == "ettc")
                {
                    log_ettc = true;
                }
                if (*it == "thw")
                {
                    log_thw = true;
                }
                if (*it == "speeding")
                {
                    log_speeding = true;
                }
            }
    if (log_ttc || log_ettc || log_thw || log_speeding)
    {
        return true;
    }

    return false;
}


