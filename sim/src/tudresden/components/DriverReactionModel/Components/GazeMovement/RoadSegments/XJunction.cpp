/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#include "XJunction.h"

#include "src/common/vector2d.h"

namespace RoadSegments {

namespace Node {

std::vector<Common::Vector2d> XJunction::CalculateControlFixPointsOnJunction() const {
    std::vector<Common::Vector2d> controlFixPointsOnXJunction;
    auto nextJunction = worldRepresentation.egoAgent->NextJunction();
    auto sidewalkLanes = SidewalkLanesOfJunction(nextJunction);
    auto cornerSidewalkLanes = CornerSidewalkLanesOfJunction(sidewalkLanes);

    for (auto sidewalkLane : cornerSidewalkLanes) {
        auto halfLength = sidewalkLane->GetLength() / 2;
        auto sCoordniate = sidewalkLane->GetFirstPoint()->sOffset;
        auto point = sidewalkLane->InterpolatePoint(sCoordniate + halfLength);
        controlFixPointsOnXJunction.push_back({point.x, point.y});
    }

    auto straightConLane = OncomingStraightConnectionLane(nextJunction);
    // calculate oncoming point
    Common::Vector2d startPoinStraightConLane = {(straightConLane->GetFirstPoint())->x, (straightConLane->GetFirstPoint())->y};
    double direction = (straightConLane->GetFirstPoint())->hdg;
    auto oncomingPoint = Common::CreatPointInDistance(viewingDepthIntoRoad, startPoinStraightConLane, direction + M_PI);
    controlFixPointsOnXJunction.push_back(oncomingPoint);

    // sketch of sorted junction fixation points
    //         |x2| |        x2  oncoming point
    //   -----x3    x1-----  A   Agent
    //   ------     -------
    //   -----x4    x0-----
    //         | |A |
    //
    SortControlFixPoints(controlFixPointsOnXJunction);
    return controlFixPointsOnXJunction;
};


GazeState XJunction::ControlGlance(CrossingPhase phase) {
    AOIProbabilities scaledAOIProbs = LookUpControlAOIProbability(phase);
    auto aoi = static_cast<ControlAOI>(Sampler::Sample(scaledAOIProbs, stochastics));

    if (worldRepresentation.egoAgent->GetJunctionDistance().on > 0 && (phase > CrossingPhase::Deceleration_TWO)) {
        // control gazes on junction
        return ControlGlanceOnXJunction(aoi, phase);
    }
    else if (worldRepresentation.egoAgent->GetJunctionDistance().toNext > 0 && phase < CrossingPhase::Deceleration_TWO) {
        // control gazes in front of junction
        return ControlGlanceOnRoad(aoi);
    }
    else {
        // control gazes while approaching an junction
        double rand = stochastics->GetUniformDistributed(0, 1);
        if (rand < 0.20 && controlFixPointsOnJunction.size() == 5) {
            return ControlGlanceOnXJunction(aoi, phase);
        }
        else {
            return ControlGlanceOnRoad(aoi);
        }
    }
}

GazeState XJunction::ControlGlanceOnRoad(ControlAOI aoi) {
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

const Common::Vector2d *XJunction::FixationPointForCGOnRoad(const std::vector<Common::Vector2d> &fixPoints, ControlAOI aoi) {
    const Common::Vector2d *fixPoint;
    if (aoi == ControlAOI::Right) {
        fixPoint = &fixPoints.at(0);
    }
    else if (aoi == ControlAOI::Oncoming) {
        fixPoint = &fixPoints.at(1);
    }
    else if (aoi == ControlAOI::Left) {
        fixPoint = &fixPoints.at(2);
    }
    else {
        std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "unknown ControlAOI";
        throw std::runtime_error(message);
    }
    return fixPoint;
}

GazeState XJunction::ControlGlanceOnXJunction(ControlAOI aoi, CrossingPhase phase) {
    GazeState gazeState;

    auto fixationPoint = FixationPointForCGOnXJunction(controlFixPointsOnJunction, phase, aoi);
    gazeState.fixationState = {GazeType::ControlGlance, static_cast<int>(aoi)};
    gazeState.target.fixationPoint = *fixationPoint;
    gazeState.openingAngle = behaviourData.gmBehaviour.XInt_controlOpeningAngle;
    DistributionEntry *de = behaviourData.gmBehaviour.XInt_controlFixationDuration.get();
    double dist = stochastics->GetNormalDistributed(de->mean, de->std_deviation);
    gazeState.fixationDuration = Common::ValueInBounds(de->min, dist, de->max);
    return gazeState;
}

const Common::Vector2d *XJunction::FixationPointForCGOnXJunction(const std::vector<Common::Vector2d> &fixPoints, CrossingPhase phase,
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

double XJunction::LookUpFixationDuration(CrossingPhase phase, ScanAOI aoi) {
}

AOIProbabilities XJunction::LookUpControlAOIProbability(CrossingPhase phase) {
    AOIProbabilities aoiProbs;

    auto &importedProbs = behaviourData.gmBehaviour.XInt_controlAOIProbabilities;

    if (importedProbs.find(phase) != importedProbs.end()) {
        for (auto &prob : importedProbs.at(phase)) {
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

AOIProbabilities XJunction::LookUpScanAOIProbability(CrossingPhase phase) {
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
