/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#include "BehaviourConverter.h"

std::map<DReaMDefinitions::AgentVehicleType, std::shared_ptr<BehaviourData>>
BehaviourConverter::Convert(const DReaM::StatisticsGroup &main) {
    for (const auto &group : main.groups) {
        if (group.first == "Car") {
            auto data = std::make_shared<BehaviourData>();
            auto agentType = DReaMDefinitions::AgentVehicleType::Car;
            data = ConvertActionDecisionParameters(data, group.second.groups.at("ActionDecision"));
            data = ConvertCognitiveMapParameters(data, group.second.groups.at("CognitiveMap"));
            data = ConvertGazeMovementParameters(data, group.second.groups.at("GazeMovement"));
            behaviourMap.insert({agentType, data});
        }
        else if (group.first == "Cyclist") {
            auto data = std::make_shared<BehaviourData>();
            auto agentType = DReaMDefinitions::AgentVehicleType::Bicycle;
            data = ConvertActionDecisionParameters(data, group.second.groups.at("ActionDecision"));
            data = ConvertCognitiveMapParameters(data, group.second.groups.at("CognitiveMap"));
            data = ConvertGazeMovementParameters(data, group.second.groups.at("GazeMovement"));
            behaviourMap.insert({agentType, data});
        }
        else if (group.first == "Motorbike") {
            auto data = std::make_shared<BehaviourData>();
            auto agentType = DReaMDefinitions::AgentVehicleType::Motorbike;
            data = ConvertActionDecisionParameters(data, group.second.groups.at("ActionDecision"));
            data = ConvertCognitiveMapParameters(data, group.second.groups.at("CognitiveMap"));
            data = ConvertGazeMovementParameters(data, group.second.groups.at("GazeMovement"));
            behaviourMap.insert({agentType, data});
        }
        else if (group.first == "Pedestrian") {
            auto data = std::make_shared<BehaviourData>();
            auto agentType = DReaMDefinitions::AgentVehicleType::Pedestrian;
            data = ConvertActionDecisionParameters(data, group.second.groups.at("ActionDecision"));
            data = ConvertCognitiveMapParameters(data, group.second.groups.at("CognitiveMap"));
            data = ConvertGazeMovementParameters(data, group.second.groups.at("GazeMovement"));
            behaviourMap.insert({agentType, data});
        }
        else if (group.first == "Truck") {
            auto data = std::make_shared<BehaviourData>();
            auto agentType = DReaMDefinitions::AgentVehicleType::Truck;
            data = ConvertActionDecisionParameters(data, group.second.groups.at("ActionDecision"));
            data = ConvertCognitiveMapParameters(data, group.second.groups.at("CognitiveMap"));
            data = ConvertGazeMovementParameters(data, group.second.groups.at("GazeMovement"));
            behaviourMap.insert({agentType, data});
        }
        else {
            throw std::runtime_error(" AgentType groupe in behaviour.xml is missing ");
        }
    }
    return std::move(behaviourMap);
}

