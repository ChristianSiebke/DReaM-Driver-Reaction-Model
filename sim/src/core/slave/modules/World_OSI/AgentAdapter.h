/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*               2018 AMFD GmbH
*               2021 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  AgentAdapter.h
//! @brief This file implements the wrapper for the agent so it can
//!        interact with the world.
//!        It permits a simple implementation of delegation concepts
//!        (composition vs. inheritance).
//-----------------------------------------------------------------------------

#pragma once

#include <QtGlobal>
#include <functional>

#include "include/agentInterface.h"
#include "include/callbackInterface.h"
#include "include/trafficObjectInterface.h"
#include "include/worldInterface.h"
#include "include/stochasticsInterface.h"
#include "egoAgent.h"
#include "Localization.h"
#include "WorldData.h"
#include "WorldDataQuery.h"
#include "WorldObjectAdapter.h"

constexpr double zeroBaseline = 1e-9;

/*!
 * \brief The AgentAdapter class
 * This class is a adapter for the communication between the framework and world.
 */
class AgentAdapter final : public WorldObjectAdapter, public AgentInterface
{
public:
    const std::string MODULENAME = "AGENTADAPTER";

    AgentAdapter(const openpass::type::EntityId id,
                 WorldInterface* world,
                 const CallbackInterface* callbacks,
                 OWL::Interfaces::WorldData* worldData,
                 const World::Localization::Localizer& localizer);

    ~AgentAdapter() override;

    void InitParameter(const AgentBlueprintInterface& agentBlueprint) override;

    ObjectTypeOSI GetType() const override
    {
        return ObjectTypeOSI::Vehicle;
    }

    int GetId() const override
    {
        return id;
    }

    EgoAgentInterface& GetEgoAgent() override
    {
        return egoAgent;
    }

    std::string GetVehicleModelType() const override
    {
        return vehicleModelType;
    }

    std::string GetScenarioName() const override
    {
        return objectName;
    }

    std::string GetDriverProfileName() const override
    {
        return driverProfileName;
    }

    double GetSpeedGoalMin() const override
    {
        return speedGoalMin;
    }

    double GetEngineSpeed() const override
    {
        return engineSpeed;
    }

    double GetDistanceReferencePointToLeadingEdge() const override
    {
        return GetBaseTrafficObject().GetDistanceReferencePointToLeadingEdge();
    }

    int GetGear() const override
    {
        return currentGear;
    }

    double GetEffAccelPedal() const override
    {
        return accelPedal;
    }

    double GetEffBrakePedal() const override
    {
        return brakePedal;
    }

    double GetSteeringWheelAngle() const override
    {
        return steeringWheelAngle;
    }

    double GetMaxAcceleration() const override
    {
        return maxAcceleration;
    }

    double GetMaxDeceleration() const override
    {
        return maxDeceleration;
    }

    bool GetHeadLight() const override;

    bool GetHighBeamLight() const override;

    bool GetHorn() const override
    {
        return hornSwitch;
    }

    bool GetFlasher() const override
    {
        return flasherSwitch;
    }

    std::vector<std::string> GetRoads(MeasurementPoint mp) const override;

    LightState GetLightState() const override;

    const ObjectPosition& GetObjectPosition() const override
    {
        return GetBaseTrafficObject().GetLocatedPosition();
    }

    double GetMainLocateS() const;

    void UpdateCollision(std::pair<ObjectTypeOSI, int> collisionPartner) override;

    std::vector<std::pair<ObjectTypeOSI, int>> GetCollisionPartners() const override
    {
        return collisionPartners;
    }

    PostCrashVelocity GetPostCrashVelocity() const override
    {
        return postCrashVelocity;
    }

    VehicleModelParameters GetVehicleModelParameters() const override
    {
        return vehicleModelParameters;
    }

    void SetPostCrashVelocity(PostCrashVelocity postCrashVelocity) override
    {
        this->postCrashVelocity = postCrashVelocity;
    }

    void SetPositionX(double value) override
    {
        world->QueueAgentUpdate([this, value]()
        {
            GetBaseTrafficObject().SetX(value);
        });
    }

    void SetPositionY(double value) override
    {
        world->QueueAgentUpdate([this, value]()
        {
            GetBaseTrafficObject().SetY(value);
        });
    }


