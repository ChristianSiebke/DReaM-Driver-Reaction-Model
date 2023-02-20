/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#pragma once
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <iostream>
#include <map>
#include <mutex>
#include <sstream>
#include <thread>

#include "../Components/GazeMovement/RoadSegments/RoadSegmentInterface.h"
#include "Common/Definitions.h"
#include "Common/WorldRepresentation.h"

#ifdef QMAKE_BUILD
#define DReaMIMPORT
#define DReaMEXPORT
#else
#if defined(_WIN32) && !defined(NODLL)
#define DReaMIMPORT __declspec(dllimport)
#define DReaMEXPORT __declspec(dllexport)

#elif (defined(__GNUC__) && __GNUC__ >= 4 || defined(__clang__))
#define DReaMEXPORT __attribute__((visibility("default")))
#define DReaMIMPORT DReaMEXPORT

#else
#define DReaMIMPORT
#define DReaMEXPORT
#endif
#endif

#if defined(AgentStateRecorder_EXPORTS)
#define EXPORT DReaMEXPORT
#else
#define EXPORT DReaMIMPORT
#endif

namespace AgentStateRecorder {
using time = int;
using agentID = int;
//! Stores all collected data in the agentStateRecorder to be processed later
struct Record {
    //! For each timestep, maps a map containing each agent's GazeState to the associated timestep
    std::map<time, std::map<agentID, GazeState>> gazeStates;

    //! For each timestep, maps a map containing each agent's CrossingInfo to the associated timestep
    std::map<time, std::map<agentID, CrossingInfo>> crossingInfos;

    //! For each timestep, maps a map containing each agent's segmentControlFixationPoints to the associated timestep
    std::map<time, std::map<agentID, std::vector<Common::Vector2d>>> segmentControlFixationPoints;

    std::shared_ptr<InfrastructurePerception> infrastructurePerception;

    //! For each timestep, maps a map containing the Id and position of other known agents for each agent, to the associated timestep
    std::map<time, std::map<agentID, std::vector<GeneralAgentPerception>>> observedAgents;

    //! For each timestep, stores the traffic signals currently in memory of an agent
    std::map<time, std::map<agentID, std::vector<OdId>>> trafficSignalMemory;
};

/*!
 * \brief Saves the states of each agent
 *
 * This singleton class saves and later outputs all neccessary data from map and agents
 *
 */
class EXPORT AgentStateRecorder {
public:
    static std::shared_ptr<AgentStateRecorder> GetInstance(std::string resultPath) {
        if (!instance)
            instance = std::shared_ptr<AgentStateRecorder>(new AgentStateRecorder(resultPath));
        return instance;
    }

    static void ResetAgentStateRecorder() {
        instance.reset();
    }

    static void SetRunId(int invocation) {
        runId = invocation;
    }

    ~AgentStateRecorder() {
        BufferSimulationOutput();
    }

    AgentStateRecorder(AgentStateRecorder const &) = delete;
    AgentStateRecorder &operator=(AgentStateRecorder const &) = delete;

    void AddInfrastructurePerception(std::shared_ptr<InfrastructurePerception> infrastructurePerception);

    void AddStoppingPoints(StoppingPointData);

    /**
     * @brief 
     * 
     * @param time 
     * @param id 
     */
    void AddGazeStates(int time, int id, GazeState);

    void AddOtherAgents(int time, int id, std::vector<GeneralAgentPerception>);

    void AddCrossingInfos(int time, int id, CrossingInfo);

    void AddFixationPoints(int time, int id, std::vector<Common::Vector2d>);

    void AddTrafficSignals(int time, int id, std::unordered_map<DReaMId, MemorizedTrafficSignal> *);

    //! All information saved in the agentStateRecorder is written into an xml file
    static void WriteOutputFile();

private:
    AgentStateRecorder(std::string inResultPath) {
        resultPath = inResultPath;
    }

    void BufferSimulationOutput();
    boost::property_tree::ptree AddInfrastructureData() const;

    static std::shared_ptr<AgentStateRecorder> instance;
    static std::mutex mtx;
    static boost::property_tree::ptree simulationOutut;
    static std::string resultPath;
    Record record;
    static int runId;
    static bool infrastructureDataWritten;

    /*!
     * \brief Generates a string containing all information for one timestep
     *
     * All data from record describing the same state (time) are collected into a
     * single string with seperators.
     *
     * @param[in]     time           Requested simulation timestep
     */
    std::string GenerateDataSet(int time, int agentId);

    std::string StoppingTypeToString(StoppingPointType);

    //! Generates a header matching and discribing the recorded datasets
    std::string GenerateHeader();
};
} // namespace agentRecordState
