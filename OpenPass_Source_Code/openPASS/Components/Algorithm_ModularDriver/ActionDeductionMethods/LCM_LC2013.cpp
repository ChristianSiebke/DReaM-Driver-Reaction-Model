/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    LCM_LC2013.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Friedemann Wesner
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @author  Leonhard Luecken
/// @date    Fri, 08.10.2013
/// @version $Id$
///
// A lane change model developed by J. Erdmann
// based on the model of D. Krajzewicz developed between 2004 and 2011 (LCM_DK2004)
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================

#include <iostream>
#include "LCM_LC2013.h"


// ===========================================================================
// variable definitions
// ===========================================================================
#define LOOK_FORWARD (double)10.

#define JAM_FACTOR (double)1.

#define LCA_RIGHT_IMPATIENCE (double)-1.
#define CUT_IN_LEFT_SPEED_THRESHOLD (double)27.

#define LOOK_AHEAD_MIN_SPEED 0.0
#define LOOK_AHEAD_SPEED_MEMORY 0.9

#define HELP_DECEL_FACTOR (double)1.0

#define HELP_OVERTAKE  (double)(10.0 / 3.6)
#define MIN_FALLBEHIND  (double)(7.0 / 3.6)

// allow overtaking to the right below this speed difference
#define OVERTAKE_RIGHT_THRESHOLD (double)(5/3.6)

#define RELGAIN_NORMALIZATION_MIN_SPEED (double)10.0
#define URGENCY (double)2.0

#define KEEP_RIGHT_TIME (double)5.0 // the number of seconds after which a vehicle should move to the right lane
#define KEEP_RIGHT_ACCEPTANCE (double)7.0 // calibration factor for determining the desire to keep right
#define ROUNDABOUT_DIST_BONUS (double)100.0 // valence (distance) for to faked per roundabout edge in front (inducing inner lane usage in roundabouts by decreasing sense of lc-urgency)

#define ROUNDABOUT_DIST_FACTOR (double)10.0 // Must be >=1.0, serves an alternative way of decreasing sense lc-urgency by multiplying the distance along the next roundabout
#define ROUNDABOUT_DIST_TRESH (double)10.0  // roundabout distances below ROUNDABOUT_DIST_TRESH are not multiplied by ROUNDABOUT_DIST_FACTOR

#define KEEP_RIGHT_HEADWAY (double)2.0
#define MAX_ONRAMP_LENGTH (double)200.
#define TURN_LANE_DIST (double)200.0 // the distance at which a lane leading elsewhere is considered to be a turn-lane that must be avoided

#define LC_RESOLUTION_SPEED_LAT (double)0.5 // the lateral speed (in m/s) for a standing vehicle which was unable to finish a continuous LC in time (in case mySpeedLatStanding==0), see #3771
#define LC_ASSUMED_DECEL (double)1.0 // the minimal constant deceleration assumed to estimate the duration of a continuous lane-change at its initiation.

#define REACT_TO_STOPPED_DISTANCE 300

// ===========================================================================
// debug defines
// ===========================================================================
//#define DEBUG_PATCH_SPEED
//#define DEBUG_INFORMED
//#define DEBUG_INFORMER
//#define DEBUG_CONSTRUCTOR
//#define DEBUG_WANTS_CHANGE
//#define DEBUG_SLOW_DOWN
//#define DEBUG_COOPERATE
//#define DEBUG_SAVE_BLOCKER_LENGTH

//#define DEBUG_COND (myVehicle.getID() == "disabled")
//#define DEBUG_COND (false)

// ===========================================================================
// member method definitions
// ===========================================================================
LCM_LC2013::LCM_LC2013(double Cycletimes, double SpeedGain, double KeepRight, double Cooperative, const CFModel* CarFollowModel, VehicleModelParameters vehicleParameters) :
    AbstractLaneChangeModel(Cycletimes, LC2013, CarFollowModel, vehicleParameters),
    mySpeedGainProbability(0),
    myKeepRightProbability(0),
    myLeadingBlockerLength(0),
    myLeftSpace(0),
    myLookAheadSpeed(LOOK_AHEAD_MIN_SPEED),
    myStrategicParam(1),
    myCooperativeParam(Cooperative),
    mySpeedGainParam(SpeedGain),
    myKeepRightParam(KeepRight),
    myLookaheadLeft(1),
    mySpeedGainRight(0.1),
    myAssertive(1)

{
    initDerivedParameters();
}

LCM_LC2013::~LCM_LC2013() {
    changed();
}


void
LCM_LC2013::initDerivedParameters() {
    myChangeProbThresholdRight = 1.000* (0.2 / mySpeedGainRight) / std::max(NUMERICAL_EPS, mySpeedGainParam);
    myChangeProbThresholdLeft = 1.000 * 0.2 / std::max(NUMERICAL_EPS, mySpeedGainParam);
}

int LCM_LC2013::wantsChange(
        int lane, int laneOffset,
        bool lexists, bool nlexists, bool nfexists,
        egoData* agent,
        int blocked,
        SurroundingMovingObjectsData* leader,
        SurroundingMovingObjectsData* neighLead,
        SurroundingMovingObjectsData* neighFollow,
        int *neighLane,
        const std::vector<int>& preb,
        double *occupancy,
        double *targetoccupancy,
        const RoadGeometry *RoadGeometry) {

    int result = _wantsChange(lane, laneOffset,
                              lexists, nlexists, nfexists,
                              agent, blocked,
                              leader, neighLead, neighFollow,
                              neighLane,
                              preb,
                              occupancy,
                              targetoccupancy,
                              RoadGeometry);
    return result;
}

double
LCM_LC2013::patchSpeed(const double *min, double wanted, const double *max, const double *v_x, const double *v_x_max, const double *a_x_max, const CFModel* cfModel, const double *MinGap) const {

    // negative min speed may be passed when using ballistic updated
    const double newSpeed = _patchSpeed(std::max(*min, 0.0), &wanted, max, v_x, v_x_max, a_x_max, cfModel, MinGap);

    return newSpeed;
}

