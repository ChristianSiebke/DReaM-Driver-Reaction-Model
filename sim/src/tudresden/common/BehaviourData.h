/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
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

    Distribution toDistribution() {
        Distribution d(mean, std_deviation, min, max);
        return d;
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
typedef std::string RoadID;
typedef std::string IntersectionID;
typedef std::map<IndicatorState, std::map<IntersectionSpot, std::shared_ptr<DistributionEntry>>> TurningVelocitis;

struct ActionDecisionBehaviour {
    std::map<IntersectionID, std::map<RoadID, TurningVelocitis>> velocityStatisticsIntersection{};
    std::map<std::string, std::shared_ptr<DistributionEntry>> velocityStatisticsSpecificRoads{};
    DistributionEntry defaultVelocity{0, 0, 0, 0};
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
    int memoryCapacity;
    int memorytime;
    DistributionEntry initialPerceptionTime{0, 0, 0, 0};
    DistributionEntry perceptionLatency{0, 0, 0, 0};

    int trafficSig_memoryCapacity;
    int trafficSig_memorytime;
};

struct ScanAreasOfInterest {
    std::map<ScanAOI, DriverGaze> driverAOIs{};
    std::map<ScanAOI, MirrorGaze> mirrorAOIs{};
};

struct GazeMovementBehaviour {
    double foresightTime;
    double minForesightDistance;
    double observe_openingAngle;
    std::shared_ptr<DistributionEntry> observe_fixationDuration = nullptr;

    ScanAreasOfInterest scanAOIs;

    double std_probabilityFixateLeadCar;
    double std_probabilityControlGlance;
    std::map<ScanAOI, std::shared_ptr<DistributionEntry>> std_scanAOIProbabilities{};

    double XInt_probabilityFixateLeadCar;
    double XInt_probabilityControlGlance;
    double XInt_viewingDepthIntoRoad;
    std::map<CrossingPhase, std::map<ControlAOI, double>> XInt_controlAOIProbabilities{};
    std::map<IndicatorState, std::map<TrafficDensity, std::map<CrossingPhase, std::map<ScanAOI, std::shared_ptr<DistributionEntry>>>>>
        XInt_scanAOIProbabilities{};

    double XInt_controlOpeningAngle;
    std::shared_ptr<DistributionEntry> XInt_controlFixationDuration = nullptr;
};

struct BehaviourData {
    ActionDecisionBehaviour adBehaviour;
    CognitiveMapBehaviour cmBehaviour;
    GazeMovementBehaviour gmBehaviour;
};

#endif // BEHAVIOURDATA_H
