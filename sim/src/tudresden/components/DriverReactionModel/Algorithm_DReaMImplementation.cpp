/******************************************************************************
 * Copyright (c) 2020 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 *****************************************************************************/

//-----------------------------------------------------------------------------
/** @file  Algorithm_DReaMImplementation.cpp */
//-----------------------------------------------------------------------------

#include "Algorithm_DReaMImplementation.h"
#include "Common/accelerationSignal.h"
#include "Common/lateralSignal.h"
#include "Common/secondaryDriverTasksSignal.h"
#include "primitiveSignals.h"
#include "vectorSignals.h"
#include <cassert>
#include <memory>
#include <qglobal.h>
#include <stringSignals.h>

void AlgorithmDReaMImplementation::UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const>& data, int time) {
    Q_UNUSED(time)

    if (localLinkId == 0) {
        std::shared_ptr<ContainerSignal<std::vector<std::shared_ptr<AgentPerception>>> const> signal =
            std::dynamic_pointer_cast<ContainerSignal<std::vector<std::shared_ptr<AgentPerception>>> const>(data);

        if (!signal) {
            const std::string msg = COMPONENTNAME + " invalid signaltype";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }

        ambientAgents = signal->value;

    } else if (localLinkId == 1) {
        std::shared_ptr<structSignal<std::shared_ptr<EgoPerception>> const> signal =
            std::dynamic_pointer_cast<structSignal<std::shared_ptr<EgoPerception>> const>(data);

        if (!signal) {
            const std::string msg = COMPONENTNAME + " invalid signaltype";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }

        egoPerception = signal->value;

    } else if (localLinkId == 2) {
        std::shared_ptr<structSignal<std::shared_ptr<InfrastructurePerception>> const> signal =
            std::dynamic_pointer_cast<structSignal<std::shared_ptr<InfrastructurePerception>> const>(data);

        if (!signal) {
            const std::string msg = COMPONENTNAME + " invalid signaltype";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }

        infrastructurePerception = signal->value;

    } else if (localLinkId == 3) {
        // from sensor driver perception
        std::shared_ptr<structSignal<DriverRoutePlanning> const> signal =
            std::dynamic_pointer_cast<structSignal<DriverRoutePlanning> const>(data);

        if (!signal) {
            const std::string msg = COMPONENTNAME + " invalid signaltype";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }

        driverRoutePlanning = signal->value;

    } else if (localLinkId == 4) {
        std::shared_ptr<ContainerSignal<std::vector<const MentalInfrastructure::TrafficSign*>> const> signal =
            std::dynamic_pointer_cast<ContainerSignal<std::vector<const MentalInfrastructure::TrafficSign*>> const>(data);

        if (!signal) {
            const std::string msg = COMPONENTNAME + " invalid signaltype";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }

        trafficSigns = signal->value;

    } else {
        const std::string msg = COMPONENTNAME + " invalid link";
        LOG(CbkLogLevel::Debug, msg);
        throw std::runtime_error(msg);
    }
}

