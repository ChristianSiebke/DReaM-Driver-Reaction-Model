#include "agentstaterecorder.h"

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
void agentStateRecorder::addConflictPoints(std::vector<ConflictPoints> conflictPoints) {
    record.conflictPoints = conflictPoints;
}

std::string agentStateRecorder::generateDataSet(int time) {
    std::string outputLine;

    // list of all agent IDs
    for (auto [agentId, values] : record.gazeStates.at(time)) {
        outputLine += "{";

        // GazeType,Int,ufovAngle,openingAngle,viewDistance
        GazeState gazeState = record.gazeStates.at(time).at(agentId);
        std::string gazeType;

        switch (gazeState.fixationState.first) {
        case GazeType::NONE:
            gazeType = "NONE";
            break;
        case GazeType::ScanGlance:
            gazeType = "ScanGlance";
            break;
        case GazeType::ControlGlance:
            gazeType = "ControlGlance";
            break;
        case GazeType::ObserveGlance:
            gazeType = "ObserveGlance";
        default:
            break;
        }
        outputLine += gazeType;
        outputLine += ",";
        outputLine += std::to_string(gazeState.fixationState.second) += ",";
        outputLine += std::to_string(gazeState.ufovAngle) += ",";
        outputLine += std::to_string(gazeState.openingAngle) += ",";
        outputLine += std::to_string(gazeState.viewDistance) += ",";
        // TODO: catch for empty GazeType

        //[otherAgentId,double,double,double]
        outputLine += "[";
        for (auto agent : record.otherAgents.at(time).at(agentId)) {
            outputLine += std::to_string(std::get<0>(agent)) += ",";
            outputLine += std::to_string(std::get<1>(agent)) += ",";
            outputLine += std::to_string(std::get<2>(agent)) += ",";
            outputLine += std::to_string(std::get<3>(agent)) += "|";
            // TODO: kein | falls letztes element
        }

        outputLine += "],";

        // crossingType,crossingPhase
        auto crossingInfo = record.crossingInfos.at(time).at(agentId);
        std::string crossingType;
        std::string crossingPhase;

        switch (crossingInfo.type) {
        case CrossingType::NA:
            crossingType = "NA";
            break;
        case CrossingType::Left:
            crossingType = "Left";
            break;
        case CrossingType::Right:
            crossingType = "Right";
            break;
        case CrossingType::Straight:
            crossingType = "Straight";
            break;
        case CrossingType::Random:
            crossingType = "Random";
            break;
        default:
            break;
        }
        switch (crossingInfo.phase) {
        case CrossingPhase::Exit:
            crossingPhase = "Exit";
            break;
        case CrossingPhase::NONE:
            crossingPhase = "Exit";
            break;
        case CrossingPhase::Approach:
            crossingPhase = "Approach";
            break;
        case CrossingPhase::Crossing_Right:
            crossingPhase = "Crossing_Right";
            break;
        case CrossingPhase::Deceleration_ONE:
            crossingPhase = "Deceleration_ONE";
            break;
        case CrossingPhase::Deceleration_TWO:
            crossingPhase = "Deceleration_TWO";
            break;
        case CrossingPhase::Crossing_Left_ONE:
            crossingPhase = "Crossing_Left_ONE";
            break;
        case CrossingPhase::Crossing_Left_TWO:
            crossingPhase = "Crossing_Left_TWO";
            break;
        case CrossingPhase::Crossing_Straight:
            crossingPhase = "Crossing_Straight";
            break;
        default:
            break;
        }

        outputLine += crossingType;
        outputLine += ",";
        outputLine += crossingPhase;
        outputLine += ",";

        //[FixationPointX,FixationPointY]
        outputLine += "[";
        for (auto point : record.segmentControlFixationPoints.at(time).at(agentId)) {
            outputLine += std::to_string(point.x) += ",";
            outputLine += std::to_string(point.y) += "|";
            // TODO: kein | falls letztes element
        }
        outputLine += "]";
        outputLine += "}";
    }
    return outputLine;
}

std::string agentStateRecorder::generateHeader() {
    std::string header;
    for (auto [agentId, values] : record.gazeStates.at(0)) {
        std::string idString = std::to_string(agentId);
        header += idString;
        header += ":GazeType, ";
        header += idString;
        header += ":Int, ";
        header += idString;
        header += ":ufovAngle, ";
        header += idString;
        header += ":openingAngle, ";
        header += idString;
        header += ":viewDistance,";
        header += idString;
        header += ":agents[";
        header += "otherAgent, ";
        header += ":posX, ";
        header += ":posY, ";
        header += "rotation], ";
        header += idString;
        header += ":crossingType, ";
        header += idString;
        header += ":crossingPhase, [";
        header += idString;
        header += ":FixationPointX, ";
        header += idString;
        header += ":FixationPointY] ";
    }
    return header;
}

