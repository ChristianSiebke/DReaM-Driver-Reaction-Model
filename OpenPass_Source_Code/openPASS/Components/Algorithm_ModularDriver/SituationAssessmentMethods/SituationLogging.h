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
//! \addtogroup Algorithm_ModularDriver
//! \addtogroup Algorithm_SituationAssessment
//-----------------------------------------------------------------------------

#pragma once
#include <boost/geometry/geometries/adapted/c_array.hpp>
#include "Common/boostGeometryCommon.h"
#include "osi/osi_sensordata.pb.h"
#include "SituationCalculation.h"
#include "Interfaces/observationInterface.h"
#include "Interfaces/modelInterface.h"

class SituationLogging
{
public:
    SituationLogging(int cycleTime, SituationCalculation *situationCalculation);

    bool Initialize(std::vector<std::string> LoggingGroups, ObservationInterface *observer);

    void CheckForLoggingStatesAndLog(int time, std::string pov);
    void LoggDoubleValue(std::string keylog, double value, int time);
    void LoggRelationValues(const std::vector<int> *Agents, std::vector<double> values, std::string key, int time);

private:

    bool log_ttc = false;
    bool log_ettc = false;
    bool log_thw = false;
    bool log_speeding = false;
    ObservationInterface *observer;

    SituationCalculation *situationCalculation;
};
