/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    CFModel_Daniel1.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Tue, 05 Jun 2012
/// @version $Id$
///
// The original Krauss (1998) car-following model and parameter
/****************************************************************************/
#ifndef CFModel_Daniel1_h
#define CFModel_Daniel1_h

// ===========================================================================
// included modules
// ===========================================================================


#include "CFModel.h"
#include <algorithm>


// ===========================================================================
// class definitions
// ===========================================================================
/** @class CFModel_Daniel1
 * @brief The original Krauss (1998) car-following model and parameter
 * @see CFModel
 */
class CFModel_Daniel1 : public CFModel {
public:
    /** @brief Constructor
     *  @param[in] vtype the type for which this model is built and also the parameter object to configure this model
     */
    CFModel_Daniel1(double Accel, double Decel, double maxAccel, double maxDecel, double Headwaytime, double CycleTime);


    /// @brief Destructor
    ~CFModel_Daniel1();


    /// @name Implementations of the CFModel interface
    /// @{

    /** @brief Applies interaction with stops and lane changing model influences
     * @param[in] veh The ego vehicle
     * @param[in] vPos The possible velocity
     * @return The velocity after applying interactions with stops and lane change model influences
     */
    double finalizeSpeed(const double *v_x, const double *v_x_max,const  double *a_x_max, double *vPos, const double *MinGap, const AbstractLaneChangeModel *LCModel) const;


    /** @brief Computes the vehicle's safe speed (no dawdling)
     * @param[in] veh The vehicle (EGO)
     * @param[in] speed The vehicle's speed
     * @param[in] gap2pred The (netto) distance to the LEADER
     * @param[in] predSpeed The speed of LEADER
     * @return EGO's safe speed
     * @see CFModel::ffeV
     */
    virtual double followSpeed(const double &v_x, const double &delta_s_front, const double &v_x_front, const double &v_x_max) const;


    /** @brief Computes the vehicle's safe speed for approaching a non-moving obstacle (no dawdling)
     * @param[in] veh The vehicle (EGO)
     * @param[in] gap2pred The (netto) distance to the the obstacle
     * @return EGO's safe speed for approaching a non-moving obstacle
     * @see CFModel::ffeS
     * @todo generic Interface, models can call for the values they need
     */
    double stopSpeed(const double *speed, const double *gap2pred, const double *v_x_max) const;

    /** @brief Returns the model's name
     * @return The model's name
     * @see CFModel::getModelName
     */
    virtual int getModelID() const {
        return 0; //SUMO_TAG_CF_DANIEL1;
    }


    /** @brief Get the driver's imperfection
     * @return The imperfection of drivers of this class
     */
    double getImperfection() const {
        return myDawdle;
    }
    /// @}



    /// @name Setter methods
    /// @{
    /** @brief Sets a new value for maximum deceleration [m/s^2]
     * @param[in] accel The new deceleration in m/s^2
     */
    void setMaxDecel(const double decel) {
        CFModel::setMaxDecel(decel);
        myDecel = decel;
    }


    /** @brief Sets a new value for driver imperfection
     * @param[in] accel The new driver imperfection
     */
    void setImperfection(double imperfection) {
        myDawdle = imperfection;
    }


    /** @brief Sets a new value for driver reaction time [s]
     * @param[in] headwayTime The new driver reaction time (in s)
     */
    void setHeadwayTime(const double headwayTime) {
        CFModel::setHeadwayTime(headwayTime);
        myHeadwayTime = headwayTime;
        myTauDecel = -myDecel * headwayTime;
    }
    /// @}


    /** @brief Duplicates the car-following model
     * @param[in] vtype The vehicle type this model belongs to (1:1)
     * @return A duplicate of this car-following model
     */
   virtual CFModel* duplicate(double Accel, double Decel, double maxAccel, double maxDecel, double Headwaytime, double CycleTime) const;

private:
    /** @brief Returns the "safe" velocity
     * @param[in] gap2pred The (netto) distance to the LEADER
     * @param[in] predSpeed The LEADER's speed
     * @return the safe velocity
     */
    virtual double _vsafe(const double &gap, const double &predSpeed) const;


    /** @brief Applies driver imperfection (dawdling / sigma)
     * @param[in] speed The speed with no dawdling
     * @return The speed after dawdling
     */
    virtual double dawdle(double speed) const;

protected:
    /// @brief The vehicle's dawdle-parameter. 0 for no dawdling, 1 for max.
    double myDawdle;

    /// @brief The precomputed value for myDecel*myTau
    double myTauDecel;

    /// @brief temporary (testing) parameter
    //double myTmp1, myTmp2, myTmp3, myTmp4, myTmp5;

};

#endif /* CFModel_Daniel1_H */

