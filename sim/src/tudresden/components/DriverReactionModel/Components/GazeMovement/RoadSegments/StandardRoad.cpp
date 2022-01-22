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

#include "StandardRoad.h"
#include <qglobal.h>

namespace RoadSegments {
namespace Edge {

GazeState StandardRoad::ControlGlance(CrossingPhase phase){
    // on standard road exist no special control gazes
};

AOIProbabilities StandardRoad::LookUpScanAOIProbability(CrossingPhase phase) {
    AOIProbabilities aoiProbs;
    // probabilities according to /no traffic /IndicatorState_Off/ phase =approach study of TU Dresden
    if (phase == CrossingPhase::NONE) {
        for(auto& prob : behaviourData.gmBehaviour.std_scanAOIProbabilities) {
            DistributionEntry* de = prob.second.get();
            double dist = stochastics->GetNormalDistributed(de->mean, de->std_deviation);
            double value = Common::ValueInBounds(de->min, dist, de->max);
            aoiProbs.insert(std::make_pair(static_cast<int>(prob.first), value));
        }
//        // gaze fixation for straight road or far away from intersection
//        aoiProbs.insert(std::make_pair(static_cast<int>(ScanAOI::Dashboard),
//                                       stochastics->GetNormalDistributed(0.0128205128205128, 0.0653720450460614)));
//        aoiProbs.insert(
//            std::make_pair(static_cast<int>(ScanAOI::Straight), stochastics->GetNormalDistributed(0.581440781440782, 0.308863771383145)));
//        aoiProbs.insert(
//            std::make_pair(static_cast<int>(ScanAOI::Left), stochastics->GetNormalDistributed(0.163827838827839, 0.198461222393695)));
//        aoiProbs.insert(
//            std::make_pair(static_cast<int>(ScanAOI::Right), stochastics->GetNormalDistributed(0.187881562881563, 0.220783615269782)));
//        aoiProbs.insert(
//            std::make_pair(static_cast<int>(ScanAOI::Other), stochastics->GetNormalDistributed(0.054029304029304, 0.114768364233879)));
    } else {
        std::string message =
            __FILE__ " Line: " + std::to_string(__LINE__) + "Gaze probabilities can not calculated invalide CrossingPhase!";
        throw std::runtime_error(message);
    }

    AOIProbabilities scaledAOIProbs = ScaleProbabilitiesToOneAndEliminateNegativeProbabilities(aoiProbs);
    return scaledAOIProbs;
};

AOIProbabilities StandardRoad::LookUpControlAOIProbability(CrossingPhase phase){
    // on standard road exist no special control gazes
};

double StandardRoad::LookUpFixationDuration(CrossingPhase phase, ScanAOI aoi){};

} // namespace Edge
} // namespace RoadSegments
