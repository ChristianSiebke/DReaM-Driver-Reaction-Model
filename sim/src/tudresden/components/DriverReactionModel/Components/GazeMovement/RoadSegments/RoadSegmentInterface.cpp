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
        if (currentGazeState.fixationState.second == static_cast<int>(ScanAOI::ShoulderCheckLeft)) {
            ufovAngle = ShoulderCheckLeft(ScanAOI::ShoulderCheckLeft).ufovAngle;
        }
        else if (currentGazeState.fixationState.second == static_cast<int>(ScanAOI::ShoulderCheckRight)) {
            ufovAngle = ShoulderCheckRight(ScanAOI::ShoulderCheckRight).ufovAngle;
        }
        else {
            ufovAngle = ScanUFOVAngle(static_cast<ScanAOI>(currentGazeState.fixationState.second));
        }
        break;
    case GazeType::ObserveGlance:
        if (auto oAgent = Common::FindAgentById(currentGazeState.target.fixationAgent, *worldRepresentation.agentMemory)) {
            ufovAngle = CalculateGlobalViewingAngle(oAgent->GetRefPosition() - worldRepresentation.egoAgent->GetGlobalDriverPosition());
        } else {
            ufovAngle = currentGazeState.ufovAngle;
        }
        break;
    case GazeType::ControlGlance:
        ufovAngle =
            CalculateGlobalViewingAngle(currentGazeState.target.fixationPoint - worldRepresentation.egoAgent->GetGlobalDriverPosition());
        break;
    default:
        std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + " UFOVAngle cannot be updated";
        throw std::runtime_error(message);
    }
    return ufovAngle;
}

double RoadSegmentInterface::ScanUFOVAngle(ScanAOI aoi) {
    bool mirrorGaze = false;
    double ufovAngle;
    if (behaviourData.gmBehaviour.scanAOIs.driverAOIs.find(aoi) != behaviourData.gmBehaviour.scanAOIs.driverAOIs.end()) {
        ufovAngle = behaviourData.gmBehaviour.scanAOIs.driverAOIs.at(aoi).direction * (M_PI / 180);
    }
    else if (behaviourData.gmBehaviour.scanAOIs.mirrorAOIs.find(aoi) != behaviourData.gmBehaviour.scanAOIs.mirrorAOIs.end()) {
        ufovAngle = behaviourData.gmBehaviour.scanAOIs.mirrorAOIs.at(aoi).direction * (M_PI / 180);
        mirrorGaze = true;
    }
    else {
        ufovAngle = 0;
    }

    double foresightDistance = worldRepresentation.egoAgent->GetVelocity() * behaviourData.gmBehaviour.foresightTime;
    double minForesightDistance = behaviourData.gmBehaviour.minForesightDistance;
    if (foresightDistance < minForesightDistance) {
        foresightDistance = minForesightDistance;
    }

    Common::Vector2d headVector{1, 0};
    if (mirrorGaze) {
        headVector.Rotate(worldRepresentation.egoAgent->GetYawAngle());
    }
    else {
        // The agent aligns his head axis to the orientation point in
        // foresightDistance on his lane.
        headVector = CalculateForesightVector(foresightDistance);
    }

    // Determine whether the agent turn his head to the left or right
    double headAngle = CalculateGlobalViewingAngle(headVector);
    ufovAngle += headAngle;
    return ufovAngle;
}

