/********************************************************************************
 * Copyright (c) 2018 AMFD GmbH
 *               2016-2021 ITK Engineering GmbH
 *               2017-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  AgentInterface.h
//! @brief This file contains the interface for communicating between framework
//!        and world.
//-----------------------------------------------------------------------------

#pragma once

#include <map>
#include <vector>

#include "common/globalDefinitions.h"
#include "common/sensorDefinitions.h"
#include "common/worldDefinitions.h"
#include "include/profilesInterface.h"
#include "include/signalInterface.h"
#include "include/spawnItemParameterInterface.h"
#include "include/worldObjectInterface.h"

using LaneTypes = std::vector<LaneType>;
class EgoAgentInterface;

/**
* \brief Agent Interface within the openPASS framework.
* \details This interface provides access to agent parameters, properties, attributes and dynamic states.
*/
class AgentInterface : public virtual WorldObjectInterface
{
public:
    AgentInterface() = default;
    AgentInterface(const AgentInterface &) = delete;
    AgentInterface(AgentInterface &&) = delete;
    AgentInterface &operator=(const AgentInterface &) = delete;
    AgentInterface &operator=(AgentInterface &&) = delete;
    virtual ~AgentInterface() = default;

    //! Returns the EgoAgent corresponding to this agent
    virtual EgoAgentInterface& GetEgoAgent() = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves the type key of an agent
    //!
    //! @return                string identifier of vehicle type
    //-----------------------------------------------------------------------------
    virtual std::string GetVehicleModelType() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves all vehicle model parameters of agent
    //!
    //! @return               VehicleModelParameters of agent
    //-----------------------------------------------------------------------------
    virtual VehicleModelParameters GetVehicleModelParameters() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves name of driver profile of agent
    //!
    //! @return               DriverProfile name of agent
    //-----------------------------------------------------------------------------
    virtual std::string GetDriverProfileName() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves the name of agent specified in the scenario, xml file
    //!
    //! @return                AgentTypeName of agent
    //-----------------------------------------------------------------------------
    virtual std::string GetScenarioName() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves agentTypeGroupName of agent
    //!
    //! @return                AgentTypeGroupName of agent
    //-----------------------------------------------------------------------------
    virtual AgentCategory GetAgentCategory() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves agentTypeName of agent
    //!
    //! @return                AgentTypeName of agent
    //-----------------------------------------------------------------------------
    virtual std::string GetAgentTypeName() const = 0;

    //-----------------------------------------------------------------------------
    //! Returns true if Agent is marked as the ego agent.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual bool IsEgoAgent() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves the current gear number
    //!
    // @return                Gear no.
    //-----------------------------------------------------------------------------
    virtual int GetGear() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves list of collisions partners of agent.
    //!
    //! @return               a value >0 if collided
    //-----------------------------------------------------------------------------
    virtual std::vector<std::pair<ObjectTypeOSI, int>> GetCollisionPartners() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves velocity of agent after crash.
    //!
    // @return                a value >0 if collided
    //-----------------------------------------------------------------------------
    virtual PostCrashVelocity GetPostCrashVelocity() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves velocity of agent after crash.
    //!
    // @return                a value >0 if collided
    //-----------------------------------------------------------------------------
    virtual void SetPostCrashVelocity(PostCrashVelocity postCrashVelocity) = 0;

    //-----------------------------------------------------------------------------
    //! Sets x-coordinate of agent
    //!
    //! @param[in]     positionX    X-coordinate
    //-----------------------------------------------------------------------------
    virtual void SetPositionX(double positionX) = 0;

    //-----------------------------------------------------------------------------
    //! Sets y-coordinate of agent
    //!
    //! @param[in]     positionY    Y-coordinate
    //-----------------------------------------------------------------------------
    virtual void SetPositionY(double positionY) = 0;

    //-----------------------------------------------------------------------------
    //! Sets the agents vehicle model parameter
    //!
    //! @param[in]     parameter    New vehicle model paramter
    //-----------------------------------------------------------------------------
    virtual void SetVehicleModelParameter (const VehicleModelParameters &parameter) = 0;

