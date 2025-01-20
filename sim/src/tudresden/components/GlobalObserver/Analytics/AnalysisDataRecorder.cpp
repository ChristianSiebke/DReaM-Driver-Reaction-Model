#include "AnalysisDataRecorder.h"

namespace GlobalObserver {

std::shared_ptr<AnalysisDataRecorder> AnalysisDataRecorder::instance = nullptr;
std::string AnalysisDataRecorder::scenarioConfigPath = "";
int AnalysisDataRecorder::runId = 0;
int AnalysisDataRecorder::totalTime = 0;
std::map<uint16_t, std::shared_ptr<std::vector<AgentData>>> AnalysisDataRecorder::analysisData{};
std::vector<TTCData> AnalysisDataRecorder::ttcData{};
std::vector<CollisionData> AnalysisDataRecorder::collisions{};
std::map<std::string, std::shared_ptr<std::map<DReaMDefinitions::AgentVehicleType, int>>> AnalysisDataRecorder::exitVehicleCounters{};
std::map<std::string, std::shared_ptr<std::map<DReaMDefinitions::AgentVehicleType, std::shared_ptr<std::vector<double>>>>>
    AnalysisDataRecorder::exitVehicleVelocities{};

void AnalysisDataRecorder::Trigger(std::shared_ptr<DetailedAgentPerception> ego, AnalysisSignal data, int time) {
    if (time > runtime)
        runtime = time;

    analysisSignalLog.insert(std::make_pair(ego->id, data));

    auto lane = ego->lanePosition.lane;
    auto s = ego->lanePosition.sCoordinate;
    double vel = ego->velocity;
    assert(!ego->route.empty());
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
        gd.agentType = ego->vehicleType;
        trajectoryData.insert(std::make_pair(ego->id, td));
        groupingData.insert(std::make_pair(ego->id, gd));
    }

    if (!relevantAgents.at(ego->id) && startRoadId == lane->GetRoad()->GetOpenDriveId() && observationStartS.find(startRoadId) != observationStartS.end() &&
        observationStartS.at(startRoadId) < s) {
        relevantAgents.at(ego->id) = true;
        lastLane.insert(std::make_pair(ego->id, lane));
        lastRoad.insert(std::make_pair(ego->id, lane->GetRoad()));
        lastS.insert(std::make_pair(ego->id, observationStartS.at(startRoadId)));
        AddAgentTrajectoryDataPoint(ego, data);
    }

