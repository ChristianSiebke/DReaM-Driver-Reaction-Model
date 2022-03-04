#include "roadnetworksensor.h"

#include <algorithm>

std::shared_ptr<RoadNetworkSensor> RoadNetworkSensor::instance = nullptr;

void ConflictAreaCalculator::AssignPotentialConflictAreasToLanes(std::shared_ptr<InfrastructurePerception> perceptionData) const {
    for (const auto &currentLane : perceptionData->lanes) {
        for (const auto &junctionLane : perceptionData->lanes) {
            if (currentLane == junctionLane)
                continue;
            if (currentLane->GetConflictAreaWithLane(junctionLane.get()))
                continue;
            if (currentLane->GetType() == LaneType::Shoulder || junctionLane->GetType() == LaneType::Shoulder)
                continue;
            if (LanesDoNotIntersect(currentLane.get(), junctionLane.get()))
                continue;
            if (LanesHavePotentialConfliceArea(currentLane.get(), junctionLane.get())) {
                if (auto conflictAreas = CalculateConflictAreas(currentLane.get(), junctionLane.get())) {
                    const_cast<MentalInfrastructure::Lane *>(currentLane.get())
                        ->AddConflictArea({junctionLane.get(), conflictAreas->first});
                    const_cast<MentalInfrastructure::Lane *>(junctionLane.get())
                        ->AddConflictArea({currentLane.get(), conflictAreas->second});
                    ConflictPoints a;
                    a.junctionOpenDriveRoadId = currentLane->GetRoad()->GetOpenDriveId();
                    a.junctionOpenDriveLaneId = currentLane->GetOpenDriveId();
                    a.currentOpenDriveRoadId = junctionLane->GetRoad()->GetOpenDriveId();
                    a.currentOpenDriveLaneId = junctionLane->GetOpenDriveId();
                    a.start = {conflictAreas->first.start.x, conflictAreas->first.start.y};
                    a.end = {conflictAreas->first.end.x, conflictAreas->first.end.y};
                    perceptionData->conflictPoints.push_back(a);

                    ConflictPoints b;
                    b.junctionOpenDriveRoadId = junctionLane->GetRoad()->GetOpenDriveId();
                    b.junctionOpenDriveLaneId = junctionLane->GetOpenDriveId();
                    b.currentOpenDriveRoadId = currentLane->GetRoad()->GetOpenDriveId();
                    b.currentOpenDriveLaneId = currentLane->GetOpenDriveId();
                    b.start = {conflictAreas->second.start.x, conflictAreas->second.start.y};
                    b.end = {conflictAreas->second.end.x, conflictAreas->second.end.y};
                    perceptionData->conflictPoints.push_back(b);
                }
            }
        }
    }
}

bool ConflictAreaCalculator::LanesDoNotIntersect(const MentalInfrastructure::Lane *laneA, const MentalInfrastructure::Lane *laneB) const {
    const auto &roadA = laneA->GetRoad();
    const auto &roadB = laneB->GetRoad();

    // lanes in row
    if (roadA == roadB) {
        return true;
    }
    if ((roadA->GetSuccessor() == roadB && roadA->GetSuccessor() != nullptr) ||
        (roadA->GetPredecessor() == roadB && roadA->GetPredecessor() != nullptr)) {
        return true;
    }
    if ((roadA->GetSuccessor() == roadB->GetJunction() && roadA->GetSuccessor() != nullptr) ||
        (roadA->GetPredecessor() == roadB->GetJunction() && roadA->GetPredecessor() != nullptr)) {
        return true;
    }
    // coming from same lane
    if (roadA->GetPredecessor() == roadB->GetPredecessor() && roadA->GetPredecessor() != nullptr) {
        return true;
    }
    // junction between lanes
    if ((roadA->IsSuccessorJunction() && roadB->IsSuccessorJunction()) ||
        (roadA->IsSuccessorJunction() && roadB->IsPredecessorJunction()) ||
        (roadA->IsPredecessorJunction() && roadB->IsSuccessorJunction()) ||
        (roadA->IsPredecessorJunction() && roadB->IsPredecessorJunction())) {
        return true;
    }
    return false;
}

