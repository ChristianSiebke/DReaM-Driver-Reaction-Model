#include "AnalysisDataRecorder.h"

namespace GlobalObserver {

std::shared_ptr<AnalysisDataRecorder> AnalysisDataRecorder::instance = nullptr;
int AnalysisDataRecorder::runId = 0;
int AnalysisDataRecorder::totalTime = 0;
std::string AnalysisDataRecorder::resultsPath = "";
std::map<uint8_t, std::shared_ptr<std::vector<AgentData>>> AnalysisDataRecorder::analysisData;
std::vector<TTCData> AnalysisDataRecorder::ttcData;
std::vector<CollisionData> AnalysisDataRecorder::collisions;
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
    if (startRoadId == "0")
        startRoadId = "3";
    if (exitRoadId == "0")
        exitRoadId = "3";

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
        UpdateGroupDataPoint(ego, data);
        UpdateTTCs(ego, data);
    }
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
        if (data.obstruction && (ego->velocity < data.targetVelocity / 2.0 || ego->acceleration < -1.5)) { // TODO maybe change threshold
            gd.obstructionCounter++;
        }
        if (gd.obstructionCounter > 9) {
            gd.obstructed = true;
        }
    }
    if (data.following) {
        gd.following = true;
    }
    gd.velocityProfile.emplace_back(data.targetDistributionOffset);
}

void AnalysisDataRecorder::UpdateTTCs(std::shared_ptr<DetailedAgentPerception> ego, AnalysisSignal data) {
    for (auto &agent : data.ttcs) {
        if (agent.second >= 3) {
            continue;
        }
        if (minTTCs.find(ego->id) == minTTCs.end()) {
            auto tmp = std::make_shared<std::map<int, double>>();
            tmp->insert(agent);
            minTTCs.insert(std::make_pair(ego->id, tmp));
        }
        else {
            if (minTTCs.at(ego->id)->find(agent.first) == minTTCs.at(ego->id)->end()) {
                minTTCs.at(ego->id)->insert(agent);
            }
            else {
                double &currentMin = minTTCs.at(ego->id)->at(agent.first);
                if (agent.second < currentMin) {
                    currentMin = agent.second;
                }
            }
        }
    }
}

void AnalysisDataRecorder::CheckCollisions(std::vector<std::pair<ObjectTypeOSI, int>> collisionPartners, int egoId, int time) {
    for (auto &partner : collisionPartners) {
        CollisionData cd;
        cd.egoId = egoId;
        cd.otherId = partner.second;
        cd.runId = this->runId;
        cd.timestamp = time;
        collisions.emplace_back(cd);
    }
}

uint8_t AnalysisDataRecorder::ComputeGroup(GroupingData &data) {
    uint8_t startRoad;
    if (data.startRoadOdId == "3") {
        startRoad = 0b00000000;
    }
    else if (data.startRoadOdId == "4") {
        startRoad = 0b01000000;
    }
    else if (data.startRoadOdId == "1") {
        startRoad = 0b10000000;
    }
    else if (data.startRoadOdId == "5") {
        startRoad = 0b11000000;
    }
    else {
        return 0b11111111;
    }
    uint8_t endRoad;
    if (data.endRoadOdId == "3") {
        endRoad = 0b00000000;
    }
    else if (data.endRoadOdId == "4") {
        endRoad = 0b00010000;
    }
    else if (data.endRoadOdId == "1") {
        endRoad = 0b00100000;
    }
    else if (data.endRoadOdId == "5") {
        endRoad = 0b00110000;
    }
    else {
        return 0b11111111;
    }
    uint8_t obstructed = data.obstructed ? 0b00001000 : 0b00000000;
    uint8_t following = data.following ? 0b00000100 : 0b00000000;
    uint8_t profile;
    std::sort(data.velocityProfile.begin(), data.velocityProfile.end());
    int median = data.velocityProfile.at(data.velocityProfile.size() / 2);
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
    return group;
}

void AnalysisDataRecorder::BufferRun() {
    for (auto &entry : minTTCs) {
        for (auto &ttc : *entry.second) {
            TTCData t;
            t.egoId = entry.first;
            t.otherId = ttc.first;
            t.runId = this->runId;
            t.minTTC = ttc.second;
            ttcData.emplace_back(t);
        }
    }

    for (auto &data : groupingData) {
        if (trajectoryData.at(data.first).velocity.empty())
            continue; // agent has not entered relevant area yet
        if (relevantAgents.at(data.first))
            continue; // agent is still in the middle of the intersection
        AgentData a;
        a.agentId = data.first;
        a.runId = this->runId;
        a.td = trajectoryData.at(data.first);
        uint8_t g = ComputeGroup(data.second);
        if (g == 255)
            continue;
        a.group = g;
        if (analysisData.find(g) != analysisData.end()) {
            analysisData.at(g)->emplace_back(a);
        }
        else {
            auto tmp = std::make_shared<std::vector<AgentData>>();
            tmp->emplace_back(a);
            analysisData.insert(std::make_pair(g, tmp));
        }
    }
}

static std::string SEPERATOR = ";";

