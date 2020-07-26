/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*               2018 AMFD GmbH
*               2016, 2017, 2018 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  AgentInterface.h
//! @brief This file contains the interface for communicating between framework
//!        and world.
//-----------------------------------------------------------------------------

#pragma once

#include <list>
#include <map>
#include <vector>

#include "Common/globalDefinitions.h"
#include "Common/sensorDefinitions.h"
#include "Common/worldDefinitions.h"
#include "Interfaces/profilesInterface.h"
#include "Interfaces/signalInterface.h"
#include "Interfaces/spawnItemParameterInterface.h"
#include "Interfaces/worldObjectInterface.h"

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

    //-----------------------------------------------------------------------------
    //! Retrieves id of agent
    //!
    // @return                Id of agent
    //-----------------------------------------------------------------------------
    virtual int GetAgentId() const = 0;

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
    //! Retrieves forward velocity of agent
    //!
    // @return                Forward velocity
    //-----------------------------------------------------------------------------
    virtual double GetVelocityX() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves sideward velocity of agent
    //!
    // @return                Sideward velocity
    //-----------------------------------------------------------------------------
    virtual double GetVelocityY() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves the current gear number
    //!
    // @return                Gear no.
    //-----------------------------------------------------------------------------
    virtual int GetGear() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves forward acceleration of agent
    //!
    // @return                Forward acceleration
    //-----------------------------------------------------------------------------
    virtual double GetAccelerationX() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves sideward acceleration of agent
    //!
    // @return                Sideward acceleration
    //-----------------------------------------------------------------------------
    virtual double GetAccelerationY() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves list of collisions partners of agent.
    //!
    //! @return               a value >0 if collided
    //-----------------------------------------------------------------------------
    virtual std::vector<std::pair<ObjectTypeOSI, int>> GetCollisionPartners() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves vector of all collisionData with a collisionPartner.
    //!
    // @return                a value >0 if collided
    //-----------------------------------------------------------------------------
    virtual std::vector<void *> GetCollisionData(int collisionPartnerId,
                                                 int collisionDataId) const = 0;


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
    virtual void SetVelocityX(double velocityX) = 0;

    //-----------------------------------------------------------------------------
    //! Sets sideward velocity of agent
    //!
    //! @param[in]     velocityY    Sideward velocity
    //-----------------------------------------------------------------------------
    virtual void SetVelocityY(double velocityY) = 0;

    //-----------------------------------------------------------------------------
    //! Sets forward velocity of agent
    //!
    //! @param[in]     velocityX    Forward velocity
    //-----------------------------------------------------------------------------
    virtual void SetVelocity(double value) = 0;

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
    //! Sets current angle of the steering wheel in degree
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
    //! Sets forward acceleration of agent
    //!
    //! @param[in]     accelerationX    forward acceleration
    //-----------------------------------------------------------------------------
    virtual void SetAccelerationX(double accelerationX) = 0;

    //-----------------------------------------------------------------------------
    //! Sets sideward acceleration of agent
    //!
    //! @param[in]     accelerationY    sideward acceleration
    //-----------------------------------------------------------------------------
    virtual void SetAccelerationY(double accelerationY) = 0;

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
    //! Inits all physical and world specific parameters of an agent.
    //!
    // @return     true for success
    //-----------------------------------------------------------------------------
    virtual bool InitAgentParameter(int id,
                                    int agentTypeId,
                                    const AgentSpawnItem *agentSpawnItem,
                                    const SpawnItemParameterInterface &spawnItemParameter) = 0;

    //-----------------------------------------------------------------------------
    //! Inits all physical and world specific parameters of an agent.
    //!
    //! @return    true for success
    //-----------------------------------------------------------------------------
    virtual bool InitAgentParameter(int id,
                                    AgentBlueprintInterface* agentBlueprint) = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves type of agent
    //!
    //! @return                Id of agent type
    //-----------------------------------------------------------------------------
    virtual int GetAgentTypeId() const = 0;

    //-----------------------------------------------------------------------------
    //! Returns true if agent is still in World located.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual bool IsAgentInWorld() const = 0;

    //-----------------------------------------------------------------------------
    //! Returns true if agent is agent is at end of road or near the end.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual bool IsAgentAtEndOfRoad() = 0;

    //-----------------------------------------------------------------------------
    //! Set the position of an agent.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual void SetPosition(Position pos) = 0;

    //-----------------------------------------------------------------------------
    //! Returns the distance to the next agent in front in a specific lane.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual double GetDistanceToFrontAgent(int laneId) = 0;

    //-----------------------------------------------------------------------------
    //! Returns the distance to the next agent behind in a specific lane.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual double GetDistanceToRearAgent(int laneId) = 0;

    //-----------------------------------------------------------------------------
    //! Removes the marker of the agent which marks it special.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual void RemoveSpecialAgentMarker() = 0;

    //-----------------------------------------------------------------------------
    //! Sets the marker of the agent which marks it special.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual void SetSpecialAgentMarker() = 0;

    //-----------------------------------------------------------------------------
    //! Sets the flag to mark the agent as obstacle.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual void SetObstacleFlag() = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the distance to the special vehicle.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual double GetDistanceToSpecialAgent() = 0;

    //-----------------------------------------------------------------------------
    //! Return true if obstacle flag is set in agent.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual bool IsObstacle() = 0;

    //! Returns if agent is partially not on the road
    virtual bool IsLeavingWorld() const = 0;

    //-----------------------------------------------------------------------------
    //! Returns the distance of the front agent to ego.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual double GetDistanceFrontAgentToEgo() = 0;

    //-----------------------------------------------------------------------------
    //! Checks whether the agent has two lanes on the left.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual bool HasTwoLeftLanes() = 0;

    //-----------------------------------------------------------------------------
    //! Checks whether the agent has two lanes on the right.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual bool HasTwoRightLanes() = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve an estimated lane change state.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual LaneChangeState EstimateLaneChangeState(double thresholdLooming) = 0;

    //-----------------------------------------------------------------------------
    //! Get a list of all agents in a lane.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual std::list<AgentInterface *> GetAllAgentsInLane(int laneID,
                                                           double minDistance,
                                                           double maxDistance,
                                                           double AccSensDist) = 0;

    //-----------------------------------------------------------------------------
    //! Returns true if agent is a bicycle.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual bool IsBicycle() const = 0;

    //-----------------------------------------------------------------------------
    //! Unregisters the agent from the world.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual void Unregister() const = 0;

    //-----------------------------------------------------------------------------
    //! Returns true if agent is the first car in lane.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual bool IsFirstCarInLane() const = 0;

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
    //! Retrieve the type of the nearest mark.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual MarkType GetTypeOfNearestMark() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the type of the nearest mark.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual std::string GetTypeOfNearestMarkString() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the distance to the nearest mark of specific markType (NONE for no
    //! specific or any markType).
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual double GetDistanceToNearestMark(MarkType markType) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the relative angle of the nearest mark of specific markType (NONE for no
    //! specific or any markType)
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual double GetOrientationOfNearestMark(MarkType markType) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the view direction to the nearest mark of specific markType (NONE for no
    //! specific or any markType). The direction is seen from the agents
    //! perspective in radiant.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual double GetViewDirectionToNearestMark(MarkType markType) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the AgentViewDirection to the nearest mark of specific markType
    //!(NONE for no specific or any markType). The direction is seen from the
    //! agents perspective.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual AgentViewDirection GetAgentViewDirectionToNearestMark(MarkType markType) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the distance to the nearest mark of specific markType (NONE for no
    //! specific or any markType) in a specific direction angle in radiant.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual double GetDistanceToNearestMarkInViewDirection(MarkType markType,
                                                           AgentViewDirection agentViewDirection) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the distance to the nearest mark of specific markType (NONE for no
    //! specific or any markType) in a specific direction angle in radiant.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual double GetDistanceToNearestMarkInViewDirection(MarkType markType,
                                                           double mainViewDirection) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the relative angle of the nearest mark of specific markType (NONE for no
    //! specific or any markType) in a specific direction angle in radiant.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual double GetOrientationOfNearestMarkInViewDirection(MarkType markType,
                                                              AgentViewDirection agentViewDirection) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the relative angle of the nearest mark of specific markType (NONE for no
    //! specific or any markType) in a specific direction angle in radiant.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual double GetOrientationOfNearestMarkInViewDirection(MarkType markType,
                                                              double mainViewDirection) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the distance to the nearest mark of specific markType (NONE for no
    //! specific or any markType) in a specific range about a viewing direction
    //! angle in radiant.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual double GetDistanceToNearestMarkInViewRange(MarkType markType,
                                                       AgentViewDirection agentViewDirection, double range) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the distance to the nearest mark of specific markType (NONE for no
    //! specific or any markType) in a specific range about a viewing direction
    //! angle in radiant.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual double GetDistanceToNearestMarkInViewRange(MarkType markType, double mainViewDirection,
                                                       double range) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the relative angle of the nearest mark of specific markType (NONE for no
    //! specific or any markType) in a specific range about a viewing direction
    //! angle in radiant.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual double GetOrientationOfNearestMarkInViewRange(MarkType markType,
                                                          AgentViewDirection agentViewDirection, double range) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the relative angle of the nearest mark of specific markType (NONE for no
    //! specific or any markType) in a specific range about a viewing direction
    //! angle in radiant.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual double GetOrientationOfNearestMarkInViewRange(MarkType markType, double mainViewDirection,
                                                          double range) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the concrete viewing direction to the nearest mark in a viewing range.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual double GetViewDirectionToNearestMarkInViewRange(MarkType markType,
                                                            AgentViewDirection agentViewDirection, double range) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the concrete viewing direction to the nearest mark in a viewing range.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual double GetViewDirectionToNearestMarkInViewRange(MarkType markType, double mainViewDirection,
                                                            double range) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the type of the nearest object in a viewing range.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual std::string GetTypeOfNearestObject(AgentViewDirection agentViewDirection,
                                               double range) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the type of the nearest object in a viewing range.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual std::string GetTypeOfNearestObject(double mainViewDirection,
                                               double range) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the distance to the nearest object of specific objectType (NONE for no
    //! specific or any objectType) in a specific range about a viewing direction
    //! angle in radiant.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual double GetDistanceToNearestObjectInViewRange(ObjectType objectType,
                                                         AgentViewDirection agentViewDirection,
                                                         double range) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the distance to the nearest object of specific objectType (NONE for no
    //! specific or any objectType) in a specific range about a viewing direction
    //! angle in radiant.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual double GetDistanceToNearestObjectInViewRange(ObjectType objectType,
                                                         double mainViewDirection,
                                                         double range) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the concrete viewing direction to the nearest object in a viewing range.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual double GetViewDirectionToNearestObjectInViewRange(ObjectType objectType,
                                                              AgentViewDirection agentViewDirection,
                                                              double range) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the concrete viewing direction to the nearest object in a viewing range.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual double GetViewDirectionToNearestObjectInViewRange(ObjectType objectType,
                                                              double mainViewDirection,
                                                              double range) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the Id of the nearest Agent in a viewing range.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual int GetIdOfNearestAgent(AgentViewDirection agentViewDirection,
                                    double range) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the Id of the nearest Agent in a viewing range.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual int GetIdOfNearestAgent(double mainViewDirection,
                                    double range) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the distance to the nearest agent in a specific range about a viewing direction
    //! angle in radiant.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual double GetDistanceToNearestAgentInViewRange(AgentViewDirection agentViewDirection,
                                                        double range) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the distance to the nearest agent in a specific range about a viewing direction
    //! angle in radiant.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual double GetDistanceToNearestAgentInViewRange(double mainViewDirection,
                                                        double range) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the concrete viewing direction to the nearest agent in a viewing range.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual double GetViewDirectionToNearestAgentInViewRange(AgentViewDirection agentViewDirection,
                                                             double range) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the concrete viewing direction to the nearest agent in a viewing range.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual double GetViewDirectionToNearestAgentInViewRange(double mainViewDirection,
                                                             double range) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the visibility to an agent in a specific range about a viewing direction
    //! angle in radiant.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual double GetVisibilityToNearestAgentInViewRange(double mainViewDirection,
                                                        double range) const = 0;

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
    virtual double GetYawAcceleration() = 0;

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
    //! Set the centripetal acceleration of the agent.
    //!
    //! @param[in]   centripetalAcceleration   The acceleration to set [m/s^2]
    //-----------------------------------------------------------------------------
    virtual void SetCentripetalAcceleration(double centripetalAcceleration) = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the trajectory of time of the agent.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual const std::vector<int> *GetTrajectoryTime() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the trajectory of x position of the agent.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual const std::vector<double> *GetTrajectoryXPos() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the trajectory of y position of the agent.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual const std::vector<double> *GetTrajectoryYPos() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the trajectory of the velocity of the agent.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual const std::vector<double> *GetTrajectoryVelocity() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the trajectory of the angle of the agent.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual const std::vector<double> *GetTrajectoryAngle() const = 0;

    //-----------------------------------------------------------------------------
    //! Set the acceleration intention of the agent.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual void SetAccelerationIntention(double accelerationIntention) = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the acceleration intention of the agent.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual double GetAccelerationIntention() const = 0;

    //-----------------------------------------------------------------------------
    //! Set the deceleration intention of the agent.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual void SetDecelerationIntention(double decelerationIntention) = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the deceleration intention of the agent.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual double GetDecelerationIntention() const = 0;

    //-----------------------------------------------------------------------------
    //! Set the angle intention of the agent.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual void SetAngleIntention(double angleIntention) = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the angle intention of the agent.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual double GetAngleIntention() const = 0;

    //-----------------------------------------------------------------------------
    //! Set the collision state of the agent. (collision occured or not)
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual void SetCollisionState(bool collisionState) = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the collision state of the agent. (collision occured or not)
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual bool GetCollisionState() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves absolute acceleration of agent
    //!
    // @return                Sideward acceleration
    //-----------------------------------------------------------------------------
    virtual double GetAccelerationAbsolute() const = 0;

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
    //! Gets current angle of the steering wheel in degree
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