bool ConflictAreaCalculator::LanesHavePotentialConfliceArea(const MentalInfrastructure::Lane *laneA,
                                                            const MentalInfrastructure::Lane *laneB) const {
    auto roadA = laneA->GetRoad();
    auto roadB = laneB->GetRoad();
    if ((roadA->GetJunction() == roadB->GetJunction() && roadA->GetJunction() != nullptr) &&
        !(roadA->GetPredecessor() == roadB->GetPredecessor()))
        return true;

    if (LanesPotentiallyIntersect(laneA->GetFirstPoint(), laneA->GetLastPoint(), laneB->GetFirstPoint(), laneB->GetLastPoint()))
        return true;

    return false;
}

bool ConflictAreaCalculator::LanesPotentiallyIntersect(const MentalInfrastructure::LanePoint *p1, const MentalInfrastructure::LanePoint *p2,
                                                       const MentalInfrastructure::LanePoint *q1,
                                                       const MentalInfrastructure::LanePoint *q2) const {
    double m1 = (p2->y - p1->y) / (p2->x - p1->x);
    double n1 = ((p1->y * p2->x) - (p2->y * p1->x)) / (p2->x - p1->x);
    double m2 = (q2->y - q1->y) / (q2->x - q1->x);
    double n2 = ((q1->y * q2->x) - (q2->y * q1->x)) / (q2->x - q1->x);

    if (m1 - m2 == 0.0)
        return false;

    double x = (n2 - n1) / (m1 - m2);
    double y = m1 * x + n1;
    if (((std::min(p1->x, p2->x) <= x && std::max(p1->x, p2->x) >= x) && ((std::min(p1->y, p2->y) <= y && std::max(p1->y, p2->y) >= y))) &&
        ((std::min(q1->x, q2->x) <= x && std::max(q1->x, q2->x) >= x) && ((std::min(q1->y, q2->y) <= y && std::max(q1->y, q2->y) >= y)))) {
        return true;
    }
    return false;
}

