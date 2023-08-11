/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#include "AgentStateRecorder.h"

namespace AgentStateRecorder {
std::shared_ptr<AgentStateRecorder> AgentStateRecorder::instance = nullptr;
boost::property_tree::ptree AgentStateRecorder::simulationOutut = {};
std::string AgentStateRecorder::resultPath = "";
boost::property_tree::ptree AgentStateRecorder::cyclesTree;
boost::property_tree::ptree AgentStateRecorder::agentTree;
boost::property_tree::ptree AgentStateRecorder::sampleTree;
boost::property_tree::ptree AgentStateRecorder::samplesTree;
boost::property_tree::ptree AgentStateRecorder::runResultTree;
boost::property_tree::ptree AgentStateRecorder::runResultsTree;
boost::property_tree::ptree AgentStateRecorder::infrastuctureTree;

// string representations of enum values, can easily be accessed using array[(int) enum_value]

const std::string gazeTypes[] = {"NONE", "ScanGlance", "ObserveGlance", "ControlGlance"};
const std::string scanAOIs[] = {"NONE",
                                "Right",
                                "Straight",
                                "Left",
                                "InnerRVM",
                                "OuterLeftRVM",
                                "OuterRightRVM",
                                "Dashboard",
                                "Other",
                                "ShoulderCheckRight",
                                "ShoulderCheckLeft"};
const std::string controlAOI[] = {"NONE", "Right", "Left", "Oncoming"};

const std::string crossingTypes[] = {"NA", "Left", "Right", "Straight", "Random"};
const std::string crossingPhases[] = {
    "NONE",           "Approach", "Deceleration_ONE", "Deceleration_TWO", "Crossing_Left_ONE", "Crossing_Left_TWO", "Crossing_Straight",
    "Crossing_Right", "Exit"};
const std::string stoppingPointTypes[] = {
    "NONE",         "Pedestrian_Right", "Pedestrian_Left", "Pedestrian_Crossing_ONE", "Pedestrian_Crossing_TWO",
    "Vehicle_Left", "Vehicle_Crossroad"};

void AgentStateRecorder::AddInfrastructurePerception(std::shared_ptr<InfrastructurePerception> infrastructure) {
    if (RECORD_DReaMOUTPUT) {
        if (infrastuctureTree.empty())
            infrastuctureTree = AddInfrastructureData(infrastructure);
    }
}

void AgentStateRecorder::BufferTimeStep(const int &agentId, const GazeState &gazeState,
                                        const std::vector<GeneralAgentPerception> &observedAgents, const CrossingInfo &crossingInfo,
                                        const std::vector<Common::Vector2d> &segmentControlFixationPoints,
                                        const std::unordered_map<DReaMId, MemorizedTrafficSignal> *trafficSignals) {
    if (RECORD_DReaMOUTPUT) {
        std::string outputLine;

        // gaze information:
        // <GazeType>, <ScanAOI>, <ufovAngle>, <openingAngle>, <viewDistance>
        outputLine += gazeTypes[static_cast<int>(gazeState.fixationState.first)] + ",";
        if (gazeState.fixationState.first == GazeType::ScanGlance) {
            outputLine += scanAOIs[gazeState.fixationState.second] + ",";
        }
        else if (gazeState.fixationState.first == GazeType::ControlGlance) {
            outputLine += controlAOI[gazeState.fixationState.second] + ",";
        }
        else if (gazeState.fixationState.first == GazeType::ObserveGlance) {
            outputLine += "Agent:" + std::to_string(gazeState.fixationState.second) + ",";
        }
        else {
            const std::string msg =
                "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " Gaze state can not be logged";
            throw std::runtime_error(msg);
        }

        outputLine += std::to_string(gazeState.ufovAngle) += ",";
        outputLine += std::to_string(gazeState.openingAngle) += ",";
        outputLine += std::to_string(gazeState.viewDistance) += ",";
        // TODO catch for empty GazeType

        // other agents as perceived by the current:
        // [{<id> | <x> | <y> | <yaw>} | { ... }]
        outputLine += "[";
        for (const auto &agent : observedAgents) {
            outputLine += "{";
            outputLine += std::to_string(agent.id) += " | ";
            outputLine += std::to_string(agent.refPosition.x) += " | ";
            outputLine += std::to_string(agent.refPosition.y) += " | ";
            outputLine += std::to_string(agent.yaw) += "}";
        }
        outputLine += "],";

        // crossingType & crossingPhase:
        // <crossingType>, <crossingPhase>
        outputLine += crossingTypes[(int)crossingInfo.type] + ",";
        outputLine += crossingPhases[(int)crossingInfo.phase] + ",";

        // fixation points:
        // [{<x> | <y>} | { ... }]
        outputLine += "[";
        for (const auto &point : segmentControlFixationPoints) {
            outputLine += ("{");
            outputLine += std::to_string(point.x) += " | ";
            outputLine += std::to_string(point.y) += "}";
        }
        outputLine += "],";

        // memorized traffic signals:
        // [<TrafficSignalOdId> | ...]
        outputLine += "[";
        for (auto iter = (*trafficSignals).begin(); iter != (*trafficSignals).end(); iter++) {
            outputLine += iter->second.trafficSignal->GetOpenDriveId();
            auto nextIter = std::next(iter, 1);
            if (nextIter != (*trafficSignals).end()) {
                outputLine += " | ";
            }
        }

        outputLine += "]";

        agentTree.put("<xmlattr>.id", agentId);
        agentTree.put_value(std::move(outputLine));
        sampleTree.add_child("Agent", agentTree);
    }
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
    header += "TrafficSignals[TrafficSignalId]";

    return header;
}

std::string doubleToString(double input, int precision = 5) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(precision) << input;
    return stream.str();
}

void AgentStateRecorder::WriteOutputFile() {
    if (RECORD_DReaMOUTPUT) {
        simulationOutut.put("SimulationOutput.<xmlattr>.SchemaVersion", "0.3.0");
        simulationOutut.add_child("SimulationOutput.InfrastructureData", std::move(infrastuctureTree));
        simulationOutut.add_child("SimulationOutput.RunResults", std::move(runResultsTree));

        boost::property_tree::xml_writer_settings<std::string> settings(' ', 2);
        boost::property_tree::write_xml(resultPath + "DReaMOutput.xml", simulationOutut, std::locale(), settings);
    }
}

boost::property_tree::ptree AgentStateRecorder::AddInfrastructureData(std::shared_ptr<InfrastructurePerception> infrastructurePerception) {
    boost::property_tree::ptree infrastructureData;
    boost::property_tree::ptree stoppingPointsTree;

    for (auto [intersectionId, lanemap] : infrastructurePerception->GetStoppingPointData().stoppingPoints) {
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

    infrastructureData.add_child("StoppingPoints", stoppingPointsTree);

    // Adds conflict points to the output ptree
    boost::property_tree::ptree conflictAreaTree;
    for (auto &conflictAreas : infrastructurePerception->GetConflictAreas()) {
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
    infrastructureData.add_child("ConflictAreas", conflictAreaTree);
    return infrastructureData;
}
} // namespace AgentStateRecorder