    //-----------------------------------------------------------------------------
    //! Sets forward velocity of agent
    //!
    //! @param[in]     velocityX    Forward velocity
    //-----------------------------------------------------------------------------
    virtual void SetVelocity(double value) = 0;

    //-----------------------------------------------------------------------------
    //! Sets velocity of agent
    //!
    //! @param[in]     vx    Forward velocity
    //! @param[in]     vy    Sideward velocity
    //! @param[in]     vz    Upward velocity
    //-----------------------------------------------------------------------------
    virtual void SetVelocityVector(double vx, double vy, double vz) = 0;

    //-----------------------------------------------------------------------------
    //! Sets forward acceleration of agent
    //!
    //! @param[in]     accelerationX    forward acceleration
    //-----------------------------------------------------------------------------
    virtual void SetAcceleration(double value) = 0;

    //-----------------------------------------------------------------------------
    //! Sets yaw angle of agent
    //!
    //! @param[in]     yawAngle    agent orientation
    //-----------------------------------------------------------------------------
    virtual void SetYaw(double value) = 0;

    //-----------------------------------------------------------------------------
    //! Sets roll angle of agent
    //!
    //! @param[in]     yawAngle    agent orientation
    //-----------------------------------------------------------------------------
    virtual void SetRoll(double value) = 0;

    //-----------------------------------------------------------------------------
    //! Sets the total traveled distance of agent
    //!
    //! @param[in]     distanceTraveled    total traveled distance
    //-----------------------------------------------------------------------------
    virtual void SetDistanceTraveled(double distanceTraveled) = 0;

    //-----------------------------------------------------------------------------
    //! Returns the total traveled distance of agent
    //!
    //! @return   total traveled distance
    //-----------------------------------------------------------------------------
    virtual double GetDistanceTraveled() const = 0;

    //-----------------------------------------------------------------------------
    //! Sets gear of vehicle
    //!
    //! @param[in]     gear    current gear
    //-----------------------------------------------------------------------------
    virtual void SetGear(int gear) = 0;

    //-----------------------------------------------------------------------------
    //! Sets current engine speed (rpm)
    //!
    //! @param[in]     engineSpeed    current engineSpeed
    //-----------------------------------------------------------------------------
    virtual void SetEngineSpeed(double engineSpeed) = 0;

    //-----------------------------------------------------------------------------
    //! Sets current position of acceleration pedal in percent
    //!
    //! @param[in]     percent    current percentage
    //-----------------------------------------------------------------------------
    virtual void SetEffAccelPedal(double percent) = 0;

    //-----------------------------------------------------------------------------
    //! Sets current position of brake pedal in percent
    //!
    //! @param[in]     percent    current percentage
    //-----------------------------------------------------------------------------
    virtual void SetEffBrakePedal(double percent) = 0;

    //-----------------------------------------------------------------------------
    //! Sets current angle of the steering wheel in radian
    //!
    //! @param[in]     steeringWheelAngle    current steering wheel angle
    //-----------------------------------------------------------------------------
    virtual void SetSteeringWheelAngle(double steeringWheelAngle) = 0;

    //-----------------------------------------------------------------------------
    //! Sets maximum acceleration of the vehicle
    //!
    //! @param[in]     maxAcceleration   maximum acceleration
    //-----------------------------------------------------------------------------
    virtual void SetMaxAcceleration(double maxAcceleration) = 0;

    //-----------------------------------------------------------------------------
    //! Sets maximum deceleration of the vehicle
    //!
    //! @param[in]     maxDeceleration   maximum deceleration
    //-----------------------------------------------------------------------------
    virtual void SetMaxDeceleration(double maxDeceleration) = 0;

    //-----------------------------------------------------------------------------
    //! update list with collision partners
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual void UpdateCollision(std::pair<ObjectTypeOSI, int> collisionPartner) = 0;

    //-----------------------------------------------------------------------------
    //! Unlocate agent in world.
    //!
    // @return
    //-----------------------------------------------------------------------------
    virtual void Unlocate() = 0;

    //-----------------------------------------------------------------------------
    //! Locate agent in world.
    //!
    // @return true, if agent could be localized
    //-----------------------------------------------------------------------------
    virtual bool Locate() = 0;

