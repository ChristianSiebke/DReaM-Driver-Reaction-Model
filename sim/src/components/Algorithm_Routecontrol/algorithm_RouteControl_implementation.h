/********************************************************************************
 * Copyright (c) 2020-2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#ifndef ALGORITHM_ROUTECONTROL_IMPLEMENTATION_H
#define ALGORITHM_ROUTECONTROL_IMPLEMENTATION_H

#include <map>
#include <vector>



#include "common/speedActionSignal.h"
#include "controlSignal.h"
#include "include/parameterInterface.h"
#include "routeControl.h"
#include "common/vector2d.h"

#include "include/modelInterface.h"
#include "common/componentPorts.h"
#include "common/primitiveSignals.h"


#include "common/trajectorySignal.h"
#include "include/parameterInterface.h"
#include "include/publisherInterface.h"


using namespace Common;

/**
 * \addtogroup Components_Special openPASS components special
 * @{
 * \addtogroup Algorithm_RouteControl
 *
 * \brief Algorithm that controls the route of the vehice,
 * taking a collection of points as a reference,
 * which describes the desired trajectory,
 * that the vehicle should follow.
 *
 * \details Depending on the "agressivity" Parameter,
 * the algorithm will drive along a route, simulating
 * how a driver with that level of agressivity would
 * follow that trajectory.
 *
 * @}
 */

/*!
 * \copydoc Algorithm_RouteControl
 * \ingroup Algorithm_RouteControl
 */
class Algorithm_Routecontrol_Implementation : public AlgorithmInterface
{
public:
    const std::string COMPONENTNAME = "Algorithm_RouteControl";

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
    Algorithm_Routecontrol_Implementation(std::string componentName,
                                          bool isInit,
                                          int priority,
                                          int offsetTime,
                                          int responseTime,
                                          int cycleTime,
                                          StochasticsInterface *stochastics,
                                          const ParameterInterface *parameters,
                                          PublisherInterface *const publisher,
                                          const CallbackInterface *callbacks,
                                          AgentInterface *agent);
    Algorithm_Routecontrol_Implementation(const Algorithm_Routecontrol_Implementation &) = delete;
    Algorithm_Routecontrol_Implementation(Algorithm_Routecontrol_Implementation &&) = delete;
    Algorithm_Routecontrol_Implementation &operator=(const Algorithm_Routecontrol_Implementation &) = delete;
    Algorithm_Routecontrol_Implementation &operator=(Algorithm_Routecontrol_Implementation &&) = delete;

    //! Destructor
    virtual ~Algorithm_Routecontrol_Implementation();

    //! Function is called by framework when another component delivers a signal over
    //! a channel to this component (scheduler calls update taks of other component).
    //!
    //! @param[in]     localLinkId    Corresponds to "id" of "ComponentInput"
    //! @param[in]     data           Referenced signal (copied by sending component)
    //! @param[in]     time           Current scheduling time
    //! @return                       True on success
    virtual void UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data, int time);

    //! Function is called by framework when this component has to deliver a signal over
    //! a channel to another component (scheduler calls update task of this component).
    //!
    //! @param[in]     localLinkId    Corresponds to "id" of "ComponentOutput"
    //! @param[out]    data           Referenced signal (copied by this component)
    //! @param[in]     time           Current scheduling time
    //! @return                       True on success
    virtual void UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time);

    //! Function is called by framework when the scheduler calls the trigger task
    //! of this component
    //!
    //! @param[in]     time           Current scheduling time
    //! @return                       True on success
    virtual void Trigger(int time_ms);

private:
    //! Function to read in trajectory data given seperately
    //!
    //! @return                       Vector of waypoints comprising all trajectory data
    void ReadWayPointData();

    //    std::map<int, ComponentPort *> inputPorts; //!< map for all InputPort
    /** \addtogroup Algorithm_RouteControl
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

    std::map<int, ComponentPort *> outputPorts; //!< map for all OutputPort
    /** \addtogroup Algorithm_RouteControl
     *  @{
     *      \name OutputPorts
     *      All output ports with PortId
     *      @{
     */
    OutputPort<ControlSignal, ControlData> control{0, &outputPorts}; //!< control values required to follow the trajectory
    /**
     *      @}
     *  @}
     */

    std::map<std::string, externalParameter<double> *> parameterMapDouble;
    /** \addtogroup Algorithm_RouteControl
     *  @{
     *      \name External Parameter
     *      Parameter which are set externally in agentConfiguration file.
     *      @{
     */
    externalParameter<double> drivingAgressivity{0, &parameterMapDouble}; //!< indicates the level of driving agressivity of the driver
    externalParameter<double> maxpower{1, &parameterMapDouble};           //!< maximum power of engine's agent []
    externalParameter<double> mintorque{2, &parameterMapDouble};          //!< minimal braque torque of the agent []
    externalParameter<double> pedalsKp{3, &parameterMapDouble};           //!< Kp parameter of the pedals PID control
    externalParameter<double> pedalsKi{4, &parameterMapDouble};           //!< Ki parameter of the pedals PID control
    externalParameter<double> pedalsKd{5, &parameterMapDouble};           //!< Kd parameter of the pedals PID control
    externalParameter<double> steeringKp{6, &parameterMapDouble};         //!< Kp parameter of the steering PID control
    externalParameter<double> steeringKi{7, &parameterMapDouble};         //!< Ki parameter of the steering PID control
    externalParameter<double> steeringKd{8, &parameterMapDouble};         //!< Kd parameter of the steering PID control
    /**
     *      @}
     *  @}
     */

    /**
     *    \name Internal objects
     *    @{
    */
    //local computation objects
    std::vector<WaypointData> *waypoints = nullptr; //!< vector of waypoints that should be followed
    RouteControl *routeControl = nullptr;           //!< class containing the actual algorithms for the route control
    double mTimeStep;                             //!< time step in s
    /**
     *    @}
    */
};

#endif // ALGORITHM_ROUTECONTROL_IMPLEMENTATION_H
