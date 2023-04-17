#include "AnalysisDataRecorder.h"

namespace GlobalObserver {

void AnalysisDataRecorder::Trigger(std::shared_ptr<DetailedAgentPerception> ego, std::shared_ptr<InfrastructurePerception> infrastructure) {
    auto lane = ego->lanePosition.lane;
    auto s = ego->lanePosition.sCoordinate;
    double vel = ego->velocity;
    auto exitRoadId = ego->route.back().roadId;
    if (lane->GetLength() < s + vel * 0.1 && exitRoadId == lane->GetRoad()->GetOpenDriveId()) {
        CountExitVelocities(ego);
    }
}

void AnalysisDataRecorder::CountExitVelocities(std::shared_ptr<DetailedAgentPerception> ego) {
    std::string odRoadId = ego->lanePosition.lane->GetRoad()->GetOpenDriveId();
    if (exitVehicleCounters.find(odRoadId) == exitVehicleCounters.end()) {
        auto tmp = std::make_shared<std::map<DReaMDefinitions::AgentVehicleType, int>>();
        tmp->insert(std::make_pair(ego->vehicleType, 1));
        exitVehicleCounters.insert(std::make_pair(odRoadId, tmp));

        auto tmp2 = std::make_shared<std::map<DReaMDefinitions::AgentVehicleType, std::shared_ptr<std::vector<double>>>>();
        auto tmpVec = std::make_shared<std::vector<double>>();
        tmpVec->emplace_back(ego->velocity);
        tmp2->insert(std::make_pair(ego->vehicleType, tmpVec));
        exitVehicleVelocities.insert(std::make_pair(odRoadId, tmp2));
    }
    else {
        if (exitVehicleCounters.at(odRoadId)->find(ego->vehicleType) == exitVehicleCounters.at(odRoadId)->end()) {
            exitVehicleCounters.at(odRoadId)->insert(std::make_pair(ego->vehicleType, 1));

            auto tmpVec = std::make_shared<std::vector<double>>();
            tmpVec->emplace_back(ego->velocity);
            exitVehicleVelocities.at(odRoadId)->insert(std::make_pair(ego->vehicleType, tmpVec));
        }
        else {
            exitVehicleCounters.at(odRoadId)->at(ego->vehicleType)++;
            exitVehicleVelocities.at(odRoadId)->at(ego->vehicleType)->emplace_back(ego->velocity);
        }
    }
}

void AnalysisDataRecorder::ComputeExitDistributions() {
}

} // namespace GlobalObserver