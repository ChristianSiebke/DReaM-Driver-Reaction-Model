/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    CFModel.cpp
/// @author  Tobias Mayer
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @author  Leonhard Lücken
/// @date    Mon, 27 Jul 2009
/// @version $Id$
///
// The car-following model abstraction
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================


//#include <cmath>
#include "CFModel.h"
#include <iostream>
#include "agentInterface.h"
#include "AbstractLaneChangeModel.h"

// ===========================================================================
// DEBUG constants
// ===========================================================================
//#define DEBUG_FINALIZE_SPEED
//#define DEBUG_DRIVER_ERRORS
//#define DEBUG_EMERGENCYDECEL
//#define DEBUG_COND (true)
#define DEBUG_COND (veh->isSelected())



// ===========================================================================
// method definitions
// ===========================================================================
CFModel::CFModel(double Accel, double Decel, double maxAccel, double maxDecel, double Headwaytime, double CycleTime) :
    myAccel(Accel),
    myDecel(Decel),
    myEmergencyDecel(maxDecel),
    myApparentDecel(Decel),
    myHeadwayTime(Headwaytime),
    CycleTime(CycleTime*0.001)
{}


CFModel::~CFModel() {}

CFModel::VehicleVariables::~VehicleVariables() {}


double CFModel::brakeGap(const double *speed) const
{
        return brakeGapEuler(speed, myDecel, myHeadwayTime);
}


double CFModel::brakeGapEuler(const double *speed, double decel, double headwayTime) const {
    /* one possibility to speed this up is to calculate speedReduction * steps * (steps+1) / 2
       for small values of steps (up to 10 maybe) and store them in an array */
    double speedReduction = (-decel)*CycleTime;
    int steps = int(*speed / speedReduction);
    double a = CycleTime*(steps * *speed - speedReduction * steps * (steps + 1) / 2) + *speed * headwayTime;
    return CycleTime*(steps * *speed + speedReduction * steps * (steps + 1) / 2) + *speed * headwayTime;
}


double CFModel::freeSpeed(const double currentSpeed, const double decel, const double dist, const double targetSpeed, const bool onInsertion)
{
    // XXX: (Leo) This seems to be exclusively called with decel = myDecel (max deceleration) and is not overridden
    // by any specific CFModel. That may cause undesirable hard braking (at junctions where the vehicle
    // changes to a road with a lower speed limit).

        // adapt speed to succeeding lane, no reaction time is involved
        // when breaking for y steps the following distance g is covered
        // (drive with v in the final step)
        // g = (y^2 + y) * 0.5 * b + y * v
        // y = ((((sqrt((b + 2.0*v)*(b + 2.0*v) + 8.0*b*g)) - b)*0.5 - v)/b)
        const double v = CycleTime*(targetSpeed);
        if (dist < v) {
            return targetSpeed;
        }
        const double b = CycleTime*CycleTime*(decel);
        const double y = std::max(0.0, ((sqrt((b + 2.0 * v) * (b + 2.0 * v) + 8.0 * b * dist) - b) * 0.5 - v) / b);
        const double yFull = floor(y);
        const double exactGap = (yFull * yFull + yFull) * 0.5 * b + yFull * v + (y > yFull ? v : 0.0);
        const double fullSpeedGain = (yFull) * CycleTime*(decel);

        //std::cout << "b: " << b << ", y: " << y << ", yFull: " << yFull << ", exactGap: " << exactGap << ", fullSpeedGain: " << fullSpeedGain << std::endl;

        return (1/CycleTime)*(std::max(0.0, dist - exactGap) / (yFull + 1)) + fullSpeedGain + targetSpeed;

}

double CFModel::finalizeSpeed(const double *v_x, const double *v_x_max, const double *a_x_max, double *vPos, const double *MinGap, const AbstractLaneChangeModel *LCModel) const
{
    // save old v for optional acceleration computation
    const double oldV = *v_x;
    // apply deceleration bounds
    const double vMin = minNextSpeed(oldV);
    // aMax: Maximal admissible acceleration until the next action step, such that the vehicle's maximal
    // desired speed on the current lane will not be exceeded when the
    // acceleration is maintained until the next action step.
    double aMax = (*v_x_max - oldV) / CycleTime;
    // apply planned speed constraints and acceleration constraints 
    double vMax = std::min(oldV + CycleTime*(aMax), maxNextSpeed(*v_x, *v_x_max, *a_x_max));
    // do not exceed max decel even if it is unsafe

    vMax = std::max(vMin, vMax);
    // apply further speed adaptations
    double vNext = patchSpeedBeforeLC(vMax);
    // apply lane-changing related speed adaptations
    vNext = vMax;
    return vNext;
}