    if (relevantAgents.at(ego->id)) {
        double dist;
        if (lastLane.at(ego->id)->GetDReaMId() == lane->GetDReaMId() &&
            lastRoad.at(ego->id)->GetDReaMId() == lane->GetRoad()->GetDReaMId()) {
            dist = s - lastS.at(ego->id);
        }
        else if (lastLane.at(ego->id)->GetDReaMId() != lane->GetDReaMId() &&
                 lastRoad.at(ego->id)->GetDReaMId() == lane->GetRoad()->GetDReaMId()) { // lane change
            dist = s - lastS.at(ego->id);
        }
        else {
            dist = lastLane.at(ego->id)->GetLength() - lastS.at(ego->id) + s;
        }

        if (dist >= trajectorySampleRate) {
            lastLane.at(ego->id) = lane;
            lastRoad.at(ego->id) = lane->GetRoad();          
            lastS.at(ego->id) = s - (dist - trajectorySampleRate);

            if (exitRoadId == lane->GetRoad()->GetOpenDriveId()){
                if(observationEndS.at(exitRoadId) - lastS.at(ego->id) >= 0){
                  AddAgentTrajectoryDataPoint(ego, data);
                }
            }
            else{
                AddAgentTrajectoryDataPoint(ego, data);
            }   
        }

        if (relevantAgents.at(ego->id) && exitRoadId == lane->GetRoad()->GetOpenDriveId() && observationEndS.find(exitRoadId) != observationEndS.end() &&
            (observationEndS.at(exitRoadId) - lastS.at(ego->id) <= trajectorySampleRate  && s >= lastS.at(ego->id))) {
            CountExitVelocities(ego);
            if (relevantAgents.at(ego->id)) {
                lastLane.erase(ego->id);
                lastRoad.erase(ego->id);
                lastS.erase(ego->id);
            }
            relevantAgents.at(ego->id) = false;
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
    if (ego->lanePosition.lane->GetRoad()->GetOpenDriveId() == "2") {
        gd.secondJunction = true;
        gd.obstructionCounter = 0;
    }

    if (!gd.obstructed) {
        if (data.obstruction && (ego->velocity < data.targetVelocity / 2.0 || ego->acceleration < data.maxComfortDeceleration)) {
            gd.obstructionCounter++;
        }
        if (!data.obstruction) {
            gd.obstructionCounter = 0;
        }
        if (gd.obstructionCounter > obstructionCounterLimit) {
            if (gd.secondJunction) {
                gd.obstructed = true;
            }
            else {
                gd.obstructed2 = true;
            }
        }
    }
    if (data.following) {
        if (gd.secondJunction) {
            gd.following = true;
        }
        else {
            gd.following2 = true;
        }
        gd.followingTarget = data.followingTarget;
    }
    gd.velocityProfile.emplace_back(data.targetDistributionOffset);
}

void AnalysisDataRecorder::UpdateTTCs(std::shared_ptr<DetailedAgentPerception> ego, AnalysisSignal data) {
    for (auto &agent : data.ttcs) {
        if (agent.second >= minTTCUpperBound) {
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

void AnalysisDataRecorder::CheckCollisions(std::vector<std::pair<int, std::shared_ptr<DetailedAgentPerception>>> partners, int egoId,
                                           std::shared_ptr<DetailedAgentPerception> egoData, AnalysisSignal data, int time) {
    if (collisionPartners.find(egoId) == collisionPartners.end()) {
        auto tmpVec = std::make_shared<std::list<int>>();
        collisionPartners.insert(std::make_pair(egoId, tmpVec));
    }

    for (auto &partner : partners) {
        if (std::find(collisionPartners.at(egoId)->begin(), collisionPartners.at(egoId)->end(), partner.first) !=
            collisionPartners.at(egoId)->end()) {
            continue;
        }

        collisionPartners.at(egoId)->emplace_back(partner.first);
        CollisionData cd;
        cd.egoId = egoId;
        cd.otherId = partner.first;
        cd.runId = this->runId;
        cd.timestamp = time;
        cd.type = DetermineCollisionType(egoId, partner.first, egoData, partner.second, data);
        cd.egoType = egoData->vehicleType;
        cd.otherType = partner.second->vehicleType;
        cd.onIntersection = egoData->junctionDistance.on >=0 || partner.second->junctionDistance.on >=0;
        collisions.emplace_back(cd);
    }
}

int AnalysisDataRecorder::DetermineCollisionType(int egoId, int otherId, std::shared_ptr<DetailedAgentPerception> egoData,
                                                 std::shared_ptr<DetailedAgentPerception> partnerData, AnalysisSignal data) {
    if (analysisSignalLog.find(otherId) == analysisSignalLog.end()) {
        std::cout << "other agent not in signal Log" << std::endl;
    }
    auto egoLane = egoData->lanePosition.lane;
    auto egoRoad = egoLane->GetRoad();
    auto egoInd = egoData->indicatorState;

    auto partnerLane = partnerData->lanePosition.lane;
    auto partnerRoad = partnerLane->GetRoad();
    auto parnterInd = partnerData->indicatorState;

    std::string appDir = "";

    if (egoData->junctionDistance.on > 0) {
        if (groupingData.at(otherId).followingTarget == egoId) {
            switch (egoInd) {
            case IndicatorState::IndicatorState_Left:
                return 201;
            case IndicatorState::IndicatorState_Right:
                return 231;
            default:
                return -1;
            }
        }

        if (!egoLane->IsJunctionLane())
            return -1;

        auto egoPred = egoRoad->GetPredecessor();
        auto partnerPred = partnerRoad->GetPredecessor();

        if (partnerRoad->HasPredecessor()) {
            if (egoPred->GetDReaMId() == partnerPred->GetDReaMId()) {
                appDir = "Behind";
            }
        }
        else {
            return -1;
        }

        auto nl = egoLane->GetPredecessors().front()->NextLanes(egoLane->GetPredecessors().front()->IsInRoadDirection());
        if (!nl.has_value())
            return -1;

        for (auto &l : nl->leftLanes) {
            if (l->GetRoad()->HasSuccessor() && l->GetRoad()->GetSuccessor()->GetDReaMId() == partnerRoad->GetDReaMId()) {
                appDir = "Left";
                break;
            }
        }
        for (auto &l : nl->rightLanes) {
            if (l->GetRoad()->HasSuccessor() && l->GetRoad()->GetSuccessor()->GetDReaMId() == partnerRoad->GetDReaMId()) {
                appDir = "Right";
                break;
            }
        }
        for (auto &l : nl->straightLanes) {
            if (l->GetRoad()->HasSuccessor() && l->GetRoad()->GetSuccessor()->GetDReaMId() == partnerRoad->GetDReaMId()) {
                appDir = "Straight";
                break;
            }
        }

        if (appDir == "Straight") {
            if (partnerData->vehicleType == DReaMDefinitions::AgentVehicleType::Bicycle) {
                switch (egoInd) {
                case IndicatorState::IndicatorState_Left:
                    return 224;
                case IndicatorState::IndicatorState_Right:
                    return 243;
                default:
                    return -1;
                }
            }
            else if (partnerData->vehicleType == DReaMDefinitions::AgentVehicleType::Pedestrian) {
                switch (egoInd) {
                case IndicatorState::IndicatorState_Left:
                    return 222;
                case IndicatorState::IndicatorState_Right:
                    return 241;
                default:
                    return -1;
                }
            }
            else {
                switch (egoInd) {
                case IndicatorState::IndicatorState_Left:
                    switch (parnterInd) {
                    case IndicatorState::IndicatorState_Left:
                        return 215;
                    case IndicatorState::IndicatorState_Right:
                        return 212;
                    case IndicatorState::IndicatorState_Off:
                        return 211;
                    default:
                        return -1;
                    }
                case IndicatorState::IndicatorState_Right:
                    return -1;
                default:
                    return -1;
                }
            }
        }
        else if (appDir == "Left") {
            if (!data.hasROW) {
                if (partnerData->vehicleType == DReaMDefinitions::AgentVehicleType::Bicycle) {
                    return 341;
                }
                switch (egoInd) {
                case IndicatorState::IndicatorState_Left:
                    switch (parnterInd) {
                    case IndicatorState::IndicatorState_Left:
                        return 302;
                    case IndicatorState::IndicatorState_Right:
                        return 306;
                    case IndicatorState::IndicatorState_Off:
                        return 302;
                    default:
                        return -1;
                    }
                case IndicatorState::IndicatorState_Right:
                    return 303;
                case IndicatorState::IndicatorState_Off:
                    return 301;
                default:
                    return -1;
                }
            }
            else {
                if (analysisSignalLog.at(otherId).hasROW) {
                    return -1;
                }
                else {
                    return 261;
                }
            }
        }
        else if (appDir == "Right") {
            if (!data.hasROW) {
                if (partnerData->vehicleType == DReaMDefinitions::AgentVehicleType::Bicycle) {
                    return 344;
                }
                switch (egoInd) {
                case IndicatorState::IndicatorState_Left:
                    return 322;
                case IndicatorState::IndicatorState_Right:
                    switch (parnterInd) {
                    case IndicatorState::IndicatorState_Left:
                        return 326;
                    case IndicatorState::IndicatorState_Right:
                        return 323;
                    case IndicatorState::IndicatorState_Off:
                        return 323;
                    default:
                        return -1;
                    }
                case IndicatorState::IndicatorState_Off:
                    return 321;
                default:
                    return -1;
                }
            }
            else {
                if (analysisSignalLog.at(otherId).hasROW) {
                    return -1;
                }
                else {
                    return 262;
                }
            }
        }
        else if (appDir == "Behind") {
            if (egoData->vehicleType == DReaMDefinitions::AgentVehicleType::Bicycle) {
                if (egoInd == IndicatorState::IndicatorState_Left) {
                    return 203;
                }
            }
            if (partnerData->vehicleType == DReaMDefinitions::AgentVehicleType::Bicycle) {
                return 374;
            }
            switch (egoInd) {
            case IndicatorState::IndicatorState_Left:
                return 201;
            case IndicatorState::IndicatorState_Right:
                return 231;
            default:
                return -1;
            }
        }
        else {
            return -1;
        }
    }
    else {
        if (egoRoad->GetDReaMId() == partnerRoad->GetDReaMId()) {
            if (partnerData->vehicleType == DReaMDefinitions::AgentVehicleType::Bicycle) {
                return 373;
            }
            if (egoLane->GetDReaMId() == partnerLane->GetDReaMId()) {
                if (egoData->lanePosition.sCoordinate < partnerData->lanePosition.sCoordinate) {
                    if (analysisSignalLog.at(otherId).following) {
                        return 611;
                    }
                    else if (!analysisSignalLog.at(otherId).hasROW) {
                        return 621;
                    }
                    else {
                        return 601;
                    }
                }
                else {
                    return -2;
                }
            }
            else if (egoLane->IsInRoadDirection() != partnerLane->IsInRoadDirection()) {
                return 681;
            }
            else {
                return -3;
            }
        }
        else {
            return -1;
        }
    }
}

uint16_t AnalysisDataRecorder::ComputeGroup(GroupingData &data) {
    uint16_t startRoad;
    if (data.startRoadOdId == "3") {
        startRoad = 0b0000000000;
    }
    else if (data.startRoadOdId == "4") {
        startRoad = 0b0100000000;
    }
    else if (data.startRoadOdId == "1") {
        startRoad = 0b1000000000;
    }
    else if (data.startRoadOdId == "5") {
        startRoad = 0b1100000000;
    }
    else {
        return 0b1111111111;
    }
    uint16_t endRoad;
    if (data.endRoadOdId == "3") {
        endRoad = 0b0000000000;
    }
    else if (data.endRoadOdId == "4") {
        endRoad = 0b0001000000;
    }
    else if (data.endRoadOdId == "1") {
        endRoad = 0b0010000000;
    }
    else if (data.endRoadOdId == "5") {
        endRoad = 0b0011000000;
    }
    else {
        return 0b1111111111;
    }
    uint16_t obstructed = (data.obstructed ? 0b0000010000 : 0b0000000000) + (data.obstructed2 ? 0b0000100000 : 0b0000000000);
    uint16_t following = (data.following ? 0b0000000100 : 0b0000000000) + (data.following2 ? 0b0000001000 : 0b0000000000);
    uint16_t profile;
    std::sort(data.velocityProfile.begin(), data.velocityProfile.end());
    int median = data.velocityProfile.at(data.velocityProfile.size() / 2);
    if (median > 0.3) {
        profile = 0b0000000010;
    }
    else if (median < -0.3) {
        profile = 0b0000000000;
    }
    else {
        profile = 0b0000000001;
    }
    uint16_t group = startRoad + endRoad + obstructed + following + profile;
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
        a.agentType = data.second.agentType;
        a.runId = this->runId;
        a.td = trajectoryData.at(data.first);
        uint16_t g = ComputeGroup(data.second);
        if (g == 1023)
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
    std::filesystem::remove_all(scenarioConfigPath + "\\analysis");
    std::filesystem::create_directories(scenarioConfigPath + "\\analysis\\groups");
    std::ofstream file;
    file.open(scenarioConfigPath + "\\analysis\\ttc.csv");
    if (file.is_open()) {
        file << "Run ID" << SEPERATOR << "Ego ID" << SEPERATOR << "Other Agent ID" << SEPERATOR << "Min TTC" << std::endl;
        for (auto &data : ttcData) {
            file << data.runId << SEPERATOR << data.egoId << SEPERATOR << data.otherId << SEPERATOR << data.minTTC << std::endl;
        }
    }

    file.close();
    file.open(scenarioConfigPath + "\\analysis\\exits.csv");
    if (file.is_open())
        ComputeExitDistributions(file);
    file.close();
    for (auto &group : analysisData) {
        file.open(scenarioConfigPath + "\\analysis\\groups\\" + std::to_string(group.first) + ".csv");
        if (!file.is_open())
            continue;
        file << GroupInfo(group.first) << std::endl;
        file << "Run ID" << SEPERATOR << "Vehicle Type" << SEPERATOR << "Agent ID" << SEPERATOR << "Velocities" << SEPERATOR
             << "Time Headways" << std::endl;
        for (auto &agent : *group.second) {
            if (agent.agentType != DReaMDefinitions::AgentVehicleType::Car) {
                continue;
            }
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
            file << agent.runId << SEPERATOR << "CAR" << SEPERATOR << agent.agentId << SEPERATOR << velocities << SEPERATOR << timeHeadways
                 << std::endl;
        }
        file.close();
    }

    file.open(scenarioConfigPath + "\\analysis\\collisions.csv");
    if (file.is_open()) {
        file << "Run ID" << SEPERATOR << "Timestamp" << SEPERATOR << "Ego ID" << SEPERATOR<< "Ego Vehicle Type" <<SEPERATOR<< "Other Agent ID" << SEPERATOR<< "Other Agent Vehicle Type"<< SEPERATOR
             << "On Junction" <<SEPERATOR << "Collision Type ID" << std::endl;
        for (auto col : collisions) {
            std::string egoType = "";
            std::string otherType = "";
            if(col.egoType == DReaMDefinitions::AgentVehicleType::Bicycle){
                    egoType = "Bicycle";
            }
            else{
                    egoType = "Car";
    
            }
            if(col.otherType == DReaMDefinitions::AgentVehicleType::Bicycle){
                otherType = "Bicycle";
            }
            else{
                    otherType = "Car";
    
            }
    
            std::string onIntersection = col.onIntersection ? "true" : "false";
    
            file << col.runId << SEPERATOR << col.timestamp << SEPERATOR << col.egoId <<SEPERATOR <<egoType<< SEPERATOR << col.otherId << SEPERATOR<<otherType <<SEPERATOR <<onIntersection<< SEPERATOR  <<col.type
                     << std::endl;
        }
    }
    file.close();
}

std::string AnalysisDataRecorder::GroupInfo(uint16_t group) {
    std::string info = "### GROUP INFO: ";
    uint16_t startRoad = group & 0b1100000000;
    uint16_t endRoad = group & 0b0011000000;
    uint16_t obstructed = group & 0b0000110000;
    uint16_t following = group & 0b0000001100;
    uint16_t profile = group & 0b0000000011;
    switch (startRoad) {
    case 0b0000000000:
        info += "Start Road = '3' (Tharandter Str. stadteinwärts) | ";
        break;
    case 0b0100000000:
        info += "Start Road = '4' (Netto Einfahrt) | ";
        break;
    case 0b1000000000:
        info += "Start Road = '1' (Tharandter Str. stadtauswärts) | ";
        break;
    case 0b1100000000:
        info += "Start Road = '5' (Frankenbergstr.) | ";
        break;
    default:
        info += "Start Road = ? | ";
        break;
    }
    switch (endRoad) {
    case 0b0000000000:
        info += "Exit Road = '3' (Tharandter Str. stadteinwärts) | ";
        break;
    case 0b0001000000:
        info += "Exit Road = '4' (Netto Einfahrt) | ";
        break;
    case 0b0010000000:
        info += "Exit Road = '1' (Tharandter Str. stadtauswärts) | ";
        break;
    case 0b0011000000:
        info += "Exit Road = '5' (Frankenbergstr.) | ";
        break;
    default:
        info += "Exit Road = ? | ";
        break;
    }
    switch (obstructed) {
    case 0b0000000000:
        info += "Obstructed: NO | ";
        break;
    case 0b0000010000:
        info += "Obstructed: 1ST | ";
        break;
    case 0b0000100000:
        info += "Obstructed: 2ND | ";
        break;
    case 0b0000110000:
        info += "Obstructed: BOTH | ";
        break;
    default:
        info += "Obstructed: ? | ";
        break;
    }
    switch (following) {
    case 0b0000000000:
        info += "Following: NO | ";
        break;
    case 0b0000000100:
        info += "Following: 1ST | ";
        break;
    case 0b0000001000:
        info += "Following: 2ND | ";
        break;
    case 0b0000001100:
        info += "Following: BOTH | ";
        break;
    default:
        info += "Following: ? | ";
        break;
    }
    switch (profile) {
    case 0b0000000000:
        info += "Driver Profile: LOW ###";
        break;
    case 0b0000000001:
        info += "Driver Profile: MED ###";
        break;
    case 0b0000000010:
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
            double count = static_cast<double>(vehicles.second);
            if (count == 0)
                continue;
            double sum = 0;
            for (const auto &vel : *exitVehicleVelocities.at(roads.first)->at(vehicles.first)) {
                sum += vel;
            }
            double mean = sum / count;
            double stdDev = 0;
            for (const auto &vel : *exitVehicleVelocities.at(roads.first)->at(vehicles.first)) {
                stdDev += (vel - mean) * (vel - mean);
            }
            stdDev = sqrt(stdDev / count);
            std::string type;
            switch (vehicles.first) {
            case DReaMDefinitions::AgentVehicleType::Car:
                type = "CAR";
                break;
            case DReaMDefinitions::AgentVehicleType::Bicycle:
                type = "BICYCLE";
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