std::optional<std::pair<MentalInfrastructure::ConflictArea, MentalInfrastructure::ConflictArea>>
ConflictAreaCalculator::CalculateConflictAreas(const MentalInfrastructure::Lane *currentLane,
                                               const MentalInfrastructure::Lane *junctionLane) const {
    auto leftLeft = CalculateLaneJunctionPoints(currentLane->GetLeftSidePoints(), junctionLane->GetLeftSidePoints());
    auto leftRight = CalculateLaneJunctionPoints(currentLane->GetLeftSidePoints(), junctionLane->GetRightSidePoints());
    auto rightLeft = CalculateLaneJunctionPoints(currentLane->GetRightSidePoints(), junctionLane->GetLeftSidePoints());
    auto rightRight = CalculateLaneJunctionPoints(currentLane->GetRightSidePoints(), junctionLane->GetRightSidePoints());

    int junctionPointNumber = !leftLeft.first.empty();
    junctionPointNumber += !leftRight.first.empty();
    junctionPointNumber += !rightLeft.first.empty();
    junctionPointNumber += !rightRight.first.empty();
    // at least three junction points have to exist to define conflict area
    if (junctionPointNumber < 3)
        return std::nullopt;

    std::vector<MentalInfrastructure::LanePoint> conflictPointsCL;
    std::move(leftLeft.first.begin(), leftLeft.first.end(), std::back_inserter(conflictPointsCL));
    std::move(leftRight.first.begin(), leftRight.first.end(), std::back_inserter(conflictPointsCL));
    std::move(rightLeft.first.begin(), rightLeft.first.end(), std::back_inserter(conflictPointsCL));
    std::move(rightRight.first.begin(), rightRight.first.end(), std::back_inserter(conflictPointsCL));

    std::vector<MentalInfrastructure::LanePoint> conflictPointsIL;
    std::move(leftLeft.second.begin(), leftLeft.second.end(), std::back_inserter(conflictPointsIL));
    std::move(leftRight.second.begin(), leftRight.second.end(), std::back_inserter(conflictPointsIL));
    std::move(rightLeft.second.begin(), rightLeft.second.end(), std::back_inserter(conflictPointsIL));
    std::move(rightRight.second.begin(), rightRight.second.end(), std::back_inserter(conflictPointsIL));

    auto minPointCL =
        *std::min_element(conflictPointsCL.begin(), conflictPointsCL.end(), [](auto a, auto b) { return a.sOffset < b.sOffset; });
    auto maxPointCL =
        *std::max_element(conflictPointsCL.begin(), conflictPointsCL.end(), [](auto a, auto b) { return a.sOffset < b.sOffset; });

    auto minPointIL =
        *std::min_element(conflictPointsIL.begin(), conflictPointsIL.end(), [](auto a, auto b) { return a.sOffset < b.sOffset; });
    auto maxPointIL =
        *std::max_element(conflictPointsIL.begin(), conflictPointsIL.end(), [](auto a, auto b) { return a.sOffset < b.sOffset; });

    MentalInfrastructure::ConflictArea clConflictArea;
    clConflictArea.start = currentLane->InterpolatePoint(minPointCL.sOffset);
    clConflictArea.end = currentLane->InterpolatePoint(maxPointCL.sOffset);

    MentalInfrastructure::ConflictArea ilConflictArea;
    ilConflictArea.start = junctionLane->InterpolatePoint(minPointIL.sOffset);
    ilConflictArea.end = junctionLane->InterpolatePoint(maxPointIL.sOffset);
    std::pair<MentalInfrastructure::ConflictArea, MentalInfrastructure::ConflictArea> a{clConflictArea, ilConflictArea};
    return {{clConflictArea, ilConflictArea}};
}

std::pair<std::vector<MentalInfrastructure::LanePoint>, std::vector<MentalInfrastructure::LanePoint>>
ConflictAreaCalculator::CalculateLaneJunctionPoints(const std::list<MentalInfrastructure::LanePoint> &lanePointsA,
                                                    const std::list<MentalInfrastructure::LanePoint> &lanePointsB) const {
    auto chunksA = SplitListIntoChunks(lanePointsA, 8);
    auto chunksB = SplitListIntoChunks(lanePointsB, 8);

    std::vector<MentalInfrastructure::LanePoint> junctionPointA;
    std::vector<MentalInfrastructure::LanePoint> junctionPointB;
    for (auto a : chunksA) {
        for (auto b : chunksB) {
            if (auto result = CalculateChunkJunctionPoint(a, b, 0.5)) {
                junctionPointA.push_back(result->first);
                junctionPointB.push_back(result->second);
            }
        }
    }
    return {junctionPointA, junctionPointB};
}

std::vector<std::list<MentalInfrastructure::LanePoint>>
ConflictAreaCalculator::SplitListIntoChunks(const std::list<MentalInfrastructure::LanePoint> &lanePoints, unsigned int segments) const {
    unsigned int stride = static_cast<unsigned int>(lanePoints.size() / segments);
    auto rest = lanePoints.size() % segments;
    std::vector<std::list<MentalInfrastructure::LanePoint>> chunkList;
    for (auto start = lanePoints.begin(), end = std::next(lanePoints.begin(), stride); std::distance(start, lanePoints.end()) >= stride;
         start = std::next(start, stride), end = std::next(end, stride)) {
        chunkList.push_back({start, end});
    }
    if (rest) {
        chunkList.push_back({std::next(lanePoints.begin(), stride * segments), lanePoints.end()});
    }
    return chunkList;
}