double CFModel::interactionGap(const double *v_x, const double *v_x_max, const double *a_x_max, const double *vL) const
{
    // Resolve the vsafe equation to gap. Assume predecessor has
    // speed != 0 and that vsafe will be the current speed plus acceleration,
    // i.e that with this gap there will be no interaction.
    const double vNext = std::min(maxNextSpeed(*v_x, *v_x_max, *a_x_max), *v_x_max);
    const double gap = (vNext - *vL) *
                       ((*v_x + *vL) / (2.*-myDecel) + myHeadwayTime) +
                       *vL * myHeadwayTime;

    // Don't allow timeHeadWay < deltaT situations.
    return std::max(gap, (vNext)/CycleTime);
}


double CFModel::maxNextSpeed(const double &speed, const double &v_x_max, const double &a_x_max) const
{
    double a = std::min(speed + (double) CycleTime*(a_x_max), v_x_max);
    return std::min(speed + (double) CycleTime*(a_x_max), v_x_max);
}


double CFModel::minNextSpeed(double speed) const
{

//        return std::max(speed - CycleTime*(getMaxDecel()), 0.);
        return std::max(speed + CycleTime*(myDecel), 0.);

}



double CFModel::insertionFollowSpeed(double *speed, double *gap2pred, double *predSpeed, double *predMaxDecel) {

        return maximumSafeFollowSpeed(gap2pred, speed, predSpeed, predMaxDecel, 0);

}


double CFModel::insertionStopSpeed(const double *speed, const double *gap, const double *v_x_max) const {

        return stopSpeed(speed, gap, v_x_max);

}


double CFModel::followSpeedTransient(double duration, double /*speed*/, double gap2pred, double predSpeed, double predMaxDecel)
{
    // minimium distance covered by the leader if braking
    double leaderMinDist = gap2pred + distAfterTime(duration, predSpeed, predMaxDecel);
    // if ego would not brake it could drive with speed leaderMinDist / duration
    // due to potentential ego braking it can safely drive faster
        // number of potential braking steps
        int a = (int)ceil(duration / CycleTime - CycleTime);
        // can we brake for the whole time?
        double spd = a * myDecel;
        double bg = brakeGap(&spd);
        if (bg <= leaderMinDist) {
            // braking continuously for duration
            // distance reduction due to braking
            double b = CycleTime * -getMaxDecel() * 0.5 * (a * a - a);

            return (b + leaderMinDist) / duration;
        } else {
            // @todo improve efficiency
            double bg = 0;
            double speed = 0;
            while (bg < leaderMinDist) {
                speed += CycleTime*(-myDecel);
                bg += CycleTime*(speed);
            }
            speed -= (1/CycleTime)*(bg - leaderMinDist);
            return speed;
        }

}

double
CFModel::distAfterTime(double t, double speed, const double accel) {
    if (accel >= 0.) {
        return (speed + 0.5 * accel * t) * t;
    }
    const double decel = -accel;
    if (speed <= decel * t) {
        // braking to a full stop
        return brakeGap(&speed);
    }

        // @todo improve efficiency
        double result = 0;
        while (t > 0) {
            speed -= CycleTime*(decel);
            result += std::max(0.0, CycleTime*(speed));
            t -= CycleTime;
        }
        return result;

}

double CFModel::estimateArrivalTime(double dist, double speed, double maxSpeed, double accel) {

    if (dist == 0.) {
        return 0.;
    }

    if ((accel < 0. && -0.5 * speed * speed / accel < dist) || (accel <= 0. && speed == 0.)) {
        // distance will never be covered with these values
        return 0;
    }

    if (accel == 0.) {
        return dist / speed;
    }

    double p = speed / accel;

    if (accel < 0.) {
        // we already know, that the distance will be covered despite breaking
        return (-p - sqrt(p * p + 2 * dist / accel));
    }

    // Here, accel > 0
    // t1 is the time to use the given acceleration
    double t1 = (maxSpeed - speed) / accel;
    // distance covered until t1
    double d1 = speed * t1 + 0.5 * accel * t1 * t1;
    if (d1 >= dist) {
        // dist is covered before changing the speed
        return (-p + sqrt(p * p + 2 * dist / accel));
    } else {
        return (-p + sqrt(p * p + 2 * d1 / accel)) + (dist - d1) / maxSpeed;
    }

}