double
LCM_LC2013::_patchSpeed(double min, const double *wanted, const double *max, const double *v_x, const double *v_x_max, const double *a_x_max, const CFModel* cfModel, const double *MinGap) const{
    int state = myOwnState;

    // letting vehicles merge in at the end of the lane in case of counter-lane change, step#2
    double MAGIC_offset = 1.;
    //   if we want to change and have a blocking leader and there is enough room for him in front of us
    if (myLeadingBlockerLength != 0) {
        double space = myLeftSpace - myLeadingBlockerLength - MAGIC_offset - *MinGap;

        if (space > 0) { // XXX space > -MAGIC_offset
            // compute speed for decelerating towards a place which allows the blocking leader to merge in in front
            double safe = cfModel->stopSpeed(v_x, &space, v_x_max);
            // if we are approaching this place
            if (safe < *wanted) {
                // return this speed as the speed to use

                return std::max(min, safe);
            }
        }
    }

    double nVSafe = *wanted;
    bool gotOne = false;
    for (std::vector<double>::const_iterator i = myLCAccelerationAdvices.begin(); i != myLCAccelerationAdvices.end(); ++i) {
        double a = (*i);
        double v = *v_x + ACCEL2SPEED(a);

        if (v >= min && v <= *max    // ballistic update: (negative speeds may appear, e.g. min<0, v<0), BUT:
                                     // XXX: LaneChanging returns -1 to indicate no restrictions, which leads to probs here (Leo), refs. #2577
                                     //      As a quick fix, we just dismiss cases where v=-1
                                     //      VERY rarely (whenever a requested help-acceleration is really indicated by v=-1)
                                     //      this can lead to failing lane-change attempts, though)
                                    || v != -1)
        {
            double nVakt = *v_x;
            nVSafe = std::min(v * myCooperativeParam + (1 - myCooperativeParam) * *wanted, nVSafe);
            gotOne = true;

        } else {
            if (v < min) {

            } else {

            }
        }
    }
    if (gotOne && !myDontBrake) { // XXX: myDontBrake is initialized as false and seems not to be changed anywhere... What's its purpose???

        return nVSafe;
    }

    // check whether the vehicle is blocked
    if ((state & LCA_WANTS_LANECHANGE) != 0 && (state & LCA_BLOCKED) != 0) {
        if ((state & LCA_STRATEGIC) != 0) {
            // necessary decelerations are controlled via vSafe. If there are
            // none it means we should speed up

            return (*max + *wanted) / (double) 2.0;
        } else if ((state & LCA_COOPERATIVE) != 0) {
            // only minor adjustments in speed should be done
            if ((state & LCA_BLOCKED_BY_LEADER) != 0) {

                if (*wanted >= 0.) {
                    return (std::max(0., min) + *wanted) / (double) 2.0;
                } else {
                    return *wanted;
                }
            }
            if ((state & LCA_BLOCKED_BY_FOLLOWER) != 0) {

                return (*max + *wanted) / (double) 2.0;
            }
            //} else { // VARIANT_16
            //    // only accelerations should be performed
            //    if ((state & LCA_BLOCKED_BY_FOLLOWER) != 0) {
            //        if (gDebugFlag2) std::cout << SIMTIME << " veh=" << myVehicle.getID() << " LCA_BLOCKED_BY_FOLLOWER\n";
            //        return (max + wanted) / (double) 2.0;
            //    }
        }
    }

    /*
    // decelerate if being a blocking follower
    //  (and does not have to change lanes)
    if ((state & LCA_AMBLOCKINGFOLLOWER) != 0) {
        f (fabs(max - myCarFollowModel->maxNextSpeed(*agent->GetState_Ego()->velocity_long, &myVehicle)) < 0.001 && min == 0) { // !!! was standing
            if (gDebugFlag2) std::cout << SIMTIME << " veh=" << myVehicle.getID() << " LCA_AMBLOCKINGFOLLOWER (standing)\n";
            return 0;
        }
        if (gDebugFlag2) std::cout << SIMTIME << " veh=" << myVehicle.getID() << " LCA_AMBLOCKINGFOLLOWER\n";

        //return min; // VARIANT_3 (brakeStrong)
        return (min + wanted) / (double) 2.0;
    }
    if ((state & LCA_AMBACKBLOCKER) != 0) {
        if (max <= myCarFollowModel->maxNextSpeed(*agent->GetState_Ego()->velocity_long, &myVehicle) && min == 0) { // !!! was standing
            if (gDebugFlag2) std::cout << SIMTIME << " veh=" << myVehicle.getID() << " LCA_AMBACKBLOCKER (standing)\n";
            //return min; VARIANT_9 (backBlockVSafe)
            return nVSafe;
        }
    }
    if ((state & LCA_AMBACKBLOCKER_STANDING) != 0) {
        if (gDebugFlag2) std::cout << SIMTIME << " veh=" << myVehicle.getID() << " LCA_AMBACKBLOCKER_STANDING\n";
        //return min;
        return nVSafe;
    }
    */

    // accelerate if being a blocking leader or blocking follower not able to brake
    //  (and does not have to change lanes)
    if ((state & LCA_AMBLOCKINGLEADER) != 0) {

        return (*max + *wanted) / (double) 2.0;
    }

    if ((state & LCA_AMBLOCKINGFOLLOWER_DONTBRAKE) != 0) {

        /*
        // VARIANT_4 (dontbrake)
        if (max <= myCarFollowModel->maxNextSpeed(*agent->GetState_Ego()->velocity_long, &myVehicle) && min == 0) { // !!! was standing
            return wanted;
        }
        return (min + wanted) / (double) 2.0;
        */
    }
    return *wanted;
}


void*
LCM_LC2013::inform(std::pair<double, int> info, AgentInterface* sender) {

    myOwnState |= info.second;

    return (void*) true;
}

double
LCM_LC2013::overtakeDistance(egoData* agent, SurroundingMovingObjectsData* follower, SurroundingMovingObjectsData* leader, const double* gap, double followerSpeed, double leaderSpeed) {
    followerSpeed = followerSpeed == INVALID_SPEED ? follower->GetState()->velocity_long : followerSpeed;
    leaderSpeed = leaderSpeed == INVALID_SPEED ? leader->GetState()->velocity_long : leaderSpeed;

    double overtakeDist = (*gap // drive to back of leader
                           + (leader->GetProperties()->lx + *agent->GetDriverInformation()->MinGap)
                           + follower->GetProperties()->lx - follower->GetProperties()->distanceReftoLeadingEdge
                           + myCarFollowModel->getSecureGap( // save gap to leader
                               &leaderSpeed, &followerSpeed, &vehicleParameters.maxDeceleration));
    return std::max(overtakeDist, 0.);
}

double
LCM_LC2013::overtakeDistance(SurroundingMovingObjectsData* leader, egoData* follower, const double* gap, double followerSpeed, double leaderSpeed) {
    followerSpeed = followerSpeed == INVALID_SPEED ? follower->GetState_Ego()->velocity_long : followerSpeed;
    leaderSpeed = leaderSpeed == INVALID_SPEED ? leader->GetState()->velocity_long : leaderSpeed;

    double overtakeDist = (*gap // drive to back of leader
                           + (leader->GetProperties()->lx + *follower->GetDriverInformation()->MinGap)
                           + vehicleParameters.length - vehicleParameters.distanceReferencePointToLeadingEdge
                           + myCarFollowModel->getSecureGap( // save gap to leader
                               &leaderSpeed, &followerSpeed, &vehicleParameters.maxDeceleration));
    return std::max(overtakeDist, 0.);
}

double
LCM_LC2013::overtakeDistance(egoData* leader, SurroundingMovingObjectsData* follower, const double* gap, double followerSpeed, double leaderSpeed) {
    followerSpeed = followerSpeed == INVALID_SPEED ? follower->GetState()->velocity_long : followerSpeed;
    leaderSpeed = leaderSpeed == INVALID_SPEED ? leader->GetState_Ego()->velocity_long : leaderSpeed;

    double overtakeDist = (*gap // drive to back of leader
                           + (vehicleParameters.length + *leader->GetDriverInformation()->MinGap)
                           + follower->GetProperties()->lx - follower->GetProperties()->distanceReftoLeadingEdge
                           + myCarFollowModel->getSecureGap( // save gap to leader
                               &leaderSpeed, &followerSpeed, &vehicleParameters.maxDeceleration));
    return std::max(overtakeDist, 0.);
}


