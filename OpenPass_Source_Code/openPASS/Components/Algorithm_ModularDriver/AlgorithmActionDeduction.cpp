/******************************************************************************
* Copyright (c) 2019 AMFD GmbH
* Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
*
* This program and the accompanying materials are made available under the
* terms of the Eclipse Public License 2.0 which is available at
* https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*****************************************************************************/

//-----------------------------------------------------------------------------
//! @file  AlgorithmActionDeduction.cpp
//! @author  Konstantin Blenz
//! @author  Daniel Krajzewicz
//! @author  Jakob Erdmann
//! @author  Friedemann Wesner
//! @author  Sascha Krieg
//! @author  Michael Behrisch
//! @date    Tue, 03.12.2019
//! @brief This file contains the calculations
//!
//! This class contains the data calculations of possible actions
//! Singleton such that data calculations is generated only once and not seperately
//! for each instance.
//!
//-----------------------------------------------------------------------------

//#include <cassert>
//#include <cmath>
//#include <array>
//#include <limits>
#include <iostream>
#include "AlgorithmActionDeduction.h"
#include "commonTools.h"

ActionDeduction::ActionDeduction(int cycletime, StochasticsInterface *stochastics, ObservationInterface *observer) :
    CycleTime(cycletime),
    stochastics(stochastics),
    targetBraking(cycletime, stochastics),
    observation(observer)
{
}

void ActionDeduction::SetActionDeduction_Input(ActionDeduction_Input &Input)
{
    AD_Input = {};
    AD_Input = &Input;
    State = AD_Input->MM_O->Ego->GetState();
    State_Ego = AD_Input->MM_O->Ego->GetState_Ego();
    DriverInformation = AD_Input->MM_O->Ego->GetDriverInformation();
    NearTraffic = &AD_Input->SA_O_BU->NearTraffic;
}

void ActionDeduction::GetActionDecution_Output_BU(ActionDeduction_Output_BU &AD_Output_BU)
{
    AD_Output_BU.gear                              = &AD_Input->MM_O->Ego->GetState_Ego()->currentGear; //GetAgent()->GetGear();
    AD_Output_BU.out_lateral_heading_error         = -AD_Input->MM_O->Ego->GetState()->roadPos.hdg;
    AD_Output_BU.out_lateral_gain_heading_error    = lateralDynamicConstants.gainHeadingError;
    AD_Output_BU.out_curvature                     = AD_Input->MM_O->EnvironmentInfo->roadGeometry.laneEgo.curvature;
    // AD_Output_BU->LatDisplacement = AD_Input->SA_O_BU->Ego.roadPos->t;
    AD_Output_BU.out_lateral_frequency             = std::sqrt(lateralDynamicConstants.lateralAcceleration / AD_Input->MM_O->EnvironmentInfo->roadGeometry.laneEgo.width);
    AD_Output_BU.out_lateral_damping               = lateralDynamicConstants.zeta;
    AD_Output_BU.notifyCollision                   = false;
    AD_Output_BU.steeringWheelAngle                = &AD_Input->MM_O->Ego->GetState_Ego()->steeringWheelAngle;
}

ActionDeduction_Output_TD* ActionDeduction::GetActionDecution_Output_TD()
{
    return &AD_Output_TD;
}