std::shared_ptr<BehaviourData> BehaviourConverter::ConvertActionDecisionParameters(std::shared_ptr<BehaviourData> data,
                                                                                   const DReaM::StatisticsGroup &main) {
    std::string key;
    try {
        data = ConvertActionDecisionStatistics(data, main.groups.at("Velocity Statistics"));
        key = "BehaviourParameters";
        const DReaM::StatisticsSet &params = main.sets.at(key);
        key = "collisionImminentMargin";
        data->adBehaviour.collisionImminentMargin = std::static_pointer_cast<DReaM::StandardDoubleEntry>(params.entries.at(key))->value;
        if (data->adBehaviour.collisionImminentMargin < 0)
            throw std::logic_error(" collisionImminentMargin must be a positive value! ");
        key = "comfortDeceleration";
        data->adBehaviour.comfortDeceleration = *std::static_pointer_cast<DReaM::NormalDistribution>(params.entries.at(key));
        if (data->adBehaviour.comfortDeceleration.mean >= 0)
            throw std::logic_error(" comfortDeceleration mean must be a negative value! ");
        if (data->adBehaviour.comfortDeceleration.max > 0)
            throw std::logic_error(" comfortDeceleration max must be a negative value! ");
        if (data->adBehaviour.comfortDeceleration.min > 0)
            throw std::logic_error(" comfortDeceleration min must be a negative value! ");
        key = "maxAcceleration";
        data->adBehaviour.maxAcceleration = std::static_pointer_cast<DReaM::StandardDoubleEntry>(params.entries.at(key))->value;
        if (data->adBehaviour.maxAcceleration < 0)
            throw std::logic_error(" maxAcceleration must be a positive value! ");
        key = "minDistanceStationaryTraffic";
        data->adBehaviour.minDistanceStationaryTraffic =
            std::static_pointer_cast<DReaM::StandardDoubleEntry>(params.entries.at(key))->value;
        if (data->adBehaviour.minDistanceStationaryTraffic < 0)
            throw std::logic_error(" minDistanceStationaryTraffic must be a positive value! ");
        key = "desiredFollowingTimeHeadway";
        data->adBehaviour.desiredFollowingTimeHeadway = std::static_pointer_cast<DReaM::StandardDoubleEntry>(params.entries.at(key))->value;
        if (data->adBehaviour.desiredFollowingTimeHeadway < 0)
            throw std::logic_error(" desiredFollowingTimeHeadway must be a positive value! ");
        key = "maxEmergencyDeceleration";
        data->adBehaviour.maxEmergencyDeceleration = *std::static_pointer_cast<DReaM::NormalDistribution>(params.entries.at(key));
        if (data->adBehaviour.maxEmergencyDeceleration.mean > 0)
            throw std::logic_error(" maxEmergencyDeceleration mean must be a negative value! ");
        if (data->adBehaviour.maxEmergencyDeceleration.max > 0)
            throw std::logic_error(" maxEmergencyDeceleration max must be a negative value! ");
        if (data->adBehaviour.maxEmergencyDeceleration.min > 0)
            throw std::logic_error(" maxEmergencyDeceleration min must be a negative value! ");
        key = "timeGapAcceptance";
        data->adBehaviour.timeGapAcceptance = *std::static_pointer_cast<DReaM::LogNormalDistribution>(params.entries.at(key));
        if (data->adBehaviour.timeGapAcceptance.sigma < 0 || data->adBehaviour.timeGapAcceptance.max < 0 ||
            data->adBehaviour.timeGapAcceptance.min < 0 || data->adBehaviour.timeGapAcceptance.mu < 0)
            throw std::logic_error(" timeGapAcceptance values must be positive! ");
        key = "minTimeEmergencyBrakeIsActive";
        data->adBehaviour.minTimeEmergencyBrakeIsActive =
            static_cast<int>(std::static_pointer_cast<DReaM::StandardDoubleEntry>(params.entries.at(key))->value * 1000);
        if (data->adBehaviour.minTimeEmergencyBrakeIsActive < 0)
            throw std::logic_error(" minTimeEmergencyBrakeIsActive must be a positive value! ");

        return data;
    } catch (const std::out_of_range& oor) {
        std::string message = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) +
                              " ConfigFile: Missing BehaviourParameter/s in Behaviour config file: cannot find " + key + " | " + oor.what();
        Log(message, error);
        throw std::runtime_error(message);
    } catch (const std::logic_error& er) {
        std::string message = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " | " + er.what();
        Log(message, error);
        throw std::runtime_error(message);
    }
}

