/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*               2018 AMFD GmbH
*               2016 ITK Engineering GmbH
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

#include "Interfaces/agentInterface.h"
#include "Interfaces/callbackInterface.h"
#include "Interfaces/trafficObjectInterface.h"
#include "Interfaces/worldInterface.h"
#include "Interfaces/stochasticsInterface.h"
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

    AgentAdapter(WorldInterface* world, const CallbackInterface* callbacks, OWL::Interfaces::WorldData* worldData, const World::Localization::Localizer& localizer);
    ~AgentAdapter() override;

    ObjectTypeOSI GetType() const override
    {
        return ObjectTypeOSI::Vehicle;
    }

    bool InitAgentParameter(int id,
                            AgentBlueprintInterface* agentBlueprint) override;

    //////////////////////////////////////////////////////////

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

    VehicleModelParameters GetVehicleModelParameters() const override
    {
        return vehicleModelParameters;
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

    void SetCentripetalAcceleration(double value) override
    {
        world->QueueAgentUpdate([this, value]()
        {
            centripetalAcceleration = value;

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

    double GetCentripetalAcceleration() const override
    {
        return centripetalAcceleration;
    }

    bool Locate() override;

    void Unlocate() override;

    bool Update() override;

    // callback from model
    void RemoveAgent() override
    {
        // disable in schedule
        isValid = false;
    }

    bool IsValid() const override
    {
        return isValid;
    }

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
        throw std::runtime_error("not implemented");
    }
    virtual double GetVelocityX() const override
    {
        throw std::runtime_error("not implemented");
    }
    virtual double GetVelocityY() const override
    {
        throw std::runtime_error("not implemented");
    }
    virtual double GetAccelerationX() const override
    {
        throw std::runtime_error("not implemented");
    }
    virtual double GetAccelerationY() const override
    {
        throw std::runtime_error("not implemented");
    }
    virtual std::vector<void *> GetCollisionData(int collisionPartnerId,
                                                 int collisionDataId) const override
    {
        Q_UNUSED(collisionPartnerId);
        Q_UNUSED(collisionDataId);

        throw std::runtime_error("not implemented");
    }
    virtual void SetVelocityX(double velocityX) override
    {
        Q_UNUSED(velocityX);

        throw std::runtime_error("not implemented");
    }
    virtual void SetVelocityY(double velocityY) override
    {
        Q_UNUSED(velocityY);

        throw std::runtime_error("not implemented");
    }
    virtual void SetAccelerationX(double accelerationX) override
    {
        Q_UNUSED(accelerationX);

        throw std::runtime_error("not implemented");
    }
    virtual void SetAccelerationY(double accelerationY) override
    {
        Q_UNUSED(accelerationY);

        throw std::runtime_error("not implemented");
    }
    virtual bool InitAgentParameter(int id,
                                    int agentTypeId,
                                    const AgentSpawnItem *agentSpawnItem,
                                    const SpawnItemParameterInterface &spawnItemParameter) override
    {
        Q_UNUSED(id);
        Q_UNUSED(agentTypeId);
        Q_UNUSED(agentSpawnItem);
        Q_UNUSED(spawnItemParameter);

        throw std::runtime_error("not implemented");
    }
    virtual int GetAgentTypeId() const override
    {
        throw std::runtime_error("not implemented");
    }
    virtual bool IsAgentAtEndOfRoad() override
    {
        throw std::runtime_error("not implemented");
    }
    virtual double GetDistanceToFrontAgent(int laneId) override
    {
        Q_UNUSED(laneId);

        throw std::runtime_error("not implemented");
    }
    virtual double GetDistanceToRearAgent(int laneId) override
    {
        Q_UNUSED(laneId);

        throw std::runtime_error("not implemented");
    }
    virtual void SetSpecialAgentMarker() override
    {
        throw std::runtime_error("not implemented");
    }
    virtual void SetObstacleFlag() override
    {
        throw std::runtime_error("not implemented");
    }
    virtual void RemoveSpecialAgentMarker() override
    {
        throw std::runtime_error("not implemented");
    }
    virtual double GetDistanceToSpecialAgent() override
    {
        throw std::runtime_error("not implemented");
    }
    virtual bool IsObstacle() override
    {
        throw std::runtime_error("not implemented");
    }
    virtual double GetDistanceFrontAgentToEgo() override
    {
        throw std::runtime_error("not implemented");
    }
    virtual bool HasTwoLeftLanes() override
    {
        throw std::runtime_error("not implemented");
    }
    virtual bool HasTwoRightLanes() override
    {
        throw std::runtime_error("not implemented");
    }
    virtual LaneChangeState EstimateLaneChangeState(double thresholdLooming) override
    {
        Q_UNUSED(thresholdLooming);

        throw std::runtime_error("not implemented");
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

        throw std::runtime_error("not implemented");
    }
    virtual bool IsBicycle() const override
    {
        throw std::runtime_error("not implemented");
    }
    virtual bool IsFirstCarInLane() const override
    {
        throw std::runtime_error("not implemented");
    }
    virtual MarkType GetTypeOfNearestMark() const override
    {
        throw std::runtime_error("not implemented");
    }
    virtual std::string GetTypeOfNearestMarkString() const override
    {
        throw std::runtime_error("not implemented");
    }
    virtual double GetDistanceToNearestMark(MarkType markType) const override
    {
        Q_UNUSED(markType);

        throw std::runtime_error("not implemented");
    }
    virtual double GetOrientationOfNearestMark(MarkType markType) const override
    {
        Q_UNUSED(markType);

        throw std::runtime_error("not implemented");
    }
    virtual double GetViewDirectionToNearestMark(MarkType markType) const override
    {
        Q_UNUSED(markType);

        throw std::runtime_error("not implemented");
    }
    virtual double GetDistanceToNearestMarkInViewDirection(MarkType markType,
                                                           AgentViewDirection agentViewDirection) const override
    {
        Q_UNUSED(markType);
        Q_UNUSED(agentViewDirection);

        throw std::runtime_error("not implemented");
    }
    virtual double GetDistanceToNearestMarkInViewDirection(MarkType markType,
                                                           double mainViewDirection) const override
    {
        Q_UNUSED(markType);
        Q_UNUSED(mainViewDirection);

        throw std::runtime_error("not implemented");
    }
    virtual double GetOrientationOfNearestMarkInViewDirection(MarkType markType,
                                                              AgentViewDirection agentViewDirection)const override
    {
        Q_UNUSED(markType);
        Q_UNUSED(agentViewDirection);

        throw std::runtime_error("not implemented");
    }
    virtual double GetOrientationOfNearestMarkInViewDirection(MarkType markType,
                                                              double mainViewDirection) const override
    {
        Q_UNUSED(markType);
        Q_UNUSED(mainViewDirection);

        throw std::runtime_error("not implemented");
    }
    virtual double GetDistanceToNearestMarkInViewRange(MarkType markType,
                                                       AgentViewDirection agentViewDirection, double range) const override
    {
        Q_UNUSED(markType);
        Q_UNUSED(agentViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("not implemented");
    }
    virtual double GetDistanceToNearestMarkInViewRange(MarkType markType, double mainViewDirection,
                                                       double range) const override
    {
        Q_UNUSED(markType);
        Q_UNUSED(mainViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("not implemented");
    }
    virtual double GetOrientationOfNearestMarkInViewRange(MarkType markType,
                                                          AgentViewDirection agentViewDirection, double range) const override
    {
        Q_UNUSED(markType);
        Q_UNUSED(agentViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("not implemented");
    }
    virtual double GetOrientationOfNearestMarkInViewRange(MarkType markType, double mainViewDirection,
                                                          double range) const override
    {
        Q_UNUSED(markType);
        Q_UNUSED(mainViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("not implemented");
    }
    virtual double GetViewDirectionToNearestMarkInViewRange(MarkType markType,
                                                            AgentViewDirection agentViewDirection, double range) const override
    {
        Q_UNUSED(markType);
        Q_UNUSED(agentViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("not implemented");
    }
    virtual double GetViewDirectionToNearestMarkInViewRange(MarkType markType, double mainViewDirection,
                                                            double range) const override
    {
        Q_UNUSED(markType);
        Q_UNUSED(mainViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("not implemented");
    }
    virtual std::string GetTypeOfNearestObject(AgentViewDirection agentViewDirection,
                                               double range) const override
    {
        Q_UNUSED(agentViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("not implemented");
    }
    virtual std::string GetTypeOfNearestObject(double mainViewDirection,
                                               double range) const override
    {
        Q_UNUSED(mainViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("not implemented");
    }
    virtual double GetDistanceToNearestObjectInViewRange(ObjectType objectType,
                                                         AgentViewDirection agentViewDirection,
                                                         double range) const override
    {
        Q_UNUSED(objectType);
        Q_UNUSED(agentViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("not implemented");
    }
    virtual double GetDistanceToNearestObjectInViewRange(ObjectType objectType,
                                                         double mainViewDirection,
                                                         double range) const override
    {
        Q_UNUSED(objectType);
        Q_UNUSED(mainViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("not implemented");
    }
    virtual double GetViewDirectionToNearestObjectInViewRange(ObjectType objectType,
                                                              AgentViewDirection agentViewDirection,
                                                              double range) const override
    {
        Q_UNUSED(objectType);
        Q_UNUSED(agentViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("not implemented");
    }
    virtual double GetViewDirectionToNearestObjectInViewRange(ObjectType objectType,
                                                              double mainViewDirection,
                                                              double range) const override
    {
        Q_UNUSED(objectType);
        Q_UNUSED(mainViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("not implemented");
    }
    virtual int GetIdOfNearestAgent(AgentViewDirection agentViewDirection,
                                    double range) const override
    {
        Q_UNUSED(agentViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("not implemented");
    }
    virtual int GetIdOfNearestAgent(double mainViewDirection,
                                    double range) const override
    {
        Q_UNUSED(mainViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("not implemented");
    }
    virtual double GetDistanceToNearestAgentInViewRange(AgentViewDirection agentViewDirection,
                                                        double range) const override
    {
        Q_UNUSED(agentViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("not implemented");
    }
    virtual double GetDistanceToNearestAgentInViewRange(double mainViewDirection,
                                                        double range) const override
    {
        Q_UNUSED(mainViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("not implemented");
    }
    virtual double GetViewDirectionToNearestAgentInViewRange(AgentViewDirection agentViewDirection,
                                                             double range) const override
    {
        Q_UNUSED(agentViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("not implemented");
    }
    virtual double GetViewDirectionToNearestAgentInViewRange(double mainViewDirection,
                                                             double range) const override
    {
        Q_UNUSED(mainViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("not implemented");
    }
    virtual double GetVisibilityToNearestAgentInViewRange(double mainViewDirection,
                                                        double range) const override
    {
        Q_UNUSED(mainViewDirection);
        Q_UNUSED(range);

        throw std::runtime_error("not implemented");
    }
    virtual AgentViewDirection GetAgentViewDirectionToNearestMark(MarkType markType) const override
    {
        Q_UNUSED(markType);

        throw std::runtime_error("not implemented");
    }
    virtual double GetYawAcceleration() override
    {
        throw std::runtime_error("not implemented");
    }
    virtual void SetYawAcceleration(double yawAcceleration) override
    {
        Q_UNUSED(yawAcceleration);

        throw std::runtime_error("not implemented");
    }
    virtual const std::vector<int> *GetTrajectoryTime() const override
    {
        throw std::runtime_error("not implemented");
    }
    virtual const std::vector<double> *GetTrajectoryXPos() const override
    {
        throw std::runtime_error("not implemented");
    }
    virtual const std::vector<double> *GetTrajectoryYPos() const override
    {
        throw std::runtime_error("not implemented");
    }
    virtual const std::vector<double> *GetTrajectoryVelocity() const override
    {
        throw std::runtime_error("not implemented");
    }
    virtual const std::vector<double> *GetTrajectoryAngle() const override
    {
        throw std::runtime_error("not implemented");
    }
    virtual void SetAccelerationIntention(double accelerationIntention) override
    {
        Q_UNUSED(accelerationIntention);

        throw std::runtime_error("not implemented");
    }
    virtual double GetAccelerationIntention() const override
    {
        throw std::runtime_error("not implemented");
    }
    virtual void SetDecelerationIntention(double decelerationIntention) override
    {
        Q_UNUSED(decelerationIntention);

        throw std::runtime_error("not implemented");
    }
    virtual double GetDecelerationIntention() const override
    {
        throw std::runtime_error("not implemented");
    }
    virtual void SetAngleIntention(double angleIntention) override
    {
        Q_UNUSED(angleIntention);

        throw std::runtime_error("not implemented");
    }
    virtual double GetAngleIntention() const override
    {
        throw std::runtime_error("not implemented");
    }
    virtual void SetCollisionState(bool collisionState) override
    {
        Q_UNUSED(collisionState);

        throw std::runtime_error("not implemented");
    }
    virtual bool GetCollisionState() const override
    {
        throw std::runtime_error("not implemented");
    }
    virtual double GetAccelerationAbsolute() const override
    {
        throw std::runtime_error("not implemented");
    }

private:
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
    double engineSpeed = 0.;
    double distanceTraveled = 0.0;

    World::Localization::Result locateResult;
    mutable std::vector<GlobalRoadPosition> boundaryPoints;

    std::vector<std::pair<ObjectTypeOSI, int>> collisionPartners;
    bool isValid = true;

    int id{0};
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