double
LCM_LC2013::informLeader(AbstractLaneChangeModel::LCMessager& msgPass,
                         egoData* agent,
                         bool nlexists,
                         int blocked,
                         int dir,
                         SurroundingMovingObjectsData* neighLead,
                         double remainingSeconds)
{
    const double gap2pred = myLeftSpace - myLeadingBlockerLength;
    double plannedSpeed = std::min(agent->GetState_Ego()->velocity_long,
                               myCarFollowModel->stopSpeed(&agent->GetState_Ego()->velocity_long, &gap2pred, &vehicleParameters.maxVelocity));
    for (std::vector<double>::const_iterator i = myLCAccelerationAdvices.begin(); i != myLCAccelerationAdvices.end(); ++i) {
        const double a = *i;
        if (a >= myCarFollowModel->getMaxDecel()) {
            plannedSpeed = std::min(plannedSpeed, agent->GetState_Ego()->velocity_long + ACCEL2SPEED(a));
        }
    }

    if ((blocked & LCA_BLOCKED_BY_LEADER) != 0 && nlexists) {

        // decide whether we want to overtake the leader or follow it
        double overtakeTime;
        const double overtakeDist = overtakeDistance(neighLead, agent, neighLead->GetDistanceToEgo(), agent->GetState_Ego()->velocity_long, neighLead->GetState()->velocity_long);
        const double dv = plannedSpeed - neighLead->GetState()->velocity_long;

        if (dv > 0) {
            overtakeTime = overtakeDist / dv;
        } else {
            // -> set overtakeTime to something indicating impossibility of overtaking
            overtakeTime = remainingSeconds + 1;
        }

        if ((dv < 0
                // overtaking on the right on an uncongested highway is forbidden (noOvertakeLCLeft)
                || (dir == LCA_MLEFT && !(agent->GetState_Ego()->velocity_long<(60/3.6)) && !myAllowOvertakingRight)
                // not enough space to overtake?
                || (myLeftSpace - myLeadingBlockerLength - myCarFollowModel->brakeGap(&agent->GetState_Ego()->velocity_long) < overtakeDist)
                // using brakeGap() without headway seems adequate in a situation where the obstacle (the lane end) is not moving [XXX implemented in branch ticket860, can be used in general if desired, refs. #2575] (Leo).
                || (myLeftSpace - myLeadingBlockerLength - myCarFollowModel->brakeGap(&agent->GetState_Ego()->velocity_long)< overtakeDist)
                // not enough time to overtake?        (skipped for a stopped leader [currently only for ballistic update XXX: check if appropriate for euler, too, refs. #2575] to ensure that it can be overtaken if only enough space is exists) (Leo)
                || (remainingSeconds < overtakeTime && !(neighLead->GetState()->velocity_long==0)))
                // opposite driving and must overtake
                && !(isOpposite() && *neighLead->GetDistanceToEgo() < 0 && neighLead->GetState()->velocity_long==0)){
            // cannot overtake
            // TODO Blinker setzen // msgPass.informNeighLeader(Info(std::numeric_limits<double>::max(), dir | LCA_AMBLOCKINGLEADER), &myVehicle);
            // slow down smoothly to follow leader
            const double tarGetVelocityX = myCarFollowModel->followSpeed(
                                           agent->GetState_Ego()->velocity_long, *neighLead->GetDistanceToEgo(), neighLead->GetState()->velocity_long, vehicleParameters.maxVelocity);
            if (tarGetVelocityX < agent->GetState_Ego()->velocity_long) {
                // slow down smoothly to follow leader
                const double decel = remainingSeconds == 0. ? myCarFollowModel->getMaxDecel() :
                                     std::max(myCarFollowModel->getMaxDecel(),
                                          -std::max(MIN_FALLBEHIND, (agent->GetState_Ego()->velocity_long - tarGetVelocityX) / remainingSeconds));
                const double nextSpeed = std::min(plannedSpeed, agent->GetState_Ego()->velocity_long + ACCEL2SPEED(decel));

                addLCSpeedAdvice(nextSpeed, agent);

                return nextSpeed;
            } else {
                // leader is fast enough anyway

                addLCSpeedAdvice(tarGetVelocityX, agent);

                return plannedSpeed;
            }
        } else {
            // overtaking, leader should not accelerate
            return -1;  // XXX: using -1 is ambiguous for the ballistic update! Currently this is being catched in patchSpeed() (Leo), consider returning INVALID_SPEED, refs. #2577
        }
    } else if (nlexists) { // (remainUnblocked)
        // we are not blocked now. make sure we stay far enough from the leader
        double dv, nextNVSpeed;
            // XXX: the decrement (HELP_OVERTAKE) should be scaled with timestep length, I think.
            //      It seems to function as an estimate nv's speed in the next simstep!? (so HELP_OVERTAKE should be an acceleration value.)
            nextNVSpeed = neighLead->GetState()->velocity_long - HELP_OVERTAKE; // conservative
            dv = SPEED2DIST(agent->GetState_Ego()->velocity_long - nextNVSpeed);
            double dist2follow = *neighLead->GetDistanceToEgo() - dv;
        const double tarGetVelocityX = myCarFollowModel->followSpeed(
                                       agent->GetState_Ego()->velocity_long, dist2follow, nextNVSpeed, vehicleParameters.maxVelocity); //KBl , nv->GetCarFollowingModel()->getMaxDecel());
        addLCSpeedAdvice(tarGetVelocityX, agent);

        return std::min(tarGetVelocityX, plannedSpeed);
    } else {
        // not overtaking
        return plannedSpeed;
    }
}

void
LCM_LC2013::informFollower(AbstractLaneChangeModel::LCMessager& msgPass,
                           egoData* agent,
                           bool nfexists,
                           int blocked,
                           int dir,
                           SurroundingMovingObjectsData* neighFollow,
                           double remainingSeconds,
                           double plannedSpeed) {

    const double plannedAccel = SPEED2ACCEL(std::max(std::min(vehicleParameters.maxVelocity, plannedSpeed - agent->GetState_Ego()->velocity_long), vehicleParameters.maxDeceleration));

    if ((blocked & LCA_BLOCKED_BY_FOLLOWER) != 0 && nfexists) {

        // are we fast enough to cut in without any help?
        if (std::max(plannedSpeed, 0.) - neighFollow->GetState()->velocity_long >= HELP_OVERTAKE) {
            const double neededGap = myCarFollowModel->getSecureGap(&neighFollow->GetState()->velocity_long, &plannedSpeed, &vehicleParameters.maxDeceleration);
            if ((neededGap - *neighFollow->GetDistanceToEgo()) / remainingSeconds < (std::max(plannedSpeed, 0.) - neighFollow->GetState()->velocity_long)) {

                // follower might even accelerate but not to much
                // XXX: I don't understand this. The needed gap was determined for nv->GetVelocityX(), not for (plannedSpeed - HELP_OVERTAKE)?! (Leo), refs. #2578
                return;
            }
        }

        // decide whether we will request help to cut in before the follower or allow to be overtaken

        // PARAMETERS
        // assume other vehicle will assume the equivalent of 1 second of
        // maximum deceleration to help us (will probably be spread over
        // multiple seconds)
        // -----------
        const double helpDecel = vehicleParameters.maxDeceleration * HELP_DECEL_FACTOR;

        // follower's new speed in next step
        double neighNewSpeed;
        // follower's new speed after 1s.
        double neighNewSpeed1s;
        // velocity difference, gap after follower-deceleration
        double dv, decelGap;

            // euler
        neighNewSpeed = std::max(0., neighFollow->GetState()->velocity_long + ACCEL2SPEED(helpDecel));
        neighNewSpeed1s = std::max(0., neighFollow->GetState()->velocity_long + helpDecel); // TODO: consider introduction of a configurable anticipationTime here (see far below in the !blocked part). Refs. #2578
        // change in the gap between ego and blocker over 1 second (not STEP!)
        // XXX: though here it is calculated as if it were one step!? (Leo) Refs. #2578
        dv = plannedSpeed - neighNewSpeed1s; // XXX: what is this quantity (if TS!=1)?
        // new gap between follower and self in case the follower does brake for 1s
        // XXX: if the step-length is not 1s., this is not the gap after 1s. deceleration!
        //      And this formula overestimates the real gap. Isn't that problematic? (Leo)
        //      Below, it seems that decelGap > secureGap is taken to indicate the possibility
        //      to cut in within the next time-step. However, this is not the case, if TS<1s.,
        //      since decelGap is (not exactly, though!) the gap after 1s. Refs. #2578
        decelGap = *neighFollow->GetDistanceToEgo() + dv;
        double speedsecuregap = std::max(neighNewSpeed1s, 0.);
        double plannedspeedsec = std::max(plannedSpeed, 0.);
        const double secureGap = myCarFollowModel->getSecureGap(&speedsecuregap,
                                 &plannedspeedsec, &vehicleParameters.maxDeceleration);

        const double onRampThreshold = vehicleParameters.maxVelocity * 0.8 * 0;

        if (decelGap > 0 && decelGap >= secureGap) {
            // XXX: This does not assure that the leader can cut in in the next step if TS < 1 (see above)
            //      this seems to be supposed in the following (euler code)...?! (Leo) Refs. #2578

            // if the blocking follower brakes it could help
            // how hard does it actually need to be?
            // to be safe in the next step the following equation has to hold for the follower's vsafe:
            //   vsafe <= followSpeed(gap=currentGap - SPEED2DIST(vsafe), ...)
            double vsafe, vsafe1;

                // euler
                // we compute an upper bound on vsafe by doing the computation twice
            double dist2follow = *neighFollow->GetDistanceToEgo() + SPEED2DIST(plannedSpeed);
            vsafe1 = std::max(neighNewSpeed, myCarFollowModel->followSpeed(
                              neighFollow->GetState()->velocity_long, dist2follow, plannedSpeed, vehicleParameters.maxVelocity));
            double dist2follow2 = *neighFollow->GetDistanceToEgo() + SPEED2DIST(plannedSpeed - vsafe1);
            vsafe = std::max(neighNewSpeed, myCarFollowModel->followSpeed(
                              neighFollow->GetState()->velocity_long, dist2follow2, plannedSpeed, vehicleParameters.maxVelocity));

        } else if ((dv > 0 && dv * remainingSeconds > (secureGap - decelGap + POSITION_EPS))
                   || (dv > 0 && dv * (remainingSeconds - 1) > (secureGap - decelGap + POSITION_EPS)))
                  {

            // XXX: Alternative formulation (encapsulating differences of euler and ballistic) TODO: test, refs. #2575
            // double eventualGap = myCarFollowModel.gapExtrapolation(remainingSeconds - 1., decelGap, plannedSpeed, neighNewSpeed1s);
            // } else if (eventualGap > secureGap + POSITION_EPS) {


            // NOTE: This case corresponds to the situation, where some time is left to perform the lc
            // For the ballistic case this is interpreted as follows:
            // If the follower breaks with helpDecel for one second, this vehicle maintains the plannedSpeed,
            // and both continue with their speeds for remainingSeconds seconds the gap will suffice for a laneChange
            // For the euler case we had the following comment:
            // 'decelerating once is sufficient to open up a large enough gap in time', but:
            // XXX: 1) Decelerating *once* does not necessarily lead to the gap decelGap! (if TS<1s.) (Leo)
            //      2) Probably, the if() for euler should test for dv * (remainingSeconds-1) > ..., too ?!, refs. #2578

        } else if (dir == LCA_MRIGHT && !myAllowOvertakingRight && (!neighFollow->GetState()->velocity_long<(60/3.6))){
            // XXX: check if this requires a special treatment for the ballistic update, refs. #2575
            const double vhelp = std::max(neighNewSpeed, HELP_OVERTAKE);

        } else {
            double vhelp = std::max(neighFollow->GetState()->velocity_long, agent->GetState_Ego()->velocity_long + HELP_OVERTAKE);
            //if (dir == LCA_MRIGHT && myVehicle.getWaitingSeconds() > LCA_RIGHT_IMPATIENCE &&
            //        nv->GetVelocityX() > *agent->GetState_Ego()->velocity_long) {
            if (neighFollow->GetState()->velocity_long > agent->GetState_Ego()->velocity_long &&
                    ((dir == LCA_MRIGHT && getWaitingSeconds(agent->GetState_Ego()->velocity_long) > LCA_RIGHT_IMPATIENCE) // NOTE: it might be considered to use myVehicle.getAccumulatedWaitingSeconds() > LCA_RIGHT_IMPATIENCE instead (Leo). Refs. #2578
                     || (dir == LCA_MLEFT && plannedSpeed > CUT_IN_LEFT_SPEED_THRESHOLD) // VARIANT_22 (slowDownLeft)
                     // XXX this is a hack to determine whether the vehicles is on an on-ramp. This information should be retrieved from the network itself
                     || (dir == LCA_MLEFT && agent->GetState_Ego()->distance_to_end_of_lane>100)
                    )) {
                // let the follower slow down to increase the likelihood that later vehicles will be slow enough to help
                // follower should still be fast enough to open a gap
                // XXX: The probability for that success would be larger if the slow down of the appropriate following vehicle
                //      would take place without the immediate follower slowing down. We might consider to model reactions of
                //      vehicles that are not immediate followers. (Leo) -> see ticket #2532
                vhelp = std::max(neighNewSpeed, agent->GetState_Ego()->velocity_long + HELP_OVERTAKE);

                    // euler
                    if ((neighFollow->GetState()->velocity_long + agent->GetState_Ego()->velocity_long) / helpDecel < remainingSeconds) {


                    // XXX: I don't understand. This vhelp might be larger than nv->GetVelocityX() but the above condition seems to rely
                    //      on the reasoning that if nv breaks with helpDecel for remaining Seconds, nv will be so slow, that this
                    //      vehicle will be able to cut in. But nv might have overtaken this vehicle already (or am I missing sth?). (Leo)
                    //      Ad: To my impression, the intention behind allowing larger speeds for the blocking follower is to prevent a
                    //      situation, where an overlapping follower keeps blocking the ego vehicle. Refs. #2578
                    return;
                    }

            }

            // This follower is supposed to overtake us. Slow down smoothly to allow this.
            const double overtakeDist = overtakeDistance(neighFollow, agent, neighFollow->GetDistanceToEgo(), vhelp, plannedSpeed);
            // speed difference to create a sufficiently large gap
            const double needDV = overtakeDist / remainingSeconds;
            // make sure the deceleration is not to strong (XXX: should be assured in finalizeSpeed -> TODO: remove the std::max if agreed) -> prob with possibly non-existing maximal deceleration for som CF Models(?) Refs. #2578
            addLCSpeedAdvice(std::max(vhelp - needDV, agent->GetState_Ego()->velocity_long + ACCEL2SPEED(myCarFollowModel->getMaxDecel())), agent);

        }
    } else if (nfexists && (blocked & LCA_BLOCKED_BY_LEADER)) {
        // we are not blocked by the follower now, make sure it remains that way
        // XXX: Does the below code for the euler case really assure that? Refs. #2578
        double vsafe, vsafe1;
            // euler
        double dist2follow = *neighFollow->GetDistanceToEgo() + SPEED2DIST(plannedSpeed);

        vsafe1 = myCarFollowModel->followSpeed(neighFollow->GetState()->velocity_long, dist2follow, plannedSpeed, vehicleParameters.maxVelocity);
        double dist2follow2= *neighFollow->GetDistanceToEgo() + SPEED2DIST(plannedSpeed - vsafe1);
        vsafe = myCarFollowModel->followSpeed(
                    neighFollow->GetState()->velocity_long, dist2follow2, plannedSpeed, vehicleParameters.maxVelocity);
            // NOTE: since vsafe1 > nv->GetVelocityX() is possible, we don't have vsafe1 < vsafe < nv->GetVelocityX here (similar pattern above works differently)

    }
}


