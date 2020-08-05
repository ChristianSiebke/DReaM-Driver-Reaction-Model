/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    AbstractLaneChangeModel.h
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
// #ifndef AbstractLaneChangeModel_h
// #define AbstractLaneChangeModel_h

#pragma once

/* defines the epsilon to use on position comparison */
#define POSITION_EPS 0.1

// ===========================================================================
// included modules
// ===========================================================================

#include "CFModel.h"
#include <iostream>
#include <map>
#include <vector>
#include <utility>
#include "ContainerStructures.h"

class Lane;

// ===========================================================================
// used enumeration
// ===========================================================================
/// @enum LaneChangeModel
enum LaneChangeModel {
    LC2013,
    DK2008,
    SL2015,
    DEFAULT
};

enum LaneChangeAction {
    /// @name currently wanted lane-change action
    /// @{
    /// @brief No action desired
    LCA_NONE = 0,
    /// @brief Needs to stay on the current lane
    LCA_STAY = 1 << 0,
    /// @brief Wants go to the left
    LCA_LEFT = 1 << 1,
    /// @brief Wants go to the right
    LCA_RIGHT = 1 << 2,
    /// @brief The action is needed to follow the route (navigational lc)
    LCA_STRATEGIC = 1 << 3,
    /// @brief The action is done to help someone else
    LCA_COOPERATIVE = 1 << 4,
    /// @brief The action is due to the wish to be faster (tactical lc)
    LCA_SPEEDGAIN = 1 << 5,
    /// @brief The action is due to the default of keeping right "Rechtsfahrgebot"
    LCA_KEEPRIGHT = 1 << 6,
    /// @brief The action is due to a TraCI request
    LCA_TRACI = 1 << 7,
    /// @brief The action is urgent (to be defined by lc-model)
    LCA_URGENT = 1 << 8,
    /// @brief The action has not been determined
    LCA_UNKNOWN = 1 << 30,
    /// @}

    /// @name External state
    /// @{
    /// @brief The vehicle is blocked by left leader
    LCA_BLOCKED_BY_LEFT_LEADER = 1 << 9,
    /// @brief The vehicle is blocked by left follower
    LCA_BLOCKED_BY_LEFT_FOLLOWER = 1 << 10,
    /// @brief The vehicle is blocked by right leader
    LCA_BLOCKED_BY_RIGHT_LEADER = 1 << 11,
    /// @brief The vehicle is blocked by right follower
    LCA_BLOCKED_BY_RIGHT_FOLLOWER = 1 << 12,
    /// @brief The vehicle is blocked being overlapping
    LCA_OVERLAPPING = 1 << 13,
    /// @brief The vehicle does not have enough space to complete a continuous lane and change before the next turning movement
    LCA_INSUFFICIENT_SPACE = 1 << 14,
    /// @brief used by the sublane model
    LCA_SUBLANE = 1 << 15,
    /// @brief Vehicle is too slow to guarantee success of lane change (used for continuous lane changing in case that maxSpeedLatStanding==0)
    LCA_INSUFFICIENT_SPEED = 1 << 28,
    /// @brief lane can change
    LCA_WANTS_LANECHANGE = LCA_LEFT | LCA_RIGHT,
    /// @brief lane can change or stay
    LCA_WANTS_LANECHANGE_OR_STAY = LCA_WANTS_LANECHANGE | LCA_STAY,
    /// @brief blocked left
    LCA_BLOCKED_LEFT = LCA_BLOCKED_BY_LEFT_LEADER | LCA_BLOCKED_BY_LEFT_FOLLOWER,
    /// @brief blocked right
    LCA_BLOCKED_RIGHT = LCA_BLOCKED_BY_RIGHT_LEADER | LCA_BLOCKED_BY_RIGHT_FOLLOWER,
    /// @brief blocked by leader
    LCA_BLOCKED_BY_LEADER = LCA_BLOCKED_BY_LEFT_LEADER | LCA_BLOCKED_BY_RIGHT_LEADER,
    /// @brief blocker by follower
    LCA_BLOCKED_BY_FOLLOWER = LCA_BLOCKED_BY_LEFT_FOLLOWER | LCA_BLOCKED_BY_RIGHT_FOLLOWER,
    /// @brief blocked in all directions
    LCA_BLOCKED = LCA_BLOCKED_LEFT | LCA_BLOCKED_RIGHT | LCA_INSUFFICIENT_SPACE | LCA_INSUFFICIENT_SPEED,
    /// @brief reasons of lane change
    LCA_CHANGE_REASONS = (LCA_STRATEGIC | LCA_COOPERATIVE | LCA_SPEEDGAIN | LCA_KEEPRIGHT | LCA_SUBLANE | LCA_TRACI),
                         // LCA_BLOCKED_BY_CURRENT_LEADER = 1 << 28
                         // LCA_BLOCKED_BY_CURRENT_FOLLOWER = 1 << 29
                         /// @}

