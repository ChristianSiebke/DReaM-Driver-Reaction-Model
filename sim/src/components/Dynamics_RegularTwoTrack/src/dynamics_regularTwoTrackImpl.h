/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*               2018, 2019 AMFD GmbH
*               2016, 2017, 2019, 2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/
#pragma once

#include <memory>
#include "include/modelInterface.h"
#include "include/parameterInterface.h"
#include "include/observationInterface.h"
#include "common/primitiveSignals.h"
#include "common/dynamicsSignal.h"
#include "common/globalDefinitions.h"
#include "common/componentPorts.h"
#include "dynamics_twotrack_vehicle.h"

#ifdef QT_DEBUG
#include <fstream>
#endif

/** \addtogroup Dynamics_RegularTwoTrack
* @{
* \brief Dynamic component to model the dynamic of a two track vehicle.
*
* \details Simple open-loop two-track model.
*
* \section Dynamics_RegularTwoTrack_Inputs Inputs
* Input variables:
* name | meaning
* -----|------
* in_accPedalPos        | Position of the gas pedal in percent
* in_brakePedalPos      | Position of the brake pedal in percent
* in_gear               | Current gear
* in_steeringWheelAngle | Steering wheel angle of the agent
*
*
* Input channel IDs:
* Input ID  | signal class                              | contained variables
* ----------|-------------------------------------------|-------------
* 1         | LongitudinalSignal                        | in_accPedalPos, in_brakePedalPos, in_gear
* 2         | SteeringSignal                            | in_steeringWheelAngle
*
*
* * \section Dynamics_RegularTwoTrack_Outputs Outputs
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
* Output Id                  | signal class    | contained variables
* ---------------------------|-----------------|-------------
* 0                          | DynamicsSignal  | acceleration, velocity, positionX, positionY, yaw, yawRate, steeringWheelAngle, travelDistance
*
* @}
*/

/*!
 * \ingroup Dynamics_RegularTwoTrack
 */
class DynamicsRegularTwoTrackImplementation : public DynamicsInterface
{
public:

    //! Name of the current component
    const std::string COMPONENTNAME = "DynamicRegularTwoTrack";

    DynamicsRegularTwoTrackImplementation(
        std::string componentName,
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
        AgentInterface *agent) :
        DynamicsInterface(
            componentName,
            isInit,
            priority,
            offsetTime,
            responseTime,
            cycleTime,
            stochastics,
            world,
            parameters,
            publisher,
            callbacks,
            agent),
        dynamicsSignal {ComponentState::Acting}
    {
    }
    DynamicsRegularTwoTrackImplementation(const DynamicsRegularTwoTrackImplementation&) = delete;
    DynamicsRegularTwoTrackImplementation(DynamicsRegularTwoTrackImplementation&&) = delete;
    DynamicsRegularTwoTrackImplementation& operator=(const DynamicsRegularTwoTrackImplementation&) = delete;
    DynamicsRegularTwoTrackImplementation& operator=(DynamicsRegularTwoTrackImplementation&&) = delete;
    virtual ~DynamicsRegularTwoTrackImplementation();

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
    virtual void UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data, int time);

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
    virtual void UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time);

    /*!
    * \brief Process data within component.
    *
    * Function is called by framework when the scheduler calls the trigger task
    * of this component
    *
    * @param[in]     time           Current scheduling time
    */
    virtual void Trigger(int time);

private:

    //! Output Signal
    DynamicsSignal dynamicsSignal;

    std::map<int, externalParameter<double>*> parameterMapDouble;
    /** \name External Parameters
     *  Parameter which are set externally in configuration file.
     *  @{ */
    externalParameter<double> radiusTire {0, &parameterMapDouble }; //!<
    externalParameter<double> muTireMax {1, &parameterMapDouble }; //!<
    externalParameter<double> muTireSlide {2, &parameterMapDouble }; //!<
    externalParameter<double> slipTireMax {3, &parameterMapDouble }; //!<
    externalParameter<double> powerEngineMax {4, &parameterMapDouble }; //!<
    externalParameter<double> torqueBrakeMin {5, &parameterMapDouble }; //!<
    /**
     * @} */


    std::map<int, ComponentPort *> inputPorts; //!< map for all InputPort
    /** \name InputPorts
     *  All input ports with PortId
     *  @{ */
    InputPort<DoubleSignal, double> throttlePedal {0, &inputPorts}; //!< throttle pedal position in the range [0...1]
    InputPort<DoubleSignal, double> brakePedal {1, &inputPorts}; //!< brake pedal position in the range [0...1]
    InputPort<DoubleSignal, double> angleTireFront {2, &inputPorts}; //!< steering angle [radian]
    /**
     * @} */

    /** \name Internal states
     * @{ */

    //! Time step as double in s
    double timeStep = 0.0;
    //! Yaw angle
    double yawAngle = 0.0;
    //! Car's position
    Common::Vector2d positionCar = {0.0, 0.0};
    //! Yaw rate
    double yawVelocity = 0.0;
    //! Car's velocity
    Common::Vector2d velocityCar = {0.0, 0.0};
    //! Yaw acceleration
    double yawAcceleration = 0.0;
    //! Car's acceleration
    Common::Vector2d accelerationCar = {0.0, 0.0};
    //! Brake position for each tire
    std::vector<double> brakeSuperpose = {0.0, 0.0, 0.0, 0.0};
    //! Vertical force on wheels
    std::vector<double> forceWheelVertical = {0.0, 0.0, 0.0, 0.0};
    /**
     * @} */

    /** \name Internal objects
     * @{ */
    //! Vehicle in trajectory CS
    std::unique_ptr<VehicleSimpleTT> vehicle;
    /**
     * @} */

    //! Update data on agent's actual position, velocity and acceleration
    void ReadPreviousState();

    //! Calculate next position, translation velocity and translation acceleration of the agent
    void NextStateTranslation();

    //! Calculate next yaw angle, rotation velocity and rotation acceleration of the agent
    void NextStateRotation();

    //! Write next position, velocity and acceleration of the agent
    void NextStateSet();

    //! Initialize parameters and vehicle, as in original constructor
    void Init();

#ifdef QT_DEBUG
    std::ofstream logFile;
#endif
};

/** @} */ // End of group DynamicsRegularTwoTrack