std::shared_ptr<BehaviourData> BehaviourConverter::ConvertActionDecisionStatistics(std::shared_ptr<BehaviourData> data,
                                                                                   const DReaM::StatisticsGroup &main) {
    std::map<IntersectionSpot, std::shared_ptr<DReaM::NormalDistribution>> intersectionSpotMap;
    TurningVelocitis indicatorMap;
    std::map<RoadID, TurningVelocitis> approachRoadMap;
    for (auto &group : main.groups) {
        IndicatorState ind;
        IntersectionSpot interEntry;
        if (group.first == "IntersectionID") {
            for (const auto &group2 : group.second.groups) {
                if (group2.first == "default") {
                    for (const auto &set : group2.second.sets) {
                        if (set.first == "IndicatorOff") {
                            ind = IndicatorState::IndicatorState_Off;
                        }
                        else if (set.first == "IndicatorLeft") {
                            ind = IndicatorState::IndicatorState_Left;
                        }
                        else if (set.first == "IndicatorRight") {
                            ind = IndicatorState::IndicatorState_Right;
                        }
                        for (const auto &entry : set.second.entries) {
                            if (entry.first == "IntersectionEntry") {
                                interEntry = IntersectionSpot::IntersectionEntry;
                            }
                            else if (entry.first == "IntersectionExit") {
                                interEntry = IntersectionSpot::IntersectionExit;
                            }
                            intersectionSpotMap.insert({interEntry, std::static_pointer_cast<DReaM::NormalDistribution>(entry.second)});
                            if (std::static_pointer_cast<DReaM::NormalDistribution>(entry.second)->mean < 0)
                                throw std::logic_error(" action decision distribution mean velocity must be a positive value! ");
                            if (std::static_pointer_cast<DReaM::NormalDistribution>(entry.second)->max < 0)
                                throw std::logic_error(" action decision distribution max velocity must be a positive value! ");
                            if (std::static_pointer_cast<DReaM::NormalDistribution>(entry.second)->min < 0)
                                throw std::logic_error(" action decision distribution min velocity must be a positive value! ");
                        }
                        indicatorMap.insert({ind, intersectionSpotMap});
                        intersectionSpotMap.clear();
                    }
                    approachRoadMap.insert({"default", indicatorMap});
                    indicatorMap.clear();
                    data->adBehaviour.velocityStatisticsIntersection.insert({"default", approachRoadMap});
                    approachRoadMap.clear();
                }
                else {
                    IntersectionID interID = group2.first;
                    for (const auto &group3 : group2.second.groups) {
                        for (const auto &group4 : group3.second.groups) {
                            RoadID roadID = group4.first;
                            for (const auto &set : group4.second.sets) {
                                if (set.first == "IndicatorOff") {
                                    ind = IndicatorState::IndicatorState_Off;
                                }
                                else if (set.first == "IndicatorLeft") {
                                    ind = IndicatorState::IndicatorState_Left;
                                }
                                else if (set.first == "IndicatorRight") {
                                    ind = IndicatorState::IndicatorState_Right;
                                }
                                for (const auto &entry : set.second.entries) {
                                    if (entry.first == "IntersectionEntry") {
                                        interEntry = IntersectionSpot::IntersectionEntry;
                                    }
                                    else if (entry.first == "IntersectionExit") {
                                        interEntry = IntersectionSpot::IntersectionExit;
                                    }
                                    intersectionSpotMap.insert(
                                        {interEntry, std::static_pointer_cast<DReaM::NormalDistribution>(entry.second)});
                                    if (std::static_pointer_cast<DReaM::NormalDistribution>(entry.second)->mean < 0)
                                        throw std::logic_error(" action decision distribution mean velocity must be a positive value! ");
                                    if (std::static_pointer_cast<DReaM::NormalDistribution>(entry.second)->max < 0)
                                        throw std::logic_error(" action decision distribution max velocity must be a positive value! ");
                                    if (std::static_pointer_cast<DReaM::NormalDistribution>(entry.second)->min < 0)
                                        throw std::logic_error(" action decision distribution min velocity must be a positive value! ");
                                }
                                indicatorMap.insert({ind, intersectionSpotMap});
                                intersectionSpotMap.clear();
                            }
                            approachRoadMap.insert({roadID, indicatorMap});
                            indicatorMap.clear();
                        }
                    }
                    data->adBehaviour.velocityStatisticsIntersection.insert({interID, approachRoadMap});
                    approachRoadMap.clear();
                }
            }
        }
        for (const auto &set : main.sets) {
            if (set.first == "default") {
                data->adBehaviour.defaultVelocity = *std::static_pointer_cast<DReaM::NormalDistribution>(set.second.entries.at("default"));
                if (data->adBehaviour.defaultVelocity.mean < 0)
                    throw std::logic_error(" action decision distribution mean default velocity must be a positive value! ");
                if (data->adBehaviour.defaultVelocity.max < 0)
                    throw std::logic_error(" action decision distribution max default velocity must be a positive value! ");
                if (data->adBehaviour.defaultVelocity.min < 0)
                    throw std::logic_error(" action decision distribution min default velocity must be a positive value! ");
                continue;
            }

            for (const auto &entry : set.second.entries) {
                if (set.first == "SpecificRoads") {
                    data->adBehaviour.velocityStatisticsSpecificRoads.insert(
                        {entry.first, std::static_pointer_cast<DReaM::NormalDistribution>(entry.second)});
                    if (std::static_pointer_cast<DReaM::NormalDistribution>(entry.second)->mean < 0)
                        throw std::logic_error(" mean velocity for specific road must be a positive value! ");
                    if (std::static_pointer_cast<DReaM::NormalDistribution>(entry.second)->max < 0)
                        throw std::logic_error(" max velocity for specific road must be a positive value!  ");
                    if (std::static_pointer_cast<DReaM::NormalDistribution>(entry.second)->min < 0)
                        throw std::logic_error(" min velocity for specific road must be a positive value!  ");
                    continue;
                }
            }
        }
    }
    return data;
}

