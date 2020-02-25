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
//! @file  TargetBraking.cpp
//! @author  Konstantin Blenz
//! @date    Tue, 03.12.2019
//! @brief calculate relations while assess the situation
//!
//-----------------------------------------------------------------------------
//! \addtogroup Algorithm_SituationAssessment
//-----------------------------------------------------------------------------

#include "TargetBraking.h"
#include "ContainerStructures.h"

TargetBraking::TargetBraking(int cycleTime, StochasticsInterface *stochastics):
    cycleTime(cycleTime),
    stochastics(stochastics)
{
}

double TargetBraking::ReactOnStopSign(StopSign *StopSign, double v_long, double distRefToLed, double v_wish, double acomfort)
{
    double dsStop = StopSign->Sign->relativeDistance - distRefToLed;
    double dsStop_next = dsStop - v_long * cycleTime*0.001;
    double delta_v_Ego = 0;

    if (targetbrakestarted)
    {
        if (dsStop > 1)
        {
            t_brk += cycleTime*0.001;
            delta_v_Ego = NextRealBrakeVelocity(a,b,c,t_brk) - v_long;
        }
        else
        {
            if (v_long<=0.1)
            {
                delta_v_Ego = -v_long;
                Reset();
            }
            else
            {
                delta_v_Ego = AccTargetBrake(v_long, dsStop) * cycleTime * 0.001;
                Reset();
            }
        }
    }
    else
    {
        if (v_long*3.6 > 40)
        {
            t_a = 0.6624*v_long + 2.06;
            double sbrake = v_long/5 * t_a - 2/3 * v_long * t_a + v_long * t_a;
            if (dsStop > sbrake)
            {
                if (dsStop_next < sbrake)
                {
                    if (dsStop > 1)
                    {
                        targetbrakestarted=true;
                        t_brk = 0;
                        a = v_long/pow(t_a,4);
                        b = 2*v_long / (t_a*t_a);
                        c = v_long;
                        t_brk += cycleTime*0.001;
                        delta_v_Ego = NextRealBrakeVelocity(a,b,c,t_brk) - v_long;
                    }
                    else
                    {
                        delta_v_Ego = AccTargetBrake(v_long, dsStop) * cycleTime*0.001;
                    }
                }
                else
                {
                    delta_v_Ego = CarFollowingModel->freeSpeed(v_long, acomfort, 0, v_wish, false) - v_long; //CalcFreeVelocityDiffWish(&v_long);
                }
            }
            else
            {
                delta_v_Ego = AccTargetBrake(v_long, dsStop) * cycleTime*0.001;
            }
        }
        else
        {
            delta_v_Ego = AccTargetBrake(v_long, dsStop) * cycleTime*0.001;
        }
    }

    return delta_v_Ego;
}

double TargetBraking::AccTargetBrake(double v, double ds)
{
    return (- v*v / (2*ds));
}

double TargetBraking::NextRealBrakeVelocity(double a, double b, double c, double t)
{
    return a*pow(t,4) - b*t*t + c;
}
