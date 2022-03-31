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

#include <cassert>
#include <memory>

#include <qglobal.h>

#include "Common/accelerationSignal.h"
#include "Common/lateralSignal.h"
#include "Common/primitiveSignals.h"
#include "Common/secondaryDriverTasksSignal.h"
#include "Common/stringSignal.h"
#include "Common/vectorSignals.h"

void AlgorithmDReaMImplementation::UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data, int time) {
    Q_UNUSED(time)

    if (localLinkId == 0) {
        std::shared_ptr<ContainerSignal<std::vector<std::shared_ptr<AgentPerception>>> const> signal =
            std::dynamic_pointer_cast<ContainerSignal<std::vector<std::shared_ptr<AgentPerception>>> const>(data);

        if (!signal) {
            const std::string msg = COMPONENTNAME + " invalid signaltype (localLinkId 0 = AgentPerception)";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }

        ambientAgents = signal->value;
    }
    else if (localLinkId == 1) {
        std::shared_ptr<structSignal<std::shared_ptr<EgoPerception>> const> signal =
            std::dynamic_pointer_cast<structSignal<std::shared_ptr<EgoPerception>> const>(data);

        if (!signal) {
            const std::string msg = COMPONENTNAME + " invalid signaltype (localLinkId 1 = EgoPerception)";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }

        egoPerception = signal->value;
    }
    else if (localLinkId == 2) {
        std::shared_ptr<structSignal<std::shared_ptr<InfrastructurePerception>> const> signal =
            std::dynamic_pointer_cast<structSignal<std::shared_ptr<InfrastructurePerception>> const>(data);

        if (!signal) {
            const std::string msg = COMPONENTNAME + " invalid signaltype (localLinkId 2 = InfrastructurePerception)";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }

        infrastructurePerception = signal->value;
    }
    else if (localLinkId == 3) {
        std::shared_ptr<ContainerSignal<std::vector<const MentalInfrastructure::TrafficSign *>> const> signal =
            std::dynamic_pointer_cast<ContainerSignal<std::vector<const MentalInfrastructure::TrafficSign *>> const>(data);

        if (!signal) {
            const std::string msg = COMPONENTNAME + " invalid signaltype (localLinkId 4 = TrafficSigns)";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }

        trafficSigns = signal->value;
    }
    else {
        const std::string msg = COMPONENTNAME + " invalid link";
        LOG(CbkLogLevel::Debug, msg);
        throw std::runtime_error(msg);
    }
}

void AlgorithmDReaMImplementation::UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time) {
    Q_UNUSED(time)
    if (localLinkId == 0) {
        try {
            data = std::make_shared<LateralSignal const>(componentState, out_laneWidth,
                                                         out_lateral_displacement, // lateral deviation
                                                         out_lateral_gain_displacement, out_heading_error, out_lateral_gain_heading_error,
                                                         out_curvature);
        }
        catch (const std::bad_alloc &) {
            const std::string msg = COMPONENTNAME + " could not instantiate signal (localLinkId 0 = LateralSignal)";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
    }
    else if (localLinkId == 1) {
        try {
            data = std::make_shared<SecondaryDriverTasksSignal const>(out_indicatorState, out_hornSwitch, out_headLight, out_highBeamLight,
                                                                      out_flasher, componentState);
        }
        catch (const std::bad_alloc &) {
            const std::string msg = COMPONENTNAME + " could not instantiate signal (localLinkId 1 = SecondaryDriverTask)";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
    }
    else if (localLinkId == 2) {
        try {
            data = std::make_shared<AccelerationSignal const>(componentState, out_longitudinalaccelerationWish);
        }
        catch (const std::bad_alloc &) {
            const std::string msg = COMPONENTNAME + " could not instantiate signal (localLinkId 2 = AccelerationSignal)";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
    }
    else if (localLinkId == 3) {
        try {
            data = std::make_shared<structSignal<GazeState> const>(outGazeState);
        }
        catch (const std::bad_alloc &) {
            const std::string msg = COMPONENTNAME + " could not instantiate signal (localLinkId 3 = GazeState)";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
    }
}

void AlgorithmDReaMImplementation::Trigger(int time) {
    Q_UNUSED(time)
    std::cout << "time" << time << std::endl;
    try {
        DReaM.UpdateInput(time, egoPerception, ambientAgents, infrastructurePerception, trafficSigns);
        DReaM.UpdateComponents();
        out_indicatorState = static_cast<int>(DReaM.GetWorldRepresentation().egoAgent->GetIndicatorState());
        out_longitudinalaccelerationWish = DReaM.GetAcceleration();
        outGazeState = DReaM.GetGazeState();
        segmentControlFixPoints = DReaM.GetSegmentControlFixationPoints();

        // LateralOutput**************************
        out_laneWidth = DReaM.GetWorldRepresentation().egoAgent->GetLaneWidth();
        out_lateral_displacement = DReaM.GetWorldRepresentation().egoAgent->GetLateralDisplacement(); // lateral deviation
        out_heading_error = DReaM.GetWorldRepresentation().egoAgent->GetHeading();
        out_curvature = DReaM.GetWorldRepresentation().egoAgent->GetCurvature();
        //****************************************

        double intersectionDistance;
        if (DReaM.GetWorldRepresentation().egoAgent->GetDistanceToNextJunction() >= 0) {
            intersectionDistance = DReaM.GetWorldRepresentation().egoAgent->GetDistanceToNextJunction();
        }
        else {
            intersectionDistance = -DReaM.GetWorldRepresentation().egoAgent->GetDistanceOnJunction();
        }
    }
    catch (const char *error) {
        const std::string msg = COMPONENTNAME + " " + error;
        LOG(CbkLogLevel::Error, msg);
    }
    catch (const std::string &error) {
        const std::string msg = COMPONENTNAME + " " + error;
        LOG(CbkLogLevel::Error, msg);
    }
    catch (const std::out_of_range &error) {
        const std::string msg = COMPONENTNAME + error.what();
        LOG(CbkLogLevel::Error, msg);
        throw std::runtime_error(msg);
    }
    catch (const std::runtime_error &error) {
        LOG(CbkLogLevel::Error, error.what());
        throw error;
    }
    catch (const std::logic_error &error) {
        LOG(CbkLogLevel::Error, error.what());
        throw error;
    }

    std::vector<std::tuple<int, double, double, double>> otherAgents;

    for (auto &oAgent : *DReaM.GetWorldRepresentation().agentMemory) {
        otherAgents.push_back(std::tuple<int, double, double, double>(oAgent->GetID(), oAgent->GetRefPosition().x,
                                                                      oAgent->GetRefPosition().y, oAgent->GetYawAngle()));
    }

    agentStateRecorder->addGazeStates(time, GetAgent()->GetId(), outGazeState);
    agentStateRecorder->addOtherAgents(time, GetAgent()->GetId(), otherAgents);
    agentStateRecorder->addCrossingInfos(time, GetAgent()->GetId(), DReaM.GetWorldInterpretation().crossingInfo);
    agentStateRecorder->addFixationPoints(time, GetAgent()->GetId(), segmentControlFixPoints);

    if (time == 0) {
        // FIXME agentStateRecorder->addStoppingPoints(GetAgent()->GetId(), GetAgent()->GetStoppingPoints());
        agentStateRecorder->addConflictPoints(infrastructurePerception->GetConflicPoints());
    }
}