void ActionDeduction::CalcAccelerationWish(ActionDeduction_Output_BU &AD_Output_BU)
{
    double delta_v_Ego;
    bool agentinfront = NearTraffic->find(RelationType::Leader) != NearTraffic->end();
    double v_long = AD_Input->MM_O->Ego->GetState_Ego()->velocity_long;
    stopSign *StopSign = &AD_Input->SA_O_BU->AssessedEnvironment.StopSign;

    double decel = minComfortDeceleration;

    if (agentinfront)
    {
        SurroundingMovingObjectsData Leader = NearTraffic->at(RelationType::Leader);
        if (StopSign->exists
            && (StopSign->Sign->distanceToStartOfRoad - AD_Input->MM_O->Ego->GetState()->roadPos.s >= -0.1 &&
                    (StopSign->Sign->distanceToStartOfRoad + 1 < Leader.GetState()->roadPos.s + Leader.GetProperties()->distanceReftoLeadingEdge
                || (StopSign->Sign->distanceToStartOfRoad - Leader.GetState()->roadPos.s + Leader.GetProperties()->distanceReftoLeadingEdge < 1 && Leader.GetState()->velocity_long>1))) )
        {
            delta_v_Ego = targetBraking.ReactOnStopSign(StopSign,
                                                        v_long,
                                                        vehicleParameters->distanceReferencePointToLeadingEdge,
                                                        *DriverInformation->v_Wish,
                                                        maxComfortAcceleration);
        }
        else
        {
            CarFollowingModel->setHeadwayTime(*AD_Input->SA_O_BU->Ego->GetDriverInformation()->thw_Wish);
            double delta_s_front = *Leader.GetDistanceToEgo() - *DriverInformation->MinGap;
            double v_long_Front = Leader.GetState()->velocity_long;
            double netttc = TrafficHelperFunctions::CalculateNetTTC(v_long, v_long_Front, delta_s_front);

            if (netttc < 3)
                decel = vehicleParameters->maxDeceleration;

            delta_v_Ego = CalcFollowVelocityDiffWish(v_long, delta_s_front, v_long_Front);
        }
    }
    else
    {
        if(StopSign->exists && StopSign->Sign->distanceToStartOfRoad - AD_Input->MM_O->Ego->GetState()->roadPos.s >= -0.1)
        {
            delta_v_Ego = targetBraking.ReactOnStopSign(StopSign,
                                                        v_long,
                                                        vehicleParameters->distanceReferencePointToLeadingEdge,
                                                        *DriverInformation->v_Wish,
                                                        maxComfortAcceleration);
        }
        else
        {
            delta_v_Ego = CarFollowingModel->freeSpeed(v_long, maxComfortAcceleration, 0, *DriverInformation->v_Wish, false) - v_long;
        }
    }

    AD_Output_BU.velocity_long_wish = v_long + delta_v_Ego;

    AD_Output_BU.acceleration_long_wish = ComputeAcceleration(&delta_v_Ego, decel);

}

double ActionDeduction::CalcFollowVelocityDiffWish(const double &v_long, const double &delta_s_front, const double &v_long_Front)
{
    double v_long_Safe = CarFollowingModel->followSpeed(v_long, delta_s_front, v_long_Front, vehicleParameters->maxVelocity);
    v_long_Safe = std::min(std::min(*DriverInformation->v_Wish, v_long_Safe),
                        vehicleParameters->maxVelocity + *DriverInformation->commonSpeedLimit_Violation);

    return (v_long_Safe-v_long);
}

double ActionDeduction::CalcFreeVelocityDiffWish(const double &v_long)
{
    return (std::min(*DriverInformation->v_Wish, vehicleParameters->maxVelocity) - v_long);
}

void ActionDeduction::CheckLaneChange(const int *time, ActionDeduction_Output_BU *AD_Output_BU)
{
    AD_Output_BU->LCState == LaneChangeState::NoLaneChange;
    const int *spawntime = &State->spawntime;
    double *v_long_next = &AD_Output_BU->velocity_long_wish;
    double *a_long_next = &AD_Output_BU->acceleration_long_wish;
    const StaticEnvironmentData *Environment = AD_Input->MM_O->EnvironmentInfo;

    AD_Output_BU->LCState = CheckforLaneChange(Environment, v_long_next, a_long_next, time, spawntime);

}

