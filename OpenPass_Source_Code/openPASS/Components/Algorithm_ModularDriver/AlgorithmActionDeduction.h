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
//! @file  AlgorithmActionDeduction.h
//! @author  Konstantin Blenz
//! @author  Daniel Krajzewicz
//! @author  Jakob Erdmann
//! @author  Friedemann Wesner
//! @author  Sascha Krieg
//! @author  Michael Behrisch
//! @date    Tue, 03.12.2019
//! @brief This file contains the calculations of possible actions
//!
//! This class contains the data calculations.
//! Singleton such that data calculations is generated only once and not seperately
//! for each instance.
//!
//-----------------------------------------------------------------------------

#pragma once

//#include <vector>
#include "Interfaces/stochasticsInterface.h"
#include "Interfaces/observationInterface.h"
#include <ContainerStructures.h>
#include "CFModel.h"
#include "CFModel_Daniel1.h"
#include "AbstractLaneChangeModel.h"
#include "LCM_LC2013.h"
#include "TargetBraking.h"

//! \brief provide data and calculate driver's action parameters.
//!
//! \ingroup AlgorithmActionDeduction

class ActionDeduction
{
public:
    ActionDeduction(int cycletime, StochasticsInterface *stochastics, ObservationInterface *observer); // avoid access of constructor of singleton outside class

    void Initialize();

    void SetActionDeduction_Input(ActionDeduction_Input &Input);

    void LogSetValues(int time);

    void GetActionDecution_Output_BU(ActionDeduction_Output_BU &AD_Output_BU);
    ActionDeduction_Output_TD* GetActionDecution_Output_TD();

    void CalcAccelerationWish(ActionDeduction_Output_BU &AD_Output_BU);
    double CalcFollowVelocityDiffWish(const double &v_long, const double &delta_s_front, const double &v_long_Front);
    double CalcFreeVelocityDiffWish(const double &v_long);

    /*!
     * \brief CheckLaneChange
     * \param time
     * \param AD_Output_BU
     */
    void CheckLaneChange(const int *time, ActionDeduction_Output_BU *AD_Output_BU);

    LaneChangeState CheckforLaneChange(const StaticEnvironmentData *Environment,
                                       double *vNext,
                                       double *aNext,
                                       const int *time,
                                       const int *spawnTime);

    LaneChangeState CheckChangeDirection(const StaticEnvironmentData *Environment,
                                         const int *time,
                                         const int *spawnTime);

    int checkChangeWithinEdge(int laneOffset,
                            const StaticEnvironmentData *Environment,
                            const std::vector<int>* preb) const;

    double ComputeAcceleration(double *vDelta, double decel);

    void prepareLanechange(ActionDeduction_Output_BU *AD_Output_BU);

    void checkForChanged();

    void SetMaxComfortAccel(double mcacc)
    {
        maxComfortAcceleration = mcacc;
    }
    void SetMinComfortDecel(double mcdec)
    {
        minComfortDeceleration = mcdec;
    }
    void SetComfortAccelDev(double caccDev)
    {
        comfortAccelDeviation = caccDev;
    }

    void SetTGapWish(double tGapWish)
    {
        this->tGapWish = tGapWish;
    }
    void SetSpeedGain(double SpeedGain)
    {
        this->SpeedGain = SpeedGain;
    }
    void SetSpeedGainDeviation(double SpeedGainDev)
    {
        this->SpeedGainDeviation = SpeedGainDev;
    }
    void SetKeepRight(double KeepRight)
    {
        this->KeepRight = KeepRight;
    }
    void SetKeepRightDeviation(double KeepRightDev)
    {
        this->KeepRightDeviation = KeepRightDev;
    }
    void SetCooperative(double Cooperative)
    {
        this->Cooperative = Cooperative;
    }
    void SetCooperativeDeviation(double CooperativeDev)
    {
        this->CooperativeDeviation = CooperativeDev;
    }
    void SetVehicleParameters (const VehicleModelParameters *vehicleParameters)
    {
        this->vehicleParameters = vehicleParameters;
    }
    bool InitialValuesLogged()
    {
        return Logged;
    }

    //!
    //! \brief AccTargetBrake
    //! brake constant to stop in given distance from given velocity
    //! \param v    start velocity
    //! \param ds   target distance
    //! \return     acceleration to brake in distance to vtarget=0
    //!
    double AccTargetBrake(double v, double ds)
    {
        return (- v*v / (2*ds));
    }

    double NextRealBrakeVelocity(double a, double b, double c, double t)
    {
        return a*pow(t,4) - b*t*t + c;
    }

private:

    int CycleTime = 100;
    StochasticsInterface *stochastics;
    TargetBraking targetBraking;
    ObservationInterface *observation;

    ActionDeduction_Input *AD_Input;
    std::map<RelationType,SurroundingMovingObjectsData> *NearTraffic;
    State *State;
    State_Ego *State_Ego;
    DriverInformation *DriverInformation;

    bool Logged = false;

    ActionDeduction_Output_TD AD_Output_TD;

    double tGapWish; //mean of lognormal-distribution for the wish-netto-timegap between leader and ego in s
    double minComfortDeceleration = -4;
    double maxComfortAcceleration = 3;
    double comfortAccelDeviation = 0.5;

    // Lanechangemodel Parameters from SUMO
    double SpeedGain = 1;
    double KeepRight = 1;
    double Cooperative = 0.1;    
    double SpeedGainDeviation = 0;
    double KeepRightDeviation = 0;
    double CooperativeDeviation = 0;

    bool targetbrakestarted = false;
    double t_brk = 0;
    double t_a = 0;
    double a = 0;
    double b = 0;
    double c = 0;

    LaneChangeState LaneChangeWish = LaneChangeState::NoLaneChange;
    CFModel *CarFollowingModel;
    AbstractLaneChangeModel *LaneChangeModel;

    bool IsChangingLanes;
    LaneChangeState LastSavedChangeStatus;
    int savedstateright;
    int savedstateleft;
    double last_lat_displacement_target = 0;
    double trigger = 0;

    int lastStepLaneId;

    LaneChangeState LCState;

    //! Calulaction parameters for the lateral acceleration wish [m/s].
    struct LateralDynamicConstants
    {
        //! Typical acceleration for a lane change [m/s].
        const double lateralAcceleration = 1.5;
        //! Typical lateral damping for a lane change [m/s].
        const double zeta = 1.0;
        //! aggressiveness of the controller for heading errors
        const double gainHeadingError = 7.5;
    };

    //! parameters for the lateral acceleration wish [m/s]
    const LateralDynamicConstants lateralDynamicConstants;

    const VehicleModelParameters *vehicleParameters;

};