    void SetVelocity(double value) override
    {
        world->QueueAgentUpdate([this, value]()
        {
            GetBaseTrafficObject().SetAbsVelocity(value);
        });
    }

    void SetVelocityVector(double vx, double vy, double vz) override
    {
        world->QueueAgentUpdate([this, vx, vy, vz]()
        {
            OWL::Primitive::AbsVelocity velocity{vx, vy, vz};
            GetBaseTrafficObject().SetAbsVelocity(velocity);
        });
    }

    void SetAcceleration(double value) override
    {
        world->QueueAgentUpdate([this, value]()
        {
            GetBaseTrafficObject().SetAbsAcceleration(value);
        });
    }

    void SetYaw(double value) override
    {
        world->QueueAgentUpdate([this, value]()
        {
            UpdateYaw(value);
        });
    }

    void SetYawRate(double value) override
    {
        world->QueueAgentUpdate([this, value]()
        {
            OWL::Primitive::AbsOrientationRate orientationRate = GetBaseTrafficObject().GetAbsOrientationRate();
            orientationRate.yawRate = value;
            GetBaseTrafficObject().SetAbsOrientationRate(orientationRate);
        });
    }

    void SetYawAcceleration(double value) override
    {
        world->QueueAgentUpdate([this, value]()
        {
            yawAcceleration = value;
        });
    }

    void SetCentripetalAcceleration(double value) override
    {
        world->QueueAgentUpdate([this, value]()
        {
            centripetalAcceleration = value;
        });
    }

    void SetTangentialAcceleration(double value) override
    {
        world->QueueAgentUpdate([this, value]()
        {
            tangentialAcceleration = value;
        });
    }

    void SetDistanceTraveled(double value) override
    {
        world->QueueAgentUpdate([this, value]()
        {
            distanceTraveled = value;
        });
    }

    void SetVehicleModelParameter(const VehicleModelParameters& parameter) override
    {
        world->QueueAgentUpdate([this, parameter]()
        {
            UpdateVehicleModelParameter(parameter);
        });
    }

    void SetMaxAcceleration(double value) override
    {
        world->QueueAgentUpdate([this, value]()
        {
            maxAcceleration = value;
        });
    }

    void SetEngineSpeed(double value) override
    {
        world->QueueAgentUpdate([this, value]()
        {
            engineSpeed = value;
        });
    }

    void SetMaxDeceleration(double maxDeceleration) override
    {
        world->QueueAgentUpdate([this, maxDeceleration]()
        {
            this->maxDeceleration = maxDeceleration;
        });
    }

    void SetGear(int gear) override
    {
        world->QueueAgentUpdate([this, gear]()
        {
            currentGear = gear;
        });
    }

    void SetEffAccelPedal(double percent) override
    {
        world->QueueAgentUpdate([this, percent]()
        {
            accelPedal = percent;
        });
    }

    void SetEffBrakePedal(double percent) override
    {
        world->QueueAgentUpdate([this, percent]()
        {
            brakePedal = percent;
        });
    }

    void SetSteeringWheelAngle(double steeringWheelAngle) override
    {
        world->QueueAgentUpdate([this, steeringWheelAngle]()
        {
            this->steeringWheelAngle = steeringWheelAngle;
        });
    }

    void SetHeadLight(bool headLight) override
    {
        world->QueueAgentUpdate([this, headLight]()
        {
            GetBaseTrafficObject().SetHeadLight(headLight);
        });
    }

    void SetHighBeamLight(bool highBeam) override
    {
        world->QueueAgentUpdate([this, highBeam]()
        {
            GetBaseTrafficObject().SetHighBeamLight(highBeam);
        });
    }

    void SetHorn(bool horn) override
    {
        world->QueueAgentUpdate([this, horn]()
        {
            hornSwitch = horn;
        });
    }

    void SetFlasher(bool flasher) override
    {
        world->QueueAgentUpdate([this, flasher]()
        {
            flasherSwitch = flasher;
        });
    }

    double GetYawRate() const override
    {
        return GetBaseTrafficObject().GetAbsOrientationRate().yawRate;
    }

    double GetYawAcceleration() const override
    {
        return yawAcceleration;
    }

    double GetCentripetalAcceleration() const override
    {
        return centripetalAcceleration;
    }

