#include "roadnetworksensor.h"

#include <algorithm>

std::shared_ptr<RoadNetworkSensor> RoadNetworkSensor::instance = nullptr;

void ConflictAreaCalculator::AssignPotentialConflictAreasToLanes(std::shared_ptr<InfrastructurePerception> perceptionData) const {
    for (const auto &currentLane : perceptionData->lanes) {
        for (const auto &intersectionLane : perceptionData->lanes) {
            if (currentLane == intersectionLane)
                continue;
            if (currentLane->GetConflictAreaWithLane(intersectionLane.get()))
                continue;
            if (currentLane->GetType() == LaneType::Shoulder || intersectionLane->GetType() == LaneType::Shoulder)
                continue;
            if (LanesDoNotIntersect(currentLane.get(), intersectionLane.get()))
                continue;
            if (LanesHavePotentialConfliceArea(currentLane.get(), intersectionLane.get())) {
                if (auto conflictAreas = CalculateConflictAreas(currentLane.get(), intersectionLane.get())) {
                    const_cast<MentalInfrastructure::Lane *>(currentLane.get())
                        ->AddConflictArea({intersectionLane.get(), conflictAreas->first});
                    const_cast<MentalInfrastructure::Lane *>(intersectionLane.get())
                        ->AddConflictArea({currentLane.get(), conflictAreas->second});

                    ConflictPoint a;
                    a.junctionOpenDriveRoadId = currentLane->GetRoad()->GetOpenDriveId();
                    a.junctionOpenDriveLaneId = currentLane->GetOpenDriveId();
                    a.currentOpenDriveRoadId = intersectionLane->GetRoad()->GetOpenDriveId();
                    a.currentOpenDriveLaneId = intersectionLane->GetOpenDriveId();
                    a.currentStartS = conflictAreas->first.start.sOffset;
                    a.currentEndS = conflictAreas->first.end.sOffset;
                    a.otherStartS = conflictAreas->second.start.sOffset;
                    a.otherEndS = conflictAreas->second.end.sOffset;
                    perceptionData->conflictPoints.push_back(a);

                    std::cout << "Current Lane: " << currentLane->GetOpenDriveId()
                              << " - Other Lane: " << intersectionLane->GetOpenDriveId() << std::endl;

                    ConflictPoint b;
                    b.junctionOpenDriveRoadId = intersectionLane->GetRoad()->GetOpenDriveId();
                    b.junctionOpenDriveLaneId = intersectionLane->GetOpenDriveId();
                    b.currentOpenDriveRoadId = currentLane->GetRoad()->GetOpenDriveId();
                    b.currentOpenDriveLaneId = currentLane->GetOpenDriveId();
                    b.currentStartS = conflictAreas->second.start.sOffset;
                    b.currentEndS = conflictAreas->second.end.sOffset;
                    b.otherStartS = conflictAreas->first.start.sOffset;
                    b.otherEndS = conflictAreas->first.end.sOffset;
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

    if (IntersectionPoints(laneA->GetFirstPoint(), laneA->GetLastPoint(), laneB->GetFirstPoint(), laneB->GetLastPoint()))
        return true;

    return false;
}

std::optional<std::pair<MentalInfrastructure::LanePoint, MentalInfrastructure::LanePoint>>
ConflictAreaCalculator::IntersectionPoints(const MentalInfrastructure::LanePoint *p1, const MentalInfrastructure::LanePoint *p2,
                                           const MentalInfrastructure::LanePoint *q1, const MentalInfrastructure::LanePoint *q2) const {
    double threshold = 0.3;
    double m1 = (p2->y - p1->y) / (p2->x - p1->x);
    double n1 = ((p1->y * p2->x) - (p2->y * p1->x)) / (p2->x - p1->x);
    double m2 = (q2->y - q1->y) / (q2->x - q1->x);
    double n2 = ((q1->y * q2->x) - (q2->y * q1->x)) / (q2->x - q1->x);

    if (m1 - m2 == 0.0)
        return std::nullopt;

    double x = (n2 - n1) / (m1 - m2);
    double y = m1 * x + n1;
    if (((std::min(p1->x, p2->x) <= x + threshold && std::max(p1->x, p2->x) >= x - threshold) &&
         ((std::min(p1->y, p2->y) <= y + threshold && std::max(p1->y, p2->y) >= y - threshold))) &&
        ((std::min(q1->x, q2->x) <= x + threshold && std::max(q1->x, q2->x) >= x - threshold) &&
         ((std::min(q1->y, q2->y) <= y + threshold && std::max(q1->y, q2->y) >= y - threshold)))) {
        auto pS = std::sqrt((std::pow((x - p1->x), 2)) + (std::pow((y - p1->y), 2)));
        auto qS = std::sqrt((std::pow((x - q1->x), 2)) + (std::pow((y - q1->y), 2)));
        return {{{x, y, p1->hdg, p1->sOffset + pS}, {x, y, q1->hdg, q1->sOffset + qS}}};
    }
    return std::nullopt;
}

std::optional<std::pair<MentalInfrastructure::ConflictArea, MentalInfrastructure::ConflictArea>>
ConflictAreaCalculator::CalculateConflictAreas(const MentalInfrastructure::Lane *currentLane,
                                               const MentalInfrastructure::Lane *intersectionLane) const {
    auto leftLeft = CalculateLaneIntersectionPoints(currentLane->GetLeftSidePoints(), intersectionLane->GetLeftSidePoints());
    auto leftRight = CalculateLaneIntersectionPoints(currentLane->GetLeftSidePoints(), intersectionLane->GetRightSidePoints());
    auto rightLeft = CalculateLaneIntersectionPoints(currentLane->GetRightSidePoints(), intersectionLane->GetLeftSidePoints());
    auto rightRight = CalculateLaneIntersectionPoints(currentLane->GetRightSidePoints(), intersectionLane->GetRightSidePoints());

    int intersectionPointNumber = !leftLeft.first.empty();
    intersectionPointNumber += !leftRight.first.empty();
    intersectionPointNumber += !rightLeft.first.empty();
    intersectionPointNumber += !rightRight.first.empty();
    // at least three intersection points have to exist to define conflict area
    if (intersectionPointNumber < 3)
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
    clConflictArea.start = minPointCL.sOffset <= currentLane->GetFirstPoint()->sOffset ? *currentLane->GetFirstPoint()
                                                                                       : currentLane->InterpolatePoint(minPointCL.sOffset);
    clConflictArea.end = maxPointCL.sOffset >= currentLane->GetLastPoint()->sOffset ? *currentLane->GetLastPoint()
                                                                                    : currentLane->InterpolatePoint(maxPointCL.sOffset);
    MentalInfrastructure::ConflictArea ilConflictArea;
    ilConflictArea.start = minPointIL.sOffset <= intersectionLane->GetFirstPoint()->sOffset
                               ? *intersectionLane->GetFirstPoint()
                               : intersectionLane->InterpolatePoint(minPointIL.sOffset);
    ilConflictArea.end = maxPointIL.sOffset >= intersectionLane->GetLastPoint()->sOffset
                             ? *intersectionLane->GetLastPoint()
                             : intersectionLane->InterpolatePoint(maxPointIL.sOffset);
    return {{clConflictArea, ilConflictArea}};
}

std::pair<std::vector<MentalInfrastructure::LanePoint>, std::vector<MentalInfrastructure::LanePoint>>
ConflictAreaCalculator::CalculateLaneIntersectionPoints(const std::list<MentalInfrastructure::LanePoint> &lanePointsA,
                                                        const std::list<MentalInfrastructure::LanePoint> &lanePointsB) const {
    std::vector<MentalInfrastructure::LanePoint> intersectionPointA;
    std::vector<MentalInfrastructure::LanePoint> intersectionPointB;
    for (auto pA1 = lanePointsA.begin(), pA2 = std::next(pA1); pA2 != lanePointsA.end(); pA1++, pA2++) {
        for (auto pB1 = lanePointsB.begin(), pB2 = std::next(pB1); pB2 != lanePointsB.end(); pB1++, pB2++) {
            if (auto result = IntersectionPoints(&(*pA1), &(*pA2), &(*pB1), &(*pB2))) {
                intersectionPointA.push_back(result->first);
                intersectionPointB.push_back(result->second);
            }
        }
    }
    return {intersectionPointA, intersectionPointB};
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
    auto newJunction = std::make_shared<MentalInfrastructure::Junction>(intersectionId, GenerateUniqueId());
    perceptionData->junctions.push_back(newJunction);
    for (auto connectionRoad : junction->GetConnectingRoads()) {
        auto startLaneId = connectionRoad->GetSections().front()->GetLanes().front()->GetPrevious().front();
        auto startLane = const_cast<OWL::Interfaces::Lane *>(worldData->GetLanes().at(startLaneId));
        auto startRoad = &startLane->GetRoad();
        while (startRoad == connectionRoad) {
            startLane = const_cast<OWL::Interfaces::Lane *>(worldData->GetLanes().at(startLane->GetPrevious().front()));
            startRoad = &startLane->GetRoad();
        }
        auto from = ConvertRoad(const_cast<OWL::Road *>(startRoad));

        auto endLaneId = connectionRoad->GetSections().front()->GetLanes().front()->GetNext().front();
        auto endLane = const_cast<OWL::Interfaces::Lane *>(worldData->GetLanes().at(endLaneId));
        auto endRoad = &endLane->GetRoad();
        while (endRoad == connectionRoad) {
            endLane = const_cast<OWL::Interfaces::Lane *>(worldData->GetLanes().at(endLane->GetNext().front()));
            endRoad = &endLane->GetRoad();
        }
        auto to = ConvertRoad(const_cast<OWL::Road *>(endRoad));
        auto with = const_cast<MentalInfrastructure::Road *>(ConvertRoad(const_cast<OWL::Road *>(connectionRoad)));
        with->SetOnJunction(newJunction.get());
        newJunction->AddConnection(from, with, to);
    }
    return newJunction.get();
}

MentalInfrastructure::Lane *RoadNetworkSensor::ConvertLane(const OWL::Lane *lane) {
    if (lane == nullptr)
        return nullptr;
    const OwlId laneId = lane->GetId();
    auto iter = std::find_if(perceptionData->lanes.begin(), perceptionData->lanes.end(),
                             [laneId](auto element) { return element->GetOwlId() == laneId; });
    if (iter != perceptionData->lanes.end()) {
        // if the lane is already in the infrastructure result skip this
        return const_cast<MentalInfrastructure::Lane *>(iter->get());
    }

    // getting the OpenDrive id of the lane
    auto openDriveId = std::to_string(lane->GetOdId());

    auto newLane = std::make_shared<MentalInfrastructure::Lane>(openDriveId, GenerateUniqueId(), laneId, lane->GetLength(),
                                                                lane->GetLaneType(), lane->GetOdId() < 0);
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

    auto newRoad = std::make_shared<MentalInfrastructure::Road>(openDriveIdRoad, GenerateUniqueId(), posXStart, posYStart, hdg, length);
    perceptionData->roads.push_back(newRoad);

    // FIXME re-implement Traffic Signs
    // the framework does not provide a way of directly finding out what road a traffic sign belongs to
    // using isValidForLane(OwlId) it would be possible to check if the sign is valid for any lane of the road and then assign it

    // convert traffic signs
    for (auto &[key, value] : worldData->GetTrafficSigns()) {
        for (auto &section : sections) {
            for (auto &lane : section->GetLanes()) {
                if (value->IsValidForLane(lane->GetId())) {
                    newRoad->AddTrafficSign(ConvertTrafficSign(newRoad.get(), value));
                }
            }
        } 
    }

    const MentalInfrastructure::Section *lastSectionPtr = nullptr;

    int secCtr = 0;
    for (auto section : sections) {
        for (auto lane : section->GetLanes()) {
            auto newLane = ConvertLane(lane);

            // add neighbor lanes
            auto leftLane = &lane->GetLeftLane();
            if (auto leftLaneInvalid = dynamic_cast<const OWL::Implementation::InvalidLane *>(leftLane)) {
                leftLane = nullptr;
            }

            auto rightLane = &lane->GetRightLane();
            if (auto rightLaneInvalid = dynamic_cast<const OWL::Implementation::InvalidLane *>(rightLane)) {
                rightLane = nullptr;
            }
            newLane->AddLeftLane(ConvertLane(leftLane));
            newLane->AddRightLane(ConvertLane(rightLane));
            //------------

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

const MentalInfrastructure::TrafficSign *RoadNetworkSensor::ConvertTrafficSign(const MentalInfrastructure::Road *road,
                                                                               const OWL::Interfaces::TrafficSign *sign) {
    // TODO fix t value being -42
    auto newSign = std::make_shared<MentalInfrastructure::TrafficSign>(
        (OdId)sign->GetId(), GenerateUniqueId(), road, sign->GetSpecification(0).value, -42, sign->GetS(),
        Common::Vector2d(sign->GetReferencePointPosition().x, sign->GetReferencePointPosition().y), sign->GetSpecification(0).type);

    perceptionData->lookupTableRoadNetwork.trafficSigns.insert(std::make_pair(sign->GetId(), newSign.get()));
    perceptionData->trafficSigns.push_back(newSign);
    return newSign.get();
}

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
        std::unordered_map<OwlId, StoppingPointMap> tmp;
        spData.stoppingPoints.insert(std::make_pair(junctionId, tmp));
        for (auto road : junction->GetIncomingRoads()) {
            // TODO get last lanes of the road, not all of them (make method in road/lane)
            for (auto lane : road->GetLanes()) {
                if (road->IsPredecessorJunction() && road->GetPredecessor()->GetOpenDriveId() == junctionId && lane->IsInRoadDirection() ||
                    road->IsSuccessorJunction() && road->GetSuccessor()->GetOpenDriveId() == junctionId && !lane->IsInRoadDirection()) {
                    continue;
                }
                if (lane->GetType() != LaneType::Driving && lane->GetType() != LaneType::Sidewalk && lane->GetType() != LaneType::Biking) {
                    continue;
                }
                std::unordered_map<StoppingPointType, StoppingPoint> sps =
                    stoppingPointCalculation.DetermineStoppingPoints(junction.get(), lane);
                // std::cout << "roadnetwork_sps: " << sps.begin()->second.road->GetOpenDriveId();
                spData.stoppingPoints.at(junctionId).insert(std::make_pair(lane->GetOwlId(), sps));
            }
        }
    }
    return spData;
}
