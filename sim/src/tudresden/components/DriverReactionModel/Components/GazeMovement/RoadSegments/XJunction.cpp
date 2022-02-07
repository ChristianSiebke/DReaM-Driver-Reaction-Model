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
#include "XJunction.h"

#include "src/common/vector2d.h"

namespace RoadSegments {

namespace Node {

XJunction::XJunction(const WorldRepresentation &worldRepresentation, StochasticsInterface *stochastics,
                     const BehaviourData &behaviourData) :
    Junction(worldRepresentation, stochastics, behaviourData) {
    probabilityFixateLeadCar = behaviourData.gmBehaviour.XInt_probabilityFixateLeadCar;
    probabilityControlGlance = behaviourData.gmBehaviour.XInt_probabilityControlGlance;
    viewingDepthIntoRoad = behaviourData.gmBehaviour.XInt_viewingDepthIntoRoad;
    CalculateControlFixPointsOnXJunction();
    CalculateControlFixPointsOnRoads();

    controlFixationPoints.insert(controlFixationPoints.begin(), controlFixPointsOnXJunction.begin(), controlFixPointsOnXJunction.end());
    controlFixationPoints.insert(controlFixationPoints.end(), controlFixPointsOnRoads.begin(), controlFixPointsOnRoads.end());
}

void XJunction::CalculateControlFixPointsOnXJunction() {
    auto nextJunction = worldRepresentation.egoAgent->NextJunction();
    auto cornerSidewalkLanes = CornerSidewalkLanesOfJunction(nextJunction);

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

    if (controlFixPointsOnXJunction.size() != 5) {
        std::string message =
            __FILE__ " Line: " + std::to_string(__LINE__) + "the number of control fixation points on X-Junction is incorrect";
        throw std::runtime_error(message);
    }
};

void XJunction::CalculateControlFixPointsOnRoads() {
    auto NextJunction = worldRepresentation.egoAgent->NextJunction();
    const auto &IncomingJunctionRoadIds = NextJunction->GetIncomingRoads();
    for (auto incomingRoad : IncomingJunctionRoadIds) {
        if (worldRepresentation.egoAgent->GetRoad() == incomingRoad) {
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
    if (controlFixPointsOnRoads.size() != 3) {
        std::string message = __FILE__ " Line: " + std::to_string(__LINE__) +
                              "the number of control fixation points on incoming roads (X-Junction) is incorrect";
        throw std::runtime_error(message);
    }
}

GazeState XJunction::ControlGlance(CrossingPhase phase) {
    AOIProbabilities scaledAOIProbs = LookUpControlAOIProbability(phase);
    auto aoi = static_cast<ControlAOI>(Sampler::Sample(scaledAOIProbs, stochastics));

    if (worldRepresentation.egoAgent->GetDistanceOnJunction() > 0 && (phase > CrossingPhase::Deceleration_TWO)) {
        // control gazes on junction
        return ControlGlanceOnXJunction(aoi, phase);
    }
    else if (worldRepresentation.egoAgent->GetDistanceToNextJunction() > 0 && phase < CrossingPhase::Deceleration_TWO) {
        // control gazes in front of junction
        return ControlGlanceOnRoad(aoi);
    }
    else {
        // control gazes while approaching an junction
        double rand = stochastics->GetUniformDistributed(0, 1);
        if (rand < 0.20) {
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
    gazeState.openingAngle = 100 * (M_PI / 180); // TODO replace by realistic behaviour
    gazeState.fixationDuration = 800;            // TODO realistic distribution
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

    auto fixationPoint = FixationPointForCGOnXJunction(controlFixPointsOnXJunction, phase, aoi);
    gazeState.fixationState = {GazeType::ControlGlance, static_cast<int>(aoi)};
    gazeState.target.fixationPoint = *fixationPoint;
    gazeState.openingAngle = 100 * (M_PI / 180); // TODO replace by realistic behaviour
    gazeState.fixationDuration = 800;            // TODO realistic distribution
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

    //    // probabilities fictional
    //    if (phase == CrossingPhase::Approach) {
    //        aoiProbs.push_back(std::make_pair(static_cast<int>(ControlAOI::Left), 0.20));
    //        aoiProbs.push_back(std::make_pair(static_cast<int>(ControlAOI::Oncoming), 0.3));
    //        aoiProbs.push_back(std::make_pair(static_cast<int>(ControlAOI::Right), 0.5));

    //    } else if (phase == CrossingPhase::Deceleration_ONE) {
    //        aoiProbs.push_back(std::make_pair(static_cast<int>(ControlAOI::Left), 0.20));
    //        aoiProbs.push_back(std::make_pair(static_cast<int>(ControlAOI::Oncoming), 0.3));
    //        aoiProbs.push_back(std::make_pair(static_cast<int>(ControlAOI::Right), 0.5));

    //    } else if (phase == CrossingPhase::Deceleration_TWO) {
    //        aoiProbs.push_back(std::make_pair(static_cast<int>(ControlAOI::Left), 0.20));
    //        aoiProbs.push_back(std::make_pair(static_cast<int>(ControlAOI::Oncoming), 0.3));
    //        aoiProbs.push_back(std::make_pair(static_cast<int>(ControlAOI::Right), 0.5));

    //    } else if (phase == CrossingPhase::Crossing_Straight) {
    //        aoiProbs.push_back(std::make_pair(static_cast<int>(ControlAOI::Left), 0.20));
    //        aoiProbs.push_back(std::make_pair(static_cast<int>(ControlAOI::Oncoming), 0.4));
    //        aoiProbs.push_back(std::make_pair(static_cast<int>(ControlAOI::Right), 0.4));

    //    } else if (phase == CrossingPhase::Crossing_Right) {
    //        // turning right do not have oncoming viewing point
    //        aoiProbs.push_back(std::make_pair(static_cast<int>(ControlAOI::Left), 0.40));
    //        aoiProbs.push_back(std::make_pair(static_cast<int>(ControlAOI::Right), 0.6));

    //    } else if (phase == CrossingPhase::Crossing_Left_ONE) {
    //        aoiProbs.push_back(std::make_pair(static_cast<int>(ControlAOI::Left), 0.20));
    //        aoiProbs.push_back(std::make_pair(static_cast<int>(ControlAOI::Oncoming), 0.5));
    //        aoiProbs.push_back(std::make_pair(static_cast<int>(ControlAOI::Right), 0.3));

    //    } else if (phase == CrossingPhase::Crossing_Left_TWO) {
    //        // turning left do not have oncoming viewing point
    //        aoiProbs.push_back(std::make_pair(static_cast<int>(ControlAOI::Left), 0.40));
    //        aoiProbs.push_back(std::make_pair(static_cast<int>(ControlAOI::Right), 0.6));

    //    } else {
    //        std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "Gaze probabilities can not calculated!";
    //        throw std::runtime_error(message);
    //    }

    AOIProbabilities scaledAOIProbs = ScaleProbabilitiesToOneAndEliminateNegativeProbabilities(aoiProbs);
    return scaledAOIProbs;
}

AOIProbabilities XJunction::LookUpScanAOIProbability(CrossingPhase phase) {
    size_t UpperBoundaryModerateTrafficDensity = 4;
    // TODO detect the traffic density in perceptionInterface and hand over by egoPerceptionData
    // consider traffic density by enum {no traffic, low, moderate, hight}

    AOIProbabilities aoiProbs;
    // TODO set all of this by Behaviour XML

    // Use of probability distributions for gaze states of the AutoDrive study of TU Dresden
    //(use values of "give priority" for unregulated junction (priority to the right)
    if (worldRepresentation.egoAgent->GetIndicatorState() == IndicatorState::IndicatorState_Left) {
        if (worldRepresentation.agentMemory->size() == 0) {
            // no traffic
            switch (phase) {
            case CrossingPhase::Approach:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.0252873563218391, 0.0949750060746114)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.754022988505747, 0.260077969987969)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left),
                                                  stochastics->GetNormalDistributed(0.122742200328407, 0.232515910096245)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.0864532019704433, 0.13733384909178)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.0114942528735632, 0.0618984460590173)));
                break;
            case CrossingPhase::Deceleration_ONE:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.00985221674876848, 0.0530558109077291)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.475322091701402, 0.256365384269271)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left),
                                                  stochastics->GetNormalDistributed(0.235464822533788, 0.253776666497274)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.244960212201592, 0.230286434051837)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.0344006568144499, 0.0855241329149771)));
                break;
            case CrossingPhase::Deceleration_TWO:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.00766283524904214, 0.0286534127530626)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.570462973140108, 0.234204642796636)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left),
                                                  stochastics->GetNormalDistributed(0.310757234108287, 0.227917594461821)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.093540565074803, 0.101018900532748)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.0175763924277598, 0.0464703612231769)));
                break;
            case CrossingPhase::Crossing_Right:
                [[fallthrough]];
            case CrossingPhase::Crossing_Left_ONE:
                [[fallthrough]];
            case CrossingPhase::Crossing_Left_TWO:
                [[fallthrough]];
            case CrossingPhase::Crossing_Straight:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard), 0));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.845566502463054, 0.280914707016578)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left),
                                                  stochastics->GetNormalDistributed(0.149507389162562, 0.282373738959805)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right), 0));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.00492610837438424, 0.00265279054538646)));
                break;
            case CrossingPhase::Exit:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.00793650793650794, 0.0363696483726654)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.964047022317699, 0.0974814269631686)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left), 0));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.00680272108843537, 0.0311739843194275)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.0212137486573577, 0.0617364400470868)));
                break;
            default:
                std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "Gaze probabilities can not calculated!";
                throw std::runtime_error(message);
            }
        }
        else if (0 < worldRepresentation.agentMemory->size() &&
                 worldRepresentation.agentMemory->size() <= UpperBoundaryModerateTrafficDensity) {
            // moderate traffic density
            switch (phase) {
            case CrossingPhase::Approach:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.0643707482993197, 0.129147384174803)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.771641156462585, 0.187375011831164)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left),
                                                  stochastics->GetNormalDistributed(0.0619047619047619, 0.132315337805032)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.102083333333333, 0.173199512088384)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other), 0));
                break;
            case CrossingPhase::Deceleration_ONE:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.0320436507936508, 0.0884553539773954)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.581964365000079, 0.272480960575393)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left),
                                                  stochastics->GetNormalDistributed(0.223455810062953, 0.245463030927821)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.154294415901559, 0.171076283518907)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.00824175824175824, 0.0436112853472185)));
                break;
            case CrossingPhase::Deceleration_TWO:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.0793650793650794, 0.0419960525565808)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.542800575036702, 0.244116866910891)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left),
                                                  stochastics->GetNormalDistributed(0.305460526687164, 0.224687365987702)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.112273111961608, 0.124394319101615)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.0315292783780179, 0.0699571660571938)));
                break;
            case CrossingPhase::Crossing_Right:
                [[fallthrough]];
            case CrossingPhase::Crossing_Left_ONE:
                [[fallthrough]];
            case CrossingPhase::Crossing_Left_TWO:
                [[fallthrough]];
            case CrossingPhase::Crossing_Straight:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard), 0));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.79937641723356, 0.33396550990062)));
                aoiProbs.push_back(
                    std::make_pair(static_cast<int>(ScanAOI::Left), stochastics->GetNormalDistributed(0.20062358276644, 0.33396550990062)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right), 0));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other), 0));
                break;
            case CrossingPhase::Exit:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.340360122968819, 0.0775092467587676)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.955819060166886, 0.0993734335492598)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left), 0));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right), 0));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.0101449275362319, 0.0339857109140429)));
                break;
            default:
                std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "Gaze probabilities can not calculated!";
                throw std::runtime_error(message);
            }
        }
        else {
            // high traffic density
            switch (phase) {
            case CrossingPhase::Approach:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.0141414141414141, 0.0625261837851948)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.715992710992711, 0.287550800646518)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left),
                                                  stochastics->GetNormalDistributed(0.198475968475968, 0.272950668983285)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.0713899063899064, 0.139228920533852)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other), 0));
                break;
            case CrossingPhase::Deceleration_ONE:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.0158284600389864, 0.0540231734235259)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.570601678736354, 0.25742044711622)));
                aoiProbs.push_back(
                    std::make_pair(static_cast<int>(ScanAOI::Left), stochastics->GetNormalDistributed(0.35403634606421, 0.24493051076891)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.0544457958622045, 0.104187579947713)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.00508771929824561, 0.0203364260183212)));
                break;
            case CrossingPhase::Deceleration_TWO:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.00277777777777778, 0.0152145154862546)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.423369414629236, 0.148700855712445)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left),
                                                  stochastics->GetNormalDistributed(0.440102396394014, 0.19351122092014)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.0797072422744819, 0.0726282555831004)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.0540431689244899, 0.0846043518945876)));
                break;
            case CrossingPhase::Crossing_Right:
                [[fallthrough]];
            case CrossingPhase::Crossing_Left_ONE:
                [[fallthrough]];
            case CrossingPhase::Crossing_Left_TWO:
                [[fallthrough]];
            case CrossingPhase::Crossing_Straight:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.0076628352490421, 0.0412656307060115)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.829652435686918, 0.273375648235423)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left),
                                                  stochastics->GetNormalDistributed(0.147167487684729, 0.277367524308031)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.00689655172413793, 0.0371390676354104)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.00862068965517241, 0.046423834544263)));
                break;
            case CrossingPhase::Exit:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.0126262626262626, 0.0428102925813755)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.966540404040404, 0.101955409343189)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left), 0));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right), 0));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.0208333333333333, 0.0954703269782467)));
                break;
            default:
                std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "Gaze probabilities can not calculated!";
                throw std::runtime_error(message);
            }
        }
    }
    else if (worldRepresentation.egoAgent->GetIndicatorState() == IndicatorState::IndicatorState_Off) {
        if (worldRepresentation.agentMemory->size() == 0) {
            // no traffic
            switch (phase) {
            case CrossingPhase::Approach:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.0128205128205128, 0.0653720450460614)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.581440781440782, 0.308863771383145)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left),
                                                  stochastics->GetNormalDistributed(0.163827838827839, 0.198461222393695)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.187881562881563, 0.220783615269782)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.054029304029304, 0.114768364233879)));
                break;
            case CrossingPhase::Deceleration_ONE:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.00699300699300699, 0.0356574791160335)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.511386493623336, 0.258611632229936)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left),
                                                  stochastics->GetNormalDistributed(0.211261983630405, 0.190460062180369)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.235559980954718, 0.232301848842389)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.0347985347985348, 0.106528560515371)));
                break;
            case CrossingPhase::Deceleration_TWO:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.0896964146964147, 0.144344860703903)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.558101620601621, 0.283367019401666)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left),
                                                  stochastics->GetNormalDistributed(0.143130480630481, 0.125405501443032)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.192637917637918, 0.207260148578553)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.164335664335664, 0.0454195669611354)));
                break;
            case CrossingPhase::Crossing_Right:
                [[fallthrough]];
            case CrossingPhase::Crossing_Left_ONE:
                [[fallthrough]];
            case CrossingPhase::Crossing_Left_TWO:
                [[fallthrough]];
            case CrossingPhase::Crossing_Straight:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.11121031746031, 0.181056523326269)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.681150793650794, 0.312733727376373)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left),
                                                  stochastics->GetNormalDistributed(0.0208333333333333, 0.0747378671910734)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.152777777777778, 0.267601020099199)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.0340277777777778, 0.787092685349135)));
                break;
            case CrossingPhase::Exit:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.163344226579521, 0.276535343226154)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.676933551198257, 0.349260810865339)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left),
                                                  stochastics->GetNormalDistributed(0.0722222222222222, 0.25755771942611)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.0652777777777778, 0.194617978119719)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.0222222222222222, 0.0860662965823871)));
                break;
            default:
                std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "Gaze probabilities can not calculated!";
                throw std::runtime_error(message);
            }
        }
        else if (0 < worldRepresentation.agentMemory->size() &&
                 worldRepresentation.agentMemory->size() <= UpperBoundaryModerateTrafficDensity) {
            // moderate traffic density
            switch (phase) {
            case CrossingPhase::Approach:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.0312037037037037, 0.0997860720824183)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.693405483405483, 0.223918281796641)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left),
                                                  stochastics->GetNormalDistributed(0.123603896103896, 0.165191253763552)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.141998556998557, 0.198184361640043)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.00978835978835979, 0.0421861217763402)));
                break;
            case CrossingPhase::Deceleration_ONE:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.0261904761904762, 0.103566474033582)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.437525252525253, 0.276580118205618)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left),
                                                  stochastics->GetNormalDistributed(0.308771783771784, 0.256817118387149)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.201059496059496, 0.230301714368495)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.0264529914529915, 0.073937403200756)));
                break;
            case CrossingPhase::Deceleration_TWO:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.0270457612562876, 0.0739733845277923)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.508408045533949, 0.186355545245287)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left),
                                                  stochastics->GetNormalDistributed(0.202078804313066, 0.154703578254376)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.233018112109959, 0.14610621716356)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.0294492767867381, 0.0551679147849017)));
                break;
            case CrossingPhase::Crossing_Right:
                [[fallthrough]];
            case CrossingPhase::Crossing_Left_ONE:
                [[fallthrough]];
            case CrossingPhase::Crossing_Left_TWO:
                [[fallthrough]];
            case CrossingPhase::Crossing_Straight:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.139444444444444, 0.280615201732136)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.617936507936508, 0.318193433742147)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left),
                                                  stochastics->GetNormalDistributed(0.00476190476190476, 0.0260820265478651)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.140634920634921, 0.221580745892426)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.0972222222222222, 0.22292393997516)));
                break;
            case CrossingPhase::Exit:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.14290293040293, 0.251526912723083)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.626963776963777, 0.397871390479016)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left), 0));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.146825396825397, 0.332757606048437)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.0833078958078958, 0.237806895757703)));
                break;
            default:
                std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "Gaze probabilities can not calculated!";
                throw std::runtime_error(message);
            }
        }
        else {
            // high traffic density
            switch (phase) {
            case CrossingPhase::Approach:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.0246913580246914, 0.0889600854472706)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.641108171663727, 0.222794110279573)));
                aoiProbs.push_back(
                    std::make_pair(static_cast<int>(ScanAOI::Left), stochastics->GetNormalDistributed(0.166446208112875, 0.197453245947)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.167754262198707, 0.193488235699029)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other), 0));
                break;
            case CrossingPhase::Deceleration_ONE:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard), 0));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.525962617629284, 0.225110962420586)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left),
                                                  stochastics->GetNormalDistributed(0.368508137952582, 0.217863398223177)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.0980470147136814, 0.142027668831093)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.00748222970445193, 0.0271027817659767)));
                break;
            case CrossingPhase::Deceleration_TWO:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard), 0));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.455726578073885, 0.169025087205105)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left),
                                                  stochastics->GetNormalDistributed(0.373555894268087, 0.152712738775313)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.129531831139336, 0.0811594497255141)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.0411856965186918, 0.0624594915276029)));
                break;
            case CrossingPhase::Crossing_Right:
                [[fallthrough]];
            case CrossingPhase::Crossing_Left_ONE:
                [[fallthrough]];
            case CrossingPhase::Crossing_Left_TWO:
                [[fallthrough]];
            case CrossingPhase::Crossing_Straight:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.111111111111111, 0.200053411671402)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.691093474426808, 0.232076483976835)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left),
                                                  stochastics->GetNormalDistributed(0.154320987654321, 0.56788316995093)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.118165784832451, 0.197802276313063)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.0641975308641975, 0.143424516591414)));
                break;
            case CrossingPhase::Exit:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.129844758416187, 0.25232572533672)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.694575266003837, 0.391018093947938)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left), 0));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.15018315018315, 0.357072407816372)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.0253968253968254, 0.0829355585880199)));
                break;
            default:
                std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "Gaze probabilities can not calculated!";
                throw std::runtime_error(message);
            }
        }
    }
    else if (worldRepresentation.egoAgent->GetIndicatorState() == IndicatorState::IndicatorState_Right) {
        if (worldRepresentation.agentMemory->size() == 0) {
            // no traffic
            switch (phase) {
            case CrossingPhase::Approach:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.0206043956043956, 0.072964260837054)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.449450549450549, 0.271551607613394)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left),
                                                  stochastics->GetNormalDistributed(0.166758241758242, 0.178761778888799)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.299786324786325, 0.269569756744311)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.0634004884004884, 0.127889299045834)));
                break;
            case CrossingPhase::Deceleration_ONE:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.00480769230769231, 0.024514516892273)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.491912040950502, 0.256131318927636)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left),
                                                  stochastics->GetNormalDistributed(0.236492673992674, 0.217189336456956)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.25076195172349, 0.247522115510086)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.016025641025641, 0.0457604315322429)));
                break;
            case CrossingPhase::Deceleration_TWO:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard), 0));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.534384235046, 0.284645253482729)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left),
                                                  stochastics->GetNormalDistributed(0.199221214306056, 0.193558510028301)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.202420744162826, 0.273173504387984)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.0639738064851187, 0.0964158642148807)));
                break;
            case CrossingPhase::Crossing_Right:
                [[fallthrough]];
            case CrossingPhase::Crossing_Left_ONE:
                [[fallthrough]];
            case CrossingPhase::Crossing_Left_TWO:
                [[fallthrough]];
            case CrossingPhase::Crossing_Straight:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard), 0));
                aoiProbs.push_back(
                    std::make_pair(static_cast<int>(ScanAOI::Straight), stochastics->GetNormalDistributed(0.7625, 0.409388182792965)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left), 0));
                aoiProbs.push_back(
                    std::make_pair(static_cast<int>(ScanAOI::Right), stochastics->GetNormalDistributed(0.2375, 0.409388182792965)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other), 0));
                break;
            case CrossingPhase::Exit:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.0243827160493827, 0.0767776870216157)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.929392212725546, 0.140907782109786)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left), 0));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.0305555555555556, 0.0930920506703658)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.0156695156695157, 0.042969750617166)));
                break;
            default:
                std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "Gaze probabilities can not calculated!";
                throw std::runtime_error(message);
            }
        }
        else if (0 < worldRepresentation.agentMemory->size() &&
                 worldRepresentation.agentMemory->size() <= UpperBoundaryModerateTrafficDensity) {
            // moderate traffic density
            switch (phase) {
            case CrossingPhase::Approach:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.0142857142857143, 0.0782460796435952)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.599867724867725, 0.244167587914613)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left),
                                                  stochastics->GetNormalDistributed(0.170544733044733, 0.208545708017924)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.209746272246272, 0.257212994542772)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.00555555555555556, 0.0304290309725092)));
                break;
            case CrossingPhase::Deceleration_ONE:
                aoiProbs.push_back(
                    std::make_pair(static_cast<int>(ScanAOI::Dashboard), stochastics->GetNormalDistributed(0.015, 0.0574606312002491)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.483882691382691, 0.228450207041309)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left),
                                                  stochastics->GetNormalDistributed(0.351683686683687, 0.241359911023428)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.121933621933622, 0.1730183432942)));
                aoiProbs.push_back(
                    std::make_pair(static_cast<int>(ScanAOI::Other), stochastics->GetNormalDistributed(0.0275, 0.0723560137121063)));
                break;
            case CrossingPhase::Deceleration_TWO:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.00196078431372549, 0.0107396579902974)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.570296465517054, 0.240531183910221)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left),
                                                  stochastics->GetNormalDistributed(0.242954031262855, 0.171215780540306)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.142004207665972, 0.192714601857278)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.427845112403936, 0.613380139884025)));
                break;
            case CrossingPhase::Crossing_Right:
                [[fallthrough]];
            case CrossingPhase::Crossing_Left_ONE:
                [[fallthrough]];
            case CrossingPhase::Crossing_Left_TWO:
                [[fallthrough]];
            case CrossingPhase::Crossing_Straight:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard), 0));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.920454545454546, 0.209681783225082)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left), 0));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.0795454545454546, 0.209681783225082)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other), 0));
                break;
            case CrossingPhase::Exit:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard), 0));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.953517316017316, 0.133615987956564)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left), 0));
                aoiProbs.push_back(
                    std::make_pair(static_cast<int>(ScanAOI::Right), stochastics->GetNormalDistributed(0.0125, 0.0503222374920561)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.033982683982684, 0.0986623024605446)));
                break;
            default:
                std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "Gaze probabilities can not calculated!";
                throw std::runtime_error(message);
            }
        }
        else {
            // high traffic density
            switch (phase) {
            case CrossingPhase::Approach:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.0358974358974359, 0.105376816730595)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.643467643467643, 0.200931103927912)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left),
                                                  stochastics->GetNormalDistributed(0.175549450549451, 0.168867557315995)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.14508547008547, 0.173377373792104)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other), 0));
                break;
            case CrossingPhase::Deceleration_ONE:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.0153846153846154, 0.0784464540552736)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.537025687987226, 0.237655815384674)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left),
                                                  stochastics->GetNormalDistributed(0.343478103093488, 0.255586886953026)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.10411159353467, 0.173222534627468)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other), 0));
                break;
            case CrossingPhase::Deceleration_TWO:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
                                                  stochastics->GetNormalDistributed(0.00153846153846154, 0.00784464540552736)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.423445588346797, 0.181895559354648)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left),
                                                  stochastics->GetNormalDistributed(0.408008330993374, 0.124167898622834)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.109119291049093, 0.118564805394163)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.0578883280722746, 0.0788599388389143)));
                break;
            case CrossingPhase::Crossing_Right:
                [[fallthrough]];
            case CrossingPhase::Crossing_Left_ONE:
                [[fallthrough]];
            case CrossingPhase::Crossing_Left_TWO:
                [[fallthrough]];
            case CrossingPhase::Crossing_Straight:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard), 0));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.804347826086957, 0.364636853779138)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left), 0));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.195652173913044, 0.364636853779138)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other), 0));
                break;
            case CrossingPhase::Exit:
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Dashboard), 0));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Straight),
                                                  stochastics->GetNormalDistributed(0.94779579394964, 0.116034621140281)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Left), 0));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Right),
                                                  stochastics->GetNormalDistributed(0.012043512043512, 0.0341670133864293)));
                aoiProbs.push_back(std::make_pair(static_cast<int>(ScanAOI::Other),
                                                  stochastics->GetNormalDistributed(0.0401606940068478, 0.106411015920781)));
                break;
            default:
                std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "Gaze probabilities can not calculated!";
                throw std::runtime_error(message);
            }
        }
    }
    else {
        std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "Gaze probabilities can not calculated!";
        throw std::runtime_error(message);
    }
    AOIProbabilities scaledAOIProbs = ScaleProbabilitiesToOneAndEliminateNegativeProbabilities(aoiProbs);

    return scaledAOIProbs;
}
} // namespace Node
} // namespace RoadSegments
