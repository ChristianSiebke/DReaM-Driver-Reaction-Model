#include "agentstaterecorder.h"

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

void agentStateRecorder::addStoppingPoints(StoppingPointData stoppingPointData) {
    // TODO if clause -> only do it if empty
    record.stoppingPointData = stoppingPointData;
}

void agentStateRecorder::addGazeStates(int time, int id, GazeState gazeState) {
    if (record.gazeStates.find(time) == record.gazeStates.end()) {
        std::map<int, GazeState> idMap;
        record.gazeStates.insert(std::make_pair(time, idMap));
    }
    record.gazeStates.at(time).insert(std::make_pair(id, gazeState));
}

void agentStateRecorder::addCrossingInfos(int time, int id, CrossingInfo info) {
    if (record.crossingInfos.find(time) == record.crossingInfos.end()) {
        std::map<int, CrossingInfo> idMap;
        record.crossingInfos.insert(std::make_pair(time, idMap));
    }
    record.crossingInfos.at(time).insert(std::make_pair(id, info));
}

void agentStateRecorder::addOtherAgents(int time, int id, std::vector<std::tuple<int, double, double, double>> agents) {
    if (record.otherAgents.find(time) == record.otherAgents.end()) {
        std::map<int, std::vector<std::tuple<int, double, double, double>>> idMap;
        record.otherAgents.insert(std::make_pair(time, idMap));
    }
    record.otherAgents.at(time).insert(std::make_pair(id, agents));
}
void agentStateRecorder::addFixationPoints(int time, int id, std::vector<Common::Vector2d> fixationPoints) {
    if (record.segmentControlFixationPoints.find(time) == record.segmentControlFixationPoints.end()) {
        std::map<int, std::vector<Common::Vector2d>> idMap;
        record.segmentControlFixationPoints.insert(std::make_pair(time, idMap));
    }
    record.segmentControlFixationPoints.at(time).insert(std::make_pair(id, fixationPoints));
}

void agentStateRecorder::addConflictPoints(std::vector<ConflictPoint> conflictPoints) {
    record.conflictPoints = conflictPoints;
}

std::string agentStateRecorder::generateDataSet(int time, int agentId) {
    std::string outputLine;

    // GazeType,Int,ufovAngle,openingAngle,viewDistance
    GazeState gazeState = record.gazeStates.at(time).at(agentId);

    outputLine += gazeTypes[(int)gazeState.fixationState.first] + ",";
    outputLine += scanAOIs[gazeState.fixationState.second] + ",";
    outputLine += std::to_string(gazeState.ufovAngle) += ",";
    outputLine += std::to_string(gazeState.openingAngle) += ",";
    outputLine += std::to_string(gazeState.viewDistance) += ",";
    // TODO: catch for empty GazeType

    //[otherAgentId,double,double,double]
    outputLine += "[";
    for (auto agent : record.otherAgents.at(time).at(agentId)) {
        outputLine += "{";
        outputLine += std::to_string(std::get<0>(agent)) += " | ";
        outputLine += std::to_string(std::get<1>(agent)) += " | ";
        outputLine += std::to_string(std::get<2>(agent)) += " | ";
        outputLine += std::to_string(std::get<3>(agent)) += "}";
    }
    outputLine += "],";

    // crossingType,crossingPhase
    auto crossingInfo = record.crossingInfos.at(time).at(agentId);

    outputLine += crossingTypes[(int)crossingInfo.type] + ",";
    outputLine += crossingPhases[(int)crossingInfo.phase] + ",";

    //[FixationPointX,FixationPointY]
    outputLine += "[";
    for (auto point : record.segmentControlFixationPoints.at(time).at(agentId)) {
        outputLine += ("{");
        outputLine += std::to_string(point.x) += " | ";
        outputLine += std::to_string(point.y) += "}";
    }
    outputLine += "]";
    outputLine += "}";

    return outputLine;
}

std::string agentStateRecorder::generateHeader() {
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
    header += "FixationPointY}]";

    return header;
}