std::optional<std::pair<MentalInfrastructure::LanePoint, MentalInfrastructure::LanePoint>>
ConflictAreaCalculator::CalculateChunkJunctionPoint(const std::list<MentalInfrastructure::LanePoint> &chunkA,
                                                    const std::list<MentalInfrastructure::LanePoint> &chunkB,
                                                    double junctionCondition) const {
    std::multimap<double, std::pair<MentalInfrastructure::LanePoint, MentalInfrastructure::LanePoint>> points;
    auto minDistanceOfClosestPoints = [&points, chunkB, junctionCondition](MentalInfrastructure::LanePoint pointA) {
        double minDistance = std::numeric_limits<double>::max();
        MentalInfrastructure::LanePoint junctionPointB;
        for (auto &pointB : chunkB) {
            double distance = std::sqrt((std::pow((pointA.x - pointB.x), 2)) + (std::pow((pointA.y - pointB.y), 2)));
            if (distance < minDistance) {
                minDistance = distance;
                junctionPointB = pointB;
            }
        }
        if (minDistance <= junctionCondition) {
            std::pair<MentalInfrastructure::LanePoint, MentalInfrastructure::LanePoint> closestPoints(pointA, junctionPointB);
            points.insert(std::make_pair(minDistance, closestPoints));
        }
    };

    std::for_each(chunkA.begin(), chunkA.end(), minDistanceOfClosestPoints);
    if (!points.empty()) {
        return points.begin()->second;
    }
    else {
        return std::nullopt;
    }
}

const MentalInfrastructure::Junction *RoadNetworkSensor::ConvertJunction(const OWL::Interfaces::Junction *junction) {
    OdId intersectionId = junction->GetId();
    auto iter = std::find_if(perceptionData->junctions.begin(), perceptionData->junctions.end(),
                             [intersectionId](auto element) { return element->GetOpenDriveId() == intersectionId; });
    if (iter != perceptionData->junctions.end()) {
        // if the intersection is already in the infrastructure result skip this
        return iter->get();
    }

    auto worldData = static_cast<OWL::WorldData *>(world->GetWorldData());
    auto newJunction = std::make_shared<MentalInfrastructure::Junction>(intersectionId);
    perceptionData->junctions.push_back(newJunction);

    for (auto connectionRoad : junction->GetConnectingRoads()) {
        auto from = ConvertRoad(const_cast<OWL::Road *>(worldData->GetRoads().at(connectionRoad->GetPredecessor())));
        auto to = ConvertRoad(const_cast<OWL::Road *>(worldData->GetRoads().at(connectionRoad->GetSuccessor())));
        auto with = ConvertRoad(const_cast<OWL::Road *>(connectionRoad));
        std::cout << from->GetOpenDriveId() << " | " << to->GetOpenDriveId() << " | " << with->GetOpenDriveId() << std::endl;
        newJunction->AddConnection(from, with, to);
    }
    return newJunction.get();
}

const MentalInfrastructure::Lane *RoadNetworkSensor::ConvertLane(const OWL::Lane *lane) {
    const OwlId laneId = lane->GetId();
    auto iter = std::find_if(perceptionData->lanes.begin(), perceptionData->lanes.end(),
                             [laneId](auto element) { return element->GetOwlId() == laneId; });
    if (iter != perceptionData->lanes.end()) {
        // if the lane is already in the infrastructure result skip this
        return iter->get();
    }

    // getting the OpenDrive id of the lane
    auto openDriveId = std::to_string(lane->GetOdId());

    auto newLane =
        std::make_shared<MentalInfrastructure::Lane>(openDriveId, laneId, lane->GetLength(), lane->GetLaneType(), lane->GetOdId() < 0);
    perceptionData->lanes.push_back(newLane);

    double width = lane->GetWidth(0);
    newLane->SetWidth(width);
    auto worldData = static_cast<OWL::WorldData *>(world->GetWorldData());

    // adding any successors and predecessors
    if (lane->GetOdId() < 0) {
        for (auto succLane : lane->GetNext()) {
            newLane->AddSuccessor(ConvertLane(const_cast<OWL::Lane *>(worldData->GetLanes().at(succLane))));
        }
        for (auto predLane : lane->GetPrevious()) {
            newLane->AddPredecessor(ConvertLane(const_cast<OWL::Lane *>(worldData->GetLanes().at(predLane))));
        }
    }
    else {
        for (auto succLane : lane->GetPrevious()) {
            newLane->AddSuccessor(ConvertLane(const_cast<OWL::Lane *>(worldData->GetLanes().at(succLane))));
        }
        for (auto predLane : lane->GetNext()) {
            newLane->AddPredecessor(ConvertLane(const_cast<OWL::Lane *>(worldData->GetLanes().at(predLane))));
        }
    }

    AddLaneGeometry(newLane.get(), lane);
    newLane->SetRoad(ConvertRoad(&lane->GetSection().GetRoad()));
    return newLane.get();
}