    /// @name originally model specific states (migrated here since
    ///       they were duplicated in all current models)
    /// @{
    LCA_AMBLOCKINGLEADER = 1 << 16,
    LCA_AMBLOCKINGFOLLOWER = 1 << 17,
    LCA_MRIGHT = 1 << 18,
    LCA_MLEFT = 1 << 19,
    // !!! never set LCA_UNBLOCK = 1 << 20,
    LCA_AMBLOCKINGFOLLOWER_DONTBRAKE = 1 << 21,
    // !!! never used LCA_AMBLOCKINGSECONDFOLLOWER = 1 << 22,
    LCA_CHANGE_TO_HELP = 1 << 23,
    // !!! never read LCA_KEEP1 = 1 << 24,
    // !!! never used LCA_KEEP2 = 1 << 25,
    LCA_AMBACKBLOCKER = 1 << 26,
    LCA_AMBACKBLOCKER_STANDING = 1 << 27
    /// @}
};

#define NUMERICAL_EPS   0.001

// the step length in seconds as double
#define TS (static_cast<double>(0.1))

// x*deltaT
#define SPEED2DIST(x) ((x)*TS)
// x/deltaT
#define DIST2SPEED(x) ((x)/TS)
// x*deltaT*deltaT
#define ACCEL2DIST(x) ((x)*TS*TS)
// x*deltaT
#define ACCEL2SPEED(x) ((x)*TS)
// x*deltaT
#define SPEED2ACCEL(x) ((x)/TS)

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class AbstractLaneChangeModel
 * @brief Interface for lane-change models
 */
class AbstractLaneChangeModel {
public:


    /** @class LCMessager
     * @brief A class responsible for exchanging messages between cars involved in lane-change interaction
     */
    class LCMessager {
    public:
        /** @brief Constructor
         * @param[in] leader The leader on the informed vehicle's lane
         * @param[in] neighLead The leader on the lane the vehicle want to change to
         * @param[in] neighFollow The follower on the lane the vehicle want to change to
         */
        LCMessager(const AgentInterface* leader, const AgentInterface* neighLead, const AgentInterface* neighFollow)
            : myLeader(leader), myNeighLeader(neighLead),
              myNeighFollower(neighFollow) { }


        /// @brief Destructor
        ~LCMessager() { }

    private:
        /// @brief The leader on the informed vehicle's lane
        const AgentInterface* myLeader;
        /// @brief The leader on the lane the vehicle want to change to
        const AgentInterface* myNeighLeader;
        /// @brief The follower on the lane the vehicle want to change to
        const AgentInterface* myNeighFollower;

    };

    struct StateAndDist {
        // @brief LaneChangeAction flags
        int state;
        // @brief Lateral distance to be completed in the next step
        double latDist;
        // @brief Full lateral distance required for the completion of the envisioned maneuver
        double maneuverDist;
        // @brief direction that was checked
        int dir;

        StateAndDist(int _state, double _latDist, double _targetDist, int _dir) :
            state(_state),
            latDist(_latDist),
            maneuverDist(_targetDist),
            dir(_dir) {}

        bool sameDirection(const StateAndDist& other) const {
            return latDist * other.latDist > 0;
        }
    };

