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
    std::map<IntersectionID, std::map<RoadID, TurningVelocitis>> velocityStatisticsIntersection;
    std::map<std::string, std::shared_ptr<DReaM::NormalDistribution>> velocityStatisticsSpecificRoads;
    DReaM::NormalDistribution defaultVelocity{0, 0, 0, 0};
    double collisionImminentMargin{0};
    double minDistanceStationaryTraffic{0};
    double desiredFollowingTimeHeadway{0};
    DReaM::NormalDistribution comfortDeceleration{0, 0, 0, 0}; //! intelligent driver model (IDM) parameter
    double maxAcceleration{0};                                 //! intelligent driver model (IDM) parameter
    DReaM::NormalDistribution maxEmergencyDeceleration{0, 0, 0, 0}; //! boundary parameter of agent
    DReaM::LogNormalDistribution timeGapAcceptance{0, 0, 0, 0};     //! time gap
    int minTimeEmergencyBrakeIsActive{0};
};

struct CognitiveMapBehaviour {
    int memoryCapacity{0};
    int memorytime{0};
    DReaM::NormalDistribution initialPerceptionTime{0, 0, 0, 0};
    DReaM::NormalDistribution perceptionLatency{0, 0, 0, 0};

    int trafficSig_memoryCapacity{0};
    int trafficSig_memorytime{0};
};

struct ScanAreasOfInterest {
    std::map<ScanAOI, DriverGaze> driverAOIs;
    std::map<ScanAOI, MirrorGaze> mirrorAOIs;
};

struct GazeMovementBehaviour {
    double foresightTime{0};
    double minForesightDistance{0};
    double observe_openingAngle{0};
    std::shared_ptr<DReaM::NormalDistribution> observe_fixationDuration = nullptr;

    ScanAreasOfInterest scanAOIs;

    double std_probabilityFixateLeadCar{0};
    double std_probabilityControlGlance{0};
    std::map<ScanAOI, std::shared_ptr<DReaM::NormalDistribution>> std_scanAOIProbabilities;

    double XInt_probabilityFixateLeadCar{0};
    double XInt_probabilityControlGlance{0};
    double XInt_viewingDepthIntoRoad{0};
    std::map<CrossingPhase, std::map<ControlAOI, double>> XInt_controlAOIProbabilities;
    std::map<IndicatorState, std::map<CrossingPhase, std::map<ScanAOI, std::shared_ptr<DReaM::NormalDistribution>>>>
        XInt_scanAOIProbabilities{};

    double XInt_controlOpeningAngle{0};
    std::shared_ptr<DReaM::NormalDistribution> XInt_controlFixationDuration = nullptr;
};

struct BehaviourData {
    ActionDecisionBehaviour adBehaviour;
    CognitiveMapBehaviour cmBehaviour;
    GazeMovementBehaviour gmBehaviour;
};

#endif // BEHAVIOURDATA_H
