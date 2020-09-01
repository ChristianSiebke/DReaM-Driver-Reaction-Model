/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*               2016, 2017, 2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include <string>
#include <list>
#include <QXmlStreamWriter>

#include "include/observationInterface.h"

class RunStatistic
{
public:
    enum class StopReason
    {
        DueToTimeOut
    };

    RunStatistic(std::uint32_t randomSeed);

    void AddStopReason(int time, StopReason reason);
    void WriteStatistics(QXmlStreamWriter* fileStream);

    // general
    int StopTime = -1; //this stays on UNDEFINED_NUMBER, if due time out -> replace in c#
    bool EgoCollision = false;
    double TotalDistanceTraveled = 0.0;
    double EgoDistanceTraveled = 0.0;
    double VisibilityDistance = -999.0; //Visibility distance of world in current run (defined in slaveConfig.xml)

    static QString BoolToString(bool b);

    ~RunStatistic() = default;

private:
    std::uint32_t _randomSeed;
    std::list<int> _followerIds;

    static const QString StopReasonsStrings[];
    int _stopReasonIdx = static_cast<int>(StopReason::DueToTimeOut);
}; // class RunStatistic


