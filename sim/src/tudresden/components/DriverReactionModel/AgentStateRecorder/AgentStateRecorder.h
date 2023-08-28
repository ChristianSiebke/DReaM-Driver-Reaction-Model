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

constexpr bool RECORD_DReaMOUTPUT = true;

class EXPORT AgentStateRecorder {
public:
    ~AgentStateRecorder() {
        std::cout << "AgentStateRecorder destroyed" << std::endl;
    }

    AgentStateRecorder(AgentStateRecorder const &) = delete;
    AgentStateRecorder &operator=(AgentStateRecorder const &) = delete;
    static std::shared_ptr<AgentStateRecorder> GetInstance(const std::string &resultPath) {
        if (!instance)
            instance = std::shared_ptr<AgentStateRecorder>(new AgentStateRecorder(resultPath));
        return instance;
    }

    static void ResetAgentStateRecorder() {
        instance.reset();
    }

    void AddInfrastructurePerception(std::shared_ptr<InfrastructurePerception> infrastructurePerception);

    static void BufferRuns(int runId) {
        if (RECORD_DReaMOUTPUT) {
            cyclesTree.add("Header", GenerateHeader());
            cyclesTree.add_child("Samples", samplesTree);
            runResultTree.add_child("Cyclics", std::move(cyclesTree));
            runResultTree.put("<xmlattr>.RunId", std::to_string(runId));
            runResultsTree.add_child("RunResult", std::move(runResultTree));
            cyclesTree.clear();
            samplesTree.clear();
            runResultTree.clear();
        }
    };

    static void BufferSamples(int time) {
        if (RECORD_DReaMOUTPUT) {
            sampleTree.put("<xmlattr>.time", std::to_string(time));
            samplesTree.add_child("Sample", sampleTree);
            agentTree.clear();
            sampleTree.clear();
        }
    };

    void BufferTimeStep(const int &agentId, const GazeState &gazeState, const std::vector<GeneralAgentPerception> &observedAgents,
                        const CrossingInfo &crossingInfo, const std::vector<Common::Vector2d> &segmentControlFixationPoints,
                        const std::unordered_map<DReaMId, MemorizedTrafficSignal> *trafficSignals);

    //! All information saved in the agentStateRecorder is written into an xml file
    static void WriteOutputFile();
    //! Generates a header matching and discribing the recorded datasets
    static std::string GenerateHeader();

private:
    AgentStateRecorder(const std::string &inResultPath) {
        resultPath = inResultPath;
    }

    boost::property_tree::ptree AddInfrastructureData(std::shared_ptr<InfrastructurePerception> infrastructure);
    std::string StoppingTypeToString(StoppingPointType);

    static std::shared_ptr<AgentStateRecorder> instance;
    static boost::property_tree::ptree simulationOutut;
    static std::string resultPath;
    static boost::property_tree::ptree agentTree;
    static boost::property_tree::ptree sampleTree;
    static boost::property_tree::ptree samplesTree;
    static boost::property_tree::ptree cyclesTree;
    static boost::property_tree::ptree runResultTree;
    static boost::property_tree::ptree runResultsTree;
    static boost::property_tree::ptree infrastuctureTree;
    std::shared_ptr<InfrastructurePerception> infrastructurePerception{nullptr};
};
} // namespace AgentStateRecorder