    /** @brief Factory method for instantiating new lane changing models
     * @param[in] lcm The type of model to build
     * @param[in] vehicle The vehicle for which this model shall be built
     */
    static AbstractLaneChangeModel* build(int lcmId, double Cycletime, double SpeedGain, double KeepRight, double Cooperative, CFModel *CarFollowingModel, const VehicleModelParameters vehicleParameters);

    /** @brief Returns the model's ID;
     * @return The model's ID
     */
    virtual LaneChangeModel getModelID() const = 0;

    /// @brief whether lanechange-output is active
    static bool haveLCOutput() {
        return myLCOutput;
    }

    /// @brief whether start of maneuvers shall be recorede
    static bool outputLCStarted() {
        return myLCStartedOutput;
    }

    /// @brief whether start of maneuvers shall be recorede
    static bool outputLCEnded() {
        return myLCEndedOutput;
    }

    /** @brief Constructor
     * @param[in] v The vehicle this lane-changer belongs to
     * @param[in] model The type of lane change model
     */
    AbstractLaneChangeModel(double Cycletime, const LaneChangeModel model, const CFModel *CarFollowingModel, VehicleModelParameters vehicleParameters);

    /// @brief Destructor
    virtual ~AbstractLaneChangeModel();

    inline int getOwnState() const {
        return myOwnState;
    }

    inline int getPrevState() const {
        /// at the time of this call myPreviousState already holds the new value
        return myPreviousState2;
    }

    virtual void setOwnState(const int state);

    /// @brief Updates the remaining distance for the current maneuver while it is continued within non-action steps (only used by sublane model)
    void setManeuverDist(const double dist);
    /// @brief Returns the remaining unblocked distance for the current maneuver. (only used by sublane model)
    double getManeuverDist() const;

    /// @brief Updates the value of safe lateral distances (in SL2015) during maneuver continuation in non-action steps
    virtual void updateSafeLatDist(const double travelledLatDist);

    const std::pair<int, int>& getSavedState(const int dir) const {
        return mySavedStates.find(dir)->second;
    }

    bool hasSavedState(const int dir) const {
        return mySavedStates.find(dir) != mySavedStates.end();
    }

    void saveState(const int dir, const int stateWithoutTraCI, const int state) {
        std::pair<int, int> A = std::make_pair((stateWithoutTraCI) | (myCanceledStates[dir]), (state));
        mySavedStates.emplace(0,A);
        //  std::map<int, std::pair<int, int> > mySavedStates2;
        // std::pair<int, int>a = std::make_pair(1,1); //
        //        std::pair<int const, std::pair<int, int>> b = std::make_pair(dir,a);
        //         mySavedStates2.emplace(b);

    }

    void setFollowerGaps();
    void setLeaderGaps(bool lexists, SurroundingMovingObjectsData* leader, double secGap, egoData* agent);
    void setOrigLeaderGaps(bool lexists, SurroundingMovingObjectsData* leader, double secGap, egoData* agent);
    void setFollowerGaps(bool fexists, SurroundingMovingObjectsData* follower, double secGap, egoData* agent);

    virtual void prepareStep() {
        //myCanceledStates[1] = LCA_NONE;
        //myCanceledStates[0] = LCA_NONE;
        //myCanceledStates[-1] = LCA_NONE;
        //saveState(-1, LCA_UNKNOWN, LCA_UNKNOWN);
        //saveState(0, LCA_UNKNOWN, LCA_UNKNOWN);
        //saveState(1, LCA_UNKNOWN, LCA_UNKNOWN);
        myLastLateralGapRight = NO_NEIGHBOR;
        myLastLateralGapLeft = NO_NEIGHBOR;
        myLastLeaderGap = NO_NEIGHBOR;
        myLastLeaderSecureGap = NO_NEIGHBOR;
        myLastFollowerGap = NO_NEIGHBOR;
        myLastFollowerSecureGap = NO_NEIGHBOR;
        myLastOrigLeaderGap = NO_NEIGHBOR;
        myLastOrigLeaderSecureGap = NO_NEIGHBOR;
        myCommittedSpeed = 0;
    }