void
LCM_LC2013::prepareStep() {
    AbstractLaneChangeModel::prepareStep();
    // keep information about strategic change direction
    myOwnState = (myOwnState & LCA_STRATEGIC) ? (myOwnState & LCA_WANTS_LANECHANGE) : 0;
    myLeadingBlockerLength = 0;
    myLeftSpace = 0;
    myLCAccelerationAdvices.clear();
    myDontBrake = false;
    // truncate to work around numerical instability between different builds
    mySpeedGainProbability = ceil(mySpeedGainProbability * 100000.0) * 0.00001;
    myKeepRightProbability = ceil(myKeepRightProbability * 100000.0) * 0.00001;
}


void
LCM_LC2013::changed() {
    myOwnState = 0;
    mySpeedGainProbability = 0;
    myKeepRightProbability = 0;
        // if we are not yet on our best lane there might still be unseen blockers
        // (during patchSpeed)
        // myLeadingBlockerLength = 0;
        // myLeftSpace = 0;
    myLookAheadSpeed = LOOK_AHEAD_MIN_SPEED;
    myLCAccelerationAdvices.clear();
    myDontBrake = false;
}


int
LCM_LC2013::_wantsChange(
        int lane, int laneOffset,
        bool lexists, bool nlexists, bool nfexists,
        egoData* agent,
        int blocked,
        SurroundingMovingObjectsData* leader,
        SurroundingMovingObjectsData* neighLead,
        SurroundingMovingObjectsData* neighFollow,
        int *neighLane,
        const std::vector<int>& preb,
        double *occupancy,
        double *targetoccupancy,
        const RoadGeometry *RoadGeometry) {
    time_t currentTime;
    // compute bestLaneOffset
    int curr, neigh, best;

    // What do these "dists" mean? Please comment. (Leo) Ad: I now think the following:
    // currentDist is the distance that the vehicle can go on its route without having to
    // change lanes from the current lane. neighDist as currentDist for the considered target lane (i.e., neigh)
    // If this is true I suggest to put this into the docu of wantsChange()
    double currentDist = 0;
    double neighDist = 0;
    int currIdx = 0;

    //if (myLCAccelerationAdvices.size()>3)
    //    myLCAccelerationAdvices.clear();

    bool checkOverTakeRightPre = false;

    if ( (laneOffset == 1 && nlexists && neighLead->GetState()->velocity_long==0 && *neighLead->GetDistanceToEgo() < 200) || (laneOffset==-1 && lexists && leader->GetState()->velocity_long==0 && *leader->GetDistanceToEgo()<200) )
        checkOverTakeRightPre = true;

    Trigger = Trigger + 100;
    int prebLane = lane; //myVehicle.GetAgentLaneId();
    // XXX: What does the following code do? Please comment. (Leo) Refs. #2578
     int prebOffset = laneOffset;
     curr = prebLane;
     neigh = *neighLane;
     currentDist =  RoadGeometry->laneEgo.distanceToEndOfLane;
     neighDist = (laneOffset == -1) ? RoadGeometry->laneRight.distanceToEndOfLane : RoadGeometry->laneLeft.distanceToEndOfLane;
     int bestLaneOffset = 0;
     bestLaneOffset = (laneOffset == -1) ? (currentDist < TURN_LANE_DIST ? -1 : 0) : (currentDist < TURN_LANE_DIST ? 1 : 0);
     if (bestLaneOffset == 0 && (prebLane>2))
     {
         bestLaneOffset = 0;
     }
     best = prebLane+bestLaneOffset; //neighLane;


    // direction specific constants
    bool right = (laneOffset == -1);

    // if ((lexists && leader.first->GetVelocityX()==0 && leader.second < 100) || (nlexists && neighLead.first->GetVelocityX()==0 && neighLead.second < 100))
    //     myAllowOvertakingRight = true;
    // else
    //     myAllowOvertakingRight = false;

    const double posOnLane = agent->GetState()->roadPos.s + vehicleParameters.distanceReferencePointToLeadingEdge; //KBl isOpposite() ? myVehicle.getLane()->getOppositePos(myVehicle.getPositionOnLane()) : myVehicle.getPositionOnLane();
    const int lca = (right ? LCA_RIGHT : LCA_LEFT);
    const int myLca = (right ? LCA_MRIGHT : LCA_MLEFT);
    const int lcaCounter = (right ? LCA_LEFT : LCA_RIGHT);
    const bool changeToBest = true; //(right && bestLaneOffset < 0) || (!right && bestLaneOffset > 0);
    // keep information about being a leader/follower
    int ret = (myOwnState & 0xffff0000);
    int req = 0; // the request to change or stay

    // we try to estimate the distance which is necessary to get on a lane
    //  we have to get on in order to keep our route
    // we assume we need something that depends on our velocity
    // and compare this with the free space on our wished lane
    //
    // if the free space is somehow(<-?) less than the space we need, we should
    //  definitely try to get to the desired lane
    //
    // this rule forces our vehicle to change the lane if a lane changing is necessary soon


    // we do not want the lookahead distance to change all the time so we let it decay slowly
    // (in contrast, growth is applied instantaneously)
    if (agent->GetState_Ego()->velocity_long > myLookAheadSpeed) {
        myLookAheadSpeed = agent->GetState_Ego()->velocity_long;
    } else {
        // memory decay factor for this action step
        const double memoryFactor = 1. - (1. - LOOK_AHEAD_SPEED_MEMORY) * 0.1;
        myLookAheadSpeed = std::max(LOOK_AHEAD_MIN_SPEED,
                                (memoryFactor * myLookAheadSpeed + (1 - memoryFactor) * agent->GetState_Ego()->velocity_long));
    }
    double laDist = myLookAheadSpeed * LOOK_FORWARD * myStrategicParam * (right ? 1 : myLookaheadLeft);
    laDist += (vehicleParameters.length + *agent->GetDriverInformation()->MinGap) * (double) 2.;


    if (bestLaneOffset == 0 && lexists && leader->GetState()->velocity_long==0) {
        // react to a stopped leader on the current lane
        // The value of laDist is doubled below for the check whether the lc-maneuver can be taken out
        // on the remaining distance (because the vehicle has to change back and forth). Therefore multiply with 0.5.
        laDist = (vehicleParameters.length + *agent->GetDriverInformation()->MinGap)
                        + leader->GetProperties()->lx - + *agent->GetDriverInformation()->MinGap;
    } else if (bestLaneOffset == laneOffset && nlexists && neighLead->GetState()->velocity_long==0) {
        // react to a stopped leader on the target lane (if it is the bestLane)
        laDist = (vehicleParameters.length + *agent->GetDriverInformation()->MinGap)
                 + (neighLead->GetProperties()->lx + *agent->GetDriverInformation()->MinGap);
    }

    // free space that is available for changing
    //const double neighSpeed = (nlexists != 0 ? neighLead.first->GetVelocityX() :
    //        nfexists != 0 ? neighFollow.first->GetVelocityX() :
    //        best.lane->GetVelocityXLimit());
    // @note: while this lets vehicles change earlier into the correct direction
    // it also makes the vehicles more "selfish" and prevents changes which are necessary to help others

    // Next we assign to roundabout edges a larger distance than to normal edges
    // in order to decrease sense of lc urgency and induce higher usage of inner roundabout lanes.
    // TODO: include ticket860 code
    // 1) get information about the next upcoming roundabout

    double occ = (nlexists && *neighLead->GetDistanceToEgo()<200 ? *targetoccupancy * JAM_FACTOR : (nfexists? *targetoccupancy * JAM_FACTOR : 0));
    const double usableDist = (currentDist - occ); //- posOnLane - occ);
    //- (best.lane->getVehicleNumber() * neighSpeed)); // VARIANT 9 jfSpeed
    const double neighLeftPlace = std::max((double) 0, neighDist);
    double vMax = std::min(vehicleParameters.maxVelocity, *agent->GetDriverInformation()->v_Wish);
    // upper bound which will be restricted successively
    double thisLaneVSafe = vMax;
    const bool checkOverTakeRight = (checkOverTakeRightPre && !agent->GetState_Ego()->velocity_long<16); // !myAllowOvertakingRight;

    bool changeLeftToAvoidOvertakeRight = true;
    if (changeToBest
            && currentDistDisallows(usableDist, 1, 500)) {
        /// @brief we urgently need to change lanes to follow our route
        ret = ret | lca | LCA_STRATEGIC | LCA_URGENT;

    } else {
        // VARIANT_20 (noOvertakeRight)
        if (nlexists && !checkOverTakeRight && !right) {
            // check for slower leader on the left. we should not overtake but
            // rather move left ourselves (unless congested)
            const double deltaV = (agent->GetState_Ego()->velocity_long - neighLead->GetState()->velocity_long);
            if (deltaV > 0) {
                double a = myCarFollowModel->getSpeedAfterMaxDecel(agent->GetState_Ego()->velocity_long);
                double b = myCarFollowModel->followSpeed(agent->GetState_Ego()->velocity_long, *neighLead->GetDistanceToEgo(), neighLead->GetState()->velocity_long, vehicleParameters.maxVelocity); //KBl , nv->GetCarFollowingModel()->getMaxDecel()));
                double vSafe = std::max(
                                   myCarFollowModel->getSpeedAfterMaxDecel(agent->GetState_Ego()->velocity_long),
                                   myCarFollowModel->followSpeed(agent->GetState_Ego()->velocity_long, *neighLead->GetDistanceToEgo(), neighLead->GetState()->velocity_long, vehicleParameters.maxVelocity)); //KBl , nv->GetCarFollowingModel()->getMaxDecel()));
                if (mySpeedGainProbability < myChangeProbThresholdLeft) {
                    vSafe = std::max(vSafe, neighLead->GetState()->velocity_long);
                }
                thisLaneVSafe = std::min(thisLaneVSafe, vSafe);
                addLCSpeedAdvice(vSafe, agent);

                // only generate impulse for overtaking left shortly before braking would be necessary
                const double deltaGapFuture = deltaV * 8;
                double deltaGap = *neighLead->GetDistanceToEgo() - deltaGapFuture;
                const double vSafeFuture = myCarFollowModel->followSpeed(
                                               agent->GetState_Ego()->velocity_long, deltaGap, neighLead->GetState()->velocity_long, vehicleParameters.maxVelocity);
                if (vSafeFuture < vSafe) {
                    const double relativeGain = deltaV / std::max(vMax,
                                                RELGAIN_NORMALIZATION_MIN_SPEED);
                    mySpeedGainProbability += 0.1 * relativeGain;
                    changeLeftToAvoidOvertakeRight = true;
                }
            }
            else
            {
                double vSafe = std::max(
                                   myCarFollowModel->getSpeedAfterMaxDecel(agent->GetState_Ego()->velocity_long),
                                   myCarFollowModel->followSpeed(agent->GetState_Ego()->velocity_long, *neighLead->GetDistanceToEgo(), neighLead->GetState()->velocity_long, vehicleParameters.maxVelocity)); //KBl , nv->GetCarFollowingModel()->getMaxDecel()));
                addLCSpeedAdvice(vSafe, agent);
            }
        }
        const double overtakeDist = (!lexists ? -1 :
                *leader->GetDistanceToEgo() + vehicleParameters.length - vehicleParameters.distanceReferencePointToLeadingEdge + (leader->GetProperties()->lx + *agent->GetDriverInformation()->MinGap));
        if (lexists && leader->GetState()->velocity_long<1 && (*leader->GetDistanceToEgo() < REACT_TO_STOPPED_DISTANCE)
                // current destination leaves enough space to overtake the leader
                && (std::min(neighDist, currentDist) > overtakeDist)
                // maybe do not overtake on the right at high speed
                && (checkOverTakeRight || !right)
                && (nlexists && neighLead->GetState()->velocity_long==0 &&
                 // neighboring stopped vehicle leaves enough space to overtake leader
                   (*neighLead->GetDistanceToEgo() > overtakeDist) && overtakeDist!=-1)) {
            // avoid becoming stuck behind a stopped leader
            currentDist = vehicleParameters.distanceReferencePointToLeadingEdge + *leader->GetDistanceToEgo();
            ret = ret | lca | LCA_STRATEGIC | LCA_URGENT;

        }

         //KBl 2911else if (bestLaneOffset == 0 && (neighLeftPlace * 2. < laDist)) {
         //KBl 2911   // the current lane is the best and a lane-changing would cause a situation
         //KBl 2911   //  of which we assume we will not be able to return to the lane we have to be on.
         //KBl 2911   // this rule prevents the vehicle from leaving the current, best lane when it is
         //KBl 2911   //  close to this lane's end
         //KBl 2911
         //KBl 2911   ret = ret | LCA_STAY | LCA_STRATEGIC;

        else if (bestLaneOffset == 0
                   && (!lexists || !leader->GetState()->velocity_long==0)
                   && neighDist < TURN_LANE_DIST) {
            // VARIANT_21 (stayOnBest)
            // we do not want to leave the best lane for a lane which leads elsewhere
            // unless our leader is stopped or we are approaching a roundabout

            ret = ret | LCA_STAY | LCA_STRATEGIC;

        }
    }
    // check for overriding TraCI requests

    // store state before canceling
    myCanceledStates[laneOffset] |= ret;

    if ((ret & LCA_STAY) != 0) {
        return ret;
    }
    if ((ret & LCA_URGENT) != 0) {
        // prepare urgent lane change maneuver
        // save the left space
        myLeftSpace = currentDist;
        if (changeToBest && abs(bestLaneOffset) > 1) {
            // there might be a vehicle which needs to counter-lane-change one lane further and we cannot see it yet
            myLeadingBlockerLength = std::max((double)(right ? 20.0 : 40.0), myLeadingBlockerLength);
        }

        // letting vehicles merge in at the end of the lane in case of counter-lane change, step#1
        //   if there is a leader and he wants to change to the opposite direction

        const double remainingSeconds = //((ret & LCA_TRACI) == 0 ?
                                         // std::max((double)STEPS2TIME(TS), (myLeftSpace-myLeadingBlockerLength) / std::max(myLookAheadSpeed, NUMERICAL_EPS) / abs(bestLaneOffset) / URGENCY) :
                                         (std::max(0.1, (myLeftSpace-myLeadingBlockerLength) / std::max(myLookAheadSpeed, (double) NUMERICAL_EPS) / abs(laneOffset) / (double) URGENCY)); //KBl :
        // NOTE: for the  ballistic update case negative speeds may indicate a stop request,
        //       while informLeader returns -1 in that case. Refs. #2577
        return ret;

    }
    // a high inconvenience prevents cooperative changes.
    const double inconvenience = std::min((double)1.0, (laneOffset < 0
                                      ? mySpeedGainProbability / myChangeProbThresholdRight
                                      : -mySpeedGainProbability / myChangeProbThresholdLeft));
    const bool speedGainInconvenient = inconvenience > myCooperativeParam;
    bool neighOccupancyInconvenient = false;
    if (nlexists)
    neighOccupancyInconvenient = (occupancy < targetoccupancy);
    else
    neighOccupancyInconvenient = true;
    // VARIANT_15

        // try to use the inner lanes of a roundabout to increase throughput
        // unless we are approaching the exit
        if (lca == LCA_LEFT) {
            // if inconvenience is not too high, request collaborative change (currently only for ballistic update)
            // TODO: test this for euler update! Refs. #2575
            if (!neighOccupancyInconvenient) {
//                if(MSGlobals::gSemiImplicitEulerUpdate || !speedGainInconvenient){
                req = ret | lca | LCA_COOPERATIVE;

            }
        } else {
            // if inconvenience is not too high, request collaborative change (currently only for ballistic update)
            if (!neighOccupancyInconvenient) {
//            if(MSGlobals::gSemiImplicitEulerUpdate || speedGainInconvenient){
                req = ret | LCA_STAY | LCA_COOPERATIVE;

            }
        }
        if (cancelRequest(req, laneOffset)) {

            return ret | req;

        }

    // let's also regard the case where the vehicle is driving on a highway...
    //  in this case, we do not want to get to the dead-end of an on-ramp
    if (right) {
        if (neighDist < TURN_LANE_DIST && (*agent->GetDriverInformation()->v_Wish) > 80. / 3.6 && myLookAheadSpeed > 0.1) {

            req = ret | LCA_STAY | LCA_STRATEGIC;

            if (cancelRequest(req, laneOffset)) {

                return ret | req;
            }
        }
    }

    if (amBlockingFollowerPlusNB()
            && (!speedGainInconvenient)
            //&& ((myOwnState & myLca) != 0) // VARIANT_6 : counterNoHelp
            && (changeToBest || currentDistAllows(neighDist, abs(bestLaneOffset), laDist))) { //KBl - 1, laDist))) {

        req = ret | lca | LCA_COOPERATIVE | LCA_URGENT ;//| LCA_CHANGE_TO_HELP;

        if (cancelRequest(req, laneOffset)) {

            return ret | req;
        }
    }

    // --------

    //// -------- security checks for krauss
    ////  (vsafe fails when gap<0)
    //if ((blocked & LCA_BLOCKED) != 0) {
    //    return ret;
    //}
    //// --------

    // -------- higher speed
    //if ((congested(nlexists) && neighLead.second < 20) || predInteraction(leader.first)) { //!!!
    //    return ret;
    //}

    double neighLaneVSafe = std::min(vehicleParameters.maxVelocity, *agent->GetDriverInformation()->v_Wish);//KBl neighLane.getVehicleMaxSpeed(&myVehicle);
    //if (neighLead.first)
    //    neighLaneVSafe = neighLead.first->GetVelocityX();

    // we wish to anticipate future speeds. This is difficult when the leading
    // vehicles are still accelerating so we resort to comparing next speeds in this case
    const bool acceleratingLeader = (nlexists  && neighLead->GetState()->acceleration_long > 0)
                                    || (lexists  && leader->GetState()->acceleration_long > 0);

    if (acceleratingLeader) {
        // followSpeed allows acceleration for 1 step, to always compare speeds
        // after 1 second of acceleration we have call the function with a correct speed value
        // TODO: This should be explained better. Refs #2
        double correctedSpeed = (agent->GetState_Ego()->velocity_long + vehicleParameters.maxAcceleration //KBl myCarFollowModel->getMaxAccel()
                                       - ACCEL2SPEED(vehicleParameters.maxAcceleration));//KBl GetCarFollowingModel()->getMaxAccel()));

        if (!nlexists) {
            neighLaneVSafe = std::min(vehicleParameters.maxVelocity, *agent->GetDriverInformation()->v_Wish); // std::min(neighLaneVSafe, myCarFollowModel->followSpeed(&correctedSpeed, &neighDist, &neighspeed, &vehicleParameters.maxVelocity));
        } else {
            neighLaneVSafe = std::min(neighLaneVSafe, myCarFollowModel->followSpeed(
                                      correctedSpeed, *neighLead->GetDistanceToEgo(), neighLead->GetState()->velocity_long, vehicleParameters.maxVelocity));
        }
        if (!lexists) {
            double prebspeed = 0;
            thisLaneVSafe = std::min(thisLaneVSafe, myCarFollowModel->followSpeed(correctedSpeed, currentDist, prebspeed, vehicleParameters.maxVelocity));
        } else {
            thisLaneVSafe = std::min(thisLaneVSafe, myCarFollowModel->followSpeed(
                                     correctedSpeed, *leader->GetDistanceToEgo(), leader->GetState()->velocity_long, vehicleParameters.maxVelocity));
        }
    } else {
        if (!nlexists) {
            neighLaneVSafe = std::min(std::min(vehicleParameters.maxVelocity, *agent->GetDriverInformation()->v_Wish), myCarFollowModel->stopSpeed(&agent->GetState_Ego()->velocity_long, &neighDist, &vehicleParameters.maxVelocity)); // std::min(neighLaneVSafe, myCarFollowModel->maximumSafeStopSpeed(&neighDist, agent->GetState_Ego()->velocity_long, true, agent->thw_Wish));
        } else {
            neighLaneVSafe = std::min(neighLaneVSafe, myCarFollowModel->followSpeed(agent->GetState_Ego()->velocity_long, *neighLead->GetDistanceToEgo(),
                                  neighLead->GetState()->velocity_long, vehicleParameters.maxVelocity)); // &neighLead->a_Min, true));
        }
        if (!lexists) {
            thisLaneVSafe = std::min(vehicleParameters.maxVelocity, *agent->GetDriverInformation()->v_Wish); //std::min(thisLaneVSafe, myCarFollowModel->maximumSafeStopSpeed(&currentDist, agent->GetState_Ego()->velocity_long, true));
        } else {
            thisLaneVSafe = std::min(thisLaneVSafe, myCarFollowModel->followSpeed(agent->GetState_Ego()->velocity_long, *leader->GetDistanceToEgo(),
                                                                                  leader->GetState()->velocity_long, vehicleParameters.maxVelocity));
        }
    }

    const double relativeGain = (neighLaneVSafe - thisLaneVSafe) / std::max(neighLaneVSafe,
                                RELGAIN_NORMALIZATION_MIN_SPEED);

    if (right) {
        // ONLY FOR CHANGING TO THE RIGHT
        if (thisLaneVSafe - 5 / 3.6 > neighLaneVSafe) {
            // ok, the current lane is faster than the right one...
            if (mySpeedGainProbability < 0) {
                mySpeedGainProbability *= pow(0.5,0.1); //KBl myVehicle.getActionStepLengthSecs());
                //myKeepRightProbability /= 2.0;
            }
        } else {
            // ok, the current lane is not (much) faster than the right one
            // @todo recheck the 5 km/h discount on thisLaneVSafe, refs. #2068

            // do not promote changing to the left just because changing to the right is bad
            // XXX: The following code may promote it, though!? (recheck!)
            //      (Think of small negative mySpeedGainProbability and larger negative relativeGain)
            //      One might think of replacing '||' by '&&' to exclude that possibility...
            //      Still, for negative relativeGain, we might want to decrease the inclination for
            //      changing to the left. Another solution could be the seperation of mySpeedGainProbability into
            //      two variables (one for left and one for right). Refs #2578
            if (mySpeedGainProbability < 0 || relativeGain > 0) {
                mySpeedGainProbability -= 0.1 * relativeGain; //KBl myVehicle.getActionStepLengthSecs() * relativeGain;
            }

            // honor the obligation to keep right (Rechtsfahrgebot)
            // XXX consider fast approaching followers on the current lane. Refs #2578
            //const double vMax = myLookAheadSpeed;
            const double acceptanceTime = KEEP_RIGHT_ACCEPTANCE * vMax * std::max((double)1, agent->GetState_Ego()->velocity_long / vehicleParameters.maxVelocity); //KBl getLane()->GetVelocityXLimit();
            double fullSpeedGap = std::max(0., neighDist - myCarFollowModel->brakeGap(&vMax));
            double fullSpeedDrivingSeconds = std::min(acceptanceTime, fullSpeedGap / vMax);
            if (nlexists && neighLead->GetState()->velocity_long < vMax) {
                fullSpeedGap = std::max(0., std::min(fullSpeedGap,
                                             *neighLead->GetDistanceToEgo() -
                                                     myCarFollowModel->getSecureGap(&vMax, &neighLead->GetState()->velocity_long, &vehicleParameters.maxDeceleration)));
                fullSpeedDrivingSeconds = std::min(fullSpeedDrivingSeconds, fullSpeedGap / (vMax - neighLead->GetState()->velocity_long));
            }
            // stay on the current lane if we cannot overtake a slow leader on the right
            if (checkOverTakeRight && lexists
                    && vehicleParameters.maxVelocity <= vMax) {
                fullSpeedGap = std::min(fullSpeedGap, *leader->GetDistanceToEgo());
                fullSpeedDrivingSeconds = std::min(fullSpeedDrivingSeconds, fullSpeedGap / (vMax - leader->GetState()->velocity_long));
                const double relativeGain = (vMax - vehicleParameters.maxVelocity) / std::max(vMax,
                                            RELGAIN_NORMALIZATION_MIN_SPEED);
                // tiebraker to avoid buridans paradox see #1312
                mySpeedGainProbability += 0.1 * relativeGain; //KBl myVehicle.getActionStepLengthSecs() * relativeGain;
            }

            const double deltaProb = (myChangeProbThresholdRight * (fullSpeedDrivingSeconds / acceptanceTime) / KEEP_RIGHT_TIME);
            myKeepRightProbability -= 0.5 * deltaProb;

            //if (myVehicle.GetAgentId()==3)
            //    std::cout << myKeepRightProbability << " , " << myChangeProbThresholdRight << std::endl;

            if (myKeepRightProbability * myKeepRightParam< -1.000000 * myChangeProbThresholdRight) {//KBl (myKeepRightProbability * myKeepRightParam < -1.000000 * myChangeProbThresholdRight) {
                req = ret | lca | LCA_KEEPRIGHT;

                //if (!cancelRequest(req, laneOffset)) {
                    return ret | req;
                //}
            }
        }

        if (mySpeedGainProbability < -myChangeProbThresholdRight
                && (neighDist / std::max((double) .1, agent->GetState_Ego()->velocity_long)) > 30.) { //./std::max((double) .1, *agent->GetState_Ego()->velocity_long)) { // -.1
            req = ret | lca | LCA_SPEEDGAIN;


            if (!cancelRequest(req, laneOffset)) {

                return ret | req;
            }
        }
    } else {
        // ONLY FOR CHANGING TO THE LEFT
        if (thisLaneVSafe > neighLaneVSafe) {
            // this lane is better
            if (mySpeedGainProbability > 0) {
                mySpeedGainProbability *= pow(0.5, 0.1); //KBl ,myVehicle.getActionStepLengthSecs());
            }
        } else if (thisLaneVSafe == neighLaneVSafe) {
            if (mySpeedGainProbability > 0) {
                mySpeedGainProbability *= pow(0.8,0.1);//KBl , myVehicle.getActionStepLengthSecs());
            }
        } else {
            // left lane is better
            mySpeedGainProbability += 0.1*relativeGain; //KBl myVehicle.getActionStepLengthSecs() * relativeGain;
        }
        // VARIANT_19 (stayRight)
        if (nfexists) {
            const double secGap = myCarFollowModel->getSecureGap(&neighFollow->GetState()->velocity_long, &agent->GetState_Ego()->velocity_long, &vehicleParameters.maxDeceleration); //KBl nv->GetCarFollowingModel()->getSecureGap(neighFollow->GetState()->velocity_long, agent->GetState_Ego()->velocity_long, vehicleParameters.maxDeceleration);
            if (*neighFollow->GetDistanceToEgo() < secGap * KEEP_RIGHT_HEADWAY) {
                // do not change left if it would inconvenience faster followers

                return ret | LCA_STAY | LCA_SPEEDGAIN;
            }
        }

        if (mySpeedGainProbability > myChangeProbThresholdLeft
                && (relativeGain > NUMERICAL_EPS || changeLeftToAvoidOvertakeRight)
                && neighDist / std::max((double) .1, agent->GetState_Ego()->velocity_long) > 2.) { // .1
            req = ret | lca | LCA_SPEEDGAIN;

            //if (myVehicle.GetAgentId()==agentidforcout && cout)
            //{std::cout << Trigger << ", " << PrintBin(req) << ": Left: mySpeedGainProbability > myChangeProbThresholdLeft && (relativeGain > NUMERICAL_EPS || changeLeftToAvoidOvertakeRight, 1809" << std::endl;
            //    if (neighLead.first)
            //    {
            //        if (neighLead.first->GetAgentId()==21)
            //            double a=1;
            //    }
            //}
            if (!cancelRequest(req, laneOffset)) {

                //if (myVehicle.GetAgentId()==agentidforcout && cout)
                //    std::cout << Trigger << ", " << PrintBin(ret | req) << ": Return: Left: mySpeedGainProbability > myChangeProbThresholdLeft && (relativeGain > NUMERICAL_EPS || changeLeftToAvoidOvertakeRight, 1809" << std::endl;

                return ret | req;         

            }
        }
    }
    // --------
    if (changeToBest && bestLaneOffset == bestLaneOffset //KBl curr.bestLaneOffset
            && (right ? mySpeedGainProbability < 0 : mySpeedGainProbability > 0)) {
        // change towards the correct lane, speedwise it does not hurt
        req = ret | lca | LCA_STRATEGIC;

        if (!cancelRequest(req, laneOffset)) {

         //   return ret | req;
        }
    }

    return ret;
}



