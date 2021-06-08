/*******************************************************************************
* Copyright (c) 2018, 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

/*!
 * \addtogroup Dynamics_TrajectoryFollower
 * @{
 * \brief makes an agent strictly follow a predefined path
 *
 * The path is defined in a file that is loaded in the importer into a map.
 * The module simply reads the coordinates defined in the map every step and forces the agent to move to that position.
 * The vehicle is in this mode very simplified (there is no driver, no logic or any intelligence). It's basically a "playback" of the trajectory.
 *
* \section Dynamics_TrajectoryFollower_Inputs Inputs
* Input variables:
* name | meaning
* -----|------
* inputAcceleration | acceleration from some vehicle component, if any
* velocity | current velocity
* maxReachableState | max ComponentState for this module is given by ComponentController
*
* Input channel IDs:
* | Input ID   | signal class              | contained variables               |
* |------------|---------------------------|-----------------------------------|
* | 0          | SteeringSignal            | componentState                    |
* | 1          | AccelerationSignal        | inputAcceleration, componentState |
* | 2          | TrajectorySignal          | trajectory                        |
*
* * \section Dynamics_TrajectoryFollower_Outputs Outputs
* Output variables:
* name                       | meaning
* ---------------------------|------
* acceleration               | Acceleration of the current agent coresponding to the pedalpositions [m/s²]
* velocity                   | The driven curvature of the car coresponding to the steering wheel angle based on Ackermann [radian]
* positionX                  | new inertial x-position [m]
* positionY                  | new inertial y-position [m]
* yaw                        | new yaw angle in current time step [radian]
* yawRate                    | change of yaw angle due to ds and curvature [radian]
* steeringWheelAngle         | new angle of the steering wheel angle [radian]
* travelDistance             | distance traveled by the agent during this timestep [m]
*
*
* Output channel IDs:
* | Output Id                  | signal class                              | contained variables                                                                            |
* |----------------------------|-------------------------------------------|------------------------------------------------------------------------------------------------|
* | 0                          | DynamicsSignal                            | acceleration, velocity, positionX, positionY, yaw, yawRate, steeringWheelAngle, travelDistance |
 * @} */

#pragma once

#include "common/accelerationSignal.h"
#include "common/dynamicsSignal.h"
#include "common/globalDefinitions.h"
#include "common/steeringSignal.h"
#include "common/openScenarioDefinitions.h"
#include "common/vector2d.h"
#include "include/eventNetworkInterface.h"
#include "include/modelInterface.h"
#include "include/publisherInterface.h"

using openScenario::Trajectory;
using openScenario::TrajectoryPoint;

/*!
 * \brief Makes an agent strictly follow a predefined path.
 *
 * \ingroup Dynamics_TrajectoryFollower
 */
class TrajectoryFollowerImplementation : public UnrestrictedModelInterface
{
public:
    static constexpr char COMPONENTNAME[] = "Dynamics_TrajectoryFollower";

    TrajectoryFollowerImplementation(std::string componentName,
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

    /*!
    * \brief Update Inputs
    *
    * Function is called by framework when another component delivers a signal over
    * a channel to this component (scheduler calls update taks of other component).
    *
    * Refer to module description for input channels and input ids.
    *
    * @param[in]     localLinkId    Corresponds to "id" of "ComponentInput"
    * @param[in]     data           Referenced signal (copied by sending component)
    * @param[in]     time           Current scheduling time
    */
    void UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data, int time) override;

    /*!
    * \brief Update outputs.
    *
    * Function is called by framework when this component has to deliver a signal over
    * a channel to another component (scheduler calls update task of this component).
    *
    * Refer to module description for output channels and output ids.
    *
    * @param[in]     localLinkId    Corresponds to "id" of "ComponentOutput"
    * @param[out]    data           Referenced signal (copied by this component)
    * @param[in]     time           Current scheduling time
    */
    void UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time) override;

    /*!
    * \brief Process data within component.
    *
    * Function is called by framework when the scheduler calls the trigger task
    * of this component
    *
    * @param[in]     time           Current scheduling time
    */
    void Trigger(int time) override;

    /*!
    * \brief Calculates dynamics for next timestep.
    *
    * @param[in]     time           Current scheduling time
    */
    void CalculateNextTimestep();

private:
        const double cycleTimeInSeconds{0.0};

    bool initialization{true};
    bool enforceTrajectory{false};
    bool automaticDeactivation{false};
    bool inputAccelerationActive{false};

    double currentTime{0};

    double inputAcceleration{0.0};

    DynamicsSignal dynamicsOutputSignal{};

    Trajectory trajectory{};
    std::vector<TrajectoryPoint>::iterator previousTrajectoryIterator{};
    std::vector<TrajectoryPoint>::iterator nextTrajectoryIterator{};

    double lastCoordinateTimestamp{0};
    double lastVelocity{0.0};
    TrajectoryPoint lastWorldPosition;
    double percentageTraveledBetweenCoordinates{0};

    ComponentState componentState{ComponentState::Disabled};
    bool canBeActivated{true};

    void Init();

    [[ noreturn ]] void ThrowCouldNotInstantiateSignalError();
    [[ noreturn ]] void ThrowInvalidSignalTypeError();

    ComponentState GetState() const;
    void UpdateState(const ComponentState newState);

    bool CheckEndOfTrajectory();

    void HandleEndOfTrajectory();

    void ParseParameters(const ParameterInterface *parameters);

    Common::Vector2d CalculateScaledVector(const TrajectoryPoint &previousPosition, const TrajectoryPoint &nextPosition, const double &factor);
    double CalculateScaledDeltaYawAngle(const TrajectoryPoint &previousPosition, const TrajectoryPoint &nextPosition, const double &factor);

    double CalculateDistanceBetweenWorldCoordinates(TrajectoryPoint previousPosition, TrajectoryPoint nextPosition);

    void TriggerWithActiveAccelerationInput();
    void TriggerWithInactiveAccelerationInput();

    void SetComponentState(const ComponentState newState);

    void UpdateDynamics(const TrajectoryPoint &previousPosition,
                        const Common::Vector2d &direction,
                        double deltaYawAngle,
                        double velocity,
                        double acceleration);
};