    /** @brief Called to examine whether the vehicle wants to change
     * using the given laneOffset.
     * This method gets the information about the surrounding vehicles
     * and whether another lane may be more preferable */
    virtual int wantsChange(
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

    }

    virtual void* inform(std::pair<double, int> info, AgentInterface* sender) = 0;

    /** @brief Called to adapt the speed in order to allow a lane change.
     *         It uses information on LC-related desired speed-changes from
     *         the call to wantsChange() at the end of the previous simulation step
     *
     * It is guaranteed that min<=wanted<=max, but the implementation needs
     * to make sure that the return value is between min and max.
     *
     * @param min The minimum resulting speed
     * @param wanted The aspired speed of the car following model
     * @param max The maximum resulting speed
     * @param cfModel The model used
     * @return the new speed of the vehicle as proposed by the lane changer
     */
    virtual double patchSpeed(const double *min, double wanted, const double *max, const double *v_x, const double *v_x_max, const double *a_x_max,
                               const CFModel* cfModel, const double *MinGap) const = 0;

    /* @brief called once when the primary lane of the vehicle changes (updates
     * the custom variables of each child implementation */
    virtual void changed() = 0;


    /// @brief return factor for modifying the safety constraints of the car-following model
    virtual double getSafetyFactor() const {
        return 1.0;
    }

    /// @brief called when a vehicle changes between lanes in opposite directions

    /** @brief Returns the lane the vehicle's shadow is on during continuous/sublane lane change
     * @return The vehicle's shadow lane
     */
    Lane* getShadowLane() const {
        return myShadowLane;
    }

    /// @brief return the shadow lane for the given lane
    Lane* getShadowLane(const Lane* lane) const;

    /// @brief return the shadow lane for the given lane and lateral offset
    Lane* getShadowLane(const Lane* lane, double posLat) const;

    /// @brief set the shadow lane
    void setShadowLane(Lane* lane) {
        myShadowLane = lane;
    }

    const std::vector<Lane*>& getShadowFurtherLanes() const {
        return myShadowFurtherLanes;
    }

    const std::vector<double>& getShadowFurtherLanesPosLat() const {
        return myShadowFurtherLanesPosLat;
    }

    /** @brief Returns the lane the vehicle has committed to enter during a sublane lane change
     *  @return The vehicle's target lane.
     */
    int getTargetLane() const {
        return myTargetLane;
    }

    const std::vector<Lane*>& getFurtherTargetLanes() const {
        return myFurtherTargetLanes;
    }

    /// @brief return whether the vehicle passed the midpoint of a continuous lane change maneuver
    inline bool pastMidpoint() const {
        return myLaneChangeCompletion >= 0.5;
    }

    /// @brief Calculates the maximal time needed to complete a lane change maneuver
    ///        if lcMaxSpeedLatFactor and lcMaxSpeedStanding are set and the vehicle breaks not harder than decel.
    ///        LC when the vehicle starts breaking now. If lcMaxSpeedStanding==0 the completion may be impossible,
    /// @param[in] speed Current longitudinal speed of the changing vehicle.
    /// @param[in] remainingManeuverDist dist which is still to be covered until LC is completed
    /// @param[in] decel Maximal assumed deceleration rate applied during the LC.
    /// @return maximal LC duration (or -1) if it is possible that it can't be completed.
    /// @note 1) For the calculation it is assumed that the vehicle starts breaking with decel (>=0) immediately.
    ///       If lcMaxSpeedStanding==0 the completion may be impossible, and -1 is returned.
    ///       2) In case that no maxSpeedLat is used to control lane changing, this is only called prior to a lane change,
    ///          and the duration is MSGlobals::gLaneChangeDuration.
    virtual double estimateLCDuration(const double speed, const double remainingManeuverDist, const double decel, const driverInformation* agent) const;

    /// @brief Get the current lane change completion ratio
    inline double getLaneChangeCompletion() const {
        return myLaneChangeCompletion;
    }