double
CFModel::estimateArrivalTime(double dist, double initialSpeed, double arrivalSpeed, double maxSpeed, double accel, double decel) {

    if (dist <= 0) {
        return 0.;
    }

    // stub-assumptions


    double accelTime = (maxSpeed - initialSpeed) / accel;
    // "ballistic" estimate for the distance covered during acceleration phase
    double accelDist = accelTime * (initialSpeed + 0.5 * (maxSpeed - initialSpeed));
    double arrivalTime;
    if (accelDist >= dist * 0.5) {
        // maximal speed will not be attained during maneuver
        arrivalTime = 4 * sqrt(accelDist) / accel;
    } else {
        // Calculate time to move with constant, maximal lateral speed
        const double constSpeedTime = (dist - accelDist * 2) / maxSpeed;
        arrivalTime = accelTime + constSpeedTime;
    }
    return arrivalTime;
}


double
CFModel::avoidArrivalAccel(double dist, double time, double speed) {
    if (dist <= 0) {
        return -std::numeric_limits<double>::max();
    } else if (time * speed > 2 * dist) {
        // stop before dist is necessary. We need
        //            d = v*v/(2*a)
        return - 0.5 * speed * speed / dist;
    } else {
        // we seek the solution a of
        //            d = v*t + a*t*t/2
        return 2 * (dist / time - speed) / time;
    }
}


double
CFModel::getMinimalArrivalSpeed(double *dist, double *currentSpeed) const {
    // ballistic update
    return estimateSpeedAfterDistance(*dist - *currentSpeed * getHeadwayTime(), currentSpeed, -getMaxDecel());
}


double
CFModel::getMinimalArrivalSpeedEuler(double *dist, double *currentSpeed) const {
    double arrivalSpeedBraking;
    // Because we use a continuous formula for computing the possible slow-down
    // we need to handle the mismatch with the discrete dynamics
    if (*dist < *currentSpeed) {
        arrivalSpeedBraking = INVALID_SPEED; // no time left for braking after this step
        //	(inserted max() to get rid of arrivalSpeed dependency within method) (Leo)
    } else if (2 * (*dist - *currentSpeed * getHeadwayTime()) * -getMaxDecel() + *currentSpeed * *currentSpeed >= 0) {
        arrivalSpeedBraking = estimateSpeedAfterDistance(*dist - *currentSpeed * getHeadwayTime(), currentSpeed, -getMaxDecel());
    } else {
        arrivalSpeedBraking = getMaxDecel();
    }
    return arrivalSpeedBraking;
}




double CFModel::gapExtrapolation(const double duration, const double currentGap, double v1,  double v2, double a1, double a2, const double maxV1, const double maxV2) const {

    double newGap = currentGap;


        for (unsigned int steps = 1; steps * CycleTime <= duration; ++steps) {
            v1 = std::min(std::max(v1 + a1, 0.), maxV1);
            v2 = std::min(std::max(v2 + a2, 0.), maxV2);
            newGap += CycleTime * (v1 - v2);
        }


    return newGap;
}



double CFModel::passingTime(const double lastPos, const double passedPos, const double currentPos, const double lastSpeed, const double currentSpeed) {

    if (passedPos > currentPos || passedPos < lastPos) {
        std::stringstream ss;
        // Debug (Leo)

        const double lastCoveredDist = currentPos - lastPos;
        const double extrapolated = passedPos > currentPos ? CycleTime * (passedPos - lastPos) / lastCoveredDist : CycleTime * (currentPos - passedPos) / lastCoveredDist;
        return extrapolated;
    } else if (currentSpeed < 0) {

        return -1;
    }

    const double distanceOldToPassed = passedPos - lastPos;

        // euler update (constantly moving with currentSpeed during [0,TS])
        const double t = distanceOldToPassed / currentSpeed;
        return std::min(CycleTime, std::max(0., t)); //rounding errors could give results out of the admissible result range

}


