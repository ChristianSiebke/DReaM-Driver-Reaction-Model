/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/

#include "StandardRoad.h"

#include <cassert>

#include <qglobal.h>

namespace RoadSegments {
namespace Edge {

GazeState StandardRoad::ControlGlance(CrossingPhase phase) { // on standard road exist no special control gazes
    assert(true && " Function should never executed");
};

AOIProbabilities StandardRoad::LookUpScanAOIProbability(CrossingPhase phase) {
    AOIProbabilities aoiProbs;
    // probabilities according to /no traffic /IndicatorState_Off/ phase =approach study of TU Dresden
    if (phase == CrossingPhase::NONE) {
        for(auto& prob : behaviourData.gmBehaviour.std_scanAOIProbabilities) {
            DReaM::NormalDistribution* de = prob.second.get();
            double dist = stochastics->GetNormalDistributed(de->mean, de->std_deviation);
            double value = Common::ValueInBounds(de->min, dist, de->max);
            aoiProbs.push_back(std::make_pair(static_cast<int>(prob.first), value));
        }
    } else {
        std::string message =
            __FILE__ " Line: " + std::to_string(__LINE__) + "Gaze probabilities can not calculated invalide CrossingPhase!";
        throw std::runtime_error(message);
    }

    AOIProbabilities scaledAOIProbs = ScaleProbabilitiesToOneAndEliminateNegativeProbabilities(aoiProbs);
    return scaledAOIProbs;
};

AOIProbabilities StandardRoad::LookUpControlAOIProbability(CrossingPhase phase) { // on standard road exist no special control gazes
    assert(true && " Function should never executed");
};

} // namespace Edge
} // namespace RoadSegments