    /// @brief return the direction of the current lane change maneuver
    inline int getLaneChangeDirection() const {
        return myLaneChangeDirection;
    }

    /// @brief return the direction in which the current shadow lane lies
    int getShadowDirection() const;

    /// @brief return the angle offset during a continuous change maneuver
    double getAngleOffset() const;

    /// @brief reset the flag whether a vehicle already moved to false
    inline bool alreadyChanged() const {
        return myAlreadyChanged;
    }

    /// @brief reset the flag whether a vehicle already moved to false
    void resetChanged() {
        myAlreadyChanged = false;
    }

    /// @brief start the lane change maneuver and return whether it continues
    bool startLaneChangeManeuver(Lane* source, Lane* target, int direction);

    /* @brief continue the lane change maneuver and return whether the midpoint
     * was passed in this step
     */

    /* @brief update lane change shadow after the vehicle moved to a new lane */
    void updateShadowLane();

    /* @brief update lane change reservations after the vehicle moved to a new lane
     * @note  The shadow lane should always be updated before updating the target lane. */
    void updateTargetLane();

    /* @brief Determines the lane which the vehicle intends to enter during its current action step.
     *        targetDir is set to the offset of the returned lane with respect to the vehicle'a current lane. */
    Lane* determineTargetLane(int& targetDir) const;

    /* @brief clean up all references to the shadow vehicle
     */
    void cleanupShadowLane();

    /* @brief clean up all references to the vehicle on its target lanes
     */
    void cleanupTargetLane();

    /// @brief reserve space at the end of the lane to avoid dead locks
    virtual void saveBlockerLength(double length) {
    }

    void setShadowPartialOccupator(Lane* lane) {
        myPartiallyOccupatedByShadow.push_back(lane);
    }

    void setNoShadowPartialOccupator(Lane* lane) {
        myNoPartiallyOccupatedByShadow.push_back(lane);
    }

    /// @brief called once when the vehicles primary lane changes
    void primaryLaneChanged(Lane* source, Lane* target, int direction);

    /// @brief called once the vehicle ends a lane change manoeuvre (non-instant)
    void laneChangeOutput(const std::string& tag, Lane* source, Lane* target, int direction);

    bool isOpposite() const {
        return myAmOpposite;
    }

    double getCommittedSpeed() const {
        return myCommittedSpeed;
    }

    /// @brief return the lateral speed of the current lane change maneuver
    double getSpeedLat() const {
        return mySpeedLat;
    }

    void setSpeedLat(double speedLat) {
        mySpeedLat = speedLat;
    }

    /// @brief decides the next lateral speed depending on the remaining lane change distance to be covered
    ///        and updates maneuverDist according to lateral safety constraints.
    virtual double computeSpeedLat(egoData* agent, double& maneuverDist);

    /// @brief Returns a deceleration value which is used for the estimation of the duration of a lane change.
    /// @note  Effective only for continuous lane-changing when using attributes myMaxSpeedLatFactor and myMaxSpeedLatStanding. See #3771
    virtual double getAssumedDecelForLaneChangeDuration() const;

    /// @brief Check for commands issued for the vehicle via TraCI and apply the appropriate state changes
    ///        For the sublane case, this includes setting a new maneuver distance if appropriate.
    void checkTraCICommands();

    virtual int getWaitingSeconds(double velocity_x);

    static const double NO_NEIGHBOR;

protected:
    virtual bool congested(SurroundingMovingObjectsData* neighLeader, egoData* agent);

    virtual bool predInteraction(bool lexists, SurroundingMovingObjectsData* leader, egoData* agent);

    /// @brief whether the influencer cancels the given request
    bool cancelRequest(int state, int laneOffset);


protected:

    /// @brief The current state of the vehicle
    int myOwnState;
    /// @brief lane changing state from the previous simulation step
    int myPreviousState;
    /// @brief lane changing state from step before the previous simulation step
    int myPreviousState2;

    std::map<int, std::pair<int, int> > mySavedStates;
    std::map<int, int> myCanceledStates;