void
LCM_LC2013::getRoundaboutAheadInfo(egoData* agent, int curr, int neigh, //KBl const AgentInterface::LaneQ& curr, const AgentInterface::LaneQ& neigh,
                                     double& roundaboutDistanceAhead, double& roundaboutDistanceAheadNeigh, int& roundaboutEdgesAhead, int& roundaboutEdgesAheadNeigh) {

        // In what follows, we check whether a roundabout is ahead (or the vehicle is on a roundabout)
    // We calculate the lengths of the continuations described by curr and neigh,
    // which are part of the roundabout. Currently only takes effect for ballistic update, refs #1807, #2576 (Leo)

    double pos = agent->GetState()->roadPos.s;
    roundaboutDistanceAhead = 1000; //KBl distanceAlongNextRoundabout(pos, veh.GetAgentLaneId(), curr.bestContinuations);

    // For the distance on the neigh.lane, we need to do a little hack since we may not
    // have access to the right initial lane (neigh.lane is only the first non-null lane of neigh.bestContinuations).
    roundaboutDistanceAheadNeigh = 0;
    double neighPosition = pos;

    // add roundabout distance from neigh.lane on
    roundaboutDistanceAheadNeigh = 1000; //KBl += distanceAlongNextRoundabout(neighPosition, neigh.lane, neigh.bestContinuations);

    // count the number of roundabout edges ahead to determine whether
    // special LC behavior is required (promoting the use of the inner lane, mainly)
    roundaboutEdgesAhead = 10; //KBl 0;

    roundaboutEdgesAheadNeigh = 10; //KBl 0;

}


