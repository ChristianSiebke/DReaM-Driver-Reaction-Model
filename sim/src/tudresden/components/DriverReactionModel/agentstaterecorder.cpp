#include "agentstaterecorder.h"



void agentStateRecorder::addStoppingPoints(int id, std::map<std::string,std::list<std::tuple<double, double>>> stoppingPointList){
    record.stoppingPoints.insert(std::make_pair(id,stoppingPointList));
}

void agentStateRecorder::addGazeStates(int time, int id, GazeState gazeState){
    if(record.gazeStates.find(time) == record.gazeStates.end()){
        std::map <int,GazeState> idMap;
        record.gazeStates.insert(std::make_pair(time,idMap));
    }
    record.gazeStates.at(time).insert(std::make_pair(id,gazeState));
}

void agentStateRecorder::addCrossingInfos(int time, int id, CrossingInfo info){
    if(record.crossingInfos.find(time) == record.crossingInfos.end()){
        std::map <int,CrossingInfo> idMap;
        record.crossingInfos.insert(std::make_pair(time,idMap));
    }
    record.crossingInfos.at(time).insert(std::make_pair(id,info));
}

void agentStateRecorder::addOtherAgents(int time, int id, std::vector<std::tuple<int, double, double, double>> agents){
    if(record.otherAgents.find(time) == record.otherAgents.end()){
        std::map <int,std::vector<std::tuple<int, double, double, double>>> idMap;
        record.otherAgents.insert(std::make_pair(time,idMap));
    }
    record.otherAgents.at(time).insert(std::make_pair(id,agents));
}
void agentStateRecorder::addFixationPoints(int time, int id, std::vector<Common::Vector2d> fixationPoints){
    if(record.segmentControlFixationPoints.find(time) == record.segmentControlFixationPoints.end()){
        std::map <int,std::vector<Common::Vector2d>> idMap;
        record.segmentControlFixationPoints.insert(std::make_pair(time,idMap));
    }
    record.segmentControlFixationPoints.at(time).insert(std::make_pair(id,fixationPoints));
}
void agentStateRecorder::addConflictPoints(std::vector<ConflictPoints> conflictPoints){
    record.conflictPoints = conflictPoints;
}


std::string agentStateRecorder::generateDataSet(int time){
    std::string outputLine;

    //list of all agent IDs
    for (auto [agentId, values] : record.gazeStates.at(time)){
        outputLine += "{";

        //GazeType,Int,ufovAngle,openingAngle,viewDistance
        GazeState gazeState = record.gazeStates.at(time).at(agentId);
        outputLine += magic_enum::enum_name(gazeState.fixationState.first);
        outputLine += ",";
        outputLine += std::to_string(gazeState.fixationState.second) += ",";
        outputLine += std::to_string(gazeState.ufovAngle) += ",";
        outputLine += std::to_string(gazeState.openingAngle) += ",";
        outputLine += std::to_string(gazeState.viewDistance) += ",";
            //TODO: catch for empty GazeType

        //[otherAgentId,double,double,double]
            outputLine += "[";
        for (auto agent : record.otherAgents.at(time).at(agentId)){

            outputLine += std::to_string(std::get<0>(agent)) += ",";
            outputLine += std::to_string(std::get<1>(agent)) += ",";
            outputLine += std::to_string(std::get<2>(agent)) += ",";
            outputLine += std::to_string(std::get<3>(agent)) += "|";
            // TODO: kein | falls letztes element
        }

        outputLine += "],";

        //crossingType,crossingPhase
        auto crossingInfo = record.crossingInfos.at(time).at(agentId);
        outputLine += magic_enum::enum_name(crossingInfo.type);
        outputLine += ",";
        outputLine += magic_enum::enum_name(crossingInfo.phase);


        //[FixationPointX,FixationPointY]
        outputLine += "[";
        for(auto point : record.segmentControlFixationPoints.at(time).at(agentId)){

            outputLine += std::to_string(point.x) += ",";
            outputLine += std::to_string(point.y) += "|";
            //TODO: kein | falls letztes element
        }
        outputLine += "]";
        outputLine += "}";
    }    
    return outputLine;
}

