/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
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
        std::shared_ptr<ContainerSignal<std::vector<std::shared_ptr<GeneralAgentPerception>>> const> signal =
            std::dynamic_pointer_cast<ContainerSignal<std::vector<std::shared_ptr<GeneralAgentPerception>>> const>(data);

        if (!signal) {
            const std::string msg = COMPONENTNAME + " invalid signaltype (localLinkId 0 = AgentPerception)";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }

        ambientAgents = signal->value;
    }
    else if (localLinkId == 1) {
        std::shared_ptr<structSignal<std::shared_ptr<DetailedAgentPerception>> const> signal =
            std::dynamic_pointer_cast<structSignal<std::shared_ptr<DetailedAgentPerception>> const>(data);

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
        std::shared_ptr<ContainerSignal<std::vector<const MentalInfrastructure::TrafficSignal *>> const> signal =
            std::dynamic_pointer_cast<ContainerSignal<std::vector<const MentalInfrastructure::TrafficSignal *>> const>(data);

        if (!signal) {
            const std::string msg = COMPONENTNAME + " invalid signaltype (localLinkId 3 = TrafficSigns)";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }

        trafficSignals = signal->value;
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
            data = std::make_shared<AccelerationSignal const>(componentState, out_longitudinalAccelerationWish);
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
    else if (localLinkId == 4) {
        try {
            data = std::make_shared<structSignal<AnalysisSignal> const>(DReaM.GetAnalysisSignal());
        }
        catch (const std::bad_alloc &) {
            const std::string msg = COMPONENTNAME + " could not instantiate signal (localLinkId 4 = AnalysisSignal)";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
    }
}

void AlgorithmDReaMImplementation::Trigger(int time) {
    Q_UNUSED(time)
    try {
        //static int lastTime;
        //if (lastTime < time) {
        //    std::cout << "time: " << time << std::endl;
        //    lastTime = time;
        //}
        DReaM.UpdateDReaM(time, egoPerception, ambientAgents, infrastructurePerception, trafficSignals);
        out_indicatorState = static_cast<int>(DReaM.GetLateralAction().indicator);
        out_longitudinalAccelerationWish = DReaM.GetAcceleration();
        outGazeState = DReaM.GetGazeState();
        // LateralOutput**************************
        out_laneWidth = DReaM.GetWorldRepresentation().egoAgent->GetLaneWidth();
        out_lateral_displacement = DReaM.GetWorldRepresentation().egoAgent->GetLateralDisplacement() -
                                   DReaM.GetLateralAction().lateralDisplacement; // lateral deviation

        out_heading_error = DReaM.GetWorldRepresentation().egoAgent->GetHeading();
        out_curvature = DReaM.GetWorldRepresentation().egoAgent->GetCurvature();
        //****************************************
        GetPublisher()->Publish("TestDReaM1234", DReaM.GetDebuggingState());
    }
    catch (const char *error) {
        const std::string msg = COMPONENTNAME + " " + error;
        LOG(CbkLogLevel::Error, msg);
        throw std::runtime_error(msg);
    }
    catch (const std::string &error) {
        const std::string msg = COMPONENTNAME + " " + error;
        LOG(CbkLogLevel::Error, msg);
        throw std::runtime_error(msg);
    }
    catch (const std::out_of_range &error) {
        const std::string msg = COMPONENTNAME + " " + error.what();
        LOG(CbkLogLevel::Error, msg);
        throw std::runtime_error(msg);
    }
    catch (const std::runtime_error &error) {
        const std::string msg = COMPONENTNAME + " " + error.what();
        LOG(CbkLogLevel::Error, msg);
        throw std::runtime_error(msg);
    }
    catch (const std::logic_error &error) {
        const std::string msg = COMPONENTNAME + " " + error.what();
        LOG(CbkLogLevel::Error, msg);
        throw std::runtime_error(msg);
    }
}