    double GetTangentialAcceleration() const override
    {
        return tangentialAcceleration;
    }

    bool Locate() override;

    void Unlocate() override;

    bool Update() override;

    void SetBrakeLight(bool brakeLightStatus) override;

    bool GetBrakeLight() const override;

    AgentCategory GetAgentCategory() const override;

    std::string GetAgentTypeName() const override;

    void SetIndicatorState(IndicatorState indicatorState) override;

    IndicatorState GetIndicatorState() const override;

    bool IsLeavingWorld() const override;

    bool IsAgentInWorld() const override;

    void SetPosition(Position pos) override;

    double GetDistanceToStartOfRoad(MeasurementPoint mp, std::string roadId) const override;

    double GetMainLocateS(const std::string& roadId) const;

    double  GetDistanceTraveled() const override
    {
        return distanceTraveled;
    }

    bool IsEgoAgent() const override;

    bool OnRoad(const OWL::Interfaces::Road& road) const;
    bool OnLane(const OWL::Interfaces::Lane& lane) const;

    double GetVelocity(VelocityScope velocityScope = VelocityScope::Absolute) const override;

    void Unregister() const override;

    double GetLaneRemainder(const std::string& roadId, Side side) const override;

    virtual const openpass::sensors::Parameters& GetSensorParameters() const override

    {
        return sensorParameters;
    }

    virtual void SetSensorParameters(openpass::sensors::Parameters sensorParameters) override
    {
        this->sensorParameters = sensorParameters;
    }

