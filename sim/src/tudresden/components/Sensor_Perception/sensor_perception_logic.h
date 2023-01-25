/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#ifndef SENSORPERCEPTIONLOGIC_H
#define SENSORPERCEPTIONLOGIC_H

#include "RouteConverter.h"
#include "Sensors/aabbtreehandler.h"
#include "Sensors/basicvisualsensor.h"
#include "Sensors/driverperception.h"
#include "Sensors/roadnetworksensor.h"
#include "Sensors/trafficsignalvisualsensor.h"
#include "include/agentInterface.h"

struct GazeState;

typedef boost::geometry::model::d2::point_xy<double> BoostVector2d;
typedef boost::geometry::model::polygon<BoostVector2d> Polygon2d;

///
/// @brief Helper class to encapsulate all methods that should be executed on Trigger() of the Senso_Perception_Implementation.
///
class SensorPerceptionLogic {
  public:
      SensorPerceptionLogic(AgentInterface *agent, WorldInterface *world) : driver(agent), world(world) {
          roadNetworkSensor = RoadNetworkSensor::GetInstance(world);
          aabbTreeHandler = AABBTreeHandler::GetInstance(world);
          driverPerception = std::make_shared<DriverPerception>(agent, world, roadNetworkSensor->GetRoadNetwork());
          visualSensor = std::make_shared<BasicVisualSensor>(agent, world, roadNetworkSensor->GetRoadNetwork());
          trafficSignalVisualSensor = std::make_shared<TrafficSignalVisualSensor>(agent, world, roadNetworkSensor->GetRoadNetwork());
      }
    ~SensorPerceptionLogic() {
        perceivedAgents.clear();
        if (roadNetworkSensor.use_count() == 2) {
            roadNetworkSensor->ResetRoadNetworkSensor();
        }
        if (aabbTreeHandler.use_count() == 2) {
            aabbTreeHandler->ResetAABBTreeHandler();
        }
    }

    ///
    /// \brief Starts the calculation of Infrastructure, DriverPerception and VisualPerception.
    ///
    void Trigger(int timestamp, double directionAngle, double sensorDistance, double sensorOpeningAngle,
                 std::optional<Common::Vector2d> mirrorPos, bool godMode, std::vector<InternWaypoint> route);

    ///
    /// \brief Returns a pointer to the visual sensor, no calculation is performed.
    ///
    std::shared_ptr<VisualSensorInterface<std::shared_ptr<GeneralAgentPerception>>> GetVisualSensor() const {
        return visualSensor;
    }

    ///
    /// \brief Returns a pointer to the driver perception, no calculation is performed.
    ///
    std::shared_ptr<DetailedAgentPerception> GetEgoPerception() const {
        return driverPerception->GetEgoPerception();
    }

    ///
    /// \brief Returns a pointer to the Infrastructure, no calculation is performed.
    ///
    std::shared_ptr<InfrastructurePerception> GetInfrastructure() const { return roadNetworkSensor->GetRoadNetwork(); }

    ///
    /// \brief Returns the list of currently visible agents, no calculation is performed.
    ///
    const std::vector<std::shared_ptr<GeneralAgentPerception>> GetGeneralAgentPerception() const {
        return perceivedAgents;
    }

    ///
    /// \brief Returns the list of currently visible traffic signs, no calculation is performed.
    ///
    const std::vector<const MentalInfrastructure::TrafficSignal *> GetTrafficSignalPerception() const {
        return perceivedTrafficSignals;
    }

  private:
      std::shared_ptr<VisualSensorInterface<std::shared_ptr<GeneralAgentPerception>>> visualSensor;
      std::shared_ptr<VisualSensorInterface<const MentalInfrastructure::TrafficSignal *>> trafficSignalVisualSensor;
      std::shared_ptr<AABBTreeHandler> aabbTreeHandler;

      std::shared_ptr<DriverPerception> driverPerception;
      std::shared_ptr<RoadNetworkSensor> roadNetworkSensor;

      std::vector<std::shared_ptr<GeneralAgentPerception>> perceivedAgents;
      std::vector<const MentalInfrastructure::TrafficSignal *> perceivedTrafficSignals;

      AgentInterface *driver;
      WorldInterface *world;
};

#endif // SENSORPERCEPTIONLOGIC_H
