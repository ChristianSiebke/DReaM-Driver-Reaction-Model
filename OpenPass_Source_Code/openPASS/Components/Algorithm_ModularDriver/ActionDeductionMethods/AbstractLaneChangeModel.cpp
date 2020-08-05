/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    AbstractLaneChangeModel.cpp
/// @author  Daniel Krajzewicz
/// @author  Friedemann Wesner
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @author  Leonhard Luecken
/// @date    Fri, 29.04.2005
/// @version $Id$
///
// Interface for lane-change models
/****************************************************************************/

// ===========================================================================
// DEBUG
// ===========================================================================
//#define DEBUG_TARGET_LANE
//#define DEBUG_SHADOWLANE



// ===========================================================================
// included modules
// ===========================================================================


#include "AbstractLaneChangeModel.h"
#include "LCM_LC2013.h"

/* -------------------------------------------------------------------------
 * static members
 * ----------------------------------------------------------------------- */
bool AbstractLaneChangeModel::myAllowOvertakingRight(false);
bool AbstractLaneChangeModel::myLCOutput(false);
bool AbstractLaneChangeModel::myLCStartedOutput(false);
bool AbstractLaneChangeModel::myLCEndedOutput(false);
const double AbstractLaneChangeModel::NO_NEIGHBOR(std::numeric_limits<double>::max());

/* -------------------------------------------------------------------------
 * AbstractLaneChangeModel-methods
 * ----------------------------------------------------------------------- */

AbstractLaneChangeModel* AbstractLaneChangeModel::build(int lcmId, double Cycletime, double SpeedGain, double KeepRight, double Cooperative, CFModel *CarFollowingModel, const VehicleModelParameters vehicleParameters)
{

    switch (lcmId)
    {
        case (0):
            return new LCM_LC2013(Cycletime, SpeedGain, KeepRight, Cooperative, CarFollowingModel, vehicleParameters);
        default:
            return new LCM_LC2013(Cycletime, SpeedGain, KeepRight, Cooperative, CarFollowingModel, vehicleParameters);
    }

}

AbstractLaneChangeModel::AbstractLaneChangeModel(double Cycletime, const LaneChangeModel model, const CFModel *CarFollowingModel, VehicleModelParameters vehicleParameters) :
    myOwnState(0),
    myPreviousState(0),
    myPreviousState2(0),
    mySpeedLat(0),
    myCommittedSpeed(0),
    myLaneChangeCompletion(1.0),
    myLaneChangeDirection(0),
    myManeuverDist(0.),
    myAlreadyChanged(false),
    myShadowLane(nullptr),
    myLastLateralGapLeft(0.),
    myLastLateralGapRight(0.),
    myLastLeaderGap(0.),
    myLastFollowerGap(0.),
    myLastLeaderSecureGap(0.),
    myLastFollowerSecureGap(0.),
    myLastOrigLeaderGap(0.),
    myLastOrigLeaderSecureGap(0.),
    myCarFollowModel(CarFollowingModel),

    Cycletime(Cycletime),
    vehicleParameters(vehicleParameters),
    myModel(model),
    myAmOpposite(false)

{
    myAllowOvertakingRight=false;
}

AbstractLaneChangeModel::~AbstractLaneChangeModel() {
}

void AbstractLaneChangeModel::setOwnState(const int state) {
    myPreviousState2 = myPreviousState;
    myOwnState = state;
    myPreviousState = state; // myOwnState is modified in prepareStep so we make a backup
}

void
AbstractLaneChangeModel::updateSafeLatDist(const double travelledLatDist) {
}


void
AbstractLaneChangeModel::setManeuverDist(const double dist) {

    myManeuverDist = dist;
}


double
AbstractLaneChangeModel::getManeuverDist() const {
    return myManeuverDist;
}


bool AbstractLaneChangeModel::congested(SurroundingMovingObjectsData* neighLeader, egoData* agent) {
    if (neighLeader == 0) {
        return false;
    }
    // Congested situation are relevant only on highways (maxSpeed > 70km/h)
    // and congested on German Highways means that the vehicles have speeds
    // below 60km/h. Overtaking on the right is allowed then.

    if ((vehicleParameters.maxVelocity <= 70.0 / 3.6)) {

        return false;
    }

    return false;
}

bool AbstractLaneChangeModel::predInteraction(bool lexists, SurroundingMovingObjectsData* leader, egoData* agent) {
    if (!lexists) {
        return false;
    }
    // let's check it on highways only
    if (leader->GetState()->velocity_long < (80.0 / 3.6)) {
        return false;
    }
    return *leader->GetDistanceToEgo() < myCarFollowModel->interactionGap(&agent->GetState_Ego()->velocity_long, &vehicleParameters.maxVelocity, &vehicleParameters.maxAcceleration, &leader->GetState()->velocity_long);
}

double
AbstractLaneChangeModel::computeSpeedLat(egoData* agent, double& maneuverDist) {

    return agent->GetState()->velocity_y;

}


double
AbstractLaneChangeModel::getAssumedDecelForLaneChangeDuration() const {
}


bool
AbstractLaneChangeModel::cancelRequest(int state, int laneOffset) {
    // store request before canceling
    myCanceledStates[laneOffset] |= state;
    int ret = state;
    return ret != state;
}

int AbstractLaneChangeModel::getWaitingSeconds(double velocity_x)
{
    if (velocity_x<0.1)
    {
        tWaiting = tWaiting + 100;
    }
    else
    {
        tWaiting = 0;
    }
    return tWaiting;
}


double
AbstractLaneChangeModel::estimateLCDuration(const double speed, const double remainingManeuverDist, const double decel, const driverInformation* agent) const {

    if(remainingManeuverDist==0)
    {
         return 0;
    }

    return remainingManeuverDist/ *agent->v_y_Max;

}

void
AbstractLaneChangeModel::setFollowerGaps(bool fexists, SurroundingMovingObjectsData* follower, double secGap, egoData* agent)  {
    if (fexists) {
        myLastFollowerGap = agent->GetState()->roadPos.s + vehicleParameters.distanceReferencePointToLeadingEdge - vehicleParameters.length - follower->GetState()->roadPos.s - *agent->GetDriverInformation()->MinGap - follower->GetProperties()->distanceReftoLeadingEdge;
        myLastFollowerSecureGap = secGap;
    }
}

void
AbstractLaneChangeModel::setLeaderGaps(bool lexists, SurroundingMovingObjectsData* leader, double secGap, egoData* agent) {
    if (lexists) {
        myLastLeaderGap = leader->GetState()->roadPos.s + leader->GetProperties()->distanceReftoLeadingEdge - leader->GetProperties()->lx - agent->GetState()->roadPos.s - *agent->GetDriverInformation()->MinGap - vehicleParameters.distanceReferencePointToLeadingEdge;
        myLastLeaderSecureGap = secGap;
    }
}

void
AbstractLaneChangeModel::setOrigLeaderGaps(bool lexists, SurroundingMovingObjectsData* leader, double secGap, egoData* agent) {
    if (lexists) {
        myLastOrigLeaderGap = leader->GetState()->roadPos.s + leader->GetProperties()->distanceReftoLeadingEdge - leader->GetProperties()->lx - agent->GetState()->roadPos.s - *agent->GetDriverInformation()->MinGap - vehicleParameters.distanceReferencePointToLeadingEdge;
        myLastOrigLeaderSecureGap = secGap;
    }
}