    /// @brief the current lateral speed
    double mySpeedLat;

    /// @brief the speed when committing to a change maneuver
    double myCommittedSpeed;

    /// @brief progress of the lane change maneuver 0:started, 1:complete
    double myLaneChangeCompletion;

    /// @brief direction of the lane change maneuver -1 means right, 1 means left
    int myLaneChangeDirection;

    /// @brief The complete lateral distance the vehicle wants to travel to finish its maneuver
    ///        Only used by sublane model, currently.
    double myManeuverDist;

    /// @brief whether the vehicle has already moved this step
    bool myAlreadyChanged;

    /// @brief A lane that is partially occupied by the front of the vehicle but that is not the primary lane
    Lane* myShadowLane;
    /* @brief Lanes that are partially (laterally) occupied by the back of the
     * vehicle (analogue to AgentInterface::myFurtherLanes) */
    std::vector<Lane*> myShadowFurtherLanes;
    std::vector<double> myShadowFurtherLanesPosLat;


    /// @brief The target lane for the vehicle's current maneuver
    /// @note  This is used by the sublane model to register the vehicle at lanes,
    ///        it will reach within the current action step, so vehicles on that lane
    ///        may react to the started lc-maneuver during the car-following process.
    ///        If the shadow lane is the same as the lc maneuver target, myTargetLane is
    ///        set to nullptr.
    ///        The current shadow lanes and further lanes should always be updated before updating the target lane.
    int myTargetLane;

    /* @brief Further upstream lanes that are affected by the vehicle's maneuver (analogue to AgentInterface::myFurtherLanes)
     * @note  If myTargetLane==nullptr, we may assume myFurtherTargetLanes.size()==0, otherwise we have
     *        myFurtherTargetLanes.size() == myVehicle.getFurtherLanes.size()
     *        Here it may occur that an element myFurtherTargetLanes[i]==nullptr if myFurtherLanes[i] has
     *        no parallel lane in the change direction.
     *  */
    std::vector<Lane*> myFurtherTargetLanes;

    /// @brief The vehicle's car following model
    const CFModel* myCarFollowModel;

    /// @brief list of lanes where the shadow vehicle is partial occupator
    std::vector<Lane*> myPartiallyOccupatedByShadow;

    /* @brief list of lanes where there is no shadow vehicle partial occupator
     * (when changing to a lane that has no predecessor) */
    std::vector<Lane*> myNoPartiallyOccupatedByShadow;

    /// @brief the minimum lateral gaps to other vehicles that were found when last changing to the left and right
    double myLastLateralGapLeft;
    double myLastLateralGapRight;

    /// @brief the actual minimum longitudinal distances to vehicles on the target lane
    double myLastLeaderGap;
    double myLastFollowerGap;
    /// @brief the minimum longitudinal distances to vehicles on the target lane that would be necessary for stringent security
    double myLastLeaderSecureGap;
    double myLastFollowerSecureGap;
    /// @brief acutal and secure distance to closest leader vehicle on the original when performing lane change
    double myLastOrigLeaderGap;
    double myLastOrigLeaderSecureGap;

    // @brief the maximum lateral speed when standing
    double myMaxSpeedLatStanding;
    // @brief the factor of maximum lateral speed to longitudinal speed
    double myMaxSpeedLatFactor;

    double Cycletime;

    int tWaiting = 0;

    int myCarFollowModelID;

    VehicleModelParameters vehicleParameters;

    /* @brief to be called by derived classes in their changed() method.
     * If dir=0 is given, the current value remains unchanged */
    void initLastLaneChangeOffset(int dir);

    /// @brief whether overtaking on the right is permitted
    static bool myAllowOvertakingRight;

    /// @brief whether to record lane-changing
    static bool myLCOutput;
    static bool myLCStartedOutput;
    static bool myLCEndedOutput;

    /// @brief the type of this model
    const LaneChangeModel myModel;

private:

    /// @brief whether the vehicle is driving in the opposite direction
    bool myAmOpposite;


private:
    /// @brief Invalidated assignment operator
};


// #endif

/****************************************************************************/