void RoadNetworkSensor::AddLaneGeometry(MentalInfrastructure::Lane *newLane, const OWL::Interfaces::Lane *lane) const {
    // adding all lane geometry
    auto referencePointType = [](const OWL::Primitive::LaneGeometryJoint::Points points) { return points.reference; };
    auto rightPointType = [](const OWL::Primitive::LaneGeometryJoint::Points points) { return points.right; };
    auto leftPointType = [](const OWL::Primitive::LaneGeometryJoint::Points points) { return points.left; };

    auto addRightPoint = [](MentalInfrastructure::Lane *newLane, double x, double y, double hdg, double so, bool inDirection) {
        newLane->AddRightPoint(x, y, hdg, so, inDirection);
    };
    auto addLeftPoint = [](MentalInfrastructure::Lane *newLane, double x, double y, double hdg, double so, bool inDirection) {
        newLane->AddLeftPoint(x, y, hdg, so, inDirection);
    };
    auto addReferencePoint = [](MentalInfrastructure::Lane *newLane, double x, double y, double hdg, double so, bool inDirection) {
        newLane->AddReferencePoint(x, y, hdg, so, inDirection);
    };

    auto addLaneGeometry = [=](auto pointType, auto addPoint) {
        std::for_each(lane->GetLaneGeometryElements().begin(), lane->GetLaneGeometryElements().end(),
                      [newLane, lane, pointType, addPoint](const OWL::Primitive::LaneGeometryElement *laneGeometry) {
                          double x = pointType(laneGeometry->joints.current.points).x;
                          double y = pointType(laneGeometry->joints.current.points).y;
                          double h = laneGeometry->joints.current.sHdg;
                          double s = laneGeometry->joints.current.sOffset;

                          addPoint(newLane, x, y, h, s, lane->GetOdId() < 0);
                      });
    };
    addLaneGeometry(referencePointType, addReferencePoint);
    addLaneGeometry(rightPointType, addRightPoint);
    addLaneGeometry(leftPointType, addLeftPoint);

    auto lastlaneGeometry = lane->GetLaneGeometryElements().back();
    // adding last lane point

    auto addLastPoint = [newLane, lane](const OWL::Primitive::LaneGeometryElement *laneGeometry, auto pointType, auto addPoint) {
        double x = pointType(laneGeometry->joints.next.points).x;
        double y = pointType(laneGeometry->joints.next.points).y;
        double h = laneGeometry->joints.next.sHdg;
        double s = laneGeometry->joints.next.sOffset;
        addPoint(newLane, x, y, h, s, lane->GetOdId() < 0);
    };
    addLastPoint(lastlaneGeometry, referencePointType, addReferencePoint);
    addLastPoint(lastlaneGeometry, rightPointType, addRightPoint);
    addLastPoint(lastlaneGeometry, leftPointType, addLeftPoint);
}