LaneChangeState ActionDeduction::CheckforLaneChange(const StaticEnvironmentData *Environment,
                                    double *vNext,
                                    double *aNext,
                                    const int *time,
                                    const int *spawnTime)
{
    double vSafe = State_Ego->velocity_long + CycleTime* *aNext *0.001; //From CarFollowingModel

    int spawnoffset = 500;

    if (*time == *spawnTime + spawnoffset)
        LaneChangeModel->prepareStep();

    if (*time > *spawnTime + spawnoffset && State_Ego->velocity_long!=0 )
    {
        // LaneChangeModel->prepareStep();
        LaneChangeWish = CheckChangeDirection(Environment, time, spawnTime);
        *vNext = CarFollowingModel->finalizeSpeed(&State_Ego->velocity_long, &vehicleParameters->maxVelocity, &maxComfortAcceleration, &vSafe, DriverInformation->MinGap, LaneChangeModel);
        //TODO Output vsaved = vNext;
        double vDelt = *vNext - State_Ego->velocity_long;
        *aNext = ComputeAcceleration(&vDelt, vehicleParameters->maxDeceleration);
        //TODO Output asaved = acceleration;
    }

    //CHECK
    //if (State->id==14)
    //{
    //    std::cout << "vNext: " << *vNext << ", aNext: " << *aNext << std::endl;
    //}
    return LaneChangeWish;
}

LaneChangeState ActionDeduction::CheckChangeDirection(const StaticEnvironmentData *Environment,
                                                      const int *time,
                                                      const int *spawnTime)
{
    std::vector<int> preb(4);
    preb[0]=2;
    preb[1]=3;
    preb[2]=4;
    preb[3]=5;

    bool *LaneChangeStatus = &IsChangingLanes;
    bool startChange = false;
    LaneChangeState DirectionWish = LaneChangeState::NoLaneChange;

    int stateRight = 0;
    if (Environment->roadGeometry.laneRight.exists) // && !(*numoflanes==4 && Ego->laneid-1 == *rightmost))
    { // && !GetAgent()->GetLaneChangeStatus()) {
        stateRight = checkChangeWithinEdge(-1, Environment, &preb);
        // change if the vehicle wants to and is allowed to change
        bool a = ((stateRight & LCA_RIGHT) != 0);
        bool b = ((stateRight & LCA_BLOCKED) == 0);
        //if (((stateRight & LCA_RIGHT) != 0) && ((stateRight & LCA_BLOCKED) == 0))
        if (a && b && (stateRight == savedstateright))
        {
            LaneChangeModel->setOwnState(stateRight);

            DirectionWish=LaneChangeState::LaneChangeRight;
            startChange = true;
            LaneChangeModel->prepareStep();
        }

        savedstateright = stateRight;

    }

    // check whether the vehicle wants and is able to change to left lane
    int stateLeft = 0;
    if (Environment->roadGeometry.laneLeft.exists) // Ego->laneid + 1 <= *leftmost){ //KBl && !GetAgent()->GetLaneChangeStatus()) {
    {
        stateLeft = checkChangeWithinEdge(1, Environment, &preb);
        // change if the vehicle wants to and is allowed to change
        bool a = ((stateLeft & LCA_LEFT) != 0);
        bool b = ((stateLeft & LCA_BLOCKED) == 0);
        if (a && b && (stateLeft == savedstateleft))
        {
                DirectionWish=LaneChangeState::LaneChangeLeft;
                LaneChangeModel->setOwnState(stateLeft);
                startChange=true;
                LaneChangeModel->prepareStep();
        }
        savedstateleft = stateLeft;
        // else
        // {
        //     LaneChangeWish=LaneChangeState::NoLaneChange;
        // }

    }

    if ((stateRight & LCA_URGENT) != 0 && (stateLeft & LCA_URGENT) != 0) {
        // ... wants to go to the left AND to the right
        // just let them go to the right lane...
        stateLeft = 0;
        DirectionWish=LaneChangeState::LaneChangeRight;
        LaneChangeModel->prepareStep();

    }


    LaneChangeModel->setOwnState(stateRight | stateLeft);

    checkForChanged();

    //CHECK
    // if (State->id == 11)
    // {
    //     std::cout<<State->id<< ": " << stateLeft << ", " << stateRight << ", lid: " << State->laneid << std::endl;
    //     //if (NearTraffic->find(RelationType::Leader)!=NearTraffic->end())
    //     //std::cout<< "l: " << NearTraffic->at(RelationType::Leader).GetState()->id << ", " << *NearTraffic->at(RelationType::Leader).GetDistanceToEgo() << std::endl;
    //     //if (NearTraffic->find(RelationType::LeaderLeft)!=NearTraffic->end())
    //     //std::cout<< "ll: " << NearTraffic->at(RelationType::LeaderLeft).GetState()->id << ", " << *NearTraffic->at(RelationType::LeaderLeft).GetDistanceToEgo() << std::endl;
    //     // std::cout<< State->roadPos.t << std::endl;
    // }

    return DirectionWish;
}

