#include "AgentStateRecorder.h"

namespace AgentStateRecorder {
std::shared_ptr<AgentStateRecorder> AgentStateRecorder::instance = nullptr;

// string representations of enum values, can easily be accessed using array[(int) enum_value]
const std::string gazeTypes[] = {"NONE", "ScanGlance", "ControlGlance", "ObserveGlance"};
const std::string scanAOIs[] = {"NONE", "Right", "Straight", "Left", "Rear", "Dashboard", "Other"};
const std::string crossingTypes[] = {"NA", "Left", "Right", "Straight", "Random"};
const std::string crossingPhases[] = {
    "NONE",           "Approach", "Deceleration_ONE", "Deceleration_TWO", "Crossing_Left_ONE", "Crossing_Left_TWO", "Crossing_Straight",
    "Crossing_Right", "Exit"};
const std::string stoppingPointTypes[] = {
    "NONE",         "Pedestrian_Right", "Pedestrian_Left", "Pedestrian_Crossing_ONE", "Pedestrian_Crossing_TWO",
    "Vehicle_Left", "Vehicle_Crossroad"};

void AgentStateRecorder::AddInfrastructurePerception(std::shared_ptr<InfrastructurePerception> infrastructurePerception) {
    record.infrastructurePerception = infrastructurePerception;
}

void AgentStateRecorder::AddGazeStates(int time, int id, GazeState gazeState) {
    if (record.gazeStates.find(time) == record.gazeStates.end()) {
        std::map<agentID, GazeState> idMap;
        record.gazeStates.insert(std::make_pair(time, idMap));
    }
    record.gazeStates.at(time).insert(std::make_pair(id, gazeState));
}

void AgentStateRecorder::AddCrossingInfos(int time, int id, CrossingInfo info) {
    if (record.crossingInfos.find(time) == record.crossingInfos.end()) {
        std::map<agentID, CrossingInfo> idMap;
        record.crossingInfos.insert(std::make_pair(time, idMap));
    }
    record.crossingInfos.at(time).insert(std::make_pair(id, info));
}

void AgentStateRecorder::AddOtherAgents(int time, int id, std::vector<AgentPerception> agents) {
    if (record.observedAgents.find(time) == record.observedAgents.end()) {
        std::map<agentID, std::vector<AgentPerception>> idMap;
        record.observedAgents.insert(std::make_pair(time, idMap));
    }
    record.observedAgents.at(time).insert(std::make_pair(id, agents));
}

void AgentStateRecorder::AddFixationPoints(int time, int id, std::vector<Common::Vector2d> fixationPoints) {
    if (record.segmentControlFixationPoints.find(time) == record.segmentControlFixationPoints.end()) {
        std::map<agentID, std::vector<Common::Vector2d>> idMap;
        record.segmentControlFixationPoints.insert(std::make_pair(time, idMap));
    }
    record.segmentControlFixationPoints.at(time).insert(std::make_pair(id, fixationPoints));
}

void AgentStateRecorder::AddTrafficSignals(int time, int id, std::unordered_map<DReaMId, MemorizedTrafficSignal> *signals) {
    if (record.trafficSignalMemory.find(time) == record.trafficSignalMemory.end()) {
        std::map<agentID, std::vector<OdId>> idMap;
        record.trafficSignalMemory.insert(std::make_pair(time, idMap));
    }

    std::vector<OdId> signalIdList;
    for (const auto &[id, signal] : *signals) {
        signalIdList.push_back(signal.trafficSignal->GetOpenDriveId());
    }
    record.trafficSignalMemory[time].insert(std::make_pair(id, signalIdList));
}

std::string AgentStateRecorder::GenerateDataSet(int time, int agentId) {
    std::string outputLine;

    // gaze information:
    // <GazeType>, <ScanAOI>, <ufovAngle>, <openingAngle>, <viewDistance>
    GazeState gazeState = record.gazeStates.at(time).at(agentId);
    outputLine += gazeTypes[(int)gazeState.fixationState.first] + ",";
    outputLine += scanAOIs[gazeState.fixationState.second] + ",";
    outputLine += std::to_string(gazeState.ufovAngle) += ",";
    outputLine += std::to_string(gazeState.openingAngle) += ",";
    outputLine += std::to_string(gazeState.viewDistance) += ",";
    // TODO catch for empty GazeType

    // other agents as perceived by the current:
    // [{<id> | <x> | <y> | <yaw>} | { ... }]
    outputLine += "[";
    for (auto agent : record.observedAgents.at(time).at(agentId)) {
        outputLine += "{";
        outputLine += std::to_string(agent.id) += " | ";
        outputLine += std::to_string(agent.refPosition.x) += " | ";
        outputLine += std::to_string(agent.refPosition.y) += " | ";
        outputLine += std::to_string(agent.yawAngle) += "}";
    }
    outputLine += "],";

    // crossingType & crossingPhase:
    // <crossingType>, <crossingPhase>
    auto crossingInfo = record.crossingInfos.at(time).at(agentId);
    outputLine += crossingTypes[(int)crossingInfo.type] + ",";
    outputLine += crossingPhases[(int)crossingInfo.phase] + ",";

    // fixation points:
    // [{<x> | <y>} | { ... }]
    outputLine += "[";
    for (auto point : record.segmentControlFixationPoints.at(time).at(agentId)) {
        outputLine += ("{");
        outputLine += std::to_string(point.x) += " | ";
        outputLine += std::to_string(point.y) += "}";
    }
    outputLine += "]";
    outputLine += "}";

    // memorized traffic signals:
    // [<TrafficSignalOdId> | ...]
    outputLine += "[";
    for (int i = 0; i < record.trafficSignalMemory.at(time).at(agentId).size(); i++) {
        outputLine += record.trafficSignalMemory.at(time).at(agentId)[i];
        if (i != record.trafficSignalMemory.at(time).at(agentId).size() - 1) {
            outputLine += " | ";
        }
    }

    outputLine += "],";

    return outputLine;
}

std::string AgentStateRecorder::GenerateHeader() {
    std::string header;

    header += "GazeType, ";
    header += "ScanAOI, ";
    header += "ufovAngle, ";
    header += "openingAngle, ";
    header += "viewDistance, ";
    header += "otherAgents[{";
    header += "agentId | ";
    header += "posX | ";
    header += "posY | ";
    header += "rotation}], ";
    header += "crossingType, ";
    header += "crossingPhase, ";
    header += "fixationPoints[{";
    header += "FixationPointX | ";
    header += "FixationPointY}], ";
    header += "TrafficSignalss[TrafficSignalId]";

    return header;
}

std::string doubleToString(double input, int precision = 5) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(precision) << input;
    return stream.str();
}