double
LCM_LC2013::roundaboutDistBonus(double roundaboutDistAhead, int roundaboutEdgesAhead) const {
    // NOTE: Currently there are two variants, one taking into account only the number
    //       of upcoming non-internal roundabout edges and adding ROUNDABOUT_DIST_BONUS per upcoming edge except the first.
    //       Another variant uses the actual distance and multiplies it by a factor ROUNDABOUT_DIST_FACTOR.
    //       Currently, the update rule decides which variant to take (because the second was experimentally implemented
    //       in the ballistic branch (ticket860)). Both variants may be combined in future. Refs. #2576
        if (roundaboutEdgesAhead > 1) {
            // Here we add a bonus length for each upcoming roundabout edge to the distance.
            // XXX: That becomes problematic, if the vehicle enters the last round about edge,
            // realizes suddenly that the change is very urgent and finds itself with very
            // few space to complete the urgent strategic change frequently leading to
            // a hang up on the inner lane.
            return roundaboutEdgesAhead * ROUNDABOUT_DIST_BONUS * myCooperativeParam;
        } else {
            return 0.;
        }

}

int
LCM_LC2013::slowDownForBlocked(SurroundingMovingObjectsData* blocked, int state, egoData* agent) {
    //  if this vehicle is blocking someone in front, we maybe decelerate to let him in
    if ((blocked) != 0) {
        double gap = blocked->GetState()->roadPos.s + blocked->GetProperties()->distanceReftoLeadingEdge - blocked->GetProperties()->lx - agent->GetState()->roadPos.s + vehicleParameters.distanceReferencePointToLeadingEdge - vehicleParameters.length - *agent->GetDriverInformation()->MinGap;
        if (gap > POSITION_EPS) {
            //const bool blockedWantsUrgentRight = (((*blocked)->getLaneChangeModel().getOwnState() & LCA_RIGHT != 0)
            //    && ((*blocked)->getLaneChangeModel().getOwnState() & LCA_URGENT != 0));

            if (agent->GetState_Ego()->velocity_long < -ACCEL2SPEED(vehicleParameters.maxDeceleration)
                    //|| blockedWantsUrgentRight  // VARIANT_10 (helpblockedRight)
               ) {
                if (blocked->GetState()->velocity_long < 0.1) { //KBl < SUMO_const_haltingSpeed) {
                    state |= LCA_AMBACKBLOCKER_STANDING;
                } else {
                    state |= LCA_AMBACKBLOCKER;
                }
                double gap2follow = (double)(gap- POSITION_EPS);
                addLCSpeedAdvice(myCarFollowModel->followSpeed(
                                     agent->GetState_Ego()->velocity_long,
                                     gap2follow, blocked->GetState()->velocity_long, vehicleParameters.maxVelocity),agent);//KBl,
                                     //KBl (*blocked)->GetMaxDeceleration()));
                //(*blocked) = 0; // VARIANT_14 (furtherBlock)

            } /* else {
            	// experimental else-branch...
                state |= LCA_AMBACKBLOCKER;
                myVSafes.push_back(myCarFollowModel.followSpeed(
                                       &myVehicle, *agent->GetState_Ego()->velocity_long,
                                       (double)(gap - POSITION_EPS), (*blocked)->GetVelocityX(),
                                       (*blocked)->GetCarFollowingModel()->getMaxDecel()));
            }*/
        }
    }
    return state;
}