int ActionDeduction::checkChangeWithinEdge(int laneOffset,
                                    const StaticEnvironmentData *Environment,
                                    const std::vector<int>* preb) const
{
    int blocked = 0;
    int blockedByLeader = 0;
    int blockedByFollower = 0;
    bool nlexists = false;
    bool nfexists = false;
    bool lexists = false;
    bool fexists = false;
    SurroundingMovingObjectsData *NeighLeader;
    SurroundingMovingObjectsData *NeighFollower;
    SurroundingMovingObjectsData *Leader;
    SurroundingMovingObjectsData *Follower;
    RelationType neighlead = laneOffset==-1 ? RelationType::LeaderRight : RelationType::LeaderLeft;
    RelationType neighfollow = laneOffset==-1 ? RelationType::FollowerRight : RelationType::FollowerLeft;
    RelationType leader = RelationType::Leader;
    RelationType follower = RelationType::Follower;

    if (NearTraffic->find(neighlead)!=NearTraffic->end())
    {
        nlexists = true;
        NeighLeader = &NearTraffic->at(neighlead);
    }
    if (NearTraffic->find(neighfollow)!=NearTraffic->end())
    {
        nfexists = true;
        NeighFollower = &NearTraffic->at(neighfollow);
    }
    if (NearTraffic->find(leader)!=NearTraffic->end())
    {
        lexists = true;
        Leader = &NearTraffic->at(leader);
    }
    if (NearTraffic->find(follower)!=NearTraffic->end())
    {
        fexists = true;
        Follower = &NearTraffic->at(follower);
    }

    if (nlexists)
    {
        blockedByLeader = (laneOffset == -1 ? LCA_BLOCKED_BY_RIGHT_LEADER : LCA_BLOCKED_BY_LEFT_LEADER);

        // overlap
        if (*NeighLeader->GetDistanceToEgo() <= 0) {
            blocked |= (blockedByLeader | LCA_OVERLAPPING);
        }
    }
    if (nfexists)
    {
        blockedByFollower = (laneOffset == -1 ? LCA_BLOCKED_BY_RIGHT_FOLLOWER : LCA_BLOCKED_BY_LEFT_FOLLOWER);

        // overlap
        if (*NeighFollower->GetDistanceToEgo() <= 0)
        {
            blocked |= (blockedByFollower | LCA_OVERLAPPING);
        }
    }


    double secureFrontGap = AbstractLaneChangeModel::NO_NEIGHBOR;
    double secureBackGap = AbstractLaneChangeModel::NO_NEIGHBOR;
    double secureOrigFrontGap = AbstractLaneChangeModel::NO_NEIGHBOR;

    // safe back gap
    if ((blocked & blockedByFollower) == 0 && nfexists) {
        // Calculate secure gap conservatively with vNextFollower / vNextLeader as
        // extrapolated speeds after the driver's expected reaction time (tau).
        // NOTE: there exists a possible source for collisions if the follower and the leader
        //       have desynchronized action steps as the extrapolated speeds can be exceeded in this case

        // Expected reaction time (tau) for the follower-vehicle.
        // (substracted TS since at this point the vehicles' states are already updated)
        // XXX: How does the ego vehicle know the value of tau for the neighboring vehicle?
        //const double followerTauRemainder = std::max(neighFollow->thw_to_ego - TS, 0.);
        //const double vNextFollower = *neighFollow->velocity_x_vehicle + 0.1 * *neighFollow->acceleration_long;//KBl followerTauRemainder * neighFollow.first->GetAccelerationX();
        //const double vNextLeader = *State_Ego->velocity_long + 0.1 * *Ego->acceleration_long;//KBl followerTauRemainder * vehicle->GetAccelerationX();
        // !!! eigentlich: vsafe braucht die Max. Geschwindigkeit beider Spuren
        secureBackGap = CarFollowingModel->getSecureGap(&NeighFollower->GetState()->velocity_long, &State_Ego->velocity_long, &vehicleParameters->maxDeceleration);
        if (*NeighFollower->GetDistanceToEgo() <= secureBackGap * 0.9) { //vehicle->GetLaneChangeModel()->getSafetyFactor()) {

            blocked |= blockedByFollower;
        }
    }

    // safe front gap
    if ((blocked & blockedByLeader) == 0 && nlexists) {
        // Calculate secure gap conservatively with vNextFollower / vNextLeader as
        // extrapolated speeds after the driver's expected reaction time (tau).
        // NOTE: there exists a possible source for collisions if the follower and the leader
        //       have desynchronized action steps as the extrapolated speeds can be exceeded in this case

        // Expected reaction time (tau) for the follower-vehicle.
        // (substracted TS since at this point the vehicles' states are already updated)
        // XXX: How does the ego vehicle know the value of tau for the neighboring vehicle?
        //const double followerTauRemainder = std::max(vehicle->GetCarFollowingModel()->getHeadwayTime() - TS, 0.);
        double vNextFollower = State_Ego->velocity_long + 0.1 * State_Ego->acceleration_long;//KBl followerTauRemainder * vehicle->GetAccelerationX();
        double vNextLeader = NeighLeader->GetState()->velocity_long + 0.1 * NeighLeader->GetState()->acceleration_long;//KBl followerTauRemainder * NeighLeader.first->GetAccelerationX();
        // !!! eigentlich: vsafe braucht die Max. Geschwindigkeit beider Spuren
        secureFrontGap = CarFollowingModel->getSecureGap(&vNextFollower,
                         &vNextLeader, &vehicleParameters->maxDeceleration);
        if (*NeighLeader->GetDistanceToEgo() <= secureFrontGap * 0.9)  {//vehicle->GetLaneChangeModel()->getSafetyFactor()) {
            blocked |= blockedByLeader;
        }
    }

    if (lexists) {
        secureOrigFrontGap = CarFollowingModel->getSecureGap(&State_Ego->velocity_long, &Leader->GetState()->velocity_long, &vehicleParameters->maxDeceleration);
    }

    double occupancy=0; //TODO
    double targetoccupancy=0; //TODO
    const int *egolaneid = &Environment->roadGeometry.laneEgo.laneid;


    //AbstractLaneChangeModel::MSLCMessager msg(leader.first, NeighLeader.first, neighFollow.first);
    int state = blocked | LaneChangeModel->wantsChange(*egolaneid,
                                                       laneOffset,
                                                       lexists, nlexists, nfexists,
                                                       AD_Input->SA_O_BU->Ego,
                                                       blocked,
                                                       Leader,
                                                       NeighLeader,
                                                       NeighFollower,
                                                       (*egolaneid + &laneOffset),
                                                       *preb,
                                                       &occupancy,
                                                       &targetoccupancy,
                                                       &Environment->roadGeometry);

    if (blocked == 0 && (state & LCA_WANTS_LANECHANGE) != 0 && nlexists)
    {
        // do a more careful (but expensive) check to ensure that a
        // safety-critical leader is not being overlooked
        const double *seen = &State_Ego->distance_to_end_of_lane;
        const double *speed = &State_Ego->velocity_long;
        const double dist = CarFollowingModel->brakeGap(&State_Ego->velocity_long) + *DriverInformation->MinGap;
    }


    if ((state & LCA_BLOCKED) == 0 && (state & LCA_WANTS_LANECHANGE) != 0) { //KBl && MSGlobals::gLaneChangeDuration > DELTA_T) {
        // Ensure that a continuous lane change manoeuvre can be completed before the next turning movement.
        // Assume lateral position == 0. (If this should change in the future add + laneOffset*vehicle->getLateralPositionOnLane() to distToNeighLane)
        const double distToNeighLane = laneOffset == -1 ? (State->roadPos.t<0 ? 3.75 + State->roadPos.t : 3.75 - State->roadPos.t) : (State->roadPos.t <0 ? 3.75 - State->roadPos.t : 3.75 + State->roadPos.t) ; //0.5*(3.75 + 3.75); //KBl vehicle->getLane()->getWidth() + targetLane->getWidth());
        //distToNeigLane = laneOffset==-1 ? distToNeighLane =
        // Extrapolate the LC duration if operating with speed dependent lateral speed.
        const double assumedDecel = LaneChangeModel->getAssumedDecelForLaneChangeDuration();
        double estimatedLCDuration = LaneChangeModel->estimateLCDuration(State_Ego->velocity_long, distToNeighLane, assumedDecel, DriverInformation);
        if (estimatedLCDuration==-1)
        {
            state |= LCA_INSUFFICIENT_SPEED;
        }
        else
        {
            // Compute covered distance, when braking for the whole lc duration
            const double decel = vehicleParameters->maxDeceleration * estimatedLCDuration;
            const double avgSpeed = 0.5 * (
                    std::max(0., State_Ego->velocity_long + ACCEL2SPEED(vehicleParameters->maxDeceleration)) +
                    std::max(0., State_Ego->velocity_long + decel));
            // Distance required for lane change.
            const double space2change = avgSpeed * estimatedLCDuration;
            // Available distance for LC maneuver (distance till next turn)
            double seen = lexists? *Leader->GetDistanceToEgo() + Leader->GetState()->velocity_long * estimatedLCDuration : State_Ego->distance_to_end_of_lane;//KBl - vehicle->GetRoadPosition().s;

            // for finding turns it doesn't matter whether we look along the current lane or the target lane
            int bestLaneConts = State->laneid;
            int view = 1;
            int nextLane = State->laneid;

            if (lexists && Leader->GetState()->velocity_long==0)
                estimatedLCDuration = estimatedLCDuration * 0.5;

            if (seen < estimatedLCDuration * State_Ego->velocity_long)
            {
            }

            if ((state & LCA_BLOCKED) == 0)
            {
                // check for dangerous leaders in case the target lane changes laterally between
                // now and the lane-changing midpoint
                seen = State_Ego->distance_to_end_of_lane;
                nextLane = State->laneid;
                view = 1;
                const double dist = CarFollowingModel->brakeGap(&State_Ego->velocity_long) + *DriverInformation->MinGap;
                if (seen <= space2change && seen <= dist)
                {
                    nextLane = State->laneid;
                }
            }
        }
    }
    const int oldstate = state;

    LaneChangeModel->saveState(laneOffset, oldstate, state);
    if (blocked == 0 && (state & LCA_WANTS_LANECHANGE))
    {
        // this lane change will be executed, save gaps
        LaneChangeModel->setFollowerGaps(nfexists, NeighFollower, secureBackGap, AD_Input->SA_O_BU->Ego);// neighFollow, secureBackGap);
        LaneChangeModel->setLeaderGaps(nlexists, NeighLeader, secureFrontGap, AD_Input->SA_O_BU->Ego);
        LaneChangeModel->setOrigLeaderGaps(lexists, Leader, secureOrigFrontGap, AD_Input->SA_O_BU->Ego);
    }
    return state;

}