double CFModel::speedAfterTime(const double t, const double v0, const double dist) {
    
        // euler: constant speed within [0,TS]
        return (1/CycleTime)*(dist);

}




double
CFModel::estimateSpeedAfterDistance(const double dist, const double *v, const double accel) const {
    // dist=v*t + 0.5*accel*t^2, solve for t and use v1 = v + accel*t
    return sqrt(2 * dist * accel + *v * *v);
}



double CFModel::maximumSafeStopSpeed(double *g /*gap*/, const double *v /*currentSpeed*/, bool onInsertion, const double headway) const {
    double vsafe;

        // XXX pass headway argument
        vsafe = maximumSafeStopSpeedEuler(g, v);

    if (myDecel != myEmergencyDecel) {

        if (vsafe < *v + CycleTime*(myDecel + NUMERICAL_EPS)) {
            // emergency deceleration required

            const double safeDecel = calculateEmergencyDeceleration(g, v, 0, 1);
            vsafe = *v + CycleTime*(myDecel + EMERGENCY_DECEL_AMPLIFIER * (safeDecel - myDecel));


            vsafe = std::max(vsafe,0.);

        }
    }

    return vsafe;
}


double
CFModel::maximumSafeStopSpeedEuler(double *gap, const double *v, double headway) const {
    //gap -= NUMERICAL_EPS; // lots of code relies on some slack XXX: it shouldn't...
    if (*gap <= 0) {
        return 0;
    } else if (*gap <= CycleTime*CycleTime*(-myDecel)) {
        // workaround for #2310
        return *v+CycleTime*(myDecel);
    }
    const double g = *gap;
    const double b = CycleTime*(-myDecel);
    const double t = headway >= 0 ? headway : myHeadwayTime;
    const double s = CycleTime;


    // h = the distance that would be covered if it were possible to stop
    // exactly after gap and decelerate with b every simulation step
    // h = 0.5 * n * (n-1) * b * s + n * b * t (solve for n)
    //n = ((1.0/2.0) - ((t + (pow(((s*s) + (4.0*((s*((2.0*h/b) - t)) + (t*t)))), (1.0/2.0))*sign/2.0))/s));
    const double n = floor(.5 - ((t + (sqrt(((s * s) + (4.0 * ((s * (2.0 * g / b - t)) + (t * t))))) * -0.5)) / s));
    const double h = 0.5 * n * (n - 1) * b * s + n * b * t;
    // compute the additional speed that must be used during deceleration to fix
    // the discrepancy between g and h
    const double r = (g - h) / (n * s + t);
    const double x = n * b + r;
    return x;
}


double
CFModel::maximumSafeStopSpeedBallistic(double g /*gap*/, double v /*currentSpeed*/, bool onInsertion, double headway) const {
    // decrease gap slightly (to avoid passing end of lane by values of magnitude ~1e-12, when exact stop is required)
    g = std::max(0., g);
    headway = headway >= 0 ? headway : -myHeadwayTime;

    // (Leo) Note that in contrast to the Euler update, for the ballistic update
    // the distance covered in the coming step depends on the current velocity, in general.
    // one exception is the situation when the vehicle is just being inserted.
    // In that case, it will not cover any distance until the next timestep by convention.

    // We treat the latter case first:
    if (onInsertion) {
        // The distance covered with constant insertion speed v0 until time tau is given as
        // G1 = tau*v0
        // The distance covered between time tau and the stopping moment at time tau+v0/b is
        // G2 = v0^2/(2b),
        // where b is an assumed constant deceleration (= myDecel)
        // We solve g = G1 + G2 for v0:
        const double btau = -myDecel * headway;
        const double v0 = -btau + sqrt(btau * btau + 2 * -myDecel * g);
        return v0;
    }

    // In the usual case during the driving task, the vehicle goes by
    // a current speed v0=v, and we seek to determine a safe acceleration a (possibly <0)
    // such that starting to break after accelerating with a for the time tau=headway
    // still allows us to stop in time.

    const double tau = headway;
    const double v0 = std::max(0., v);
    // We first consider the case that a stop has to take place within time tau
    if (v0 * tau >= 2 * g) {
        if (g == 0.) {
            if (v0 > 0.) {
                // indicate to brake as hard as possible
                return -std::numeric_limits<double>::max();
            } else {
                // stay stopped
                return 0.;
            }
        }
        // In general we solve g = v0^2/(-2a), where the the rhs is the distance
        // covered until stop when breaking with a<0
        const double a = -v0 * v0 / (2 * g);
        return v0 + a * CycleTime;
    }

    // The last case corresponds to a situation, where the vehicle may go with a positive
    // speed v1 = v0 + tau*a after time tau.
    // The distance covered until time tau is given as
    // G1 = tau*(v0+v1)/2
    // The distance covered between time tau and the stopping moment at time tau+v1/b is
    // G2 = v1^2/(2b),
    // where b is an assumed constant deceleration (= myDecel)
    // We solve g = G1 + G2 for v1>0:
    // <=> 0 = v1^2 + b*tau*v1 + b*tau*v0 - 2bg
    //  => v1 = -b*tau/2 + sqrt( (b*tau)^2/4 + b(2g - tau*v0) )

    const double btau2 = -myDecel * tau / 2;
    const double v1 = -btau2 + sqrt(btau2 * btau2 - myDecel * (2 * g - tau * v0));
    const double a = (v1 - v0) / tau;
    return v0 + a * CycleTime;
}