void AnalysisDataRecorder::WriteOutput() {
    instance.reset();
    std::filesystem::remove_all(resultsPath + "\\analysis");
    std::filesystem::create_directories(resultsPath + "\\analysis\\groups");
    std::ofstream file(resultsPath + "\\analysis\\ttc.csv");
    if (file.is_open()) {
        file << "Run ID" << SEPERATOR << "Ego ID" << SEPERATOR << "Other Agent ID" << SEPERATOR << "Min TTC" << std::endl;
        for (auto &data : ttcData) {
            file << data.runId << SEPERATOR << data.egoId << SEPERATOR << data.otherId << SEPERATOR << data.minTTC << std::endl;
        }
        file.close();
    }
    file.open(resultsPath + "\\analysis\\exits.csv");
    if (file.is_open())
        ComputeExitDistributions(file);
    file.close();

    for (auto &group : analysisData) {
        file.open(resultsPath + "\\analysis\\groups\\" + std::to_string(group.first) + ".csv");
        if (!file.is_open())
            continue;
        file << GroupInfo(group.first) << std::endl;
        file << "Run ID" << SEPERATOR << "Agent ID" << SEPERATOR << "Velocities" << SEPERATOR << "Time Headways" << std::endl;
        for (auto &agent : *group.second) {
            std::string velocities = "{";
            std::string timeHeadways = "{";
            for (auto &v : agent.td.velocity) {
                velocities += std::to_string(v) + ",";
            }
            for (auto &t : agent.td.timeHeadway) {
                if (t > 100 || t < 0) {
                    timeHeadways += "---,";
                }
                else {
                    timeHeadways += std::to_string(t) + ",";
                }
            }
            velocities.back() = '}';
            timeHeadways.back() = '}';
            file << agent.runId << SEPERATOR << agent.agentId << SEPERATOR << velocities << SEPERATOR << timeHeadways << std::endl;
        }
        file.close();
    }

    file.open(resultsPath + "\\analysis\\collisions.csv");
    if (file.is_open()) {
        // TODO collisions wieder einbinden
        file << "Run ID" << SEPERATOR << "Ego ID" << SEPERATOR << "Other Agent ID" << SEPERATOR << "Collision Type ID" << std::endl;
    }
    file.close();
}

std::string AnalysisDataRecorder::GroupInfo(uint8_t group) {
    std::string info = "### GROUP INFO: ";
    uint8_t startRoad = group & 0b11000000;
    uint8_t endRoad = group & 0b00110000;
    uint8_t obstructed = group & 0b00001000;
    uint8_t following = group & 0b00000100;
    uint8_t profile = group & 0b00000011;
    switch (startRoad) {
    case 0b00000000:
        info += "Start Road = '3' (Tharandter Str. stadteinwärts) | ";
        break;
    case 0b01000000:
        info += "Start Road = '4' (Frankenbergstr.) | ";
        break;
    case 0b10000000:
        info += "Start Road = '1' (Tharandter Str. stadtauswärts) | ";
        break;
    case 0b11000000:
        info += "Start Road = '5' (Netto Einfahrt) | ";
        break;
    default:
        info += "Start Road = ? | ";
        break;
    }
    switch (endRoad) {
    case 0b00000000:
        info += "Exit Road = '3' (Tharandter Str. stadteinwärts) | ";
        break;
    case 0b00010000:
        info += "Exit Road = '4' (Frankenbergstr.) | ";
        break;
    case 0b00100000:
        info += "Exit Road = '1' (Tharandter Str. stadtauswärts) | ";
        break;
    case 0b00110000:
        info += "Exit Road = '5' (Netto Einfahrt) | ";
        break;
    default:
        info += "Exit Road = ? | ";
        break;
    }
    switch (obstructed) {
    case 0b00000000:
        info += "Obstructed: NO | ";
        break;
    case 0b00001000:
        info += "Obstructed: YES | ";
        break;
    default:
        info += "Obstructed: ? | ";
        break;
    }
    switch (following) {
    case 0b00000000:
        info += "Following: NO | ";
        break;
    case 0b00000100:
        info += "Following: YES | ";
        break;
    default:
        info += "Following: ? | ";
        break;
    }
    switch (profile) {
    case 0b00000000:
        info += "Driver Profile: LOW ###";
        break;
    case 0b00000001:
        info += "Driver Profile: MED ###";
        break;
    case 0b00000010:
        info += "Driver Profile: HIGH ####";
        break;
    default:
        info += "Driver Profile: ? ####";
        break;
    }
    return info;
}

void AnalysisDataRecorder::ComputeExitDistributions(std::ofstream &file) {
    file << "Road ID" << SEPERATOR << "Vehicle Type" << SEPERATOR << "Traffic Rate" << SEPERATOR << "Velocity Mean" << SEPERATOR
         << "Velocity StdDev" << std::endl;
    for (auto &roads : exitVehicleCounters) {
        for (auto &vehicles : *roads.second.get()) {
            int count = vehicles.second;
            if (count == 0)
                continue;
            double sum = 0;
            for (auto &vel : *exitVehicleVelocities.at(roads.first)->at(vehicles.first)) {
                sum += vel;
            }
            double mean = sum / count;
            double sum2 = 0;
            for (auto &vel : *exitVehicleVelocities.at(roads.first)->at(vehicles.first)) {
                sum += (vel - mean) * (vel - mean);
            }
            double stdDev = sqrt(sum2 / count);
            std::string type;
            switch (vehicles.first) {
            case DReaMDefinitions::AgentVehicleType::Car:
                type = "Car";
                break;
            case DReaMDefinitions::AgentVehicleType::Bicycle:
                type = "Bicycle";
                break;
            default:
                type = "Other";
                break;
            }
            double rate = count / (totalTime / (1000.0 * 60.0 * 60.0));
            file << roads.first << SEPERATOR << type << SEPERATOR << rate << SEPERATOR << mean << SEPERATOR << stdDev << std::endl;
        }
    }
}

} // namespace GlobalObserver