double ActionDeduction::ComputeAcceleration(double *vDelta, double decel)
{
    double acceleration = 0;
    if (*vDelta>0)
    {
        acceleration= std::min(maxComfortAcceleration, (*vDelta)/(CycleTime*0.001));
    }
    else if (*vDelta<0)
    {
        acceleration = std::max(decel, ((*vDelta)/(CycleTime*0.001)));
    }
    else
    {
        acceleration = 0;
    }
    return acceleration;
}

void ActionDeduction::prepareLanechange(ActionDeduction_Output_BU *AD_Output_BU)
{
    double out_lat_displacement = 0;

    if (AD_Output_BU->LCState == LaneChangeState::NoLaneChange &&
        LastSavedChangeStatus == LaneChangeState::NoLaneChange)
    {
        out_lat_displacement = -State->roadPos.t;
        if (State->roadPos.t >= 0.3)
        {
            trigger-= 0.05;
        }
        else if (State->roadPos.t <= -0.3)
        {
            trigger+= 0.05;
        }
        else
        {
            trigger = -State->roadPos.t;
        }
    }
    else if (AD_Output_BU->LCState == LaneChangeState::NoLaneChange &&
             LastSavedChangeStatus == LaneChangeState::LaneChangeLeft)
    {
        trigger+= 0.05;
    }
    else if (AD_Output_BU->LCState == LaneChangeState::NoLaneChange &&
             LastSavedChangeStatus == LaneChangeState::LaneChangeRight)
    {
        trigger-= 0.05;
    }
    else if (AD_Output_BU->LCState==LaneChangeState::LaneChangeLeft)
    {
        trigger+= 0.05;
    }
    else if (AD_Output_BU->LCState==LaneChangeState::LaneChangeRight)
    {
        trigger-= 0.05;
    }
    trigger = std::max(-0.3, std::min(0.3, trigger));
    AD_Output_BU->out_lateral_displacement = trigger;
    LastSavedChangeStatus = AD_Output_BU->LCState;
}

