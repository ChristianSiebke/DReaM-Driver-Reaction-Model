/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#include "RoadNetworkConverter.h"

#include <algorithm>

namespace GlobalObserver::Converters {

const MentalInfrastructure::Junction *RoadNetworkConverter::ConvertJunction(const OWL::Interfaces::Junction *junction) {
    OdId intersectionId = junction->GetId();
    auto iter = std::find_if(infrastructurePerception->junctions.begin(), infrastructurePerception->junctions.end(),
                             [intersectionId](auto element) { return element->GetOpenDriveId() == intersectionId; });
    if (iter != infrastructurePerception->junctions.end()) {
        // if the intersection is already in the infrastructure result skip this
        return iter->get();
    }

    auto worldData = static_cast<OWL::WorldData *>(world->GetWorldData());
    auto newJunction = std::make_shared<MentalInfrastructure::Junction>(intersectionId, GenerateUniqueId());
    infrastructurePerception->junctions.push_back(newJunction);
    for (auto connectionRoad : junction->GetConnectingRoads()) {
        auto startLaneId = connectionRoad->GetSections().front()->GetLanes().front()->GetPrevious().front();
        auto startLane = const_cast<OWL::Interfaces::Lane *>(worldData->GetLanes().at(startLaneId));
        auto startRoad = &startLane->GetRoad();
        while (startRoad == connectionRoad) {
            startLane = const_cast<OWL::Interfaces::Lane *>(worldData->GetLanes().at(startLane->GetPrevious().front()));
            startRoad = &startLane->GetRoad();
        }

        auto endLaneId = connectionRoad->GetSections().front()->GetLanes().front()->GetNext().front();
        auto endLane = const_cast<OWL::Interfaces::Lane *>(worldData->GetLanes().at(endLaneId));
        auto endRoad = &endLane->GetRoad();
        while (endRoad == connectionRoad) {
            endLane = const_cast<OWL::Interfaces::Lane *>(worldData->GetLanes().at(endLane->GetNext().front()));
            endRoad = &endLane->GetRoad();
        }

        auto connectionLaneId = connectionRoad->GetSections().front()->GetLanes().front()->GetOdId();
        if (connectionLaneId > 0) {
            auto temp = startRoad;
            startRoad = endRoad;
            endRoad = temp;
        }
        auto from = ConvertRoad(const_cast<OWL::Road *>(startRoad));
        auto to = ConvertRoad(const_cast<OWL::Road *>(endRoad));
        auto with = const_cast<MentalInfrastructure::Road *>(ConvertRoad(const_cast<OWL::Road *>(connectionRoad)));
        with->SetOnJunction(newJunction.get());
        newJunction->AddConnection(from, with, to);
    }
    return newJunction.get();
}

MentalInfrastructure::Lane *RoadNetworkConverter::ConvertLane(const OWL::Lane *lane) {
    if (lane == nullptr)
        return nullptr;
    const OwlId laneId = lane->GetId();
    auto iter = std::find_if(infrastructurePerception->lanes.begin(), infrastructurePerception->lanes.end(),
                             [laneId](auto element) { return element->GetOwlId() == laneId; });
    if (iter != infrastructurePerception->lanes.end()) {
        // if the lane is already in the infrastructure result skip this
        return const_cast<MentalInfrastructure::Lane *>(iter->get());
    }

    // getting the OpenDrive id of the lane
    auto openDriveId = std::to_string(lane->GetOdId());

    auto newLane = std::make_shared<MentalInfrastructure::Lane>(openDriveId, GenerateUniqueId(), laneId, lane->GetLength(),
                                                                (MentalInfrastructure::LaneType)lane->GetLaneType(), lane->GetOdId() < 0);
    infrastructurePerception->lanes.push_back(newLane);

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
    auto road = ConvertRoad(&lane->GetSection().GetRoad());
    newLane->SetRoad(road);
    return newLane.get();
}

void RoadNetworkConverter::AddLaneGeometry(MentalInfrastructure::Lane *newLane, const OWL::Interfaces::Lane *lane) const {
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

const MentalInfrastructure::Road *RoadNetworkConverter::ConvertRoad(const OWL::Interfaces::Road *road) {
    OdId openDriveIdRoad = road->GetId();
    auto iter = std::find_if(infrastructurePerception->roads.begin(), infrastructurePerception->roads.end(),
                             [openDriveIdRoad](auto element) { return element->GetOpenDriveId() == openDriveIdRoad; });
    if (iter != infrastructurePerception->roads.end()) {
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
    infrastructurePerception->roads.push_back(newRoad);

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
            // convert traffic signs
            for (auto &value : lane->GetTrafficSigns()) {
                newRoad->AddTrafficSign(ConvertTrafficSign(newLane, newRoad.get(), value));
            }
            // convert traffic lights
            for (auto &value : lane->GetTrafficLights()) {
                newRoad->AddTrafficLight(ConvertTrafficLight(newLane, newRoad.get(), value));
            }
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

const MentalInfrastructure::TrafficSign *RoadNetworkConverter::ConvertTrafficSign(const MentalInfrastructure::Lane *lane,
                                                                                  const MentalInfrastructure::Road *road,
                                                                                  const OWL::Interfaces::TrafficSign *sign) {
    OdId openDriveIdSign = sign->GetId();
    auto iter = std::find_if(infrastructurePerception->trafficSigns.begin(), infrastructurePerception->trafficSigns.end(),
                             [openDriveIdSign](auto element) { return element->GetOpenDriveId() == openDriveIdSign; });
    if (iter != infrastructurePerception->trafficSigns.end()) {
        if (std::none_of(
                (*iter)->GetValidLanes().begin(), (*iter)->GetValidLanes().end(),
                [lane](const MentalInfrastructure::Lane *element) { return lane->GetOpenDriveId() == element->GetOpenDriveId(); })) {
            auto it = const_cast<MentalInfrastructure::TrafficSign *>(iter->get());
            it->AddValidLane(lane);
        }
        // if the road is already in the infrastructure result return it
        return iter->get();
    }

    auto newSign = std::make_shared<MentalInfrastructure::TrafficSign>(
        (OdId)sign->GetId(), GenerateUniqueId(), road, sign->GetS(),
        Common::Vector2d(sign->GetReferencePointPosition().x, sign->GetReferencePointPosition().y), sign->GetSpecification(0).value,
        sign->GetSpecification(0).type);

    newSign->AddValidLane(lane);
    infrastructurePerception->trafficSigns.push_back(newSign);
    return newSign.get();
}

const MentalInfrastructure::TrafficLight *RoadNetworkConverter::ConvertTrafficLight(const MentalInfrastructure::Lane *lane,
                                                                                    const MentalInfrastructure::Road *road,
                                                                                    const OWL::Interfaces::TrafficLight *trafficLight) {
    OdId openDriveIdTrafficLight = trafficLight->GetId();
    auto iter = std::find_if(infrastructurePerception->trafficLights.begin(), infrastructurePerception->trafficLights.end(),
                             [openDriveIdTrafficLight](auto element) { return element->GetOpenDriveId() == openDriveIdTrafficLight; });
    if (iter != infrastructurePerception->trafficLights.end()) {
        if (std::none_of((*iter)->GetValidLanes().begin(), (*iter)->GetValidLanes().end(),
                         [lane](auto element) { return lane->GetOpenDriveId() == element->GetOpenDriveId(); })) {
            auto it = const_cast<MentalInfrastructure::TrafficLight *>(iter->get());
            it->AddValidLane(lane);
        }
        // if the road is already in the infrastructure result return it
        return iter->get();
    }

    auto newLight = std::make_shared<MentalInfrastructure::TrafficLight>(
        (OdId)trafficLight->GetId(), GenerateUniqueId(), road, trafficLight->GetS(),
        Common::Vector2d(trafficLight->GetReferencePointPosition().x, trafficLight->GetReferencePointPosition().y),
        (MentalInfrastructure::TrafficLightType)trafficLight->GetSpecification(0).type);

    newLight->AddValidLane(lane);
    infrastructurePerception->trafficLights.push_back(newLight);
    return newLight.get();
}

void RoadNetworkConverter::UpdateTrafficLights() {
    auto worldData = static_cast<OWL::WorldData *>(world->GetWorldData());
    auto trafficLightLookup = infrastructurePerception->lookupTableRoadNetwork.trafficLights;

    // TODO maybe switch the for loops after extensive testing so the error message is not necessary?
    // should only be done if it is certain, that all traffic lights are converted correctly
    for (auto &[key, value] : worldData->GetTrafficLights()) {
        if (trafficLightLookup.find(value->GetId()) != trafficLightLookup.end()) {
            trafficLightLookup.at(value->GetId())->SetState((MentalInfrastructure::TrafficLightState)value->GetState());
        }
        else {
            // this case should not occur if all traffic lights are added correctly during scenery import
            std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + " The traffic light " + value->GetId() +
                                  " was never converted (is it not valid for a lane?). Skipping...";
            std::cerr << message << std::endl;
        }
    }
}

void RoadNetworkConverter::Populate() {
    if (infrastructureConverted) {
        UpdateTrafficLights();
        return;
    }

    auto worldData = static_cast<OWL::WorldData *>(world->GetWorldData());

    // convert all roads
    for (auto &[key, value] : worldData->GetRoads()) {
        ConvertRoad(value);
    }

    // add traffic signs
    for (auto sign : worldData->GetTrafficSigns()) {
        for (auto lane : infrastructurePerception->lanes) {
            if (sign.second->IsValidForLane(lane->GetOwlId())) {
            }
        }
    }

    // convert all intersections
    for (auto &[key, value] : worldData->GetJunctions()) {
        ConvertJunction(value);
    }

    PrepareLookupTableRoadNetwork();
    infrastructureConverted = true;
}

void RoadNetworkConverter::PrepareLookupTableRoadNetwork() {
    for (const auto &lane : infrastructurePerception->lanes) {
        infrastructurePerception->lookupTableRoadNetwork.lanes.insert({lane->GetOwlId(), lane.get()});
    }
    for (const auto &road : infrastructurePerception->roads) {
        infrastructurePerception->lookupTableRoadNetwork.roads.insert({road->GetOpenDriveId(), road.get()});
    }

    for (const auto &sign : infrastructurePerception->trafficSigns) {
        infrastructurePerception->lookupTableRoadNetwork.trafficSigns.insert({sign->GetOpenDriveId(), sign.get()});
    }
}
}