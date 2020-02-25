/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    CFModel_Daniel1.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Tue, 05 Jun 2012
/// @version $Id$
///
// The original Krauss (1998) car-following model and parameter
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================

#include "CFModel_Daniel1.h"
#include "AbstractLaneChangeModel.h"

// ===========================================================================
// method definitions
// ===========================================================================
CFModel_Daniel1::CFModel_Daniel1(double Accel, double Decel, double maxAccel, double maxDecel, double Headwaytime, double CycleTime) :
    CFModel(Accel, Decel, maxAccel, maxDecel, Headwaytime, CycleTime),
    myDawdle(0),
    myTauDecel(-myDecel * myHeadwayTime)

{ }


CFModel_Daniel1::~CFModel_Daniel1() {}


double CFModel_Daniel1::finalizeSpeed(const double *v_x, const double *v_x_max, const double *a_x_max, double *vPos, const double *MinGap, const AbstractLaneChangeModel *LCModel) const {
    const double oldV = *v_x; // save old v for optional acceleration computation
    const double vSafe = *vPos; // process stops
    // we need the acceleration for emission computation;
    //  in this case, we neglect dawdling, nonetheless, using
    //  vSafe does not incorporate speed reduction due to interaction
    //  on lane changing
    const double vMin = getSpeedAfterMaxDecel(oldV);
    const double vMax = std::min(std::min(*v_x_max, maxNextSpeed(*v_x, *v_x_max, *a_x_max)), vSafe);

    return LCModel->patchSpeed(&vMin, std::max(vMin, vMax), &vMax, v_x, v_x_max, a_x_max, this, MinGap); // agent->GetLaneChangeModel()->patchSpeed(vMin, std::max(vMin, vMax), vMax, *this);
}

double CFModel_Daniel1::followSpeed(const double &speed, const double &gap, const double &predSpeed, const double &v_x_max) const {

   return std::min(_vsafe(gap, predSpeed),maxNextSpeed(speed, v_x_max, myAccel));
}


double CFModel_Daniel1::stopSpeed(const double *speed, const double *gap, const double *v_x_max) const {
    double v = 0;
    return std::min(_vsafe(*gap, v), maxNextSpeed(*speed, *v_x_max, myAccel));
}


double CFModel_Daniel1::dawdle(double speed) const {
    return std::max(0., speed + CycleTime*(0 * -myDecel * 1));
}


/** Returns the SK-vsafe. */
double CFModel_Daniel1::_vsafe(const double &gap, const double &predSpeed) const {
    if (predSpeed <= 0.5 && gap < 2) {
        return 0;
    }

    double vsafe = (double)(-1. * myTauDecel + sqrt((myTauDecel * myTauDecel) + (predSpeed * predSpeed) + (2. * -myDecel * gap)));

    return vsafe;
}


CFModel* CFModel_Daniel1::duplicate(double Accel, double Decel, double maxAccel, double maxDecel, double Headwaytime, double CycleTime) const {
    return new CFModel_Daniel1(Accel, Decel, maxAccel, maxDecel, Headwaytime, CycleTime);
}
