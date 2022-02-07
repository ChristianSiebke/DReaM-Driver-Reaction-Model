#include "BehaviourConverter.h"

std::unique_ptr<BehaviourData> BehaviourConverter::Convert(const StatisticsGroup& main) {
    behaviourData = std::make_unique<BehaviourData>();
    ConvertActionDecisionParameters(main.groups.at("ActionDecision"));
    ConvertCognitiveMapParameters(main.groups.at("CognitiveMap"));
    ConvertGazeMovementParameters(main.groups.at("GazeMovement"));
    return std::move(behaviourData);
}

void BehaviourConverter::ConvertActionDecisionParameters(const StatisticsGroup& main) {
    std::string key;
    try {
        ConvertActionDecisionStatistics(main.groups.at("Velocity Statistics"));
        key = "BehaviourParameters";
        const StatisticsSet& params = main.sets.at(key);
        key = "collisionImminentMargin";
        behaviourData->adBehaviour.collisionImminentMargin = std::static_pointer_cast<StandardDoubleEntry>(params.entries.at(key))->value;
        if (behaviourData->adBehaviour.collisionImminentMargin < 0)
            throw std::logic_error(" collisionImminentMargin must be a positive value! ");
        key = "comfortDeceleration";
        behaviourData->adBehaviour.comfortDeceleration = *std::static_pointer_cast<DistributionEntry>(params.entries.at(key));
        if (behaviourData->adBehaviour.comfortDeceleration.mean >= 0)
            throw std::logic_error(" comfortDeceleration mean must be a negative value! ");
        if (behaviourData->adBehaviour.comfortDeceleration.max > 0)
            throw std::logic_error(" comfortDeceleration max must be a negative value! ");
        if (behaviourData->adBehaviour.comfortDeceleration.min > 0)
            throw std::logic_error(" comfortDeceleration min must be a negative value! ");
        key = "maxAcceleration";
        behaviourData->adBehaviour.maxAcceleration = std::static_pointer_cast<StandardDoubleEntry>(params.entries.at(key))->value;
        if (behaviourData->adBehaviour.maxAcceleration < 0)
            throw std::logic_error(" maxAcceleration must be a positive value! ");
        key = "minDistanceStationaryTraffic";
        behaviourData->adBehaviour.minDistanceStationaryTraffic =
            std::static_pointer_cast<StandardDoubleEntry>(params.entries.at(key))->value;
        if (behaviourData->adBehaviour.minDistanceStationaryTraffic < 0)
            throw std::logic_error(" minDistanceStationaryTraffic must be a positive value! ");
        key = "desiredFollowingTimeHeadway";
        behaviourData->adBehaviour.desiredFollowingTimeHeadway =
            std::static_pointer_cast<StandardDoubleEntry>(params.entries.at(key))->value;
        if (behaviourData->adBehaviour.desiredFollowingTimeHeadway < 0)
            throw std::logic_error(" desiredFollowingTimeHeadway must be a positive value! ");
        key = "maxEmergencyDeceleration";
        behaviourData->adBehaviour.maxEmergencyDeceleration = *std::static_pointer_cast<DistributionEntry>(params.entries.at(key));
        if (behaviourData->adBehaviour.maxEmergencyDeceleration.mean > 0)
            throw std::logic_error(" maxEmergencyDeceleration mean must be a negative value! ");
        if (behaviourData->adBehaviour.maxEmergencyDeceleration.max > 0)
            throw std::logic_error(" maxEmergencyDeceleration max must be a negative value! ");
        if (behaviourData->adBehaviour.maxEmergencyDeceleration.min > 0)
            throw std::logic_error(" maxEmergencyDeceleration min must be a negative value! ");
        key = "timeGapAcceptance";
        behaviourData->adBehaviour.timeGapAcceptance = std::static_pointer_cast<StandardDoubleEntry>(params.entries.at(key))->value;
        if (behaviourData->adBehaviour.timeGapAcceptance < 0)
            throw std::logic_error(" timeGapAcceptance must be a positive value! ");
        key = "minTimeEmergencyBrakeIsActive";
        behaviourData->adBehaviour.minTimeEmergencyBrakeIsActive =
            static_cast<int>(std::static_pointer_cast<StandardDoubleEntry>(params.entries.at(key))->value * 1000);
        if (behaviourData->adBehaviour.minTimeEmergencyBrakeIsActive < 0)
            throw std::logic_error(" minTimeEmergencyBrakeIsActive must be a positive value! ");
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

void BehaviourConverter::ConvertActionDecisionStatistics(const StatisticsGroup& main) {
    behaviourData->adBehaviour.velocityStatistics.insert({IndicatorState::IndicatorState_Off, {}});
    behaviourData->adBehaviour.velocityStatistics.insert({IndicatorState::IndicatorState_Left, {}});
    behaviourData->adBehaviour.velocityStatistics.insert({IndicatorState::IndicatorState_Right, {}});

    for (auto& set : main.sets) {
        if (set.first == "default") {
            behaviourData->adBehaviour.maxEmergencyDeceleration =
                *std::static_pointer_cast<DistributionEntry>(set.second.entries.at("default"));
            continue;
        }
        IndicatorState ind;
        if (set.first == "IndicatorOff") {
            ind = IndicatorState::IndicatorState_Off;
        } else if (set.first == "IndicatorLeft") {
            ind = IndicatorState::IndicatorState_Left;
        } else if (set.first == "IndicatorRight") {
            ind = IndicatorState::IndicatorState_Right;
        } else {
            continue;
        }

        for (auto& entry : set.second.entries) {
            CrossingPhase cr;
            if (entry.first == "Phase APP") {
                cr = CrossingPhase::Approach;
            } else if (entry.first == "Phase DEC1") {
                cr = CrossingPhase::Deceleration_ONE;
            } else if (entry.first == "Phase DEC2") {
                cr = CrossingPhase::Deceleration_TWO;
            } else if (entry.first == "Phase CR_S") {
                cr = CrossingPhase::Crossing_Straight;
            } else if (entry.first == "Phase CR_R") {
                cr = CrossingPhase::Crossing_Right;
            } else if (entry.first == "Phase CR_L1") {
                cr = CrossingPhase::Crossing_Left_ONE;
            } else if (entry.first == "Phase CR_L2") {
                cr = CrossingPhase::Crossing_Left_TWO;
            } else if (entry.first == "Phase EX") {
                cr = CrossingPhase::Exit;
            } else {
                continue;
            }
            behaviourData->adBehaviour.velocityStatistics.at(ind).insert(
                std::make_pair(cr, std::static_pointer_cast<DistributionEntry>(entry.second)));
            if (std::static_pointer_cast<DistributionEntry>(entry.second)->mean < 0)
                throw std::logic_error(" action decision distribution mean must be a positive value! ");
            if (std::static_pointer_cast<DistributionEntry>(entry.second)->max < 0)
                throw std::logic_error(" action decision distribution max must be a positive value! ");
            if (std::static_pointer_cast<DistributionEntry>(entry.second)->min < 0)
                throw std::logic_error(" action decision distribution min must be a positive value! ");
        }
    }
}

void BehaviourConverter::ConvertCognitiveMapParameters(const StatisticsGroup& main) {
    std::string key;
    try {
        key = "BehaviourParameters";
        const StatisticsSet& params = main.sets.at(key);
        key = "memorytime";
        behaviourData.get()->cmBehaviour.memorytime =
            static_cast<int>(std::static_pointer_cast<StandardDoubleEntry>(params.entries.at(key))->value);
        if (behaviourData->cmBehaviour.memorytime < 0)
            throw std::logic_error(" memorytime must be a positive value! ");
        key = "memoryCapacity";
        behaviourData.get()->cmBehaviour.memoryCapacity =
            static_cast<int>(std::static_pointer_cast<StandardDoubleEntry>(params.entries.at(key))->value);
        if (behaviourData->cmBehaviour.memoryCapacity < 0)
            throw std::logic_error(" memoryCapacity must be a positive value! ");
        key = "perceptionLatency";
        behaviourData.get()->cmBehaviour.perceptionLatency = *std::static_pointer_cast<DistributionEntry>(params.entries.at(key));
        if (behaviourData->cmBehaviour.perceptionLatency.mean < 0)
            throw std::logic_error(" perceptionLatency mean must be a positive value! ");
        if (behaviourData->cmBehaviour.perceptionLatency.max < 0)
            throw std::logic_error(" perceptionLatency max must be a positive value! ");
        if (behaviourData->cmBehaviour.perceptionLatency.min < 0)
            throw std::logic_error(" perceptionLatency min must be a positive value! ");
        key = "initialPerceptionTime";
        behaviourData.get()->cmBehaviour.initialPerceptionTime = *std::static_pointer_cast<DistributionEntry>(params.entries.at(key));
        if (behaviourData->cmBehaviour.initialPerceptionTime.mean < 0)
            throw std::logic_error(" initialPerceptionTime mean must be a positive value! ");
        if (behaviourData->cmBehaviour.initialPerceptionTime.max < 0)
            throw std::logic_error(" initialPerceptionTime max must be a positive value! ");
        if (behaviourData->cmBehaviour.initialPerceptionTime.min < 0)
            throw std::logic_error(" initialPerceptionTime min must be a positive value! ");
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

void BehaviourConverter::ConvertGazeMovementParameters(const StatisticsGroup& main) {
    behaviourData->gmBehaviour.XInt_controlAOIProbabilities.insert({CrossingPhase::Approach, {}});
    behaviourData->gmBehaviour.XInt_controlAOIProbabilities.insert({CrossingPhase::Deceleration_ONE, {}});
    behaviourData->gmBehaviour.XInt_controlAOIProbabilities.insert({CrossingPhase::Deceleration_TWO, {}});
    behaviourData->gmBehaviour.XInt_controlAOIProbabilities.insert({CrossingPhase::Crossing_Right, {}});
    behaviourData->gmBehaviour.XInt_controlAOIProbabilities.insert({CrossingPhase::Crossing_Straight, {}});
    behaviourData->gmBehaviour.XInt_controlAOIProbabilities.insert({CrossingPhase::Crossing_Left_ONE, {}});
    behaviourData->gmBehaviour.XInt_controlAOIProbabilities.insert({CrossingPhase::Crossing_Left_TWO, {}});

    std::string key;
    try {
        key = "Standard Road";
        StatisticsGroup standardRoad = main.groups.at(key);
        key = "XJunction";
        StatisticsGroup XJunction = main.groups.at(key);
        key = "BehaviourParameters";
        StatisticsSet std_params = standardRoad.sets.at(key);
        StatisticsSet XInt_params = XJunction.sets.at(key);
        key = "probabilityFixateLeadCar";
        behaviourData.get()->gmBehaviour.std_probabilityFixateLeadCar =
            static_cast<int>(std::static_pointer_cast<StandardDoubleEntry>(std_params.entries.at(key))->value);
        if (behaviourData->gmBehaviour.std_probabilityFixateLeadCar < 0)
            throw std::logic_error(" standard road -> probabilityFixateLeadCar must be a positive value! ");
        behaviourData.get()->gmBehaviour.XInt_probabilityFixateLeadCar =
            static_cast<int>(std::static_pointer_cast<StandardDoubleEntry>(XInt_params.entries.at(key))->value);
        if (behaviourData->gmBehaviour.XInt_probabilityFixateLeadCar < 0)
            throw std::logic_error(" XJunction -> probabilityFixateLeadCar must be a positive value! ");

        key = "probabilityControlGlance";
        behaviourData.get()->gmBehaviour.std_probabilityControlGlance =
            static_cast<int>(std::static_pointer_cast<StandardDoubleEntry>(std_params.entries.at(key))->value);
        if (behaviourData->gmBehaviour.std_probabilityControlGlance < 0)
            throw std::logic_error(" standard road -> probabilityControlGlance must be a positive value! ");
        behaviourData.get()->gmBehaviour.XInt_probabilityControlGlance =
            static_cast<int>(std::static_pointer_cast<StandardDoubleEntry>(XInt_params.entries.at(key))->value);
        if (behaviourData->gmBehaviour.XInt_probabilityControlGlance < 0)
            throw std::logic_error(" XJunction -> probabilityControlGlance must be a positive value! ");

        key = "viewingDepthIntoRoad";
        behaviourData.get()->gmBehaviour.XInt_viewingDepthIntoRoad =
            static_cast<int>(std::static_pointer_cast<StandardDoubleEntry>(XInt_params.entries.at(key))->value);
        if (behaviourData->gmBehaviour.XInt_viewingDepthIntoRoad < 0)
            throw std::logic_error(" XJunction -> viewingDepthIntoRoad must be a positive value! ");

        key = "ScanAOIProbabilities";
        StatisticsSet std_scan = standardRoad.sets.at(key);

        for(auto& entry : std_scan.entries) {
            ScanAOI sc;
            if (entry.first == "Scan Dashboard") {
                sc = ScanAOI::Dashboard;
            } else if (entry.first == "Scan Straight") {
                sc = ScanAOI::Straight;
            } else if (entry.first == "Scan Left") {
                sc = ScanAOI::Left;
            } else if (entry.first == "Scan Right") {
                sc = ScanAOI::Right;
            } else if (entry.first == "Scan Other") {
                sc = ScanAOI::Other;
            } else {
                continue;
            }
            behaviourData->gmBehaviour.std_scanAOIProbabilities.insert(
                std::make_pair(sc, std::static_pointer_cast<DistributionEntry>(entry.second)));
            if (std::static_pointer_cast<DistributionEntry>(entry.second)->mean < 0)
                throw std::logic_error(" gaze state scan AOI distribution mean must be a positive value! ");
            if (std::static_pointer_cast<DistributionEntry>(entry.second)->max < 0)
                throw std::logic_error(" gaze state scan AOI distribution max must be a positive value! ");
            if (std::static_pointer_cast<DistributionEntry>(entry.second)->min < 0)
                throw std::logic_error(" gaze state scan AOI distribution min must be a positive value! ");
        }


        key = "ControlAOIProbabilities";
        StatisticsGroup XInt_control = XJunction.groups.at(key);

        for (auto& set : XInt_control.sets) {
            CrossingPhase cr;
            if (set.first == "Phase APP") {
                cr = CrossingPhase::Approach;
            } else if (set.first == "Phase DEC1") {
                cr = CrossingPhase::Deceleration_ONE;
            } else if (set.first == "Phase DEC2") {
                cr = CrossingPhase::Deceleration_TWO;
            } else if (set.first == "Phase CR_S") {
                cr = CrossingPhase::Crossing_Straight;
            } else if (set.first == "Phase CR_R") {
                cr = CrossingPhase::Crossing_Right;
            } else if (set.first == "Phase CR_L1") {
                cr = CrossingPhase::Crossing_Left_ONE;
            } else if (set.first == "Phase CR_L2") {
                cr = CrossingPhase::Crossing_Left_TWO;
            } else {
                continue;
            }

            for (auto& entry : set.second.entries) {
                ControlAOI con;
                if (entry.first == "Control Left") {
                    con = ControlAOI::Left;
                } else if (entry.first == "Control Oncoming") {
                    con = ControlAOI::Oncoming;
                } else if (entry.first == "Control Right") {
                    con = ControlAOI::Right;
                } else {
                    continue;
                }
                behaviourData->gmBehaviour.XInt_controlAOIProbabilities.at(cr).insert(
                    std::make_pair(con, std::static_pointer_cast<StandardDoubleEntry>(entry.second)->value));
                if (std::static_pointer_cast<StandardDoubleEntry>(entry.second)->value < 0)
                    throw std::logic_error(" XJunction -> Control AOI probability must be a positive value! ");
            }
        }


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
