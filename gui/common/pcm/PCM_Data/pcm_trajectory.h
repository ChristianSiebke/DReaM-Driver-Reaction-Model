/*********************************************************************
* Copyright (c) 2017, 2018, 2021 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

//-----------------------------------------------------------------------------
//! @file  pcm_trajectory.h
//! @brief This file contains class for a trajectory that might be exchanged
//!        between modules and its signal class.
//-----------------------------------------------------------------------------
#ifndef PCM_TRAJECTORY_H
#define PCM_TRAJECTORY_H

#include <vector>
#include <sstream>
#include <QString>

//-----------------------------------------------------------------------------
//! Class for a trajectory that might be exchanged between modules
//-----------------------------------------------------------------------------
class PCM_Trajectory
{
public:
    PCM_Trajectory() = default;
    //-----------------------------------------------------------------------------
    //! Constructor from vectors
    //!
    //! @param[in]     timeVec      The vector of the time of the trajectory points
    //! @param[in]     xPosVec      The vector of the x-coordinates of the trajectory points
    //! @param[in]     yPosVec      The vector of the y-coordinates of the trajectory points
    //! @param[in]     uVelVec      The vector of the velocities of the trajectory points (forward)
    //! @param[in]     vVelVec      The vector of the velocities of the trajectory points (sideward)
    //! @param[in]     psiVec       The vector of the yaw angles of the trajectory points
    //-----------------------------------------------------------------------------
    PCM_Trajectory(std::vector<double> *timeVec,
                   std::vector<double> *xPosVec,
                   std::vector<double> *yPosVec,
                   std::vector<double> *uVelVec,
                   std::vector<double> *vVelVec,
                   std::vector<double> *psiVec);

    // removing operators
    PCM_Trajectory(const PCM_Trajectory &) = delete;
    PCM_Trajectory(PCM_Trajectory &&) = delete;
    PCM_Trajectory &operator=(const PCM_Trajectory &) = delete;
    PCM_Trajectory &operator=(PCM_Trajectory &&) = delete;

    //-----------------------------------------------------------------------------
    //! Destructor
    //-----------------------------------------------------------------------------
    virtual ~PCM_Trajectory();

    //-----------------------------------------------------------------------------
    //! Getter function
    //! @return     the end time of the trajectory = last entry of the time vector
    //-----------------------------------------------------------------------------
    double GetEndTime() const;

    //-----------------------------------------------------------------------------
    //! Getter function
    //! @return     the start velocity at the trajectory'S beginning = first entry of the longitudinal velocity vector
    //-----------------------------------------------------------------------------
    double GetStartVelocity() const;

    /*!
     * \name
     *
     * Standard getter functions
     * @return  The vector with the time/x-coordinates/y-coordinates/velocities/yaw angles of the trajectory points
     * @{
     *
     */
    std::vector<double> *GetTimeVec() const;
    std::vector<double> *GetXPosVec() const;
    std::vector<double> *GetYPosVec() const;
    std::vector<double> *GetUVelVec() const;
    std::vector<double> *GetVVelVec() const;
    std::vector<double> *GetPsiVec() const;

    QString GetTimeVecString() const;
    QString GetXPosVecString() const;
    QString GetYPosVecString() const;
    QString GetUVelVecString() const;
    QString GetVVelVecString() const;
    QString GetPsiVecString() const;
    /**
     *  @}
     */

    //-----------------------------------------------------------------------------
    //! Function transforming a vector of ints to one QString where the vector
    //! values are comma separated
    //!
    //! @param[in]     inVec    vector of int to be transformed into a comma separated list
    //! @return                 QString with the comma separated list
    //-----------------------------------------------------------------------------
    QString VecIntToCsv(const std::vector<int> *inVec) const;

    //-----------------------------------------------------------------------------
    //! Function transforming a vector of doubles to one QString where the vector
    //! values are comma separated
    //!
    //! @param[in]     inVec    vector of int to be transformed into a comma separated list
    //! @return                 QString with the comma separated list
    //-----------------------------------------------------------------------------
    QString VecDoubleToCsv(const std::vector<double> *inVec) const;

    //! Shift trajectory about given forward distance.
    //!
    //! @param[in]     distance       distance to be shifted.
    //-----------------------------------------------------------------------------
    void ShiftForward(double distance) const;

    //! Shift trajectory about given distance in a specific direction.
    //!
    //! @param[in]     distance       distance to be shifted.
    //! @param[in]     angle          angle around to be shifted.
    //-----------------------------------------------------------------------------
    void ShiftPosition(double distance, double angle) const;

    //! Scale the velocity about a given value.
    //! The position will be adapted as well to stay consistent.
    //!
    //! @param[in]     velScale       scale of the velocity.
    //-----------------------------------------------------------------------------
    void ScaleVelocity(double velScale) const;

    //! Reset the trajectory to its original values.
    //-----------------------------------------------------------------------------
    void ResetToOriginalState();

    //! Save the original state
    void SaveOriginalState();

private:
    //! Clear all trajectory data.
    void Clear();

    std::vector<double> *timeVec = nullptr; //!< vector of the time of the trajectory points
    std::vector<double> *xPosVec = nullptr; //!< vector of the x-coordinates of the trajectory points
    std::vector<double> *yPosVec = nullptr; //!< vector of the y-coordinates of the trajectory points
    std::vector<double> *uVelVec = nullptr; //!< vector of the velocities of the trajectory points (forward)
    std::vector<double> *vVelVec = nullptr; //!< vector of the velocities of the trajectory points (sideward)
    std::vector<double> *psiVec = nullptr;  //!< vector of the yaw angles of the trajectory points

    // original values which will not be changed
    std::vector<double> timeVecOriginal;
    std::vector<double> xPosVecOriginal;
    std::vector<double> yPosVecOriginal;
    std::vector<double> uVelVecOriginal;
    std::vector<double> vVelVecOriginal;
    std::vector<double> psiVecOriginal;
};

#endif // PCM_TRAJECTORY_H
