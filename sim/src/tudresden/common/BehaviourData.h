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

//********** Program-specific Data **********
typedef std::string RoadID;
typedef std::string IntersectionID;
typedef std::map<IndicatorState, std::map<IntersectionSpot, std::shared_ptr<DReaM::NormalDistribution>>> TurningVelocitis;

struct ActionDecisionBehaviour {
    std::map<IntersectionID, std::map<RoadID, TurningVelocitis>> velocityStatisticsIntersection{};
    std::map<std::string, std::shared_ptr<DReaM::NormalDistribution>> velocityStatisticsSpecificRoads{};
    DReaM::NormalDistribution defaultVelocity{0, 0, 0, 0};
    double collisionImminentMargin;
    double minDistanceStationaryTraffic;
    double desiredFollowingTimeHeadway;
    DReaM::NormalDistribution comfortDeceleration{0, 0, 0, 0}; //! intelligent driver model (IDM) parameter
    double maxAcceleration;                                 //! intelligent driver model (IDM) parameter
    DReaM::NormalDistribution maxEmergencyDeceleration{0, 0, 0, 0}; //! boundary parameter of agent
    DReaM::LogNormalDistribution timeGapAcceptance{0, 0, 0, 0};     //! time gap
    int minTimeEmergencyBrakeIsActive;
};

struct CognitiveMapBehaviour {
    int memoryCapacity;
    int memorytime;
    DReaM::NormalDistribution initialPerceptionTime{0, 0, 0, 0};
    DReaM::NormalDistribution perceptionLatency{0, 0, 0, 0};

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
    std::shared_ptr<DReaM::NormalDistribution> observe_fixationDuration = nullptr;

    ScanAreasOfInterest scanAOIs;

    double std_probabilityFixateLeadCar;
    double std_probabilityControlGlance;
    std::map<ScanAOI, std::shared_ptr<DReaM::NormalDistribution>> std_scanAOIProbabilities{};

    double XInt_probabilityFixateLeadCar;
    double XInt_probabilityControlGlance;
    double XInt_viewingDepthIntoRoad;
    std::map<CrossingPhase, std::map<ControlAOI, double>> XInt_controlAOIProbabilities{};
    std::map<IndicatorState, std::map<CrossingPhase, std::map<ScanAOI, std::shared_ptr<DReaM::NormalDistribution>>>>
        XInt_scanAOIProbabilities{};

    double XInt_controlOpeningAngle;
    std::shared_ptr<DReaM::NormalDistribution> XInt_controlFixationDuration = nullptr;
};

struct BehaviourData {
    ActionDecisionBehaviour adBehaviour;
    CognitiveMapBehaviour cmBehaviour;
    GazeMovementBehaviour gmBehaviour;
};

#endif // BEHAVIOURDATA_H