Common::Vector2d RoadSegmentInterface::CalculateForesightVector(double foresightDistance) {
    Common::Vector2d headVector{1, 0};
    foresightDistance = worldRepresentation.egoAgent->IsMovingInLaneDirection() ? foresightDistance : -foresightDistance;
    if (auto position = worldRepresentation.egoAgent->FindNewPositionInDistance(foresightDistance)) {
        MentalInfrastructure::LanePoint foresightPoint((*position).lane->InterpolatePoint(position->sCoordinate));
        headVector = {foresightPoint.x, foresightPoint.y};
        headVector.Sub(worldRepresentation.egoAgent->GetGlobalDriverPosition());
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

    gazeState.fixationState = {GazeType::ObserveGlance, agentId};
    gazeState.target.fixationAgent = agentId;
    gazeState.openingAngle = behaviourData.gmBehaviour.observe_openingAngle;
    DReaM::NormalDistribution *de = behaviourData.gmBehaviour.observe_fixationDuration.get();
    double dist = stochastics->GetNormalDistributed(de->mean, de->std_deviation);
    gazeState.fixationDuration = Common::ValueInBounds(de->min, dist, de->max);
    return gazeState;
}

GazeState RoadSegmentInterface::ScanGlance(CrossingPhase phase) {
    AOIProbabilities aoiProbs = LookUpScanAOIProbability(phase);

    ScanAOI aoi;
    if (std::all_of(aoiProbs.begin(), aoiProbs.end(), [](auto element) { return element.second == 0; })) {
        aoi = ScanAOI::Other;
    }
    else {
        aoi = static_cast<ScanAOI>(Sampler::Sample(aoiProbs, stochastics));
    }

    GazeState gazeState;

    if (behaviourData.gmBehaviour.scanAOIs.driverAOIs.find(aoi) != behaviourData.gmBehaviour.scanAOIs.driverAOIs.end()) {
        gazeState.openingAngle = behaviourData.gmBehaviour.scanAOIs.driverAOIs.at(aoi).openingAngle;
        DReaM::NormalDistribution de = behaviourData.gmBehaviour.scanAOIs.driverAOIs.at(aoi).fixationDuration;
        double dist = stochastics->GetNormalDistributed(de.mean, de.std_deviation);
        gazeState.fixationDuration = Common::ValueInBounds(de.min, dist, de.max);
        gazeState.viewDistance = 100;
    }
    else if (behaviourData.gmBehaviour.scanAOIs.mirrorAOIs.find(aoi) != behaviourData.gmBehaviour.scanAOIs.mirrorAOIs.end()) {
        gazeState.openingAngle = behaviourData.gmBehaviour.scanAOIs.mirrorAOIs.at(aoi).openingAngle;
        gazeState.mirrorGaze = true;
        gazeState.mirrorPos = behaviourData.gmBehaviour.scanAOIs.mirrorAOIs.at(aoi).pos;
        DReaM::NormalDistribution de = behaviourData.gmBehaviour.scanAOIs.mirrorAOIs.at(aoi).fixationDuration;
        double dist = stochastics->GetNormalDistributed(de.mean, de.std_deviation);
        gazeState.fixationDuration = Common::ValueInBounds(de.min, dist, de.max);
        gazeState.viewDistance = 100;
    }
    else {
        std::string message =
            __FILE__ " Line: " + std::to_string(__LINE__) + " AOI: " + std::to_string(static_cast<int>(aoi)) + " does not exist";
        throw std::runtime_error(message);
    }

    gazeState.fixationState = {GazeType::ScanGlance, static_cast<int>(aoi)};

    if (aoi == ScanAOI::Other || aoi == ScanAOI::Dashboard) {
        // no agents visible, opening angle too
        gazeState.openingAngle = 0;
        gazeState.viewDistance = 0;
    }
    return gazeState;
}

GazeState RoadSegmentInterface::ShoulderCheckRight(ScanAOI gaze) {
    GazeState gazeState;
    gazeState.fixationState = {GazeType::ScanGlance, static_cast<int>(gaze)};
    gazeState.ufovAngle = worldRepresentation.egoAgent->GetYawAngle() + 110 * (M_PI / 180);
    gazeState.openingAngle = behaviourData.gmBehaviour.observe_openingAngle;
    DReaM::NormalDistribution *de = behaviourData.gmBehaviour.observe_fixationDuration.get();
    double dist = stochastics->GetNormalDistributed(de->mean, de->std_deviation);
    gazeState.fixationDuration = Common::ValueInBounds(de->min, dist, de->max);
    gazeState.viewDistance = 100;
    return gazeState;
}
GazeState RoadSegmentInterface::ShoulderCheckLeft(ScanAOI gaze) {
    GazeState gazeState;
    gazeState.fixationState = {GazeType::ScanGlance, static_cast<int>(gaze)};
    gazeState.ufovAngle = worldRepresentation.egoAgent->GetYawAngle() - 110 * (M_PI / 180);
    gazeState.openingAngle = behaviourData.gmBehaviour.observe_openingAngle;
    DReaM::NormalDistribution *de = behaviourData.gmBehaviour.observe_fixationDuration.get();
    double dist = stochastics->GetNormalDistributed(de->mean, de->std_deviation);
    gazeState.fixationDuration = Common::ValueInBounds(de->min, dist, de->max);
    gazeState.viewDistance = 100;
    return gazeState;
}

GazeState RoadSegmentInterface::MirrowGaze(ScanAOI aoi) {
    GazeState gazeState;
    if (behaviourData.gmBehaviour.scanAOIs.mirrorAOIs.find(aoi) != behaviourData.gmBehaviour.scanAOIs.mirrorAOIs.end()) {
        gazeState.fixationState = {GazeType::ScanGlance, static_cast<int>(aoi)};
        gazeState.openingAngle = behaviourData.gmBehaviour.scanAOIs.mirrorAOIs.at(aoi).openingAngle;
        gazeState.mirrorGaze = true;
        gazeState.mirrorPos = behaviourData.gmBehaviour.scanAOIs.mirrorAOIs.at(aoi).pos;
        DReaM::NormalDistribution de = behaviourData.gmBehaviour.scanAOIs.mirrorAOIs.at(aoi).fixationDuration;
        double dist = stochastics->GetNormalDistributed(de.mean, de.std_deviation);
        gazeState.fixationDuration = Common::ValueInBounds(de.min, dist, de.max);
        gazeState.viewDistance = 100;
    }
    else {
        std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + " AOI does not exist";
        throw std::runtime_error(message);
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
    if (sum == 0) {
        return aoiProbs;
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
        if (worldRepresentation.egoAgent->GetLanePosition().lane->GetRoad() == incomingRoad) {
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

    std::vector<const MentalInfrastructure::Lane *> sidewalkLanes;
    for (const auto &junctionRoad : junctionRoads) {
        for (auto lane : junctionRoad->GetLanes()) {
            if (lane->GetType() == MentalInfrastructure::LaneType::Sidewalk) {
                sidewalkLanes.push_back(lane);
            }
        }
    }
    return sidewalkLanes;
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
    auto conRoadsToCurrentRoad =
        currentJunction->PredecessorConnectionRoads(worldRepresentation.egoAgent->GetLanePosition().lane->GetRoad());
    for (auto conRoad : conRoadsToCurrentRoad) {
        for (auto conLane : conRoad->GetLanes()) {
            if (conLane->GetType() != MentalInfrastructure::LaneType::Sidewalk) {
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
                       result.Sub(worldRepresentation.egoAgent->GetGlobalDriverPosition());
                       return result;
                   });

    std::vector<double> viewAngles;
    std::transform(unsortedViewVec.begin(), unsortedViewVec.end(), std::back_inserter(viewAngles),
                   [this](Common::Vector2d element) { return CalculateGlobalViewingAngle(element); });

    std::map<double, Common::Vector2d> sortedFixationPointsByViewAngle;
    for (size_t i = 0; i < viewAngles.size(); i++) {
        if (!sortedFixationPointsByViewAngle.insert(std::make_pair(viewAngles.at(i), controlFixPointsOnXJunction.at(i))).second) {
            std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "view Angle is not unique!";
            throw std::runtime_error(message);
        }
    }
    std::transform(sortedFixationPointsByViewAngle.begin(), sortedFixationPointsByViewAngle.end(), controlFixPointsOnXJunction.begin(),
                   [](std::pair<double, Common::Vector2d> element) { return element.second; });
}

} // namespace Node
} // namespace RoadSegments
