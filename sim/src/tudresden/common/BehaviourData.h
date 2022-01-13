#ifndef BEHAVIOURDATA_H
#define BEHAVIOURDATA_H

#include <common/Definitions.h>
#include <map>
#include <memory>
#include <string>

//********** General XML Data **********

class StatisticsEntry {
public:
    virtual ~StatisticsEntry() {
    }
    std::string id = "0";
};
class DistributionEntry : public StatisticsEntry {
public:
    double mean;
    double std_deviation;
    double min;
    double max;

    DistributionEntry(double m, double stdd, double min, double max) : mean(m), std_deviation(stdd), min(min), max(max) {
    }
    virtual ~DistributionEntry() {
    }
};

class StandardDoubleEntry : public StatisticsEntry {
public:
    double value;

    StandardDoubleEntry(double v) : value(v) {
    }
    virtual ~StandardDoubleEntry() {
    }
};

struct StatisticsSet {
    std::string identifier;
    std::map<std::string, std::shared_ptr<StatisticsEntry>> entries;

    StatisticsSet(std::string id) : identifier(id) {
    }
};

struct StatisticsGroup {
    std::string identifier;
    std::map<std::string, StatisticsSet> sets;
    std::map<std::string, StatisticsGroup> groups;

    StatisticsGroup(std::string id) : identifier(id) {
    }
};

//********** Program-specific Data **********

struct ActionDecisionBehaviour {
    std::map<IndicatorState, std::map<CrossingPhase, std::shared_ptr<DistributionEntry>>> velocityStatistics;

    double collisionImminentMargin;
    double minDistanceStationaryTraffic;
    double desiredFollowingTimeHeadway;
    DistributionEntry comfortDeceleration{0, 0, 0, 0};      //! intelligent driver model (IDM) parameter
    double maxAcceleration;                                 //! intelligent driver model (IDM) parameter
    DistributionEntry maxEmergencyDeceleration{0, 0, 0, 0}; //! boundary parameter of agent
    double timeGapAcceptance;                               //! time gap
    int minTimeEmergencyBrakeIsActive;
};

struct CognitiveMapBehaviour {
    unsigned int memoryCapacity;
    int memorytime;
    DistributionEntry initialPerceptionTime{0, 0, 0, 0};
    DistributionEntry perceptionLatency{0, 0, 0, 0};
};

struct GazeMovementBehaviour {
    double std_probabilityFixateLeadCar;
    double std_probabilityControlGlance;
    std::map<ScanAOI, std::shared_ptr<DistributionEntry>> std_scanAOIProbabilities;

    double XInt_probabilityFixateLeadCar;
    double XInt_probabilityControlGlance;
    double XInt_viewingDepthIntoRoad;
    std::map<CrossingPhase, std::map<ControlAOI, double>> XInt_controlAOIProbabilities;
};

struct BehaviourData {
    ActionDecisionBehaviour adBehaviour;
    CognitiveMapBehaviour cmBehaviour;
    GazeMovementBehaviour gmBehaviour;
};

#endif // BEHAVIOURDATA_H
