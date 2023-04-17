#include <map>
#include <vector>

#include "Common/Definitions.h"
#include "Common/PerceptionData.h"

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

#if defined(AnalysisDataRecorder_EXPORTS)
#define EXPORT DReaMEXPORT
#else
#define EXPORT DReaMIMPORT
#endif

namespace GlobalObserver {

class AnalysisDataRecorder {
public:
    AnalysisDataRecorder() = default;
    AnalysisDataRecorder(AnalysisDataRecorder const &) = delete;
    AnalysisDataRecorder &operator=(AnalysisDataRecorder const &) = delete;
    // TODO destructor!!

    void Trigger(std::shared_ptr<DetailedAgentPerception> ego, std::shared_ptr<InfrastructurePerception> infrastructure);

private:
    void CountExitVelocities(std::shared_ptr<DetailedAgentPerception> ego);
    void ComputeExitDistributions();

private:
    std::map<std::string, std::shared_ptr<std::map<DReaMDefinitions::AgentVehicleType, int>>> exitVehicleCounters;
    std::map<std::string, std::shared_ptr<std::map<DReaMDefinitions::AgentVehicleType, std::shared_ptr<std::vector<double>>>>>
        exitVehicleVelocities;
};

} // namespace GlobalObserver