void agentStateRecorder::writeOutputFile() {
    std::cout << "confictAreas: " << record.conflictPoints.size() << std::endl;

    std::string path = "SimulationOutput.RunResults.RunResult.Cyclics";
    boost::property_tree::ptree valueTree;
    valueTree.put("SimulationOutput.<xmlattr>.SchemaVersion", "0.3.0");

    // adds stopping points to the output ptree
    std::cout << "stoppingPointsSize " << record.stoppingPointData.stoppingPoints.size() << std::endl;
    // std::cout << "YEEE HAW" << std::endl;
    boost::property_tree::ptree stoppingPointsTree;

    for (auto [intersectionId, lanemap] : record.stoppingPointData.stoppingPoints) {
        // std::cout << "for 1: here 1" << std::endl;

        if (!&lanemap) {
            // std::cout << "alarm" << std::endl;
        }

        boost::property_tree::ptree intersectionTree;
        intersectionTree.put("<xmlattr>.Id", intersectionId);

        // std::cout << "for 1: here 2" << std::endl;

        for (auto [laneId, pointmap] : lanemap) {
            if (!&pointmap) {
                // std::cout << "alaAAArm" << std::endl;
            }

            // std::cout << "for 2: here 1" << std::endl;

            boost::property_tree::ptree laneTree;
            laneTree.put("<xmlattr>.Id", laneId);

            for (auto [type, point] : pointmap) {
                if (point.type != StoppingPointType::NONE) {
                    auto stoppingPointTree = new boost::property_tree::ptree();
                    stoppingPointTree->put("<xmlattr>.posX", point.posX);
                    stoppingPointTree->put("<xmlattr>.posY", point.posY);
                    stoppingPointTree->put("<xmlattr>.RoadId", point.road->GetOpenDriveId());
                    stoppingPointTree->put("<xmlattr>.LaneId", point.lane->GetOpenDriveId());
                    stoppingPointTree->put("<xmlattr>.Type", stoppingPointTypes[(int)type]);

                    // std::cout << "for 3: here 1" << std::endl;

                    laneTree.add_child("Point", *stoppingPointTree);

                    // std::cout << "for 3: here 2" << std::endl;
                }
            }

            // std::cout << "for 2: here 2" << std::endl;

            intersectionTree.add_child("Lane", laneTree);

            // std::cout << "for 2: here 3" << std::endl;
        }

        // std::cout << "for 1: here 3" << std::endl;

        stoppingPointsTree.add_child("Intersection", intersectionTree);
    }

    valueTree.add_child("SimulationOutput.RunResults.RunResult.StoppingPoints", stoppingPointsTree);

    // Adds conflict points to the output ptree
    boost::property_tree::ptree conflictPointTree;
    for (ConflictPoint conflictPoint : record.conflictPoints) {
        boost::property_tree::ptree parameterTree;

        parameterTree.put("<xmlattr>.currentOdRoadId", conflictPoint.currentOpenDriveRoadId);
        parameterTree.put("<xmlattr>.currentOdLaneId", conflictPoint.currentOpenDriveLaneId);

        parameterTree.put("<xmlattr>.intersectOdRoadId", conflictPoint.junctionOpenDriveRoadId);
        parameterTree.put("<xmlattr>.intersectOdLaneId", conflictPoint.junctionOpenDriveLaneId);

        parameterTree.put("<xmlattr>.startSa", conflictPoint.currentStartS);
        parameterTree.put("<xmlattr>.endSa", conflictPoint.currentEndS);
        parameterTree.put("<xmlattr>.startSb", conflictPoint.otherStartS);
        parameterTree.put("<xmlattr>.endSb", conflictPoint.otherEndS);

        conflictPointTree.add_child("ConflictArea", parameterTree);
    }
    valueTree.add_child("SimulationOutput.RunResults.RunResult.ConflictAreas", conflictPointTree);

    valueTree.add("SimulationOutput.RunResults.RunResult.Cyclics.Header", this->generateHeader());

    // Creates Samples for each timestep, each matching the structure given above
    boost::property_tree::ptree samplesTree;
    for (auto [time, values] : record.gazeStates) {
        boost::property_tree::ptree sampleTree;
        boost::property_tree::ptree agentTree;
        sampleTree.put("<xmlattr>.Time", std::to_string(time));
        for (auto [agentId, values] : record.gazeStates.at(time)) {
            boost::property_tree::ptree agentTree;
            agentTree.put("<xmlattr>.Id", agentId);
            agentTree.put_value(generateDataSet(time, agentId));
            sampleTree.add_child("A", agentTree);
        }
        samplesTree.add_child("Sample", sampleTree);
    }
    valueTree.add_child("SimulationOutput.RunResults.RunResult.Cyclics.Samples", samplesTree);

    std::cout << "test" << std::endl;

    boost::property_tree::xml_writer_settings<std::string> settings(' ', 2);
    boost::property_tree::write_xml(resultPath + "DReaMOutput.xml", valueTree, std::locale(), settings);
}