    //-----------------------------------------------------------------------------
    //! Update agent for in new timestep
    //!
    //! @return true, if agent could be localized
    //-----------------------------------------------------------------------------
    virtual bool Update() = 0;

    //-----------------------------------------------------------------------------
    //! Set the brake light on or off.
    //!
    // @param[in]     brakeLightStatus    status of brake light
    //-----------------------------------------------------------------------------
    virtual void SetBrakeLight(bool brakeLightStatus) = 0;

    //-----------------------------------------------------------------------------
    //! Returns the status of the brake light.
    //!
    // @return         true if brake light is on
    //-----------------------------------------------------------------------------
    virtual bool GetBrakeLight() const = 0;

    //! Sets to Indicator in a specific state
    virtual void SetIndicatorState(IndicatorState indicatorState) = 0;

    //! Retrieve the state of the indicator
    virtual IndicatorState GetIndicatorState() const = 0;

    //! Set the Horn on or off.
    //!
    //! @param[in]     hornSwitch    status of horn
    //! @return
    //-----------------------------------------------------------------------------
    virtual void SetHorn(bool hornSwitch) = 0;

    //-----------------------------------------------------------------------------
    //! Returns the status of the HornSwitch.
    //!
    //! @return        true if HornSwtich is pressed
    //-----------------------------------------------------------------------------
    virtual bool GetHorn() const = 0;

    //! Set the Headlight on or off.
    //!
    //! @param[in]     headLightSwitch    status of headlight
    //! @return
    //-----------------------------------------------------------------------------
    virtual void SetHeadLight(bool headLightSwitch) = 0;

    //-----------------------------------------------------------------------------
    //! Returns the status of the Headlightswitch.
    //!
    //! @return        true if Headlightswitch is on
    //-----------------------------------------------------------------------------
    virtual bool GetHeadLight() const = 0;

    //! Set the Highbeamlight on or off.
    //!
    //! @param[in]     highBeamLightSwitch    status of highbeamlight
    //! @return
    //-----------------------------------------------------------------------------
    virtual void SetHighBeamLight(bool headLightSwitch) = 0;

    //-----------------------------------------------------------------------------
    //! Returns the status of the Highbeamlight.
    //!
    //! @return        true if Highbeamlightswitch is on
    //-----------------------------------------------------------------------------
    virtual bool GetHighBeamLight() const = 0;


    //-----------------------------------------------------------------------------
    //! Returns the status of lights
    //!
    //! @return        priorized light-state of agent (lowest = off , highest=flasher)
    //-----------------------------------------------------------------------------
    virtual LightState GetLightState() const = 0;

    //-----------------------------------------------------------------------------
    //! Set the Flasher on or off.
    //!
    //! @param[in]     flasherSwitch    status of flasher
    //! @return
    //-----------------------------------------------------------------------------
    virtual void SetFlasher(bool flasherSwitch) = 0;

    //-----------------------------------------------------------------------------
    //! Returns the status of the Flasher.
    //!
    //! @return        true if Flasherswitch is on
    //-----------------------------------------------------------------------------
    virtual bool GetFlasher() const = 0;

    //-----------------------------------------------------------------------------
    //! Initializes the parameters of an agent
    //!
    //! @param[in] agentBlueprint    Blueprint holding parameters such as dimensions
    //!                              of a vehicle, or its initial spawning velocity
    //-----------------------------------------------------------------------------
    virtual void InitParameter(const AgentBlueprintInterface& agentBlueprint) = 0;

    //-----------------------------------------------------------------------------
    //! Returns true if agent is still in World located.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual bool IsAgentInWorld() const = 0;

    //-----------------------------------------------------------------------------
    //! Set the position of an agent.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual void SetPosition(Position pos) = 0;

    //! Returns if agent is partially not on the road
    virtual bool IsLeavingWorld() const = 0;

    //-----------------------------------------------------------------------------
    //! Unregisters the agent from the world.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual void Unregister() const = 0;