const MentalInfrastructure::Road *RoadNetworkSensor::ConvertRoad(const OWL::Interfaces::Road *road) {
    OdId openDriveIdRoad = road->GetId();

    auto iter = std::find_if(perceptionData->roads.begin(), perceptionData->roads.end(),
                             [openDriveIdRoad](auto element) { return element->GetOpenDriveId() == openDriveIdRoad; });
    if (iter != perceptionData->roads.end()) {
        // if the road is already in the infrastructure result return it
        return iter->get();
    }

    auto worldData = static_cast<OWL::WorldData *>(world->GetWorldData());

    double posXStart = maxDouble;
    double posYStart = maxDouble;
    double hdg = maxDouble;
    double length = road->GetLength();

    auto sections = road->GetSections();
    sections.sort([](auto sectionA, auto sectionB) { return sectionA->GetSOffset() < sectionB->GetSOffset(); });
    const auto lanes = sections.front()->GetLanes();

    for (const auto &lane : lanes) {
        auto openDriveIdLane = lane->GetOdId();

        if (openDriveIdLane == -1) {
            // Lane with OpenDrive Id = 0 is reference lane of road --> points of reference lane 0 are identical  with
            // left points of lane id -1
            OWL::Primitive::LaneGeometryJoint geometry = lane->GetLaneGeometryElements().front()->joints.current;
            posXStart = geometry.points.left.x;
            posYStart = geometry.points.left.y;
            hdg = geometry.sHdg;
        }
        if (openDriveIdLane == 1) {
            // Lane with OpenDrive Id = 0 is reference lane of road --> points of reference lane 0 are identical  with
            // right points of lane id 1
            OWL::Primitive::LaneGeometryJoint geometry = lane->GetLaneGeometryElements().front()->joints.current;
            posXStart = geometry.points.right.x;
            posYStart = geometry.points.right.y;
            hdg = geometry.sHdg;
        }
    }
    if (std::fabs(posXStart - maxDouble) < 0.001 || std::fabs(posYStart - maxDouble) < 0.001 || std::fabs(hdg - maxDouble) < 0.001) {
        std::string message =
            __FILE__ " Line: " + std::to_string(__LINE__) + "Road: " + openDriveIdRoad + " does not have a reference lane (laneid = 0)";
        throw std::runtime_error(message);
    }

    auto newRoad = std::make_shared<MentalInfrastructure::Road>(openDriveIdRoad, posXStart, posYStart, hdg, length);
    perceptionData->roads.push_back(newRoad);

    // FIXME re-implement Traffic Signs
    // the framework does not provide a way of directly finding out what road a traffic sign belongs to
    // using isValidForLane(OwlId) it would be possible to check if the sign is valid for any lane of the road and then assign it

    // convert traffic signs
    // for (auto &[key, value] : worldData->GetTrafficSigns()) {
    //     for (auto &section : sections) {
    //         for (auto &lane : section->GetLanes()) {
    //             if (value.is)
    //         }
    //     }

    //     if (value->GetRoadId() != roadId)
    //         continue;
    //     newRoad->AddTrafficSign(ConvertTrafficSign(newRoad.get(), value));
    // }

    const MentalInfrastructure::Section *lastSectionPtr = nullptr;

    int secCtr = 0;
    for (auto section : sections) {
        for (auto lane : section->GetLanes()) {
            auto newLane = ConvertLane(lane);
            newRoad->AddLane(newLane);
        }
    }

    auto isSuccJunction = worldData->GetJunctions().find(road->GetSuccessor()) != worldData->GetJunctions().end();
    auto isPredJunction = worldData->GetJunctions().find(road->GetPredecessor()) != worldData->GetJunctions().end();

    if (isSuccJunction) {
        newRoad->SetSuccessor(ConvertJunction(const_cast<OWL::Interfaces::Junction *>(worldData->GetJunctions().at(road->GetSuccessor()))));
    }
    else {
        auto succRoad = road->GetSuccessor();
        if (worldData->GetRoads().find(succRoad) != worldData->GetRoads().end()) {
            newRoad->SetSuccessor(ConvertRoad(const_cast<OWL::Interfaces::Road *>(worldData->GetRoads().at(succRoad))));
        }
    }
    if (isPredJunction) {
        newRoad->SetPredecessor(
            ConvertJunction(const_cast<OWL::Interfaces::Junction *>(worldData->GetJunctions().at(road->GetPredecessor()))));
    }
    else {
        auto predRoad = road->GetPredecessor();
        if (worldData->GetRoads().find(predRoad) != worldData->GetRoads().end()) {
            newRoad->SetPredecessor(ConvertRoad(const_cast<OWL::Interfaces::Road *>(worldData->GetRoads().at(predRoad))));
        }
    }

    return newRoad.get();
}