std::shared_ptr<BehaviourData> BehaviourConverter::ConvertCognitiveMapParameters(std::shared_ptr<BehaviourData> data,
                                                                                 const DReaM::StatisticsGroup &main) {
    std::string key;
    try {
        key = "BehaviourParameters";
        const DReaM::StatisticsSet &params = main.sets.at(key);
        key = "memorytime";
        data->cmBehaviour.memorytime =
            static_cast<int>(std::static_pointer_cast<DReaM::StandardDoubleEntry>(params.entries.at(key))->value);
        if (data->cmBehaviour.memorytime < 0)
            throw std::logic_error(" memorytime must be a positive value! ");
        key = "memoryCapacity";
        data->cmBehaviour.memoryCapacity =
            static_cast<int>(std::static_pointer_cast<DReaM::StandardDoubleEntry>(params.entries.at(key))->value);
        if (data->cmBehaviour.memoryCapacity < 0)
            throw std::logic_error(" memoryCapacity must be a positive value! ");
        key = "perceptionLatency";
        data->cmBehaviour.perceptionLatency = *std::static_pointer_cast<DReaM::NormalDistribution>(params.entries.at(key));
        if (data->cmBehaviour.perceptionLatency.mean < 0)
            throw std::logic_error(" perceptionLatency mean must be a positive value! ");
        if (data->cmBehaviour.perceptionLatency.max < 0)
            throw std::logic_error(" perceptionLatency max must be a positive value! ");
        if (data->cmBehaviour.perceptionLatency.min < 0)
            throw std::logic_error(" perceptionLatency min must be a positive value! ");
        key = "initialPerceptionTime";
        data->cmBehaviour.initialPerceptionTime = *std::static_pointer_cast<DReaM::NormalDistribution>(params.entries.at(key));
        if (data->cmBehaviour.initialPerceptionTime.mean < 0)
            throw std::logic_error(" initialPerceptionTime mean must be a positive value! ");
        if (data->cmBehaviour.initialPerceptionTime.max < 0)
            throw std::logic_error(" initialPerceptionTime max must be a positive value! ");
        if (data->cmBehaviour.initialPerceptionTime.min < 0)
            throw std::logic_error(" initialPerceptionTime min must be a positive value! ");

        key = "TrafficSignalMemoryParameters";
        const DReaM::StatisticsSet &tsParams = main.sets.at(key);
        key = "memorytime";
        data->cmBehaviour.trafficSig_memorytime =
            static_cast<int>(std::static_pointer_cast<DReaM::StandardDoubleEntry>(tsParams.entries.at(key))->value);
        if (data->cmBehaviour.trafficSig_memorytime < 0)
            throw std::logic_error(" memorytime must be a positive value! ");
        key = "memoryCapacity";
        data->cmBehaviour.trafficSig_memoryCapacity =
            static_cast<int>(std::static_pointer_cast<DReaM::StandardDoubleEntry>(tsParams.entries.at(key))->value);
        if (data->cmBehaviour.trafficSig_memoryCapacity < 0)
            throw std::logic_error(" memoryCapacity must be a positive value! ");
        return data;
    }
    catch (const std::out_of_range &oor) {
        std::string message = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) +
                              " ConfigFile: Missing BehaviourParameter/s in Behaviour config file: cannot find " + key + " | " + oor.what();
        Log(message, error);
        throw std::runtime_error(message);
    }
    catch (const std::logic_error &er) {
        std::string message = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " | " + er.what();
        Log(message, error);
        throw std::runtime_error(message);
    }
}

