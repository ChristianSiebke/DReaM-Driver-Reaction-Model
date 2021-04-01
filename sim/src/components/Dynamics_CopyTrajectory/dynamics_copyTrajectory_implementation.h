/*********************************************************************
* Copyright (c) 2021 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#ifndef DYNAMICS_COPYTRAJECTORY_IMPLEMENTATION_H
#define DYNAMICS_COPYTRAJECTORY_IMPLEMENTATION_H

#include <QString>
#include <vector>
#include "include/modelInterface.h"
#include "include/publisherInterface.h"
#include "common/trajectorySignal.h"

/** \addtogroup Components_PCM openPASS components pcm
* @{
* \addtogroup Dynamics_CopyTrajectory
* \brief Dynamic component to follow a exactly a given trajectory.
*
* \details This module uses the given trajectory of the agent to set its position,
* orientation and velocity at every timestep.
*
* @} */

/*!
* \copydoc Dynamics_CopyTrajectory
* \ingroup Dynamics_CopyTrajectory
*/
class Dynamics_CopyTrajectory_Implementation : public DynamicsInterface
{
public:
    const std::string COMPONENTNAME = "Dynamics_CopyTrajectory";

    //-----------------------------------------------------------------------------
    //! Constructor
    //!
    //! @param[in]     componentName  Name of the component
    //! @param[in]     isInit         Corresponds to "init" of "Component"
    //! @param[in]     priority       Corresponds to "priority" of "Component"
    //! @param[in]     offsetTime     Corresponds to "offsetTime" of "Component"
    //! @param[in]     responseTime   Corresponds to "responseTime" of "Component"
    //! @param[in]     cycleTime      Corresponds to "cycleTime" of "Component"
    //! @param[in]     stochastics    Pointer to the stochastics class loaded by the framework
    //! @param[in]     parameters     Pointer to the parameters of the module
    //! @param[in]     publisher      Pointer to the publisher instance
    //! @param[in]     callbacks      Pointer to the callbacks
    //! @param[in]     agent          Pointer to agent instance
    //-----------------------------------------------------------------------------
    Dynamics_CopyTrajectory_Implementation(std::string componentName,
                                           bool isInit,
                                           int priority,
                                           int offsetTime,
                                           int responseTime,
                                           int cycleTime,
                                           StochasticsInterface *stochastics,
                                           WorldInterface *world,
                                           const ParameterInterface *parameters,
                                           PublisherInterface * const publisher,
                                           const CallbackInterface *callbacks,
                                           AgentInterface *agent);
    //-----------------------------------------------------------------------------
    //! Destructor
    //-----------------------------------------------------------------------------
    virtual ~Dynamics_CopyTrajectory_Implementation() {}

    Dynamics_CopyTrajectory_Implementation(const Dynamics_CopyTrajectory_Implementation &) = delete;
    Dynamics_CopyTrajectory_Implementation(Dynamics_CopyTrajectory_Implementation &&) = delete;
    Dynamics_CopyTrajectory_Implementation &operator=(const Dynamics_CopyTrajectory_Implementation &) =
        delete;
    Dynamics_CopyTrajectory_Implementation &operator=(Dynamics_CopyTrajectory_Implementation &&) =
        delete;

    //-----------------------------------------------------------------------------
    //! Function is called by framework when another component delivers a signal over
    //! a channel to this component (scheduler calls update taks of other component).
    //!
    //! @param[in]     localLinkId    Corresponds to "id" of "ComponentInput"
    //! @param[in]     data           Referenced signal (copied by sending component)
    //! @param[in]     time           Current scheduling time
    //! @return                       True on success
    //-----------------------------------------------------------------------------
    virtual void UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data,
                             int time);

    //-----------------------------------------------------------------------------
    //! Function is called by framework when this component has to deliver a signal over
    //! a channel to another component (scheduler calls update task of this component).
    //!
    //! @param[in]     localLinkId    Corresponds to "id" of "ComponentOutput"
    //! @param[out]    data           Referenced signal (copied by this component)
    //! @param[in]     time           Current scheduling time
    //! @return                       True on success
    //-----------------------------------------------------------------------------
    virtual void UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time);

    //-----------------------------------------------------------------------------
    //! Function is called by framework when the scheduler calls the trigger task
    //! of this component
    //!
    //! @param[in]     time           Current scheduling time
    //! @return                       True on success
    //-----------------------------------------------------------------------------
    virtual void Trigger(int time);

private:

    /** \addtogroup Dynamics_CopyTrajectory
     *  @{
     *      \name InputPorts
     *      All input ports with PortId
     *      @{
     */
//    InputPort<TrajectorySignal, openScenario::Trajectory> trajectory{0, &inputPorts}; //!< given trajectory to follow
    openScenario::Trajectory trajectory; //!< given trajectory to follow
    /**
     *      @}
     *  @}
     */

    void ReadWayPointData();

    /**
     *    \name Internal objects
     *    @{
    */
    //local computation objects
    double timeStep;            //!< Time step as double in s
    double timeVecNext;
    unsigned int indexVecNext;
    std::vector<double> timeVec;   //!< time vector of trajectory
    std::vector<double> posX;   //!< x coordinate vector of trajectory
    std::vector<double> posY;   //!< y coordinate vector of trajectory
    std::vector<double> velX;    //!< velocity vector of trajectory
    std::vector<double> velY;    //!< velocity vector of trajectory
    std::vector<double> angleYaw;    //!< yaw angle vector of trajectory
    std::vector<double> rateYaw;    //!< yaw angle vector of trajectory
    /**
     *    @}
    */

};

#endif // DYNAMICS_COPYTRAJECTORY_IMPLEMENTATION_H