void AlgorithmDReaMImplementation::UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const>& data, int time) {
    Q_UNUSED(time)
    if (localLinkId == 0) {
        try {
            data = std::make_shared<LateralSignal const>(out_laneWidth,
                                                         out_lateral_displacement, // lateral deviation
                                                         out_lateral_gain_displacement, out_lateral_heading_error,
                                                         out_lateral_gain_heading_error, out_curvature, componentState);
        } catch (const std::bad_alloc&) {
            const std::string msg = COMPONENTNAME + " could not instantiate signal";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
    } else if (localLinkId == 1) {
        try {
            data = std::make_shared<SecondaryDriverTasksSignal const>(out_routeDecision, out_hornSwitch, out_headLight, out_highBeamLight,
                                                                      out_flasher, componentState);
        } catch (const std::bad_alloc&) {
            const std::string msg = COMPONENTNAME + " could not instantiate signal";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
    } else if (localLinkId == 2) {
        try {
            data = std::make_shared<AccelerationSignal const>(componentState, out_longitudinalaccelerationWish);

        } catch (const std::bad_alloc&) {
            const std::string msg = COMPONENTNAME + " could not instantiate signal";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
    }
}

void AlgorithmDReaMImplementation::Trigger(int time) {
    Q_UNUSED(time)
    try {
        DReaM.UpdateInput(time, egoPerception, ambientAgents, infrastructurePerception, trafficSigns);
        DReaM.UpdateComponents();

        out_routeDecision = DReaM.GetRouteDecision();
        out_longitudinalaccelerationWish = DReaM.GetAcceleration();
        outGazeState = DReaM.GetGazeState();
        segmentControlFixPoints = DReaM.GetSegmentControlFixationPoints();

        // LateralOutput**************************
        out_laneWidth = DReaM.GetWorldRepresentation().egoAgent->GetLaneWidth();
        out_lateral_displacement = DReaM.GetWorldRepresentation().egoAgent->GetLateralDisplacement(); // lateral deviation
        out_curvature = DReaM.GetWorldRepresentation().egoAgent->GetCurvature();
        out_lateral_heading_error = DReaM.GetWorldRepresentation().egoAgent->GetHeading();
        //****************************************

        double intersectionDistance;
        if (DReaM.GetWorldRepresentation().egoAgent->GetDistanceToNextIntersection() >= 0) {

            intersectionDistance = DReaM.GetWorldRepresentation().egoAgent->GetDistanceToNextIntersection();
        } else {
            intersectionDistance = -DReaM.GetWorldRepresentation().egoAgent->GetDistanceOnIntersection();
        }
        observerInstance->Insert(time, GetAgent()->GetId(), LoggingGroup::Visualization, "DistanceToIntersection",
                                 std::to_string(intersectionDistance));

    } catch (const char* error) {
        const std::string msg = COMPONENTNAME + " " + error;
        LOG(CbkLogLevel::Error, msg);
    } catch (const std::string& error) {
        const std::string msg = COMPONENTNAME + " " + error;
        LOG(CbkLogLevel::Error, msg);
    } catch (const std::out_of_range& error) {
        const std::string msg = COMPONENTNAME + error.what();
        LOG(CbkLogLevel::Error, msg);
        throw std::runtime_error(msg);
    } catch (const std::runtime_error& error) {
        LOG(CbkLogLevel::Error, error.what());
        throw error;
    } catch (const std::logic_error& error) {
        LOG(CbkLogLevel::Error, error.what());
        throw error;
    }

    std::vector<std::tuple<int, double, double, double>> otherAgents;

    for (auto& oAgent : *DReaM.GetWorldRepresentation().agentMemory) {
        otherAgents.push_back(std::tuple<int, double, double, double>(oAgent->GetID(), oAgent->GetRefPosition().x,
                                                                      oAgent->GetRefPosition().y, oAgent->GetYawAngle()));
    }

    // TODO pull out of algorithm modul! -->in action modul
    GetAgent()->SetCrossingPhase(static_cast<int>(DReaM.GetWorldInterpretation().crossingInfo.phase));
    GetAgent()->SetOtherAgents(otherAgents);
    GetAgent()->SetCurrentGazeState(outGazeState);
    GetAgent()->SetSegmentControlFixationPoints(segmentControlFixPoints);
    agentStateRecorder->addGazeStates(time, GetAgent()->GetId(), outGazeState);
    agentStateRecorder->addOtherAgents(time, GetAgent()->GetId(), otherAgents);
    agentStateRecorder->addCrossingInfos(time, GetAgent()->GetId(), DReaM.GetWorldInterpretation().crossingInfo);
    agentStateRecorder->addFixationPoints(time, GetAgent()->GetId(), segmentControlFixPoints);

    // TODO Stopping points
    // VA 06.04.2020
    // adding all stopping points to the agent
    for (const auto& stoppingPoint : DReaM.GetWorldInterpretation().crossingInfo.egoStoppingPoints) {
        if (stoppingPoint.second.type == StoppingPointType::NONE) {
            continue;
        }
        if (DReaM.GetWorldRepresentation().egoAgent->GetVehicleType() == AgentVehicleType::Car ||
            DReaM.GetWorldRepresentation().egoAgent->GetVehicleType() == AgentVehicleType::Pedestrian ||
            DReaM.GetWorldRepresentation().egoAgent->GetVehicleType() == AgentVehicleType::Bicycle) {
            // TODO calcualte stopping points for als road users (not only cars)
            GetAgent()->AddStoppingPoint(DReaM.GetWorldInterpretation().crossingInfo.intersectionOdId, stoppingPoint.second.posX,
                                         stoppingPoint.second.posY);
        }
    }

    if (time == 0) {
        agentStateRecorder->addStoppingPoints(GetAgent()->GetId(), GetAgent()->GetStoppingPoints());
        agentStateRecorder->addConflictPoints(infrastructurePerception->GetConflicPoints());
    }
}