std::shared_ptr<BehaviourData> BehaviourConverter::ConvertGazeMovementParameters(std::shared_ptr<BehaviourData> data,
                                                                                 const DReaM::StatisticsGroup &main) {
    data->gmBehaviour.XInt_controlAOIProbabilities.insert({CrossingPhase::Approach, {}});
    data->gmBehaviour.XInt_controlAOIProbabilities.insert({CrossingPhase::Deceleration_ONE, {}});
    data->gmBehaviour.XInt_controlAOIProbabilities.insert({CrossingPhase::Deceleration_TWO, {}});
    data->gmBehaviour.XInt_controlAOIProbabilities.insert({CrossingPhase::Crossing_Right, {}});
    data->gmBehaviour.XInt_controlAOIProbabilities.insert({CrossingPhase::Crossing_Straight, {}});
    data->gmBehaviour.XInt_controlAOIProbabilities.insert({CrossingPhase::Crossing_Left_ONE, {}});
    data->gmBehaviour.XInt_controlAOIProbabilities.insert({CrossingPhase::Crossing_Left_TWO, {}});

    std::string key;
    try {
        key = "ScanAreasOfInterest";
        DReaM::StatisticsGroup scanAOIs = main.groups.at(key);
        key = "DriverGaze";
        DReaM::StatisticsGroup driverGaze = scanAOIs.groups.at(key);
        for (auto &set : driverGaze.sets) {
            ScanAOI scan;
            if (set.first == "Left") {
                scan = ScanAOI::Left;
            }
            else if (set.first == "Straight") {
                scan = ScanAOI::Straight;
            }
            else if (set.first == "Right") {
                scan = ScanAOI::Right;
            }
            else if (set.first == "Dashboard") {
                scan = ScanAOI::Dashboard;
            }
            else if (set.first == "Other") {
                scan = ScanAOI::Other;
            }
            else {
                continue;
            }
            DriverGaze dr;
            dr.direction = std::static_pointer_cast<DReaM::StandardDoubleEntry>(set.second.entries.at("direction"))->value;
            dr.openingAngle = std::static_pointer_cast<DReaM::StandardDoubleEntry>(set.second.entries.at("openingAngle"))->value;
            if (dr.openingAngle < 0)
                throw std::logic_error(" DriverGaze -> openingAngle must be a positive value! ");
            dr.fixationDuration = *std::static_pointer_cast<DReaM::NormalDistribution>(set.second.entries.at("fixationDuration"));

            data->gmBehaviour.scanAOIs.driverAOIs.insert(std::make_pair(scan, dr));
        }

        key = "MirrorGaze";
        DReaM::StatisticsGroup mirrorGaze = scanAOIs.groups.at(key);
        for (auto &set : mirrorGaze.sets) {
            ScanAOI scan;
            if (set.first == "Left") {
                scan = ScanAOI::OuterLeftRVM;
            }
            else if (set.first == "Inside") {
                scan = ScanAOI::InnerRVM;
            }
            else if (set.first == "Right") {
                scan = ScanAOI::OuterRightRVM;
            }
            else {
                continue;
            }
            MirrorGaze mir;
            mir.direction = std::static_pointer_cast<DReaM::StandardDoubleEntry>(set.second.entries.at("direction"))->value;
            mir.openingAngle = std::static_pointer_cast<DReaM::StandardDoubleEntry>(set.second.entries.at("openingAngle"))->value;
            if (mir.openingAngle < 0)
                throw std::logic_error(" MirrorGaze -> openingAngle must be a positive value! ");
            mir.pos.x = std::static_pointer_cast<DReaM::StandardDoubleEntry>(set.second.entries.at("mirrorPosX"))->value;
            mir.pos.y = std::static_pointer_cast<DReaM::StandardDoubleEntry>(set.second.entries.at("mirrorPosY"))->value;
            mir.fixationDuration = *std::static_pointer_cast<DReaM::NormalDistribution>(set.second.entries.at("fixationDuration"));

            data->gmBehaviour.scanAOIs.mirrorAOIs.insert(std::make_pair(scan, mir));
        }

        key = "BaseParameters";
        DReaM::StatisticsSet base = main.sets.at(key);
        data->gmBehaviour.foresightTime = std::static_pointer_cast<DReaM::StandardDoubleEntry>(base.entries.at("foresightTime"))->value;
        if (data->gmBehaviour.XInt_controlOpeningAngle < 0)
            throw std::logic_error(" GazeMovement -> foresightTime must be a positive value! ");
        data->gmBehaviour.minForesightDistance =
            std::static_pointer_cast<DReaM::StandardDoubleEntry>(base.entries.at("minForesightDistance"))->value;
        if (data->gmBehaviour.XInt_controlOpeningAngle < 0)
            throw std::logic_error(" GazeMovement ->  minForesightDistance must be a positive value! ");

        key = "AgentObserveParameters";
        DReaM::StatisticsSet obs = main.sets.at(key);
        data->gmBehaviour.observe_openingAngle =
            std::static_pointer_cast<DReaM::StandardDoubleEntry>(obs.entries.at("openingAngle"))->value;
        if (data->gmBehaviour.XInt_controlOpeningAngle < 0)
            throw std::logic_error(" GazeMovement -> AgentObserveParameters -> openingAngle must be a positive value! ");
        data->gmBehaviour.observe_fixationDuration =
            std::static_pointer_cast<DReaM::NormalDistribution>(obs.entries.at("fixationDuration"));

        key = "Standard Road";
        DReaM::StatisticsGroup standardRoad = main.groups.at(key);
        key = "XJunction";
        DReaM::StatisticsGroup XJunction = main.groups.at(key);
        key = "BehaviourParameters";
        DReaM::StatisticsSet std_params = standardRoad.sets.at(key);
        DReaM::StatisticsSet XInt_params = XJunction.sets.at(key);
        key = "probabilityFixateLeadCar";
        data->gmBehaviour.std_probabilityFixateLeadCar =
            static_cast<int>(std::static_pointer_cast<DReaM::StandardDoubleEntry>(std_params.entries.at(key))->value);
        if (data->gmBehaviour.std_probabilityFixateLeadCar < 0)
            throw std::logic_error(" standard road -> probabilityFixateLeadCar must be a positive value! ");
        data->gmBehaviour.XInt_probabilityFixateLeadCar =
            static_cast<int>(std::static_pointer_cast<DReaM::StandardDoubleEntry>(XInt_params.entries.at(key))->value);
        if (data->gmBehaviour.XInt_probabilityFixateLeadCar < 0)
            throw std::logic_error(" XJunction -> probabilityFixateLeadCar must be a positive value! ");

        key = "probabilityControlGlance";
        data->gmBehaviour.std_probabilityControlGlance =
            static_cast<int>(std::static_pointer_cast<DReaM::StandardDoubleEntry>(std_params.entries.at(key))->value);
        if (data->gmBehaviour.std_probabilityControlGlance < 0)
            throw std::logic_error(" standard road -> probabilityControlGlance must be a positive value! ");
        data->gmBehaviour.XInt_probabilityControlGlance =
            static_cast<int>(std::static_pointer_cast<DReaM::StandardDoubleEntry>(XInt_params.entries.at(key))->value);
        if (data->gmBehaviour.XInt_probabilityControlGlance < 0)
            throw std::logic_error(" XJunction -> probabilityControlGlance must be a positive value! ");

        key = "viewingDepthIntoRoad";
        data->gmBehaviour.XInt_viewingDepthIntoRoad =
            static_cast<int>(std::static_pointer_cast<DReaM::StandardDoubleEntry>(XInt_params.entries.at(key))->value);
        if (data->gmBehaviour.XInt_viewingDepthIntoRoad < 0)
            throw std::logic_error(" XJunction -> viewingDepthIntoRoad must be a positive value! ");

        key = "ControlGlanceParameters";
        DReaM::StatisticsSet ctrl = XJunction.sets.at(key);
        data->gmBehaviour.XInt_controlOpeningAngle =
            std::static_pointer_cast<DReaM::StandardDoubleEntry>(ctrl.entries.at("openingAngle"))->value;
        if (data->gmBehaviour.XInt_controlOpeningAngle < 0)
            throw std::logic_error(" XJunction -> ControlGlanceParameters -> openingAngle must be a positive value! ");
        data->gmBehaviour.XInt_controlFixationDuration =
            std::static_pointer_cast<DReaM::NormalDistribution>(ctrl.entries.at("fixationDuration"));

        key = "ScanAOIProbabilities";
        DReaM::StatisticsSet std_scan = standardRoad.sets.at(key);

        for (auto &entry : std_scan.entries) {
            ScanAOI sc;
            if (entry.first == "Scan Dashboard") {
                sc = ScanAOI::Dashboard;
            }
            else if (entry.first == "Scan Straight") {
                sc = ScanAOI::Straight;
            }
            else if (entry.first == "Scan Left") {
                sc = ScanAOI::Left;
            }
            else if (entry.first == "Scan Right") {
                sc = ScanAOI::Right;
            }
            else if (entry.first == "Scan Other") {
                sc = ScanAOI::Other;
            }
            else {
                continue;
            }
            data->gmBehaviour.std_scanAOIProbabilities.insert(
                std::make_pair(sc, std::static_pointer_cast<DReaM::NormalDistribution>(entry.second)));
            if (std::static_pointer_cast<DReaM::NormalDistribution>(entry.second)->mean < 0)
                throw std::logic_error(" gaze state scan AOI distribution mean must be a positive value! ");
            if (std::static_pointer_cast<DReaM::NormalDistribution>(entry.second)->max < 0)
                throw std::logic_error(" gaze state scan AOI distribution max must be a positive value! ");
            if (std::static_pointer_cast<DReaM::NormalDistribution>(entry.second)->min < 0)
                throw std::logic_error(" gaze state scan AOI distribution min must be a positive value! ");
        }

        key = "ControlAOIProbabilities";
        DReaM::StatisticsGroup XInt_control = XJunction.groups.at(key);

        for (auto &set : XInt_control.sets) {
            CrossingPhase cr;
            if (set.first == "Phase APP") {
                cr = CrossingPhase::Approach;
            }
            else if (set.first == "Phase DEC1") {
                cr = CrossingPhase::Deceleration_ONE;
            }
            else if (set.first == "Phase DEC2") {
                cr = CrossingPhase::Deceleration_TWO;
            }
            else if (set.first == "Phase CR_S") {
                cr = CrossingPhase::Crossing_Straight;
            }
            else if (set.first == "Phase CR_R") {
                cr = CrossingPhase::Crossing_Right;
            }
            else if (set.first == "Phase CR_L1") {
                cr = CrossingPhase::Crossing_Left_ONE;
            }
            else if (set.first == "Phase CR_L2") {
                cr = CrossingPhase::Crossing_Left_TWO;
            }
            else {
                continue;
            }

            for (auto &entry : set.second.entries) {
                ControlAOI con;
                if (entry.first == "Control Left") {
                    con = ControlAOI::Left;
                }
                else if (entry.first == "Control Oncoming") {
                    con = ControlAOI::Oncoming;
                }
                else if (entry.first == "Control Right") {
                    con = ControlAOI::Right;
                }
                else {
                    continue;
                }
                data->gmBehaviour.XInt_controlAOIProbabilities.at(cr).insert(
                    std::make_pair(con, std::static_pointer_cast<DReaM::StandardDoubleEntry>(entry.second)->value));
                if (std::static_pointer_cast<DReaM::StandardDoubleEntry>(entry.second)->value < 0)
                    throw std::logic_error(" XJunction -> Control AOI probability must be a positive value! ");
            }
        }

        std::map<CrossingPhase, std::map<ScanAOI, std::shared_ptr<DReaM::NormalDistribution>>> tmp;
        tmp.insert({CrossingPhase::Approach, {}});
        tmp.insert({CrossingPhase::Deceleration_ONE, {}});
        tmp.insert({CrossingPhase::Deceleration_TWO, {}});
        tmp.insert({CrossingPhase::Crossing_Right, {}});
        tmp.insert({CrossingPhase::Crossing_Straight, {}});
        tmp.insert({CrossingPhase::Crossing_Left_ONE, {}});
        tmp.insert({CrossingPhase::Crossing_Left_TWO, {}});
        tmp.insert({CrossingPhase::Exit, {}});

        data->gmBehaviour.XInt_scanAOIProbabilities.insert({IndicatorState::IndicatorState_Left, tmp});
        data->gmBehaviour.XInt_scanAOIProbabilities.insert({IndicatorState::IndicatorState_Off, tmp});
        data->gmBehaviour.XInt_scanAOIProbabilities.insert({IndicatorState::IndicatorState_Right, tmp});

        key = "ScanAOIProbabilities";
        DReaM::StatisticsGroup XInt_scan = XJunction.groups.at(key);

        for (auto &indicatorGroup : XInt_scan.groups) {
            IndicatorState ind;
            if (indicatorGroup.first == "IndicatorLeft") {
                ind = IndicatorState::IndicatorState_Left;
            }
            else if (indicatorGroup.first == "IndicatorOff") {
                ind = IndicatorState::IndicatorState_Off;
            }
            else if (indicatorGroup.first == "IndicatorRight") {
                ind = IndicatorState::IndicatorState_Right;
            }
            else {
                continue;
            }
            for (auto &phaseSet : indicatorGroup.second.sets) {
                CrossingPhase cr;
                if (phaseSet.first == "Phase APP") {
                    cr = CrossingPhase::Approach;
                }
                else if (phaseSet.first == "Phase DEC1") {
                    cr = CrossingPhase::Deceleration_ONE;
                }
                else if (phaseSet.first == "Phase DEC2") {
                    cr = CrossingPhase::Deceleration_TWO;
                }
                else if (phaseSet.first == "Phase CR_S") {
                    cr = CrossingPhase::Crossing_Straight;
                }
                else if (phaseSet.first == "Phase CR_R") {
                    cr = CrossingPhase::Crossing_Right;
                }
                else if (phaseSet.first == "Phase CR_L1") {
                    cr = CrossingPhase::Crossing_Left_ONE;
                }
                else if (phaseSet.first == "Phase CR_L2") {
                    cr = CrossingPhase::Crossing_Left_TWO;
                }
                else if (phaseSet.first == "Phase EX") {
                    cr = CrossingPhase::Exit;
                }
                else {
                    continue;
                }

                for (auto &entry : phaseSet.second.entries) {
                    ScanAOI sc;
                    if (entry.first == "Scan Dashboard") {
                        sc = ScanAOI::Dashboard;
                    }
                    else if (entry.first == "Scan Straight") {
                        sc = ScanAOI::Straight;
                    }
                    else if (entry.first == "Scan Left") {
                        sc = ScanAOI::Left;
                    }
                    else if (entry.first == "Scan Right") {
                        sc = ScanAOI::Right;
                    }
                    else if (entry.first == "Scan Other") {
                        sc = ScanAOI::Other;
                    }
                    else {
                        continue;
                    }
                    data->gmBehaviour.XInt_scanAOIProbabilities.at(ind).at(cr).insert(
                        std::make_pair(sc, std::static_pointer_cast<DReaM::NormalDistribution>(entry.second)));
                    if (std::static_pointer_cast<DReaM::NormalDistribution>(entry.second)->mean < 0)
                        throw std::logic_error(" gaze state scan AOI distribution mean must be a positive value! ");
                    if (std::static_pointer_cast<DReaM::NormalDistribution>(entry.second)->max < 0)
                        throw std::logic_error(" gaze state scan AOI distribution max must be a positive value! ");
                    if (std::static_pointer_cast<DReaM::NormalDistribution>(entry.second)->min < 0)
                        throw std::logic_error(" gaze state scan AOI distribution min must be a positive value! ");
                }
            }
        }
        return data;
    }
    catch (const std::out_of_range &oor) {
        std::string message = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) +
                              " ConfigFile: Missing BehaviourParameter/s in Behaviour config file: cannot find " + key + " | " + oor.what();
        Log(message, error);
        throw std::runtime_error(message);
    }
    catch (const std::logic_error &er) {
        std::string message = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " | " + er.what();
        Log(message, error);
        throw std::runtime_error(message);
    }
}
