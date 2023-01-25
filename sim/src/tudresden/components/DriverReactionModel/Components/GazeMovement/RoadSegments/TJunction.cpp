/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 *****************************************************************************/
#include "TJunction.h"

#include "src/common/vector2d.h"

namespace RoadSegments {

namespace Node {

TJunction::TJunction(const WorldRepresentation &worldRepresentation, StochasticsInterface *stochastics, const BehaviourData &behaviourData,
                     TJunctionLayout layout) :
    layout{layout}, Junction(worldRepresentation, stochastics, behaviourData) {
    probabilityFixateLeadCar = behaviourData.gmBehaviour.XInt_probabilityFixateLeadCar;
    probabilityControlGlance = behaviourData.gmBehaviour.XInt_probabilityControlGlance;
    viewingDepthIntoRoad = behaviourData.gmBehaviour.XInt_viewingDepthIntoRoad;
    CalculateControlFixPointsOnTJunction();
    CalculateControlFixPointsOnRoads();

    controlFixationPoints.insert(controlFixationPoints.begin(), controlFixPointsOnTJunction.begin(), controlFixPointsOnTJunction.end());
    controlFixationPoints.insert(controlFixationPoints.end(), controlFixPointsOnRoads.begin(), controlFixPointsOnRoads.end());
}

void TJunction::CalculateControlFixPointsOnTJunction() {
    auto nextJunction = worldRepresentation.egoAgent->NextJunction();
    auto cornerSidewalkLanes = CornerSidewalkLanesOfJunction(nextJunction);

    for (auto sidewalkLane : cornerSidewalkLanes) {
        auto halfLength = sidewalkLane->GetLength() / 2;
        auto sCoordniate = sidewalkLane->GetFirstPoint()->sOffset;
        auto point = sidewalkLane->InterpolatePoint(sCoordniate + halfLength);
        controlFixPointsOnTJunction.push_back({point.x, point.y});
    }

    auto straightConLane = OncomingStraightConnectionLane(nextJunction);
    if (straightConLane) {
        if (layout == TJunctionLayout::LeftRight) {
            std::string message =
                __FILE__ " Line: " + std::to_string(__LINE__) + "invalid T-Junction Layout specified - unexpected oncoming lane found";
            throw std::runtime_error(message);
            return;
        }
        // calculate oncoming point
        Common::Vector2d startPoinStraightConLane = {(straightConLane->GetFirstPoint())->x, (straightConLane->GetFirstPoint())->y};
        double direction = (straightConLane->GetFirstPoint())->hdg;
        auto oncomingPoint = Common::CreatPointInDistance(viewingDepthIntoRoad, startPoinStraightConLane, direction + M_PI);
        auto oncomingPoint1 = Common::CreatPointInDistance(viewingDepthIntoRoad, startPoinStraightConLane, direction + M_PI - .1);
        auto oncomingPoint2 = Common::CreatPointInDistance(viewingDepthIntoRoad, startPoinStraightConLane, direction + M_PI + .1);
        controlFixPointsOnTJunction.push_back(oncomingPoint);
        controlFixPointsOnTJunction.push_back(oncomingPoint1);
        controlFixPointsOnTJunction.push_back(oncomingPoint2);
    }
    else {
        if (layout != TJunctionLayout::LeftRight) {
            std::string message =
                __FILE__ " Line: " + std::to_string(__LINE__) + "invalid T-Junction Layout specified - no oncoming lane found";
            throw std::runtime_error(message);
            return;
        }
        SortControlFixPoints(controlFixPointsOnTJunction);
        Common::Vector2d mid1 =
            controlFixPointsOnTJunction.at(0) + (controlFixPointsOnTJunction.at(2) - controlFixPointsOnTJunction.at(0)) * 0.5;
        Common::Vector2d mid2 =
            controlFixPointsOnTJunction.at(3) + (controlFixPointsOnTJunction.at(1) - controlFixPointsOnTJunction.at(3)) * 0.5;
        Common::Vector2d mid = {(mid1.x + mid2.x) / 2, (mid1.y + mid2.y) / 2};
        controlFixPointsOnTJunction.push_back(mid);
    }

    // sketch of sorted junction fixation points (Left-Right Layout)
    //                       x2  oncoming point
    //   -----x3----x1-----  A   Agent
    //   ------  x2 -------
    //   -----x4    x0-----
    //         | |A |
    //

    // sketch of sorted junction fixation points (Straight-Right Layout)
    //         |x2| |        x2  oncoming point [x2==x3==x4]
    //         |    x1-----  A   Agent
    //         |    -------
    //         |    x0-----
    //         | |A |
    //

    // sketch of sorted junction fixation points (Left-Straight Layout)
    //         |x2| |        x2  oncoming point [x0==x1==x2]
    //   -----x3    |        A   Agent
    //   ------     |
    //   -----x4    |
    //         | |A |
    //
    SortControlFixPoints(controlFixPointsOnTJunction);
};

void TJunction::CalculateControlFixPointsOnRoads() {
    auto NextJunction = worldRepresentation.egoAgent->NextJunction();
    const auto &IncomingJunctionRoadIds = NextJunction->GetIncomingRoads();
    for (auto incomingRoad : IncomingJunctionRoadIds) {
        if (worldRepresentation.egoAgent->GetLanePosition().lane->GetRoad() == incomingRoad) {
            continue;
        }
        auto conRoads = NextJunction->GetConnectionRoads(incomingRoad);
        auto conRoad = conRoads.front();
        auto startConRoad = conRoad->GetStartPosition();

        auto viewVector = Common::CreatPointInDistance(viewingDepthIntoRoad, startConRoad, conRoad->GetStartHeading() + M_PI);
        controlFixPointsOnRoads.push_back(viewVector);
    }

    // sketch of sorted junction fixation points
    //        | x1 |   |
    //        |    |   |             x fixation points in roads
    //   -----         -----
    //                    x0         A   Agent
    //   -----         -------
    //    x2
    //   -----         -----
    //        |   |A  |
    //        |   |   |
    SortControlFixPoints(controlFixPointsOnRoads);
    if (controlFixPointsOnRoads.size() != 2) {
        std::string message = __FILE__ " Line: " + std::to_string(__LINE__) +
                              "the number of control fixation points on incoming roads (T-Junction) is incorrect";
        throw std::runtime_error(message);
    }
}

GazeState TJunction::ControlGlance(CrossingPhase phase) {
    AOIProbabilities scaledAOIProbs = LookUpControlAOIProbability(phase);
    auto aoi = static_cast<ControlAOI>(Sampler::Sample(scaledAOIProbs, stochastics));

    if (worldRepresentation.egoAgent->GetJunctionDistance().on > 0 && (phase > CrossingPhase::Deceleration_TWO)) {
        // control gazes on junction
        return ControlGlanceOnTJunction(aoi, phase);
    }
    else if (worldRepresentation.egoAgent->GetJunctionDistance().toNext > 0 && phase < CrossingPhase::Deceleration_TWO) {
        // control gazes in front of junction
        return ControlGlanceOnRoad(aoi);
    }
    else {
        // control gazes while approaching an junction
        double rand = stochastics->GetUniformDistributed(0, 1);
        if (rand < 0.20) {
            return ControlGlanceOnTJunction(aoi, phase);
        }
        else {
            return ControlGlanceOnRoad(aoi);
        }
    }
}

GazeState TJunction::ControlGlanceOnRoad(ControlAOI aoi) {
    GazeState gazeState;

    auto fixationPoint = FixationPointForCGOnRoad(controlFixPointsOnRoads, aoi);
    gazeState.fixationState = {GazeType::ControlGlance, static_cast<int>(aoi)};
    gazeState.target.fixationPoint = *fixationPoint;
    gazeState.openingAngle = behaviourData.gmBehaviour.XInt_controlOpeningAngle;
    DistributionEntry *de = behaviourData.gmBehaviour.XInt_controlFixationDuration.get();
    double dist = stochastics->GetNormalDistributed(de->mean, de->std_deviation);
    gazeState.fixationDuration = Common::ValueInBounds(de->min, dist, de->max);
    return gazeState;
}

const Common::Vector2d *TJunction::FixationPointForCGOnRoad(const std::vector<Common::Vector2d> &fixPoints, ControlAOI aoi) {
    const Common::Vector2d *fixPoint;
    if (aoi == ControlAOI::Right) {
        if (layout == TJunctionLayout::LeftRight) {
            fixPoint = &fixPoints.at(0);
        }
        else if (layout == TJunctionLayout::StraightRight) {
            fixPoint = &fixPoints.at(0);
        }
        else {
            std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "invalid ControlAOI for T-Junction Layout";
            throw std::runtime_error(message);
        }
    }
    else if (aoi == ControlAOI::Oncoming) {
        if (layout == TJunctionLayout::LeftStraight) {
            fixPoint = &fixPoints.at(0);
        }
        else if (layout == TJunctionLayout::StraightRight) {
            fixPoint = &fixPoints.at(1);
        }
        else {
            std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "invalid ControlAOI for T-Junction Layout";
            throw std::runtime_error(message);
        }
    }
    else if (aoi == ControlAOI::Left) {
        if (layout == TJunctionLayout::LeftStraight) {
            fixPoint = &fixPoints.at(1);
        }
        else if (layout == TJunctionLayout::LeftRight) {
            fixPoint = &fixPoints.at(1);
        }
        else {
            std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "invalid ControlAOI for T-Junction Layout";
            throw std::runtime_error(message);
        }
    }
    else {
        std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "unknown ControlAOI";
        throw std::runtime_error(message);
    }
    return fixPoint;
}

GazeState TJunction::ControlGlanceOnTJunction(ControlAOI aoi, CrossingPhase phase) {
    GazeState gazeState;

    auto fixationPoint = FixationPointForCGOnTJunction(controlFixPointsOnTJunction, phase, aoi);
    gazeState.fixationState = {GazeType::ControlGlance, static_cast<int>(aoi)};
    gazeState.target.fixationPoint = *fixationPoint;
    gazeState.openingAngle = behaviourData.gmBehaviour.XInt_controlOpeningAngle;
    DistributionEntry *de = behaviourData.gmBehaviour.XInt_controlFixationDuration.get();
    double dist = stochastics->GetNormalDistributed(de->mean, de->std_deviation);
    gazeState.fixationDuration = Common::ValueInBounds(de->min, dist, de->max);
    return gazeState;
}

const Common::Vector2d *TJunction::FixationPointForCGOnTJunction(const std::vector<Common::Vector2d> &fixPoints, CrossingPhase phase,
                                                                 ControlAOI aoi) {
    const Common::Vector2d *fixPoint;
    switch (phase) {
    case CrossingPhase::Deceleration_ONE:
        [[fallthrough]];
    case CrossingPhase::Deceleration_TWO:
        if (aoi == ControlAOI::Right) {
            fixPoint = &fixPoints.at(0);
        }
        else if (aoi == ControlAOI::Oncoming) {
            fixPoint = &fixPoints.at(2);
        }
        else if (aoi == ControlAOI::Left) {
            fixPoint = &fixPoints.at(4);
        }
        else {
            std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "useful field of view angle can not be calculated!";
            throw std::runtime_error(message);
        }
        break;
    case CrossingPhase::Crossing_Straight:
        if (aoi == ControlAOI::Right) {
            fixPoint = &fixPoints.at(1);
        }
        else if (aoi == ControlAOI::Oncoming) {
            fixPoint = &fixPoints.at(2);
        }
        else if (aoi == ControlAOI::Left) {
            fixPoint = &fixPoints.at(3);
        }
        else {
            std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "useful field of view angle can not be calculated!";
            throw std::runtime_error(message);
        }
        break;
    case CrossingPhase::Crossing_Right:
        if (aoi == ControlAOI::Right) {
            fixPoint = &fixPoints.at(0);
        }
        else if (aoi == ControlAOI::Left) {
            fixPoint = &fixPoints.at(1);
        }
        else {
            std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "useful field of view angle can not be calculated!";
            throw std::runtime_error(message);
        }
        break;
    case CrossingPhase::Crossing_Left_ONE:
        if (aoi == ControlAOI::Right) {
            fixPoint = &fixPoints.at(3);
        }
        else if (aoi == ControlAOI::Oncoming) {
            fixPoint = &fixPoints.at(2);
        }
        else if (aoi == ControlAOI::Left) {
            fixPoint = &fixPoints.at(4);
        }
        else {
            std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "useful field of view angle can not be calculated!";
            throw std::runtime_error(message);
        }
        break;
    case CrossingPhase::Crossing_Left_TWO:
        if (aoi == ControlAOI::Right) {
            fixPoint = &fixPoints.at(3);
        }
        else if (aoi == ControlAOI::Left) {
            fixPoint = &fixPoints.at(4);
        }
        else {
            std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "useful field of view angle can not be calculated!";
            throw std::runtime_error(message);
        }
        break;
    default:
        std::string message = __FILE__ " Line: " + std::to_string(__LINE__) +
                              "Control fixation point is not defined: useful field of view angle can not be calculated!";
        throw std::runtime_error(message);
    }
    return fixPoint;
}

double TJunction::LookUpFixationDuration(CrossingPhase phase, ScanAOI aoi) {
}

AOIProbabilities TJunction::LookUpControlAOIProbability(CrossingPhase phase) {
    AOIProbabilities aoiProbs;

    auto &importedProbs = behaviourData.gmBehaviour.XInt_controlAOIProbabilities;

    if (importedProbs.find(phase) != importedProbs.end()) {
        for (auto &prob : importedProbs.at(phase)) {
            switch (layout) {
            case TJunctionLayout::LeftRight:
                if (prob.first == ControlAOI::Oncoming)
                    continue;
                break;
            case TJunctionLayout::StraightRight:
                if (prob.first == ControlAOI::Left)
                    continue;
                break;
            case TJunctionLayout::LeftStraight:
                if (prob.first == ControlAOI::Right)
                    continue;
                break;
            default:
                std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "Invalid T-Junction Layout specified!";
                throw std::runtime_error(message);
                break;
            }
            aoiProbs.push_back(std::make_pair(static_cast<int>(prob.first), prob.second));
        }
    }
    else {
        std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "Gaze probabilities can not calculated!";
        throw std::runtime_error(message);
    }
    AOIProbabilities scaledAOIProbs = ScaleProbabilitiesToOneAndEliminateNegativeProbabilities(aoiProbs);
    return scaledAOIProbs;
}

AOIProbabilities TJunction::LookUpScanAOIProbability(CrossingPhase phase) {
    // TODO move magic number to behaviour
    size_t UpperBoundaryModerateTrafficDensity = 4;
    TrafficDensity dens;
    if (worldRepresentation.agentMemory->size() == 0) {
        dens = TrafficDensity::LOW;
    }
    else if (0 < worldRepresentation.agentMemory->size() &&
             worldRepresentation.agentMemory->size() <= UpperBoundaryModerateTrafficDensity) {
        dens = TrafficDensity::MODERATE;
    }
    else {
        dens = TrafficDensity::HIGH;
    }

    IndicatorState ind = worldRepresentation.egoAgent->GetIndicatorState();
    if (ind == IndicatorState::IndicatorState_Warn) {
        std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "No Handling For Indicator State Warn!";
        throw std::runtime_error(message);
    }

    AOIProbabilities aoiProbs;
    try {
        for (auto &prob : behaviourData.gmBehaviour.XInt_scanAOIProbabilities.at(ind).at(dens).at(phase)) {
            DistributionEntry *de = prob.second.get();
            double dist = stochastics->GetNormalDistributed(de->mean, de->std_deviation);
            double value = Common::ValueInBounds(de->min, dist, de->max);
            aoiProbs.push_back(std::make_pair(static_cast<int>(prob.first), value));
        }
    }
    catch (const std::out_of_range &oor) {
        std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "Gaze probabilities can not calculated!";
        throw std::runtime_error(message);
    }

    AOIProbabilities scaledAOIProbs = ScaleProbabilitiesToOneAndEliminateNegativeProbabilities(aoiProbs);

    return scaledAOIProbs;
}
} // namespace Node
} // namespace RoadSegments