void
LCM_LC2013::saveBlockerLength(SurroundingMovingObjectsData* blocker, int lcaCounter, egoData* agent) {

    if (blocker != 0 && (blocker->GetState()->indicatorstate!=IndicatorState::IndicatorState_Off & lcaCounter) != 0) {
        // is there enough space in front of us for the blocker?
        const double potential = myLeftSpace - myCarFollowModel->brakeGap(
                                     &agent->GetState_Ego()->velocity_long);
        if ((blocker->GetProperties()->lx + *agent->GetDriverInformation()->MinGap) <= potential) {
            // save at least his length in myLeadingBlockerLength
            myLeadingBlockerLength = std::max((blocker->GetProperties()->lx + *agent->GetDriverInformation()->MinGap), myLeadingBlockerLength);

        } else {
            // we cannot save enough space for the blocker. It needs to save
            // space for ego instead

            //KBl blocker->GetLaneChangeModel()->saveBlockerLength(vehicleParameters.length + *agent->GetDriverInformation()->MinGap);
        }
    }
}


//KBl void
//KBl LCM_LC2013::adaptSpeedToPedestrians(const Lane* lane, double& v) {
//KBl     if (MSPModel::getModel()->hasPedestrians(lane)) {
//KBl #ifdef DEBUG_WANTS_CHANGE
//KBl         if (DEBUG_COND) {
//KBl             std::cout << SIMTIME << " adapt to pedestrians on lane=" << lane->getID() << "\n";
//KBl         }
//KBl #endif
//KBl         PersonDist leader = MSPModel::getModel()->nextBlocking(lane, myVehicle.getPositionOnLane(),
//KBl                             myVehicle.getRightSideOnLane(), myVehicle.getRightSideOnLane() + myVehicle.getVehicleType().getWidth(),
//KBl                             ceil(*agent->GetState_Ego()->velocity_long / myCarFollowModel->getMaxDecel()));
//KBl         if (lexists != 0) {
//KBl             const double stopSpeed = myCarFollowModel->stopSpeed(&myVehicle, *agent->GetState_Ego()->velocity_long, leader.second - myVehicle.getVehicleType().getMinGap());
//KBl             v = std::min(v, stopSpeed);
//KBl #ifdef DEBUG_WANTS_CHANGE
//KBl             if (DEBUG_COND) {
//KBl                 std::cout << SIMTIME << "    pedLeader=" << leader.first->getID() << " dist=" << leader.second << " v=" << v << "\n";
//KBl             }
//KBl #endif
//KBl         }
//KBl     }
//KBl }