void agentStateRecorder::writeOutputFile() {
    std::string path = "SimulationOutput.RunResults.RunResult.Cyclics";
    boost::property_tree::ptree valueTree;
    valueTree.put("SimulationOutput.<xmlattr>.SchemaVersion","0.3.0");

    for(auto [agentId,pointMap]: record.stoppingPoints){

        boost::property_tree::ptree agentTree;
        boost::property_tree::ptree intersectionTree;
        boost::property_tree::ptree positionTree;

        agentTree.add("Agent.<xmlattr>.Id",std::to_string(agentId));


        for(auto [intersectionId,points]: pointMap){
            intersectionTree.add("Intersection.<xmlattr>",intersectionId);

            for(auto point : points){
                positionTree.put("Position.<xmlattr>.PosX",std::get<0>(point));
                positionTree.put("Position.<xmlattr>.PosY",std::get<1>(point));
                intersectionTree.add_child("Intersection",positionTree);
            }

        agentTree.add_child("Agent.StoppingPoint",intersectionTree);

        valueTree.add_child("SimulationOutput.RunResults.RunResult.StoppingPoints",agentTree);

        }
    }
    for(ConflictPoints conflictPoint : record.conflictPoints){

        boost::property_tree::ptree conflictPointTree;

        conflictPointTree.put("ConflictPoint.<xmlattr>.currentOdRoadId",conflictPoint.currentOpenDriveRoadId);
        conflictPointTree.put("ConflictPoint.<xmlattr>.currentOdLaneId",conflictPoint.currentOpenDriveLaneId);

        conflictPointTree.put("ConflictPoint.<xmlattr>.intersecOdRoadId",conflictPoint.junctionOpenDriveRoadId);
        conflictPointTree.put("ConflictPoint.<xmlattr>.intersecOdLaneId",conflictPoint.currentOpenDriveLaneId);

        conflictPointTree.put("ConflictPoint.<xmlattr>.startX",conflictPoint.start.x);
        conflictPointTree.put("ConflictPoint.<xmlattr>.startY",conflictPoint.start.y);
        conflictPointTree.put("ConflictPoint.<xmlattr>.endX",conflictPoint.end.x);
        conflictPointTree.put("ConflictPoint.<xmlattr>.endY",conflictPoint.end.y);

        valueTree.add_child("SimulationOutput.RunResults.RunResult.ConflictPoints",conflictPointTree);

    }

    std::string header;
    for( auto [agentId, values] :record.gazeStates.at(0)){
        std::string idString = std::to_string(agentId);
        header += idString;
        header += ":GazeType, ";
        header += idString;
        header += ":Int, ";
        header += idString;
        header += ":ufovAngle, ";
        header += idString ;
        header += ":openingAngle, ";
        header += idString ;
        header += ":viewDistance, [";
        header += idString ;
        header += ":otherAgent, ";
        header += idString ;
        header += ":double, ";
        header += idString ;
        header += ":double, ";
        header += idString ;
        header += ":double], ";
        header += idString;
        header += ":crossingType, ";
        header += idString ;
        header += ":crossingPhase, [";
        header += idString ;
        header += ":FixationPointX, ";
        header += idString;
        header += ":FixationPointY] ";
    }

    valueTree.add("SimulationOutput.RunResults.RunResult.Cyclics.Header",header);


    boost::property_tree::ptree samplesTree;

    for(auto [time,values] : record.gazeStates){

        boost::property_tree::ptree sampleTree;
        sampleTree.put("Sample",generateDataSet(time));
        sampleTree.put("<xmlattr>.Time",std::to_string(time));
        samplesTree.add_child("Samples.Sample",sampleTree);

        //valueTree.add_child(path,subtree);

    }
    valueTree.add_child(path,samplesTree);

    boost::property_tree::xml_writer_settings<std::string> settings (' ',2);
    boost::property_tree::write_xml(resultPath + "DReaMOutput.xml", valueTree, std::locale(), settings);
}
