#ifndef SENSORPERCEPTIONLOGIC_H
#define SENSORPERCEPTIONLOGIC_H

#include "include/agentInterface.h"
#include "Sensors/basicvisualsensor.h"
#include "Sensors/driverperception.h"
#include "Sensors/roadnetworksensor.h"
#include "Sensors/trafficsignvisualsensor.h"
#include "Sensors/aabbtreehandler.h"

struct GazeState;

typedef boost::geometry::model::d2::point_xy<double> BoostVector2d;
typedef boost::geometry::model::polygon<BoostVector2d> Polygon2d;

///
/// @brief Helper class to encapsulate all methods that should be executed on Trigger() of the Senso_Perception_Implementation.
///
class SensorPerceptionLogic {
  public:
    SensorPerceptionLogic(AgentInterface* agent, WorldInterface* world) : driver(agent), world(world) {
        roadNetworkSensor = RoadNetworkSensor::GetInstance(world);
        aabbTreeHandler = AABBTreeHandler::GetInstance(world);
        driverPerception = std::make_shared<DriverPerception>(agent, world, roadNetworkSensor->GetRoadNetwork());
        visualSensor = std::make_shared<BasicVisualSensor>(agent, world, roadNetworkSensor->GetRoadNetwork());
        trafficSignVisualSensor = std::make_shared<TrafficSignVisualSensor>(agent, world, roadNetworkSensor->GetRoadNetwork());
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
    void Trigger(int timestamp, double directionAngle, double sensorDistance, double sensorOpeningAngle);

    ///
    /// \brief Returns a pointer to the visual sensor, no calculation is performed.
    ///
    std::shared_ptr<VisualSensorInterface<std::shared_ptr<AgentPerception>>> GetVisualSensor() const { return visualSensor; }

    ///
    /// \brief Returns a pointer to the driver perception, no calculation is performed.
    ///
    std::shared_ptr<EgoPerception> GetEgoPerception() const { return driverPerception->GetEgoPerception(); }

    ///
    /// \brief Returns a pointer to the Infrastructure, no calculation is performed.
    ///
    std::shared_ptr<InfrastructurePerception> GetInfrastructure() const { return roadNetworkSensor->GetRoadNetwork(); }

    ///
    /// \brief Returns the list of currently visible agents, no calculation is performed.
    ///
    const std::vector<std::shared_ptr<AgentPerception>> GetAgentPerception() const { return perceivedAgents; }

    ///
    /// \brief Returns the list of currently visible traffic signs, no calculation is performed.
    ///
    const std::vector<const MentalInfrastructure::TrafficSign*> GetTrafficSignPerception() const { return perceivedTrafficSigns; }

  private:
    std::shared_ptr<VisualSensorInterface<std::shared_ptr<AgentPerception>>> visualSensor;
    std::shared_ptr<VisualSensorInterface<const MentalInfrastructure::TrafficSign*>> trafficSignVisualSensor;
    std::shared_ptr<AABBTreeHandler> aabbTreeHandler;

    std::shared_ptr<DriverPerception> driverPerception;
    std::shared_ptr<RoadNetworkSensor> roadNetworkSensor;

    std::vector<std::shared_ptr<AgentPerception>> perceivedAgents;
    std::vector<const MentalInfrastructure::TrafficSign*> perceivedTrafficSigns;

    AgentInterface* driver;
    WorldInterface* world;
};

#endif // SENSORPERCEPTIONLOGIC_H
