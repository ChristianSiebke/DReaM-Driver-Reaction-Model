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

        key = "TrafficSignalMemoryParameters";
        const StatisticsSet &tsParams = main.sets.at(key);
        key = "memorytime";
        behaviourData.get()->cmBehaviour.trafficSig_memorytime =
            static_cast<int>(std::static_pointer_cast<StandardDoubleEntry>(tsParams.entries.at(key))->value);
        if (behaviourData->cmBehaviour.trafficSig_memorytime < 0)
            throw std::logic_error(" memorytime must be a positive value! ");
        key = "memoryCapacity";
        behaviourData.get()->cmBehaviour.trafficSig_memoryCapacity =
            static_cast<int>(std::static_pointer_cast<StandardDoubleEntry>(tsParams.entries.at(key))->value);
        if (behaviourData->cmBehaviour.trafficSig_memoryCapacity < 0)
            throw std::logic_error(" memoryCapacity must be a positive value! ");
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
        key = "ScanAreasOfInterest";
        StatisticsGroup scanAOIs = main.groups.at(key);
        key = "DriverGaze";
        StatisticsGroup driverGaze = scanAOIs.groups.at(key);
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
            else {
                continue;
            }
            DriverGaze dr;
            dr.direction = std::static_pointer_cast<StandardDoubleEntry>(set.second.entries.at("direction"))->value;
            dr.openingAngle = std::static_pointer_cast<StandardDoubleEntry>(set.second.entries.at("openingAngle"))->value;
            if (dr.openingAngle < 0)
                throw std::logic_error(" DriverGaze -> openingAngle must be a positive value! ");
            dr.fixationDuration = std::static_pointer_cast<DistributionEntry>(set.second.entries.at("fixationDuration"))->toDistribution();

            behaviourData->gmBehaviour.scanAOIs.driverAOIs.insert(std::make_pair(scan, dr));
        }

        key = "MirrorGaze";
        StatisticsGroup mirrorGaze = scanAOIs.groups.at(key);
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
            mir.direction = std::static_pointer_cast<StandardDoubleEntry>(set.second.entries.at("direction"))->value;
            mir.openingAngle = std::static_pointer_cast<StandardDoubleEntry>(set.second.entries.at("openingAngle"))->value;
            if (mir.openingAngle < 0)
                throw std::logic_error(" MirrorGaze -> openingAngle must be a positive value! ");
            mir.pos.x = std::static_pointer_cast<StandardDoubleEntry>(set.second.entries.at("mirrorPosX"))->value;
            mir.pos.y = std::static_pointer_cast<StandardDoubleEntry>(set.second.entries.at("mirrorPosY"))->value;
            mir.fixationDuration = std::static_pointer_cast<DistributionEntry>(set.second.entries.at("fixationDuration"))->toDistribution();

            behaviourData->gmBehaviour.scanAOIs.mirrorAOIs.insert(std::make_pair(scan, mir));
        }

        key = "BaseParameters";
        StatisticsSet base = main.sets.at(key);
        behaviourData->gmBehaviour.foresightTime = std::static_pointer_cast<StandardDoubleEntry>(base.entries.at("foresightTime"))->value;
        if (behaviourData->gmBehaviour.XInt_controlOpeningAngle < 0)
            throw std::logic_error(" GazeMovement -> foresightTime must be a positive value! ");
        behaviourData->gmBehaviour.minForesightDistance =
            std::static_pointer_cast<StandardDoubleEntry>(base.entries.at("minForesightDistance"))->value;
        if (behaviourData->gmBehaviour.XInt_controlOpeningAngle < 0)
            throw std::logic_error(" GazeMovement ->  minForesightDistance must be a positive value! ");

        key = "AgentObserveParameters";
        StatisticsSet obs = main.sets.at(key);
        behaviourData->gmBehaviour.observe_openingAngle =
            std::static_pointer_cast<StandardDoubleEntry>(obs.entries.at("openingAngle"))->value;
        if (behaviourData->gmBehaviour.XInt_controlOpeningAngle < 0)
            throw std::logic_error(" GazeMovement -> AgentObserveParameters -> openingAngle must be a positive value! ");
        behaviourData->gmBehaviour.observe_fixationDuration =
            std::static_pointer_cast<DistributionEntry>(obs.entries.at("fixationDuration"));

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

        key = "ControlGlanceParameters";
        StatisticsSet ctrl = XJunction.sets.at(key);
        behaviourData->gmBehaviour.XInt_controlOpeningAngle =
            std::static_pointer_cast<StandardDoubleEntry>(ctrl.entries.at("openingAngle"))->value;
        if (behaviourData->gmBehaviour.XInt_controlOpeningAngle < 0)
            throw std::logic_error(" XJunction -> ControlGlanceParameters -> openingAngle must be a positive value! ");
        behaviourData->gmBehaviour.XInt_controlFixationDuration =
            std::static_pointer_cast<DistributionEntry>(ctrl.entries.at("fixationDuration"));

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

        std::map<CrossingPhase, std::map<ScanAOI, std::shared_ptr<DistributionEntry>>> tmp;
        tmp.insert({CrossingPhase::Approach, {}});
        tmp.insert({CrossingPhase::Deceleration_ONE, {}});
        tmp.insert({CrossingPhase::Deceleration_TWO, {}});
        tmp.insert({CrossingPhase::Crossing_Right, {}});
        tmp.insert({CrossingPhase::Crossing_Straight, {}});
        tmp.insert({CrossingPhase::Crossing_Left_ONE, {}});
        tmp.insert({CrossingPhase::Crossing_Left_TWO, {}});

        std::map<TrafficDensity, std::map<CrossingPhase, std::map<ScanAOI, std::shared_ptr<DistributionEntry>>>> tmp2;
        tmp2.insert({TrafficDensity::LOW, tmp});
        tmp2.insert({TrafficDensity::MODERATE, tmp});
        tmp2.insert({TrafficDensity::HIGH, tmp});

        behaviourData->gmBehaviour.XInt_scanAOIProbabilities.insert({IndicatorState::IndicatorState_Left, tmp2});
        behaviourData->gmBehaviour.XInt_scanAOIProbabilities.insert({IndicatorState::IndicatorState_Off, tmp2});
        behaviourData->gmBehaviour.XInt_scanAOIProbabilities.insert({IndicatorState::IndicatorState_Right, tmp2});

        key = "ScanAOIProbabilities";
        StatisticsGroup XInt_scan = XJunction.groups.at(key);

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
            for (auto &densityGroup : indicatorGroup.second.groups) {
                TrafficDensity dens;
                if (densityGroup.first == "Density Low") {
                    dens = TrafficDensity::LOW;
                }
                else if (densityGroup.first == "Density Moderate") {
                    dens = TrafficDensity::MODERATE;
                }
                else if (densityGroup.first == "Density High") {
                    dens = TrafficDensity::HIGH;
                }
                else {
                    continue;
                }
                for (auto &phaseSet : densityGroup.second.sets) {
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
                        behaviourData->gmBehaviour.XInt_scanAOIProbabilities.at(ind).at(dens).at(cr).insert(
                            std::make_pair(sc, std::static_pointer_cast<DistributionEntry>(entry.second)));
                        if (std::static_pointer_cast<DistributionEntry>(entry.second)->mean < 0)
                            throw std::logic_error(" gaze state scan AOI distribution mean must be a positive value! ");
                        if (std::static_pointer_cast<DistributionEntry>(entry.second)->max < 0)
                            throw std::logic_error(" gaze state scan AOI distribution max must be a positive value! ");
                        if (std::static_pointer_cast<DistributionEntry>(entry.second)->min < 0)
                            throw std::logic_error(" gaze state scan AOI distribution min must be a positive value! ");
                    }
                }
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
