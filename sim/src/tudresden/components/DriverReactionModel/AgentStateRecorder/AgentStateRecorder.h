
#pragma once
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <iostream>
#include <map>
#include <sstream>
#include <thread>

#include "../Components/GazeMovement/RoadSegments/RoadSegmentInterface.h"
#include "Common/Definitions.h"

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
    std::map<time, std::map<agentID, std::vector<AgentPerception>>> observedAgents;
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
        std::cout << "AgentStateRecorder count: " << instance.use_count() << std::endl;
        instance.reset();
    }

    ~AgentStateRecorder() {
        WriteOutputFile();
        std::cout << " AgentStateRecorderdestroyed" << std::endl;
    }

private:
    AgentStateRecorder(std::string resultPath) : resultPath(resultPath) {
    }

public:
    AgentStateRecorder(AgentStateRecorder const &) = delete;
    AgentStateRecorder &operator=(AgentStateRecorder const &) = delete;

    void AddInfrastructurePerception(std::shared_ptr<InfrastructurePerception> infrastructurePerception);

    void AddStoppingPoints(StoppingPointData);

    void AddGazeStates(int, int, GazeState);

    void AddOtherAgents(int, int, std::vector<AgentPerception>);

    void AddCrossingInfos(int, int, CrossingInfo);

    void AddFixationPoints(int, int, std::vector<Common::Vector2d>);

private:
    static std::shared_ptr<AgentStateRecorder> instance;
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
    std::string GenerateDataSet(int time, int agentId);

    std::string StoppingTypeToString(StoppingPointType);

    //! Generates a header matching and discribing the recorded datasets
    std::string GenerateHeader();

    //! All information saved in the agentStateRecorder is written into an xml file
    void WriteOutputFile();
};
} // namespace agentRecordState