std::string agentStateRecorder::stoppingTypeToString(StoppingPointType type) {
    std::string typeString;
    switch (type) {
    case StoppingPointType::NONE:
        typeString = "NONE";
        break;
    case StoppingPointType::Pedestrian_Crossing_ONE:
        typeString = "Pedestrian_Crossing_ONE";
        break;
    case StoppingPointType::Pedestrian_Crossing_TWO:
        typeString = "Pedestrian_Crossing_TWO";
        break;
    case StoppingPointType::Pedestrian_Left:
        typeString = "Predestrian_Left";
        break;
    case StoppingPointType::Pedestrian_Right:
        typeString = "Pedestrian_Right";
        break;
    case StoppingPointType::Vehicle_Crossroad:
        typeString = "Vehicle_Crossroad";
        break;
    case StoppingPointType::Vehicle_Left:
        typeString = "Vehicle_Left";
        break;
    default:
        typeString = "";
        break;
    }
}

void agentStateRecorder::writeOutputFile() {
    std::cout << "confictPoints: " << record.conflictPoints.size() << std::endl;

    std::string path = "SimulationOutput.RunResults.RunResult.Cyclics";
    boost::property_tree::ptree valueTree;
    valueTree.put("SimulationOutput.<xmlattr>.SchemaVersion", "0.3.0");

    // adds stopping points to the output ptree
    std::cout << "stoppingPointsSize" << record.stoppingPointData.stoppingPoints.size() << std::endl;
    boost::property_tree::ptree stoppingPointsTree;
    boost::property_tree::ptree stoppingPointTree;
    for (auto [roadId, lanemap] : record.stoppingPointData.stoppingPoints) {
        for (auto [laneId, pointmap] : lanemap) {
            for (auto [type, point] : pointmap) {
                std::cout << "hello" << std::endl;
                stoppingPointTree.put("<xmlattr>.posX", point.posX);
                stoppingPointTree.put("<xmlattr>.posY", point.posY);
                stoppingPointTree.put("<xmlattr>.OdRoadId", roadId);
                stoppingPointTree.put("<xmlattr>.OdLaneId", point.lane->GetOpenDriveId());

                stoppingPointTree.put("xmlattr>.Type", stoppingTypeToString(type));

                stoppingPointsTree.add_child("StoppingPoint", stoppingPointTree);
            }
        }
    }
    valueTree.add_child("SimulationOutput.RunResults.RunResult.StoppingPoints", stoppingPointsTree);

    // Adds conflict points to the output ptree
    boost::property_tree::ptree conflictPointTree;
    for (ConflictPoints conflictPoint : record.conflictPoints) {
        boost::property_tree::ptree parameterTree;

        parameterTree.put("<xmlattr>.currentOdRoadId", conflictPoint.currentOpenDriveRoadId);
        parameterTree.put("<xmlattr>.currentOdLaneId", conflictPoint.currentOpenDriveLaneId);

        parameterTree.put("<xmlattr>.intersecOdRoadId", conflictPoint.junctionOpenDriveRoadId);
        parameterTree.put("<xmlattr>.intersecOdLaneId", conflictPoint.currentOpenDriveLaneId);

        parameterTree.put("<xmlattr>.startX", conflictPoint.start.x);
        parameterTree.put("<xmlattr>.startY", conflictPoint.start.y);
        parameterTree.put("<xmlattr>.endX", conflictPoint.end.x);
        parameterTree.put("<xmlattr>.endY", conflictPoint.end.y);

        conflictPointTree.add_child("ConflictPoint", parameterTree);
    }
    valueTree.add_child("SimulationOutput.RunResults.RunResult.ConflictPoints", conflictPointTree);

    valueTree.add("SimulationOutput.RunResults.RunResult.Cyclics.Header", this->generateHeader());

    // Creates Samples for each timestep, each matching the structure given above
    boost::property_tree::ptree samplesTree;
    for (auto [time, values] : record.gazeStates) {
        boost::property_tree::ptree sampleTree;
        sampleTree.put("<xmlattr>.Time", std::to_string(time));
        sampleTree.put_value(generateDataSet(time));
        samplesTree.add_child("Sample", sampleTree);
    }
    valueTree.add_child("SimulationOutput.RunResults.RunResult.Cyclics.Samples", samplesTree);

    boost::property_tree::xml_writer_settings<std::string> settings(' ', 2);
    boost::property_tree::write_xml(resultPath + "DReaMOutput.xml", valueTree, std::locale(), settings);
}
