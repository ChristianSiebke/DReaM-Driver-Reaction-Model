#ifndef AGENTSTATERECORDER_H
#define AGENTSTATERECORDER_H
#include <map>
#include "RoadSegmentInterface.h"
#include "Definitions.h"
#include <iostream>
#include "Common/libs/magic_enum.hpp"


#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>


struct Record{
    std::map<int,std::map<std::string,std::list<std::tuple<double,double>>>> stoppingPoints;

    std::map<int,std::map<int, GazeState>> gazeStates;

    std::map<int,std::map<int,std::vector<std::tuple<int, double, double, double>>>> otherAgents;

    std::map<int,std::map<int, CrossingInfo>> crossingInfos;

    std::map<int,std::map<int,std::vector<Common::Vector2d>>> segmentControlFixationPoints;

    std::vector<ConflictPoints> conflictPoints;

};


class agentStateRecorder
{
public:
    static agentStateRecorder& getInstance(){

        static agentStateRecorder instance;

        return instance;
    }
private:
    agentStateRecorder(){}

    ~agentStateRecorder(){writeOutputFile();}

public:
    agentStateRecorder(agentStateRecorder const&) = delete;

    void operator=(agentStateRecorder const&)     = delete;

    void addStoppingPoints(int,std::map<std::string,std::list<std::tuple<double,double>>>);

    void addGazeStates(int,int,GazeState);

    void addOtherAgents(int,int,std::vector<std::tuple<int, double, double, double>>);

    void addCrossingInfos(int,int,CrossingInfo);

    void addFixationPoints(int,int,std::vector<Common::Vector2d>);

    void addConflictPoints(std::vector<ConflictPoints>);

    //TODO GAZEPOINTS!!11! (segmentfixationpoints)



private:
    Record record;

    void printAgentStates();

    std::string generateDataSet(int time);

    std::string gazeStateOutput(GazeState gazeState);

    void writeOutputFile();

};



#endif // AGENTSTATERECORDER_H
