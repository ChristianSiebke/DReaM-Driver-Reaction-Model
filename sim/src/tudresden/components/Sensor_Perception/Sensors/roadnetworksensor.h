#ifndef ROADNETWORKSENSOR_H
#define ROADNETWORKSENSOR_H

#include "WorldData.h"
#include "stoppingpointcalculation.h"

class ConflictAreaCalculator {
public:
    ConflictAreaCalculator() {
    }

    void AssignPotentialConflictAreasToLanes(std::shared_ptr<InfrastructurePerception> perceptionData) const;

private:
    bool LanesDoNotIntersect(const MentalInfrastructure::Lane *laneA, const MentalInfrastructure::Lane *laneB) const;

    bool LanesHavePotentialConfliceArea(const MentalInfrastructure::Lane *laneA, const MentalInfrastructure::Lane *laneB) const;

    bool LanesPotentiallyIntersect(const MentalInfrastructure::LanePoint *p1, const MentalInfrastructure::LanePoint *p2,
                                   const MentalInfrastructure::LanePoint *q1, const MentalInfrastructure::LanePoint *q2) const;

    std::optional<std::pair<MentalInfrastructure::ConflictArea, MentalInfrastructure::ConflictArea>>
    CalculateConflictAreas(const MentalInfrastructure::Lane *currentLane, const MentalInfrastructure::Lane *junctionLane) const;

    std::pair<std::vector<MentalInfrastructure::LanePoint>, std::vector<MentalInfrastructure::LanePoint>>
    CalculateLaneJunctionPoints(const std::list<MentalInfrastructure::LanePoint> &lanePointsA,
                                const std::list<MentalInfrastructure::LanePoint> &lanePointsB) const;

    std::vector<std::list<MentalInfrastructure::LanePoint>>
    SplitListIntoChunks(const std::list<MentalInfrastructure::LanePoint> &lanePoints, unsigned int segments) const;

    std::optional<std::pair<MentalInfrastructure::LanePoint, MentalInfrastructure::LanePoint>>
    CalculateChunkJunctionPoint(const std::list<MentalInfrastructure::LanePoint> &chunkA,
                                const std::list<MentalInfrastructure::LanePoint> &chunkB, double junctionCondition) const;
};

///
/// \brief The RoadNetworkSensor is used to convert the OSI road network into a proprietary structure. This class is built using the
/// Singleton pattern. There can only be one instance of this class at a time to avoid duplicates of the generated data.
///
class RoadNetworkSensor {
public:
    static std::shared_ptr<RoadNetworkSensor> GetInstance(WorldInterface *world) {
        if (!instance)
            instance = std::shared_ptr<RoadNetworkSensor>(new RoadNetworkSensor(world));
        return instance;
    }
    std::shared_ptr<InfrastructurePerception> GetRoadNetwork();

    static void ResetRoadNetworkSensor() {
        instance.reset();
    }

private:
    void PrepareLookupTableRoadNetwork();

    const MentalInfrastructure::TrafficSign *ConvertTrafficSign(const MentalInfrastructure::Road *road,
                                                                const OWL::Implementation::TrafficSign *sign);

    const MentalInfrastructure::Road *ConvertRoad(const OWL::Interfaces::Road *road);

    const MentalInfrastructure::Section *ConvertSection(const OWL::Interfaces::Section *section);

    const MentalInfrastructure::Lane *ConvertLane(const OWL::Interfaces::Lane *lane);

    void AddLaneGeometry(MentalInfrastructure::Lane *newLane, const OWL::Interfaces::Lane *lane) const;

    const MentalInfrastructure::Junction *ConvertJunction(const OWL::Interfaces::Junction *junction);

    RoadmapGraph::RoadmapGraph CreateGraph(std::vector<std::shared_ptr<const MentalInfrastructure::Lane>> &lanes);

    StoppingPointData CreateStoppingPoints(std::vector<std::shared_ptr<const MentalInfrastructure::Junction>> &junctions);

    // public methods to handle correct deletion
public:
    RoadNetworkSensor(RoadNetworkSensor const &) = delete;
    RoadNetworkSensor &operator=(RoadNetworkSensor const &) = delete;
    ~RoadNetworkSensor() {
    }

private:
    RoadNetworkSensor(WorldInterface *world) : world(world) {
        perceptionData = std::make_shared<InfrastructurePerception>();
    }

private:
    static std::shared_ptr<RoadNetworkSensor> instance;
    WorldInterface *world;             // the world all agents are in
    bool infrastructureExists = false; // if the perception data has already been generated

    std::shared_ptr<InfrastructurePerception> perceptionData;
    RoadmapGraph::RoadmapGraph roadMapGraph;

    ConflictAreaCalculator conflictAreaCalculator;

    StoppingPointCalculation stoppingPointCalculation;
};

#endif // ROADNETWORKSENSOR_H
