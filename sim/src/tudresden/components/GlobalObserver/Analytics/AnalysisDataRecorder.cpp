#include "AnalysisDataRecorder.h"

namespace GlobalObserver {

std::shared_ptr<AnalysisDataRecorder> AnalysisDataRecorder::instance = nullptr;
int AnalysisDataRecorder::runId = 0;
int AnalysisDataRecorder::totalTime = 0;
std::vector<AgentData> AnalysisDataRecorder::analysisData;
std::vector<TTCData> AnalysisDataRecorder::ttcData;
std::map<std::string, std::shared_ptr<std::map<DReaMDefinitions::AgentVehicleType, int>>> AnalysisDataRecorder::exitVehicleCounters;
std::map<std::string, std::shared_ptr<std::map<DReaMDefinitions::AgentVehicleType, std::shared_ptr<std::vector<double>>>>>
    AnalysisDataRecorder::exitVehicleVelocities;

void AnalysisDataRecorder::Trigger(std::shared_ptr<DetailedAgentPerception> ego, std::shared_ptr<InfrastructurePerception> infrastructure,
                                   AnalysisSignal data, int time) {
    if (time > runtime)
        runtime = time;

    auto lane = ego->lanePosition.lane;
    auto s = ego->lanePosition.sCoordinate;
    double vel = ego->velocity;
    auto startRoadId = ego->route.front().roadId;
    auto exitRoadId = ego->route.back().roadId;

    if (relevantAgents.find(ego->id) == relevantAgents.end()) {
        relevantAgents.insert(std::make_pair(ego->id, false));
        TrajectoryData td;
        GroupingData gd;
        gd.startRoadOdId = startRoadId;
        gd.endRoadOdId = exitRoadId;
        trajectoryData.insert(std::make_pair(ego->id, td));
        groupingData.insert(std::make_pair(ego->id, gd));
    }
    if (startRoadId == lane->GetRoad()->GetOpenDriveId() && observationStartS.find(startRoadId) != observationStartS.end() &&
        observationStartS.at(startRoadId) < s) {
        relevantAgents.at(ego->id) = true;
        lastLane.insert(std::make_pair(ego->id, lane));
        lastS.insert(std::make_pair(ego->id, s));
    }
    if (exitRoadId == lane->GetRoad()->GetOpenDriveId() && observationEndS.find(exitRoadId) != observationEndS.end() &&
        observationEndS.at(exitRoadId) < s + vel * 0.1) {
        CountExitVelocities(ego);
        if (relevantAgents.at(ego->id)) {
            lastLane.erase(ego->id);
            lastS.erase(ego->id);
        }
        relevantAgents.at(ego->id) = false;
    }
    if (relevantAgents.at(ego->id)) {
        double dist;
        if (lastLane.at(ego->id)->GetDReaMId() == lane->GetDReaMId()) {
            dist = s - lastS.at(ego->id);
        }
        else {
            dist = lastLane.at(ego->id)->GetLength() - lastS.at(ego->id) + s;
        }
        if (dist > 5.0) {
            lastLane.at(ego->id) = lane;
            lastS.at(ego->id) = s;

            AddAgentTrajectoryDataPoint(ego, data);
        }
    }
    UpdateGroupDataPoint(ego, data);
}

void AnalysisDataRecorder::CountExitVelocities(std::shared_ptr<DetailedAgentPerception> ego) {
    DReaMDefinitions::AgentVehicleType vehType = ego->vehicleType;
    if (ego->vehicleType == DReaMDefinitions::AgentVehicleType::Car || ego->vehicleType == DReaMDefinitions::AgentVehicleType::Truck ||
        ego->vehicleType == DReaMDefinitions::AgentVehicleType::Motorbike) {
        vehType = DReaMDefinitions::AgentVehicleType::Car;
    }
    else if (ego->vehicleType != DReaMDefinitions::AgentVehicleType::Bicycle) {
        return;
    }

    std::string odRoadId = ego->lanePosition.lane->GetRoad()->GetOpenDriveId();
    if (exitVehicleCounters.find(odRoadId) == exitVehicleCounters.end()) {
        auto tmp = std::make_shared<std::map<DReaMDefinitions::AgentVehicleType, int>>();
        tmp->insert(std::make_pair(vehType, 1));
        exitVehicleCounters.insert(std::make_pair(odRoadId, tmp));

        auto tmp2 = std::make_shared<std::map<DReaMDefinitions::AgentVehicleType, std::shared_ptr<std::vector<double>>>>();
        auto tmpVec = std::make_shared<std::vector<double>>();
        tmpVec->emplace_back(ego->velocity);
        tmp2->insert(std::make_pair(vehType, tmpVec));
        exitVehicleVelocities.insert(std::make_pair(odRoadId, tmp2));
    }
    else {
        if (exitVehicleCounters.at(odRoadId)->find(vehType) == exitVehicleCounters.at(odRoadId)->end()) {
            exitVehicleCounters.at(odRoadId)->insert(std::make_pair(vehType, 1));

            auto tmpVec = std::make_shared<std::vector<double>>();
            tmpVec->emplace_back(ego->velocity);
            exitVehicleVelocities.at(odRoadId)->insert(std::make_pair(vehType, tmpVec));
        }
        else {
            exitVehicleCounters.at(odRoadId)->at(vehType)++;
            exitVehicleVelocities.at(odRoadId)->at(vehType)->emplace_back(ego->velocity);
        }
    }
}

void AnalysisDataRecorder::AddAgentTrajectoryDataPoint(std::shared_ptr<DetailedAgentPerception> ego, AnalysisSignal data) {
    trajectoryData.at(ego->id).velocity.emplace_back(ego->velocity);
    trajectoryData.at(ego->id).timeHeadway.emplace_back(data.timeHeadway);
}

void AnalysisDataRecorder::UpdateGroupDataPoint(std::shared_ptr<DetailedAgentPerception> ego, AnalysisSignal data) {
    GroupingData &gd = groupingData.at(ego->id);
    if (!gd.obstructed) {
        if (data.obstruction && (ego->velocity < data.targetVelocity / 2.0 || ego->acceleration < -1.5)) {
            gd.obstructionCounter++;
        }
        if (gd.obstructionCounter > 10) {
            gd.obstructed = true;
        }
    }
    if (data.following) {
        gd.following = true;
    }
    gd.velocityProfile.emplace_back(data.targetDistributionOffset);
}

void AnalysisDataRecorder::ComputeGroups() {
    for (auto &agent : groupingData) {
        uint8_t startRoad;
        if (agent.second.startRoadOdId == "") {
            startRoad = 0b00000000;
        }
        else if (agent.second.startRoadOdId == "") {
            startRoad = 0b01000000;
        }
        else if (agent.second.startRoadOdId == "") {
            startRoad = 0b10000000;
        }
        else if (agent.second.startRoadOdId == "") {
            startRoad = 0b11000000;
        }
        else {
            continue;
        }
        uint8_t endRoad;
        if (agent.second.endRoadOdId == "") {
            startRoad = 0b00000000;
        }
        else if (agent.second.endRoadOdId == "") {
            startRoad = 0b00010000;
        }
        else if (agent.second.endRoadOdId == "") {
            startRoad = 0b00100000;
        }
        else if (agent.second.endRoadOdId == "") {
            startRoad = 0b00110000;
        }
        else {
            continue;
        }
        uint8_t obstructed = agent.second.obstructed ? 0b00001000 : 0b00000000;
        uint8_t following = agent.second.following ? 0b00000100 : 0b00000000;
        uint8_t profile;
        int median = agent.second.velocityProfile.at(agent.second.velocityProfile.size() / 2);
        if (median > 0.3) {
            profile = 0b00000010;
        }
        else if (median < -0.3) {
            profile = 0b00000000;
        }
        else {
            profile = 0b00000001;
        }
        uint8_t group = startRoad + endRoad + obstructed + following + profile;
    }
}

// TODO funktion aufrufen
void AnalysisDataRecorder::ComputeExitDistributions() {
    for (auto &roads : exitVehicleCounters) {
        for (auto &vehicles : *roads.second.get()) {
            int count = vehicles.second;
            if (count == 0)
                continue;
            double sum = 0;
            for (auto &vel : *exitVehicleVelocities.at(roads.first)->at(vehicles.first)) {
                sum += vel;
            }
            double mean = sum / count; // TODO output
            double sum2 = 0;
            for (auto &vel : *exitVehicleVelocities.at(roads.first)->at(vehicles.first)) {
                sum += (vel - mean) * (vel - mean);
            }
            double stdDev = sqrt(sum2 / count); // TODO output
        }
    }
}

} // namespace GlobalObserver