void ActionDeduction::checkForChanged()
{
    if (lastStepLaneId != State->laneid)
    {
        LaneChangeModel->changed();
    }
    lastStepLaneId = State->laneid;
}

void ActionDeduction::Initialize()
{
    maxComfortAcceleration = std::max(1.0, stochastics->GetNormalDistributed(maxComfortAcceleration, comfortAccelDeviation));
    minComfortDeceleration = std::min(-0.5, stochastics->GetNormalDistributed(minComfortDeceleration, comfortAccelDeviation));
    CarFollowingModel = new CFModel_Daniel1(vehicleParameters->maxAcceleration, vehicleParameters->maxDeceleration, vehicleParameters->maxAcceleration, vehicleParameters->maxDeceleration, tGapWish, CycleTime);

    double SpeedGainInit = SpeedGain < 0 ? 1 : SpeedGain;
    double KeepRightInit = KeepRight < 0 ? 3 : KeepRight;
    double CooperativeInit = Cooperative < 0 && Cooperative >1 ? 0.1 : Cooperative;

    SpeedGain = stochastics->GetNormalDistributed(SpeedGain, SpeedGainDeviation);
    KeepRight = stochastics->GetNormalDistributed(KeepRight, KeepRightDeviation);
    Cooperative = stochastics->GetNormalDistributed(Cooperative, CooperativeDeviation);

    while (SpeedGain <0)
    {
        SpeedGain = SpeedGainInit;
        SpeedGain = stochastics->GetNormalDistributed(SpeedGain, SpeedGainDeviation);
    }
    while (KeepRight <0)
    {
        KeepRight = KeepRightInit;
        KeepRight = stochastics->GetNormalDistributed(KeepRight, KeepRightDeviation);
    }
    while (Cooperative <0 && Cooperative >1)
    {
        Cooperative = CooperativeInit;
        Cooperative = stochastics->GetNormalDistributed(Cooperative, CooperativeDeviation);
    }

    LaneChangeModel = AbstractLaneChangeModel::build(0, CycleTime, SpeedGain, KeepRight, Cooperative, CarFollowingModel, *vehicleParameters);
    targetBraking.UpdateCFModel(CarFollowingModel);
}

void ActionDeduction::LogSetValues(int time)
{
    Logged = true;
    observation->Insert(time,
                        AD_Input->MM_O->Ego->GetState()->id,
                        LoggingGroup::Driver,
                        "MinComfortDeceleration",
                        std::to_string(minComfortDeceleration));
    observation->Insert(time,
                        AD_Input->MM_O->Ego->GetState()->id,
                        LoggingGroup::Driver,
                        "MaxComfortAcceleration",
                        std::to_string(maxComfortAcceleration));
    observation->Insert(time,
                        AD_Input->MM_O->Ego->GetState()->id,
                        LoggingGroup::Driver,
                        "SpeedGain",
                        std::to_string(SpeedGain));
    observation->Insert(time,
                        AD_Input->MM_O->Ego->GetState()->id,
                        LoggingGroup::Driver,
                        "KeepRight",
                        std::to_string(KeepRight));
    observation->Insert(time,
                        AD_Input->MM_O->Ego->GetState()->id,
                        LoggingGroup::Driver,
                        "Cooperative",
                        std::to_string(Cooperative));
}
