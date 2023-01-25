/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/

#include "RoadSegmentInterface.h"

namespace RoadSegments {

double RoadSegmentInterface::UpdateUFOVAngle(GazeState currentGazeState) {
    double ufovAngle;
    switch (currentGazeState.fixationState.first) {
    case GazeType::ScanGlance:
        ufovAngle = ScanUFOVAngle(static_cast<ScanAOI>(currentGazeState.fixationState.second));
        break;
    case GazeType::ObserveGlance:
        if (auto oAgent = Common::FindAgentById(currentGazeState.target.fixationAgent, *worldRepresentation.agentMemory)) {
            ufovAngle = CalculateGlobalViewingAngle(oAgent->GetRefPosition() - worldRepresentation.egoAgent->GetDriverPosition());
        } else {
            ufovAngle = currentGazeState.ufovAngle;
        }
        break;
    case GazeType::ControlGlance:
        ufovAngle = CalculateGlobalViewingAngle(currentGazeState.target.fixationPoint - worldRepresentation.egoAgent->GetDriverPosition());
        break;
    default:
        std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + " UFOVAngle cannot be updated";
        throw std::runtime_error(message);
    }
    return ufovAngle;
}

double RoadSegmentInterface::ScanUFOVAngle(ScanAOI aoi) {
    double ufovAngle = 0;
    switch (aoi) {
    case ScanAOI::Left:
        ufovAngle += 30 * (M_PI / 180);
        break;
    case ScanAOI::Straight:
        ufovAngle += 0 * (M_PI / 180);
        break;
    case ScanAOI::Right:
        ufovAngle -= 30 * (M_PI / 180);
        break;
    case ScanAOI::Rear:
        ufovAngle += 180 * (M_PI / 180);
        break;
    case ScanAOI::Other:
        [[fallthrough]];
    case ScanAOI::Dashboard:
        ufovAngle += 0;
        break;
    default:
        std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "can not set Gaze State!";
        throw std::runtime_error(message);
    }
    // TODO: as input in BehaviourXML: look  2 seconds ahead
    double foresightDistance = worldRepresentation.egoAgent->GetVelocity() * 2;
    double minForesightDistance = 5;
    if (foresightDistance < minForesightDistance) {
        foresightDistance = minForesightDistance;
    }
    // The agent aligns his head axis to the orientation point in
    // foresightDistance on his lane.
    auto headVector = CalculateForesightVector(foresightDistance);

    // Determine whether the agent turn his head to the left or right
    double headAngle = CalculateGlobalViewingAngle(headVector);
    ufovAngle += headAngle;
    return ufovAngle;
}

Common::Vector2d RoadSegmentInterface::CalculateForesightVector(double foresightDistance) {
    Common::Vector2d headVector{1, 0};
    foresightDistance = worldRepresentation.egoAgent->IsMovingInLaneDirection() ? foresightDistance : -foresightDistance;
    if (auto position = worldRepresentation.egoAgent->FindNewPositionInDistance(foresightDistance)) {
        MentalInfrastructure::LanePoint foresightPoint((*position).newLane->InterpolatePoint(position->newSCoordinate));
        headVector = {foresightPoint.x, foresightPoint.y};
        headVector.Sub(worldRepresentation.egoAgent->GetDriverPosition());
    } else {
        // look straight ahead when agent is at the end of the road network
        headVector.Rotate(worldRepresentation.egoAgent->GetYawAngle());
    }
    return headVector;
}

double RoadSegmentInterface::CalculateGlobalViewingAngle(Common::Vector2d viewVector) const {
    Common::Vector2d forwardVectorVehicle{1, 0};
    forwardVectorVehicle.Rotate(worldRepresentation.egoAgent->GetYawAngle());
    // Determine whether the agent view to the left or right
    double crossProduct = forwardVectorVehicle.Cross(viewVector);
    double viewAngle = worldRepresentation.egoAgent->GetYawAngle();
    auto angle = forwardVectorVehicle.AngleBetween(viewVector);

    if (crossProduct > 0) {
        viewAngle += angle;
    } else {
        viewAngle -= angle;
    }
    return viewAngle;
}

GazeState RoadSegmentInterface::AgentObserveGlance(int agentId) {
    GazeState gazeState;
    ObservationAOI oAOI = Common::FindAgentById(agentId, *worldRepresentation.agentMemory)->GetVehicleType();

    gazeState.fixationState = {GazeType::ObserveGlance, static_cast<int>(oAOI)};
    gazeState.target.fixationAgent = agentId;
    gazeState.openingAngle = 100 * (M_PI / 180); // TODO replace by realistic behaviour
    gazeState.fixationDuration = 800;            // TODO realistic distribution
    return gazeState;
}

GazeState RoadSegmentInterface::ScanGlance(CrossingPhase phase) {
    AOIProbabilities aoiProbs = LookUpScanAOIProbability(phase);
    ScanAOI aoi;
    try {
        aoi = static_cast<ScanAOI>(Sampler::Sample(aoiProbs, stochastics));
    } catch (std::logic_error e) {
        // If all AOIs have a probability of 0%
        aoi = ScanAOI::Other;
    }
    GazeState gazeState;

    gazeState.fixationState = {GazeType::ScanGlance, static_cast<int>(aoi)};
    gazeState.openingAngle = 100 * (M_PI / 180); // TODO replace by realistic behaviour
    gazeState.fixationDuration = 400;            // TODO realistic distribution

    if (aoi == ScanAOI::Other || aoi == ScanAOI::Dashboard) {
        // no agents visible, opening angle too
        gazeState.openingAngle = 0;
        gazeState.viewDistance = 0;
    }
    return gazeState;
}

AOIProbabilities RoadSegmentInterface::ScaleProbabilitiesToOneAndEliminateNegativeProbabilities(AOIProbabilities aoiProbs) {
    // scale probabilities to one
    double sum = 0;
    for (auto& element : aoiProbs) {
        if (element.second < 0) {
            element.second = 0;
        }
        sum += element.second;
    }

    std::for_each(aoiProbs.begin(), aoiProbs.end(), [sum](std::pair<int, double> &element) { element.second = element.second / sum; });
    return aoiProbs;
}

namespace Node {

Junction::Junction(const WorldRepresentation &worldRepresentation, StochasticsInterface *stochastics, const BehaviourData &behaviourData) :
    RoadSegmentInterface(worldRepresentation, stochastics, behaviourData) {
    probabilityFixateLeadCar = behaviourData.gmBehaviour.XInt_probabilityFixateLeadCar;
    probabilityControlGlance = behaviourData.gmBehaviour.XInt_probabilityControlGlance;
    viewingDepthIntoRoad = behaviourData.gmBehaviour.XInt_viewingDepthIntoRoad;
    controlFixPointsOnRoads = CalculateControlFixPointsOnRoads();
    controlFixationPoints.insert(controlFixationPoints.begin(), controlFixPointsOnJunction.begin(), controlFixPointsOnJunction.end());
    controlFixationPoints.insert(controlFixationPoints.end(), controlFixPointsOnRoads.begin(), controlFixPointsOnRoads.end());
}

std::vector<Common::Vector2d> Junction::CalculateControlFixPointsOnRoads() const {
    std::vector<Common::Vector2d> controlFixPointsOnRoads;
    auto NextJunction = worldRepresentation.egoAgent->NextJunction();
    const auto &IncomingJunctionRoadIds = NextJunction->GetIncomingRoads();
    for (auto incomingRoad : IncomingJunctionRoadIds) {
        if (worldRepresentation.egoAgent->GetRoad() == incomingRoad) {
            continue;
        }
        auto conRoads = NextJunction->GetConnectionRoads(incomingRoad);
        auto conLane = conRoads.front()->GetLanes().front();
        Common::Vector2d startConLane{conLane->GetFirstPoint()->x, conLane->GetFirstPoint()->y};
        auto viewVector = Common::CreatPointInDistance(viewingDepthIntoRoad, startConLane, conLane->GetFirstPoint()->hdg + M_PI);
        controlFixPointsOnRoads.push_back(viewVector);
    }
    SortControlFixPoints(controlFixPointsOnRoads);
    return controlFixPointsOnRoads;

    // sketch of sorted junction fixation points
    //
    //                               x fixation points in roads
    //   ---------------------
    //                    x0         A   Agent
    //   -----         -------
    //    x2
    //   -----         -----
    //        |   |A  |
    //        |   |   |
}

std::vector<const MentalInfrastructure::Lane *>
Junction::SidewalkLanesOfJunction(const MentalInfrastructure::Junction *currentJunction) const {
    auto junctionRoads = currentJunction->GetAllRoadsOnJunction();

    std::vector<const MentalInfrastructure::Lane *> cornerSidewalkLanes;
    for (const auto &junctionRoad : junctionRoads) {
        for (auto lane : junctionRoad->GetLanes()) {
            if (lane->GetType() == LaneType::Sidewalk) {
                cornerSidewalkLanes.push_back(lane);
            }
        }
    }
    return cornerSidewalkLanes;
}

std::vector<const MentalInfrastructure::Lane *>
Junction::CornerSidewalkLanesOfJunction(std::vector<const MentalInfrastructure::Lane *> sidewalkLanes) const {
    std::vector<const MentalInfrastructure::Lane *> cornerSidewalkLanes;
    std::copy_if(sidewalkLanes.begin(), sidewalkLanes.end(), std::back_inserter(cornerSidewalkLanes),
                 [](const MentalInfrastructure::Lane *lane) {
                     double hdgFirstPoint = (lane->GetFirstPoint())->hdg;
                     double hdgLastPoint = (lane->GetLastPoint())->hdg;
                     return std::fabs(hdgFirstPoint - hdgLastPoint) > 50 * M_PI / 180;
                 });
    return cornerSidewalkLanes;
}

const MentalInfrastructure::Lane *Junction::OncomingStraightConnectionLane(const MentalInfrastructure::Junction *currentJunction) const {
    // first straight oncoming junction connection lane
    auto conRoadsToCurrentRoad = currentJunction->PredecessorConnectionRoads(worldRepresentation.egoAgent->GetRoad());
    for (auto conRoad : conRoadsToCurrentRoad) {
        for (auto conLane : conRoad->GetLanes()) {
            if (conLane->GetType() != LaneType::Sidewalk) {
                Common::Vector2d directionVec = {(conLane->GetFirstPoint())->x, (conLane->GetFirstPoint())->y};
                Common::Vector2d LaneEndPoint = {(conLane->GetLastPoint()->x), (conLane->GetLastPoint())->y};
                directionVec.Sub(LaneEndPoint);
                Common::Vector2d pastLaneEndPoint = Common::CreatPointInDistance(1, LaneEndPoint, (conLane->GetLastPoint())->hdg);
                Common::Vector2d straightVec = LaneEndPoint;
                straightVec.Sub(pastLaneEndPoint);

                auto cosAngle = straightVec.AngleBetween(directionVec);
                // find straight lane
                if (15 > cosAngle * (180 / M_PI)) {
                    return conLane;
                }
            }
        }
    }
    return nullptr;
}

void Junction::SortControlFixPoints(std::vector<Common::Vector2d> &controlFixPointsOnXJunction) const {
    std::vector<Common::Vector2d> unsortedViewVec;
    std::transform(controlFixPointsOnXJunction.begin(), controlFixPointsOnXJunction.end(), std::back_inserter(unsortedViewVec),
                   [this](const Common::Vector2d element) {
                       auto result = element;
                       result.Sub(worldRepresentation.egoAgent->GetDriverPosition());
                       return result;
                   });

    std::vector<double> viewAngles;
    std::transform(unsortedViewVec.begin(), unsortedViewVec.end(), std::back_inserter(viewAngles),
                   [this](Common::Vector2d element) { return CalculateGlobalViewingAngle(element); });

    std::map<double, Common::Vector2d> sortedFixationPointsByViewAngle;
    for (size_t i = 0; i < viewAngles.size(); ++i) {
        if (!sortedFixationPointsByViewAngle.insert(std::make_pair(viewAngles[i], controlFixPointsOnXJunction[i])).second) {
            std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "view Angle is not unique!";
            throw std::runtime_error(message);
        }
    }
    std::transform(sortedFixationPointsByViewAngle.begin(), sortedFixationPointsByViewAngle.end(), controlFixPointsOnXJunction.begin(),
                   [](std::pair<double, Common::Vector2d> element) { return element.second; });
}

} // namespace Node
} // namespace RoadSegments