// FIXME re-implement traffic signs
// const MentalInfrastructure::TrafficSign* RoadNetworkSensor::ConvertTrafficSign(const MentalInfrastructure::Road* road,
//                                                                                const OWL::Interfaces::TrafficSign* sign) {
//     auto newSign = std::make_shared<MentalInfrastructure::TrafficSign>(
//         sign->GetId(), sign->GetOpenDriveId(), road, sign->GetValue(), sign->GetT(), sign->GetS(),
//         Common::Vector2d(sign->GetReferencePointPosition().x, sign->GetReferencePointPosition().y), sign->GetType());

//     perceptionData->lookupTableRoadNetwork.trafficSigns.insert(std::make_pair(sign->GetId(), newSign.get()));
//     perceptionData->trafficSigns.push_back(newSign);
//     return newSign.get();
// }

std::shared_ptr<InfrastructurePerception> RoadNetworkSensor::GetRoadNetwork() {
    if (infrastructureExists)
        return perceptionData;

    auto worldData = static_cast<OWL::WorldData *>(world->GetWorldData());

    // convert all roads
    for (auto &[key, value] : worldData->GetRoads()) {
        ConvertRoad(value);
    }

    // convert all intersections
    for (auto &[key, value] : worldData->GetJunctions()) {
        ConvertJunction(value);
    }

    conflictAreaCalculator.AssignPotentialConflictAreasToLanes(perceptionData);
    PrepareLookupTableRoadNetwork();
    // create the graph for the road network
    perceptionData->graph = CreateGraph(perceptionData->lanes);

    perceptionData->stoppingPointData = CreateStoppingPoints(perceptionData->junctions);

    infrastructureExists = true;
    return perceptionData;
}

void RoadNetworkSensor::PrepareLookupTableRoadNetwork() {
    for (const auto &lane : perceptionData->lanes) {
        perceptionData->lookupTableRoadNetwork.lanes.insert({lane->GetOwlId(), lane.get()});
    }
    for (const auto &road : perceptionData->roads) {
        perceptionData->lookupTableRoadNetwork.roads.insert({road->GetOpenDriveId(), road.get()});
    }
}

RoadmapGraph::RoadmapGraph RoadNetworkSensor::CreateGraph(std::vector<std::shared_ptr<const MentalInfrastructure::Lane>> &lanes) {
    RoadmapGraph::RoadmapGraph g(lanes);
    return g;
}

StoppingPointData RoadNetworkSensor::CreateStoppingPoints(std::vector<std::shared_ptr<const MentalInfrastructure::Junction>> &junctions) {
    StoppingPointData spData;

    for (auto junction : junctions) {
        auto junctionId = junction->GetOpenDriveId();
        std::map<OwlId, StoppingPointMap> tmp;
        spData.stoppingPoints.insert(std::make_pair(junctionId, tmp));
        for (auto road : junction->GetIncomingRoads()) {
            // TODO get last lanes of the road, not all of them (make method in road/lane)
            for (auto lane : road->GetLanes()) {
                std::map<StoppingPointType, StoppingPoint> sps = stoppingPointCalculation.DetermineStoppingPoints(junction.get(), lane);
                spData.stoppingPoints.at(junctionId).insert(std::make_pair(lane->GetOwlId(), sps));
            }
        }
    }
    return spData;
}