/** Returns the SK-vsafe. */
double CFModel::maximumSafeFollowSpeed(const double *gap, const double *egoSpeed, const double *predSpeed, const double *predMaxDecel, bool onInsertion) const
{

    // the speed is safe if allows the ego vehicle to come to a stop behind the leader even if
    // the leaders starts braking hard until stopped
    // unfortunately it is not sufficient to compare stopping distances if the follower can brake harder than the leader
    // (the trajectories might intersect before both vehicles are stopped even if the follower has a shorter stopping distance than the leader)
    // To make things safe, we ensure that the leaders brake distance is computed with an deceleration that is at least as high as the follower's.
    // @todo: this is a conservative estimate for safe speed which could be increased

//    // For negative gaps, we return the lowest meaningful value by convention
//    // XXX: check whether this is desireable (changes test results, therefore I exclude it for now (Leo), refs. #2575)

//    //      It must be done. Otherwise, negative gaps at high speeds can create nonsense results from the call to maximumSafeStopSpeed() below

//    if(gap<0){
//        if(MSGlobals::gSemiImplicitEulerUpdate){
//            return 0.;
//        } else {
//            return -INVALID_SPEED;
//        }
//    }

    // The following commented code is a variant to assure brief stopping behind a stopped leading vehicle:
    // if leader is stopped, calculate stopSpeed without time-headway to prevent creeping stop
    // NOTE: this can lead to the strange phenomenon (for the Krauss-model at least) that if the leader comes to a stop,
    //       the follower accelerates for a short period of time. Refs #2310 (Leo)
    //    const double headway = predSpeed > 0. ? myHeadwayTime : 0.;

    double headway = myHeadwayTime;
    double g = *gap + brakeGap(predSpeed);
    double x = maximumSafeStopSpeed(&g, egoSpeed, onInsertion, headway);

    return x;
}


double
CFModel::calculateEmergencyDeceleration(double *gap, const double *egoSpeed, double *predSpeed, double predMaxDecel) const {
    // There are two cases:
    // 1) Either, stopping in time is possible with a deceleration b <= predMaxDecel, then this value is returned
    // 2) Or, b > predMaxDecel is required in this case the minimal value b allowing to stop safely under the assumption maxPredDecel=b is returned

    // Apparent braking distance for the leader
    const double predBrakeDist = 0.5* 0; // predSpeed* *predSpeed/ - predMaxDecel;
    // Required deceleration according to case 1)
    const double b1 = 0.5* *egoSpeed* *egoSpeed/(*gap + predBrakeDist);

    if (b1 <= -  predMaxDecel) {
        // Case 1) applies

        return b1;
    }

    // Case 2) applies
    if (*gap <= 0.) {
        return - std::numeric_limits<double>::max();
    }
    // Required deceleration according to case 2)
    const double b2 = 0.5*(*egoSpeed * *egoSpeed - 0) / *gap; //*predSpeed * *predSpeed)/ *gap;

    return b2;
}










/****************************************************************************/
