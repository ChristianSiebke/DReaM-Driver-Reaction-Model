#ifndef AGENTSTATERECORDER_H
#define AGENTSTATERECORDER_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <iostream>
#include <map>
#include <sstream>
#include <thread>

#include "Common/Definitions.h"
#include "Common/libs/magic_enum.hpp"
#include "Components/GazeMovement/RoadSegments/RoadSegmentInterface.h"

//! Stores all collected data in the agentStateRecorder to be processed later
struct Record {
    //! Maps  intersections to their respective stoppingPoints on that intersection
    StoppingPointData stoppingPointData;

    //! For each timestep, maps a map containing each agent's GazeState to the associated timestep
    std::map<int, std::map<int, GazeState>> gazeStates;

    //! For each timestep, maps a map containing the Id and position of other known agents for each agent, to the associated timestep
    std::map<int, std::map<int, std::vector<std::tuple<int, double, double, double>>>> otherAgents;

    //! For each timestep, maps a map containing each agent's CrossingInfo to the associated timestep
    std::map<int, std::map<int, CrossingInfo>> crossingInfos;

    //! For each timestep, maps a map containing each agent's segmentControlFixationPoints to the associated timestep
    std::map<int, std::map<int, std::vector<Common::Vector2d>>> segmentControlFixationPoints;

    //! Contains all existant ConflictPoints;
    std::vector<ConflictPoint> conflictPoints;
};

/*!
 * \brief Saves the states of each agent
 *
 * This singleton class saves and later outputs all neccessary data from map and agents
 *
 */
class agentStateRecorder {
public:
    static agentStateRecorder &getInstance(std::string resultPath) {
        static agentStateRecorder instance(resultPath);

        return instance;
    }

private:
    agentStateRecorder(std::string resultPath) : resultPath(resultPath) {
    }

    ~agentStateRecorder() {
        writeOutputFile();
    }

public:
    agentStateRecorder(agentStateRecorder const &) = delete;

    void operator=(agentStateRecorder const &) = delete;

    void addStoppingPoints(StoppingPointData);

    void addGazeStates(int, int, GazeState);

    void addOtherAgents(int, int, std::vector<std::tuple<int, double, double, double>>);

    void addCrossingInfos(int, int, CrossingInfo);

    void addFixationPoints(int, int, std::vector<Common::Vector2d>);

    void addConflictPoints(std::vector<ConflictPoint>);

private:
    std::string resultPath;
    Record record;

    /*!
     * \brief Generates a string containing all information for one timestep
     *
     * All data from record describing the same state (time) are collected into a
     * single string with seperators.
     *
     * @param[in]     time           Requested simulation timestep
     */
    std::string generateDataSet(int time, int agentId);

    std::string stoppingTypeToString(StoppingPointType);

    //! Generates a header matching and discribing the recorded datasets
    std::string generateHeader();

    //! All information saved in the agentStateRecorder is written into an xml file
    void writeOutputFile();
};

#endif // AGENTSTATERECORDER_H