    //! Returns the s coordinate distance from the front of the agent to the first point where his lane intersects another.
    //! As the agent may not yet be on the junction, it has to be specified which connecting road he will take in the junction
    //!
    //! \param intersectingConnectorId  OpenDrive id of the connecting road that intersects with the agent
    //! \param intersectingLaneId       OpenDrive id of the lane on the intersecting connecting road
    //! \param ownConnectorId           OpenDrive id of the connecting road that this agent is assumed to drive on
    //!
    //! \return distance of front of agent to the intersecting lane
    virtual double GetDistanceToConnectorEntrance(std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const = 0;

    //! Returns the s coordinate distance from the rear of the agent to the furthest point where his lane intersects another.
    //! As the agent may not yet be on the junction, it has to be specified which connecting road he will take in the junction
    //!
    //! \param intersectingConnectorId  OpenDrive id of the connecting road that intersects with the agent
    //! \param intersectingLaneId       OpenDrive id of the lane on the intersecting connecting road
    //! \param ownConnectorId           OpenDrive id of the connecting road that this agent is assumed to drive on
    //!
    //! \return distance of rear of agent to the farther side of the intersecting lane
    virtual double GetDistanceToConnectorDeparture(std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the yaw rate of the agent.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual double GetYawRate() const = 0;

    //-----------------------------------------------------------------------------
    //! Set the yaw rate of the agent.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual void SetYawRate(double yawRate) = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the yaw acceleration of the agent.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual double GetYawAcceleration() const = 0;

    //-----------------------------------------------------------------------------
    //! Set the yaw acceleration of the agent.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual void SetYawAcceleration(double yawAcceleration) = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the centripetal acceleration of the agent.
    //!
    //! @return   Centripetal acceleration [m/s^2]
    //-----------------------------------------------------------------------------
    virtual double GetCentripetalAcceleration() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the tangential acceleration of the agent.
    //!
    //! @return   Tangential acceleration [m/s^2]
    //-----------------------------------------------------------------------------
    virtual double GetTangentialAcceleration() const = 0;

    //-----------------------------------------------------------------------------
    //! Set the centripetal acceleration of the agent.
    //!
    //! @param[in]   centripetalAcceleration   The acceleration to set [m/s^2]
    //-----------------------------------------------------------------------------
    virtual void SetCentripetalAcceleration(double centripetalAcceleration) = 0;

    //-----------------------------------------------------------------------------
    //! Set the tangential acceleration of the agent.
    //!
    //! @param[in]   tangentialAcceleration   The acceleration to set [m/s^2]
    //-----------------------------------------------------------------------------
    virtual void SetTangentialAcceleration(double tangentialAcceleration) = 0;

    virtual const ObjectPosition& GetObjectPosition() const = 0;

    virtual std::vector<std::string> GetRoads(MeasurementPoint mp) const = 0;

    /// \brief  Get distance (s coordinate) w.r.t. the specified measurement point on the specified road
    /// \param mp       point where s coordinate is measured
    /// \param roadId   OpenDrive id of the road
    /// \return distance
    virtual double GetDistanceToStartOfRoad(MeasurementPoint mp, std::string roadId) const = 0;

    virtual double GetDistanceReferencePointToLeadingEdge() const = 0;

    virtual double GetEngineSpeed() const = 0;

    //-----------------------------------------------------------------------------
    //! Gets current position of acceleration pedal in percent
    //!
    //! @return     accelPedal    current percentage
    //-----------------------------------------------------------------------------
    virtual double GetEffAccelPedal() const = 0;

    //-----------------------------------------------------------------------------
    //! Gets current position of brake pedal in percent
    //!
    //! @return     brakePedal    current percentage
    //-----------------------------------------------------------------------------
    virtual double GetEffBrakePedal() const = 0;

    //-----------------------------------------------------------------------------
    //! Gets current angle of the steering wheel in radian
    //!
    //! @return     current steering wheel angle
    //-----------------------------------------------------------------------------
    virtual double GetSteeringWheelAngle() const = 0;

    virtual double GetMaxAcceleration() const = 0;
    virtual double GetMaxDeceleration() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves the minimum speed goal of agent
    //!
    //! @return               Speed Goal Min
    //-----------------------------------------------------------------------------
    virtual double GetSpeedGoalMin() const = 0;

    virtual const openpass::sensors::Parameters& GetSensorParameters() const = 0;

    virtual void SetSensorParameters(openpass::sensors::Parameters sensorParameters) = 0;
};
