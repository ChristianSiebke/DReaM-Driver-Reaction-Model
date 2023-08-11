#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

#include "Common/Definitions.h"
#include "Common/PerceptionData.h"

#ifdef QMAKE_BUILD
#define ADRIMPORT
#define ADREXPORT
#else
#if defined(_WIN32) && !defined(NODLL)
#define ADRIMPORT __declspec(dllimport)
#define ADREXPORT __declspec(dllexport)

#elif (defined(__GNUC__) && __GNUC__ >= 4 || defined(__clang__))
#define ADREXPORT __attribute__((visibility("default")))
#define ADRIMPORT ADREXPORT

#else
#define ADRIMPORT
#define ADREXPORT
#endif
#endif

#if defined(AnalysisDataRecorder_EXPORTS)
#define DLL_EXPORT ADREXPORT
#else
#define DLL_EXPORT ADRIMPORT
#endif

namespace GlobalObserver {

struct TrajectoryData {
    std::vector<double> velocity;
    std::vector<double> timeHeadway;
};
struct GroupingData {
    std::string startRoadOdId;
    std::string endRoadOdId;
    DReaMDefinitions::AgentVehicleType agentType;
    bool secondJunction = false;
    int obstructionCounter = 0;
    int followingTarget = -1;

    bool obstructed = false;
    bool following = false;
    bool obstructed2 = false;
    bool following2 = false;
    // vector of: offset to velocity distribution mean in relation to std deviation
    std::vector<double> velocityProfile;
};

struct AgentData {
    int runId;
    int agentId;
    DReaMDefinitions::AgentVehicleType agentType;
    uint8_t group;
    TrajectoryData td;
};

struct TTCData {
    int runId;
    int egoId;
    int otherId;

    double minTTC;
};

struct CollisionData {
    int timestamp;
    int runId;
    int egoId;
    int otherId;
    int type;
};

class DLL_EXPORT AnalysisDataRecorder {
public:
    static std::shared_ptr<AnalysisDataRecorder> GetInstance() {
        if (!instance)
            instance = std::shared_ptr<AnalysisDataRecorder>(new AnalysisDataRecorder());
        return instance;
    }

    AnalysisDataRecorder() {
        observationStartS.insert(std::make_pair("3", 144));
        observationStartS.insert(std::make_pair("4", 255));
        observationStartS.insert(std::make_pair("1", 160));
        observationStartS.insert(std::make_pair("5", 220));

        observationEndS.insert(std::make_pair("3", 15));
        observationEndS.insert(std::make_pair("4", 15));
        observationEndS.insert(std::make_pair("1", 40));
        observationEndS.insert(std::make_pair("5", 12));
    }
    AnalysisDataRecorder(AnalysisDataRecorder const &) = delete;
    AnalysisDataRecorder &operator=(AnalysisDataRecorder const &) = delete;
    ~AnalysisDataRecorder() {
        BufferRun();
        totalTime += runtime;
    }

    /**
     * @brief Triggers an update of the internally stored shared representations as well as the logic for detecting and categorizing crash
     * events.
     *
     * @param time current time in the simulation
     */
    void Trigger(std::shared_ptr<DetailedAgentPerception> ego, AnalysisSignal data, int time);

    /**
     * @brief Resets the instance of AnalysisDataRecorder.
     *
     */
    static void Reset() {
        instance.reset();
    }

    /**
     * @brief Sets the current run ID.
     *
     */
    static void SetRunId(int invocation) {
        runId = invocation;
    }

    static void SetScenarioConfigPath(const std::string &path) {
        scenarioConfigPath = path;
    }

    /**
     * @brief Write the output files.
     *
     */
    static void WriteOutput();

    void CheckCollisions(std::vector<std::pair<int, std::shared_ptr<DetailedAgentPerception>>> partners, int egoId,
                         std::shared_ptr<DetailedAgentPerception> egoData, AnalysisSignal data, int time);

private:
    void CountExitVelocities(std::shared_ptr<DetailedAgentPerception> ego);
    void AddAgentTrajectoryDataPoint(std::shared_ptr<DetailedAgentPerception> ego, AnalysisSignal data);
    void UpdateGroupDataPoint(std::shared_ptr<DetailedAgentPerception> ego, AnalysisSignal data);
    void UpdateTTCs(std::shared_ptr<DetailedAgentPerception> ego, AnalysisSignal data);
    uint16_t ComputeGroup(GroupingData &data);
    static void ComputeExitDistributions(std::ofstream &file);
    static std::string GroupInfo(uint16_t group);
    int DetermineCollisionType(int egoId, int otherId, std::shared_ptr<DetailedAgentPerception> egoData,
                               std::shared_ptr<DetailedAgentPerception> partnerData, AnalysisSignal data);

    void BufferRun();

private:
    // singleton related fields
    static std::shared_ptr<AnalysisDataRecorder> instance;
    static int runId;
    static std::string scenarioConfigPath;

    // buffered data
    static std::map<uint16_t, std::shared_ptr<std::vector<AgentData>>> analysisData;
    static std::vector<TTCData> ttcData;
    static std::map<std::string, std::shared_ptr<std::map<DReaMDefinitions::AgentVehicleType, int>>> exitVehicleCounters;
    static std::map<std::string, std::shared_ptr<std::map<DReaMDefinitions::AgentVehicleType, std::shared_ptr<std::vector<double>>>>>
        exitVehicleVelocities;
    static std::vector<CollisionData> collisions;
    static int totalTime;

    std::map<int, TrajectoryData> trajectoryData{};
    std::map<int, GroupingData> groupingData{};
    std::map<int, bool> relevantAgents{};
    std::map<int, const MentalInfrastructure::Lane *> lastLane{};
    std::map<int, const MentalInfrastructure::Road *> lastRoad{};
    std::map<int, uint64_t> lastS{};
    std::map<int, std::shared_ptr<std::map<int, double>>> minTTCs{};
    std::map<int, std::shared_ptr<std::list<int>>> collisionPartners{};
    std::map<int, AnalysisSignal> analysisSignalLog{};

    std::map<std::string, double> observationStartS{};
    std::map<std::string, double> observationEndS{};

    int runtime = 0;

    int obstructionCounterLimit = 9;
    double minTTCUpperBound = 3.0;
    double trajectorySampleRate = 5.0; // every x meters
};

} // namespace GlobalObserver