void LCM_LC2013::addLCSpeedAdvice(const double vSafe, egoData* agent) {
    const double accel = SPEED2ACCEL(vSafe - agent->GetState_Ego()->velocity_long);
    myLCAccelerationAdvices.push_back(accel);
}


double
LCM_LC2013::computeSpeedLat(double latDist, double& maneuverDist, egoData* agent) {
    double speedBound = myMaxSpeedLatStanding + myMaxSpeedLatFactor * agent->GetState_Ego()->velocity_long;
    //KBl if (isChangingLanes()) {
    //KBl     // Don't stay caught in the middle of a lane change while vehicle is standing, workaround for #3771
    //KBl     speedBound = std::max(LC_RESOLUTION_SPEED_LAT, speedBound);
    //KBl }
    return std::max(-speedBound, std::min(speedBound,
                AbstractLaneChangeModel::computeSpeedLat(agent, maneuverDist)));
}

double
LCM_LC2013::getAssumedDecelForLaneChangeDuration(egoData* agent) const {
    return std::max(LC_ASSUMED_DECEL, agent->GetState_Ego()->acceleration_long);
}

double
LCM_LC2013::getSafetyFactor() const {
    return 1 / myAssertive;
}



long long LCM_LC2013::PrintBin(int n)
{
    long long binaryNumber = 0;
        int remainder, i = 1, step = 1;

        while (n!=0)
        {
            remainder = n%2;
            //std::cout << "Step " << step++ << ": " << n << "/2, Remainder = " << remainder << ", Quotient = " << n/2 << std::endl;
            n /= 2;
            binaryNumber += remainder*i;
            i *= 10;
        }
        return binaryNumber;
}

/****************************************************************************/