    virtual double GetDistanceToConnectorEntrance(std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const override;

    virtual double GetDistanceToConnectorDeparture(std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const override;

protected:
    //-----------------------------------------------------------------------------
    //! Provides callback to LOG() macro
    //!
    //! @param[in]     logLevel    Importance of log
    //! @param[in]     file        Name of file where log is called
    //! @param[in]     line        Line within file where log is called
    //! @param[in]     message     Message to log
    //-----------------------------------------------------------------------------
    void Log(CbkLogLevel logLevel,
             const char* file,
             int line,
             const std::string& message) const
    {
        if (callbacks)
        {
            callbacks->Log(logLevel,
                           file,
                           line,
                           message);
        }
    }

public:
    virtual int GetAgentId() const override
    {
        throw std::runtime_error("AgentAdapter::GetAgentId not implemented");
    }
    virtual double GetVelocityX() const override
    {
        throw std::runtime_error("AgentAdapter::GetVelocityX not implemented");
    }
    virtual double GetVelocityY() const override
    {
        throw std::runtime_error("AgentAdapter::GetVelocityY not implemented");
    }
    virtual double GetAccelerationX() const override
    {
        throw std::runtime_error("AgentAdapter::GetAccelerationX not implemented");
    }
    virtual double GetAccelerationY() const override
    {
        throw std::runtime_error("AgentAdapter::GetAccelerationY not implemented");
    }
    virtual std::vector<void *> GetCollisionData(int collisionPartnerId,
                                                 int collisionDataId) const override
    {
        Q_UNUSED(collisionPartnerId);
        Q_UNUSED(collisionDataId);

        throw std::runtime_error("AgentAdapter::GetCollisionData not implemented");
    }
    virtual void SetVelocityX(double velocityX) override
    {
        Q_UNUSED(velocityX);

        throw std::runtime_error("AgentAdapter::SetVelocityX not implemented");
    }
    virtual void SetVelocityY(double velocityY) override
    {
        Q_UNUSED(velocityY);

        throw std::runtime_error("AgentAdapter::SetVelocityY not implemented");
    }
    virtual void SetAccelerationX(double accelerationX) override
    {
        Q_UNUSED(accelerationX);

        throw std::runtime_error("AgentAdapter::SetAccelerationX not implemented");
    }
    virtual void SetAccelerationY(double accelerationY) override
    {
        Q_UNUSED(accelerationY);

        throw std::runtime_error("AgentAdapter::SetAccelerationY not implemented");
    }
    bool InitAgentParameter([[maybe_unused]] int id, 
                            [[maybe_unused]] AgentBlueprintInterface* agentBlueprint) override
    {
        throw std::runtime_error("Deprecated method: See agentInterface.h");
    }
    virtual bool InitAgentParameter([[maybe_unused]] int id,
                                    [[maybe_unused]] int agentTypeId,
                                    [[maybe_unused]] const AgentSpawnItem *agentSpawnItem,
                                    [[maybe_unused]] const SpawnItemParameterInterface &spawnItemParameter) override
    {
        throw std::runtime_error("Deprecated method: See agentInterface.h");
    }
    virtual int GetAgentTypeId() const override
    {
        throw std::runtime_error("AgentAdapter::GetAgentTypeId not implemented");
    }
    virtual bool IsAgentAtEndOfRoad() override
    {
        throw std::runtime_error("AgentAdapter::IsAgentAtEndOfRoad not implemented");
    }
    virtual double GetDistanceToFrontAgent(int laneId) override
    {
        Q_UNUSED(laneId);

        throw std::runtime_error("AgentAdapter::GetDistanceToFrontAgent not implemented");
    }
    virtual double GetDistanceToRearAgent(int laneId) override
    {
        Q_UNUSED(laneId);

        throw std::runtime_error("AgentAdapter::GetDistanceToRearAgent not implemented");
    }
    virtual void SetSpecialAgentMarker() override
    {
        throw std::runtime_error("AgentAdapter::SetSpecialAgentMarker not implemented");
    }
    virtual void SetObstacleFlag() override
    {
        throw std::runtime_error("AgentAdapter::SetObstacleFlag not implemented");
    }
    virtual void RemoveSpecialAgentMarker() override
    {
        throw std::runtime_error("AgentAdapter::RemoveSpecialAgentMarker not implemented");
    }
    virtual double GetDistanceToSpecialAgent() override
    {
        throw std::runtime_error("AgentAdapter::GetDistanceToSpecialAgent not implemented");
    }
    virtual bool IsObstacle() override
    {
        throw std::runtime_error("AgentAdapter::IsObstacle not implemented");
    }
    virtual double GetDistanceFrontAgentToEgo() override
    {
        throw std::runtime_error("AgentAdapter::GetDistanceFrontAgentToEgo not implemented");
    }
    virtual bool HasTwoLeftLanes() override
    {
        throw std::runtime_error("AgentAdapter::HasTwoLeftLanes not implemented");
    }
    virtual bool HasTwoRightLanes() override
    {
        throw std::runtime_error("AgentAdapter::HasTwoRightLanes not implemented");
    }
    virtual LaneChangeState EstimateLaneChangeState(double thresholdLooming) override
    {
        Q_UNUSED(thresholdLooming);

        throw std::runtime_error("AgentAdapter::EstimateLaneChangeState not implemented");
    }
    virtual std::list<AgentInterface *> GetAllAgentsInLane(int laneId,
                                                           double minDistance,
                                                           double maxDistance,
                                                           double AccSensDist) override
    {
        Q_UNUSED(laneId);
        Q_UNUSED(minDistance);
        Q_UNUSED(maxDistance);
        Q_UNUSED(AccSensDist);

        throw std::runtime_error("AgentAdapter::GetAllAgentsInLane not implemented");
    }
    virtual bool IsBicycle() const override
    {
        throw std::runtime_error("AgentAdapter::IsBicycle not implemented");
    }
    virtual bool IsFirstCarInLane() const override
    {
        throw std::runtime_error("AgentAdapter::IsFirstCarInLane not implemented");
    }
    virtual MarkType GetTypeOfNearestMark() const override
    {
        throw std::runtime_error("AgentAdapter::GetTypeOfNearestMark not implemented");
    }
    virtual std::string GetTypeOfNearestMarkString() const override
    {
        throw std::runtime_error("AgentAdapter::GetTypeOfNearestMarkString not implemented");
    }
    virtual double GetDistanceToNearestMark(MarkType markType) const override
    {
        Q_UNUSED(markType);

        throw std::runtime_error("AgentAdapter::GetDistanceToNearestMark not implemented");
    }
    virtual double GetOrientationOfNearestMark(MarkType markType) const override
    {
        Q_UNUSED(markType);

        throw std::runtime_error("AgentAdapter::GetOrientationOfNearestMark not implemented");
    }
    virtual double GetViewDirectionToNearestMark(MarkType markType) const override
    {
        Q_UNUSED(markType);

        throw std::runtime_error("AgentAdapter::GetViewDirectionToNearestMark not implemented");
    }
    virtual double GetDistanceToNearestMarkInViewDirection(MarkType markType,
                                                           AgentViewDirection agentViewDirection) const override
    {
        Q_UNUSED(markType);
        Q_UNUSED(agentViewDirection);

        throw std::runtime_error("AgentAdapter::GetDistanceToNearestMarkInViewDirection not implemented");
    }
    virtual double GetDistanceToNearestMarkInViewDirection(MarkType markType,
                                                           double mainViewDirection) const override
    {
        Q_UNUSED(markType);
        Q_UNUSED(mainViewDirection);

        throw std::runtime_error("AgentAdapter::GetDistanceToNearestMarkInViewDirection not implemented");
    }
    virtual double GetOrientationOfNearestMarkInViewDirection(MarkType markType,
                                                              AgentViewDirection agentViewDirection)const override
    {
        Q_UNUSED(markType);
        Q_UNUSED(agentViewDirection);

        throw std::runtime_error("AgentAdapter::GetOrientationOfNearestMarkInViewDirection not implemented");
    }
    virtual double GetOrientationOfNearestMarkInViewDirection(MarkType markType,
                                                              double mainViewDirection) const override
    {
        Q_UNUSED(markType);
        Q_UNUSED(mainViewDirection);

        throw std::runtime_error("AgentAdapter::GetOrientationOfNearestMarkInViewDirection not implemented");
    }
    virtual double GetDistanceToNearestMarkInViewRange(MarkType markType,
                                                       AgentViewDirection agentViewDirection, double range) const override
    {
        Q_UNUSED(markType);
        Q_UNUSED(agentViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("AgentAdapter::GetDistanceToNearestMarkInViewRange not implemented");
    }
    virtual double GetDistanceToNearestMarkInViewRange(MarkType markType, double mainViewDirection,
                                                       double range) const override
    {
        Q_UNUSED(markType);
        Q_UNUSED(mainViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("AgentAdapter::GetDistanceToNearestMarkInViewRange not implemented");
    }
    virtual double GetOrientationOfNearestMarkInViewRange(MarkType markType,
                                                          AgentViewDirection agentViewDirection, double range) const override
    {
        Q_UNUSED(markType);
        Q_UNUSED(agentViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("AgentAdapter::GetOrientationOfNearestMarkInViewRange not implemented");
    }
    virtual double GetOrientationOfNearestMarkInViewRange(MarkType markType, double mainViewDirection,
                                                          double range) const override
    {
        Q_UNUSED(markType);
        Q_UNUSED(mainViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("AgentAdapter::GetOrientationOfNearestMarkInViewRange not implemented");
    }
    virtual double GetViewDirectionToNearestMarkInViewRange(MarkType markType,
                                                            AgentViewDirection agentViewDirection, double range) const override
    {
        Q_UNUSED(markType);
        Q_UNUSED(agentViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("AgentAdapter::GetViewDirectionToNearestMarkInViewRange not implemented");
    }
    virtual double GetViewDirectionToNearestMarkInViewRange(MarkType markType, double mainViewDirection,
                                                            double range) const override
    {
        Q_UNUSED(markType);
        Q_UNUSED(mainViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("AgentAdapter::GetViewDirectionToNearestMarkInViewRange not implemented");
    }
    virtual std::string GetTypeOfNearestObject(AgentViewDirection agentViewDirection,
                                               double range) const override
    {
        Q_UNUSED(agentViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("AgentAdapter::GetTypeOfNearestObject not implemented");
    }
    virtual std::string GetTypeOfNearestObject(double mainViewDirection,
                                               double range) const override
    {
        Q_UNUSED(mainViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("AgentAdapter::GetTypeOfNearestObject not implemented");
    }
    virtual double GetDistanceToNearestObjectInViewRange(ObjectType objectType,
                                                         AgentViewDirection agentViewDirection,
                                                         double range) const override
    {
        Q_UNUSED(objectType);
        Q_UNUSED(agentViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("AgentAdapter::GetDistanceToNearestObjectInViewRange not implemented");
    }
    virtual double GetDistanceToNearestObjectInViewRange(ObjectType objectType,
                                                         double mainViewDirection,
                                                         double range) const override
    {
        Q_UNUSED(objectType);
        Q_UNUSED(mainViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("AgentAdapter::GetDistanceToNearestObjectInViewRange not implemented");
    }
    virtual double GetViewDirectionToNearestObjectInViewRange(ObjectType objectType,
                                                              AgentViewDirection agentViewDirection,
                                                              double range) const override
    {
        Q_UNUSED(objectType);
        Q_UNUSED(agentViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("AgentAdapter::GetViewDirectionToNearestObjectInViewRange not implemented");
    }
    virtual double GetViewDirectionToNearestObjectInViewRange(ObjectType objectType,
                                                              double mainViewDirection,
                                                              double range) const override
    {
        Q_UNUSED(objectType);
        Q_UNUSED(mainViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("AgentAdapter::GetViewDirectionToNearestObjectInViewRange not implemented");
    }
    virtual int GetIdOfNearestAgent(AgentViewDirection agentViewDirection,
                                    double range) const override
    {
        Q_UNUSED(agentViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("AgentAdapter::GetIdOfNearestAgent not implemented");
    }
    virtual int GetIdOfNearestAgent(double mainViewDirection,
                                    double range) const override
    {
        Q_UNUSED(mainViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("AgentAdapter::GetIdOfNearestAgent not implemented");
    }
    virtual double GetDistanceToNearestAgentInViewRange(AgentViewDirection agentViewDirection,
                                                        double range) const override
    {
        Q_UNUSED(agentViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("AgentAdapter::GetDistanceToNearestAgentInViewRange not implemented");
    }
    virtual double GetDistanceToNearestAgentInViewRange(double mainViewDirection,
                                                        double range) const override
    {
        Q_UNUSED(mainViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("AgentAdapter::GetDistanceToNearestAgentInViewRange not implemented");
    }
    virtual double GetViewDirectionToNearestAgentInViewRange(AgentViewDirection agentViewDirection,
                                                             double range) const override
    {
        Q_UNUSED(agentViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("AgentAdapter::GetViewDirectionToNearestAgentInViewRange not implemented");
    }
    virtual double GetViewDirectionToNearestAgentInViewRange(double mainViewDirection,
                                                             double range) const override
    {
        Q_UNUSED(mainViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("AgentAdapter::GetViewDirectionToNearestAgentInViewRange not implemented");
    }
    virtual double GetVisibilityToNearestAgentInViewRange(double mainViewDirection,
                                                        double range) const override
    {
        Q_UNUSED(mainViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("AgentAdapter::GetVisibilityToNearestAgentInViewRange not implemented");
    }
    virtual AgentViewDirection GetAgentViewDirectionToNearestMark(MarkType markType) const override
    {
        Q_UNUSED(markType);

        throw std::runtime_error("AgentAdapter::GetAgentViewDirectionToNearestMark not implemented");
    }

    virtual const std::vector<int> *GetTrajectoryTime() const override
    {
        throw std::runtime_error("AgentAdapter::GetTrajectoryTime not implemented");
    }
    virtual const std::vector<double> *GetTrajectoryXPos() const override
    {
        throw std::runtime_error("AgentAdapter::GetTrajectoryXPos not implemented");
    }
    virtual const std::vector<double> *GetTrajectoryYPos() const override
    {
        throw std::runtime_error("AgentAdapter::GetTrajectoryYPos not implemented");
    }
    virtual const std::vector<double> *GetTrajectoryVelocity() const override
    {
        throw std::runtime_error("AgentAdapter::GetTrajectoryVelocity not implemented");
    }
    virtual const std::vector<double> *GetTrajectoryAngle() const override
    {
        throw std::runtime_error("AgentAdapter::GetTrajectoryAngle not implemented");
    }
    virtual void SetAccelerationIntention(double accelerationIntention) override
    {
        Q_UNUSED(accelerationIntention);

        throw std::runtime_error("AgentAdapter::SetAccelerationIntention not implemented");
    }
    virtual double GetAccelerationIntention() const override
    {
        throw std::runtime_error("AgentAdapter::GetAccelerationIntention not implemented");
    }
    virtual void SetDecelerationIntention(double decelerationIntention) override
    {
        Q_UNUSED(decelerationIntention);

        throw std::runtime_error("AgentAdapter::SetDecelerationIntention not implemented");
    }
    virtual double GetDecelerationIntention() const override
    {
        throw std::runtime_error("AgentAdapter::GetDecelerationIntention not implemented");
    }
    virtual void SetAngleIntention(double angleIntention) override
    {
        Q_UNUSED(angleIntention);

        throw std::runtime_error("AgentAdapter::SetAngleIntention not implemented");
    }
    virtual double GetAngleIntention() const override
    {
        throw std::runtime_error("AgentAdapter::GetAngleIntention not implemented");
    }
    virtual void SetCollisionState(bool collisionState) override
    {
        Q_UNUSED(collisionState);

        throw std::runtime_error("AgentAdapter::SetCollisionState not implemented");
    }
    virtual bool GetCollisionState() const override
    {
        throw std::runtime_error("AgentAdapter::GetCollisionState not implemented");
    }
    virtual double GetAccelerationAbsolute() const override
    {
        throw std::runtime_error("AgentAdapter::GetAccelerationAbsolute not implemented");
    }

private:
    const int id;
    WorldInterface* world;
    const CallbackInterface* callbacks;
    OWL::Interfaces::WorldData* worldData;
    const World::Localization::Localizer& localizer;
    EgoAgent egoAgent;

    OWL::Interfaces::MovingObject& GetBaseTrafficObject()
    {
        return *(static_cast<OWL::Interfaces::MovingObject*>(&baseTrafficObject));
    }

    OWL::Interfaces::MovingObject& GetBaseTrafficObject() const
    {
        return *(static_cast<OWL::Interfaces::MovingObject*>(&baseTrafficObject));
    }

    void UpdateVehicleModelParameter(const VehicleModelParameters& parameter)
    {
        OWL::Primitive::Dimension dimension = baseTrafficObject.GetDimension();
        dimension.width = parameter.width;
        dimension.length = parameter.length;
        dimension.height = parameter.height;

        GetBaseTrafficObject().SetDimension(dimension);
        GetBaseTrafficObject().SetDistanceReferencPointToLeadingEdge(parameter.distanceReferencePointToLeadingEdge);

        vehicleModelParameters = parameter;
    }

    void UpdateYaw(double yawAngle)
    {
        OWL::Primitive::AbsOrientation orientation = baseTrafficObject.GetAbsOrientation();
        orientation.yaw = yawAngle;
        GetBaseTrafficObject().SetAbsOrientation(orientation);
    }

    //-----------------------------------------------------------------------------
    //! Initialize the ego vehicle object inside the drivingView.
    //-----------------------------------------------------------------------------
    void InitEgoVehicle();

    //-----------------------------------------------------------------------------
    //! Update the ego vehicle object inside the drivingView.
    //-----------------------------------------------------------------------------
    void UpdateEgoVehicle();

    struct LaneObjParameters
    {
        double distance;
        double relAngle;
        double latPosition;
        Common::Vector2d upperLeftCoord;
        Common::Vector2d upperRightCoord;
        Common::Vector2d lowerLeftCoord;
        Common::Vector2d lowerRightCoord;
    };

    bool hornSwitch = false;
    bool flasherSwitch = false;
    int currentGear = 0;
    double maxAcceleration = 0.0;
    double maxDeceleration = 0.0;
    double accelPedal = 0.;
    double brakePedal = 0.;
    double steeringWheelAngle = 0.0;
    double centripetalAcceleration = 0.0;
    double tangentialAcceleration = 0.0;
    double yawAcceleration = 0.0;
    double engineSpeed = 0.;
    double distanceTraveled = 0.0;

    World::Localization::Result locateResult;
    mutable std::vector<GlobalRoadPosition> boundaryPoints;

    std::vector<std::pair<ObjectTypeOSI, int>> collisionPartners;
    PostCrashVelocity postCrashVelocity {};
    bool isValid = true;

    AgentCategory agentCategory;
    std::string agentTypeName;
    std::string vehicleModelType;
    std::string driverProfileName;
    std::string objectName;
    VehicleModelParameters vehicleModelParameters;

    double speedGoalMin;

    bool completlyInWorld = false;

    openpass::sensors::Parameters sensorParameters;
};
