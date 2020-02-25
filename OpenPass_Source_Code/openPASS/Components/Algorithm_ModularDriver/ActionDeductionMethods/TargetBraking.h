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
//! @file  TargetBraking.h
//! @author  Konstantin Blenz
//! @date    Tue, 03.12.2019
//! @brief calculate relations while assess the situation
//!
//-----------------------------------------------------------------------------
//! \addtogroup Algorithm_SituationAssessment
//-----------------------------------------------------------------------------

#pragma once
#include "Interfaces/stochasticsInterface.h"
#include "ContainerStructures.h"
#include "CFModel.h"

class TargetBraking
{
public:
    TargetBraking(int cycleTime, StochasticsInterface *stochastics);

    double ReactOnStopSign(StopSign *StopSign, double v_long, double distRefToLed, double v_wish, double acomfort);

    double NextRealBrakeVelocity(double a, double b, double c, double t_brk);

    double AccTargetBrake(double v_long, double dsStop);

    void Reset()
    {
        targetbrakestarted = false;
    }
    void UpdateCFModel(CFModel *CarFollowingModel)
    {
        this->CarFollowingModel = CarFollowingModel;
    }

private:

    StochasticsInterface *stochastics;
    int cycleTime;
    CFModel *CarFollowingModel;

    bool targetbrakestarted = false;
    double t_brk = 0;
    double t_a = 0;
    double a = 0;
    double b = 0;
    double c = 0;

};