void AgentStateRecorder::WriteOutputFile() {
    std::string path = "SimulationOutput.RunResults.RunResult.Cyclics";
    boost::property_tree::ptree valueTree;
    valueTree.put("SimulationOutput.<xmlattr>.SchemaVersion", "0.3.0");

    // adds stopping points to the output ptree
    boost::property_tree::ptree stoppingPointsTree;

    for (auto [intersectionId, lanemap] : record.infrastructurePerception->GetStoppingPointData().stoppingPoints) {
        boost::property_tree::ptree intersectionTree;
        intersectionTree.put("<xmlattr>.id", intersectionId);
        for (auto [laneId, pointmap] : lanemap) {
            boost::property_tree::ptree laneTree;
            laneTree.put("<xmlattr>.id", laneId);
            for (auto [type, point] : pointmap) {
                if (point.type != StoppingPointType::NONE) {
                    boost::property_tree::ptree stoppingPointTree;
                    stoppingPointTree.put("<xmlattr>.posX", doubleToString(point.posX));
                    stoppingPointTree.put("<xmlattr>.posY", doubleToString(point.posY));
                    stoppingPointTree.put("<xmlattr>.road", point.road->GetOpenDriveId());
                    stoppingPointTree.put("<xmlattr>.lane", point.lane->GetOpenDriveId());
                    stoppingPointTree.put("<xmlattr>.type", stoppingPointTypes[(int)type]);
                    laneTree.add_child("Point", stoppingPointTree);
                }
            }
            intersectionTree.add_child("Lane", laneTree);
        }
        stoppingPointsTree.add_child("Junction", intersectionTree);
    }

    valueTree.add_child("SimulationOutput.RunResults.RunResult.StoppingPoints", stoppingPointsTree);

    // Adds conflict points to the output ptree
    boost::property_tree::ptree conflictAreaTree;
    for (auto &conflictAreas : record.infrastructurePerception->GetConflictAreas()) {
        boost::property_tree::ptree conflictAreaJunctionTree;
        conflictAreaJunctionTree.put("<xmlattr>.id", conflictAreas.first);
        for (auto &conflictArea : conflictAreas.second) {
            boost::property_tree::ptree parameterTree;

            parameterTree.put("<xmlattr>.roadA", conflictArea.first.road->GetOpenDriveId());
            parameterTree.put("<xmlattr>.laneA", conflictArea.first.lane->GetOpenDriveId());

            parameterTree.put("<xmlattr>.roadB", conflictArea.second.road->GetOpenDriveId());
            parameterTree.put("<xmlattr>.laneB", conflictArea.second.lane->GetOpenDriveId());

            parameterTree.put("<xmlattr>.startA",
                              std::stoi(conflictArea.first.lane->GetOpenDriveId()) < 0
                                  ? doubleToString(conflictArea.first.start.sOffset)
                                  : doubleToString(conflictArea.first.lane->GetLength() - conflictArea.first.end.sOffset));
            parameterTree.put("<xmlattr>.endA",
                              std::stoi(conflictArea.first.lane->GetOpenDriveId()) < 0
                                  ? doubleToString(conflictArea.first.end.sOffset)
                                  : doubleToString(conflictArea.first.lane->GetLength() - conflictArea.first.start.sOffset));
            parameterTree.put("<xmlattr>.startB",
                              std::stoi(conflictArea.second.lane->GetOpenDriveId()) < 0
                                  ? doubleToString(conflictArea.second.start.sOffset)
                                  : doubleToString(conflictArea.second.lane->GetLength() - conflictArea.second.end.sOffset));
            parameterTree.put("<xmlattr>.endB",
                              std::stoi(conflictArea.second.lane->GetOpenDriveId()) < 0
                                  ? doubleToString(conflictArea.second.end.sOffset)
                                  : doubleToString(conflictArea.second.lane->GetLength() - conflictArea.second.start.sOffset));

            conflictAreaJunctionTree.add_child("ConflictArea", parameterTree);
        }
        conflictAreaTree.add_child("Junction", conflictAreaJunctionTree);
    }
    valueTree.add_child("SimulationOutput.RunResults.RunResult.ConflictAreas", conflictAreaTree);

    valueTree.add("SimulationOutput.RunResults.RunResult.Cyclics.Header", this->GenerateHeader());

    // Creates Samples for each timestep, each matching the structure given above
    boost::property_tree::ptree samplesTree;
    for (auto [time, values] : record.gazeStates) {
        boost::property_tree::ptree sampleTree;
        boost::property_tree::ptree agentTree;
        sampleTree.put("<xmlattr>.time", std::to_string(time));
        for (auto [agentId, values] : record.gazeStates.at(time)) {
            boost::property_tree::ptree agentTree;
            agentTree.put("<xmlattr>.id", agentId);
            agentTree.put_value(GenerateDataSet(time, agentId));
            sampleTree.add_child("A", agentTree);
        }
        samplesTree.add_child("Sample", sampleTree);
    }
    valueTree.add_child("SimulationOutput.RunResults.RunResult.Cyclics.Samples", samplesTree);

    boost::property_tree::xml_writer_settings<std::string> settings(' ', 2);
    boost::property_tree::write_xml(resultPath + "DReaMOutput.xml", valueTree, std::locale(), settings);
}
} // namespace AgentStateRecorder