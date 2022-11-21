/********************************************************************************
 * Copyright (c) 2018-2019 AMFD GmbH
 *               2016-2017 ITK Engineering GmbH
 *               2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
/** @file  algorithm_lateralImplementation.cpp */
//-----------------------------------------------------------------------------

#include "lateralImpl.h"

#include <memory>

#include <qglobal.h>

#include "PerceptionData.h"
#include "common/commonTools.h"
#include "common/globalDefinitions.h"
#include "common/lateralSignal.h"
#include "common/parametersVehicleSignal.h"
#include "common/steeringSignal.h"
#include "complexSignals.h"
#include "components/Sensor_Driver/src/Signals/sensorDriverSignal.h"
#include "components/common/vehicleProperties.h"
void AlgorithmLateralImplementation::UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data, int time)
{
    Q_UNUSED(time);


    if (localLinkId == 0)
    {
        // from PrioritizerLateralVehicleComponent or Driver
        const std::shared_ptr<ComponentStateSignalInterface const> stateSignal = std::dynamic_pointer_cast<ComponentStateSignalInterface const>(data);
        if(stateSignal->componentState == ComponentState::Acting)
        {
            isActive = true;
            const std::shared_ptr<LateralSignal const> signal = std::dynamic_pointer_cast<LateralSignal const>(data);
            if (!signal)
            {
                const std::string msg = COMPONENTNAME + " invalid signaltype";
                LOG(CbkLogLevel::Debug, msg);
                throw std::runtime_error(msg);
            }

            steeringController.SetLateralInput(*signal);
        }
        else
        {
            isActive = false;
        }
    }
    else if (localLinkId == 100)
    {
        // from ParametersAgent
        const std::shared_ptr<ParametersVehicleSignal const> signal = std::dynamic_pointer_cast<ParametersVehicleSignal const>(data);
        if (!signal)
        {
            const std::string msg = COMPONENTNAME + " invalid signaltype";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
        const auto steeringRatio = helper::map::query(signal->vehicleParameters.properties, vehicle::properties::SteeringRatio);
        THROWIFFALSE(steeringRatio.has_value(), "SteeringRatio was not defined in VehicleCatalog");

        steeringController.SetVehicleParameter(steeringRatio.value(),
                                               signal->vehicleParameters.frontAxle.maxSteering * steeringRatio.value(),
                                               signal->vehicleParameters.frontAxle.positionX - signal->vehicleParameters.rearAxle.positionX);
    }
    else if (localLinkId == 101 || localLinkId == 102)
    {
        // from SensorDriver

        if (const std::shared_ptr<SensorDriverSignal const> signal = std::dynamic_pointer_cast<SensorDriverSignal const>(data)) {
            steeringController.SetVelocityAndSteeringWheelAngle(signal->GetOwnVehicleInformation().absoluteVelocity,
                                                                signal->GetOwnVehicleInformation().steeringWheelAngle);
        }
        else if (std::shared_ptr<structSignal<std::shared_ptr<EgoPerception>> const> signal =
                     std::dynamic_pointer_cast<structSignal<std::shared_ptr<EgoPerception>> const>(data)) {
            steeringController.SetVelocityAndSteeringWheelAngle(signal->value->velocity, signal->value->steeringWheelAngle);
        }
        else {
            const std::string msg = COMPONENTNAME + " invalid signaltype";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
    }
    else
    {
        const std::string msg = COMPONENTNAME + " invalid link";
        LOG(CbkLogLevel::Debug, msg);
        throw std::runtime_error(msg);
    }
}

void AlgorithmLateralImplementation::UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time)
{
    Q_UNUSED(time);

    if(localLinkId == 0)
    {
        try
        {
            if (isActive)
            {
                data = std::make_shared<SteeringSignal const>(ComponentState::Acting, out_desiredSteeringWheelAngle);
            }
            else
            {
                data = std::make_shared<SteeringSignal const>(ComponentState::Disabled, 0.0);
            }
        }
        catch(const std::bad_alloc&)
        {
            const std::string msg = COMPONENTNAME + " could not instantiate signal";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
    }
    else
    {
        const std::string msg = COMPONENTNAME + " invalid link";
        LOG(CbkLogLevel::Debug, msg);
        throw std::runtime_error(msg);
    }
}

void AlgorithmLateralImplementation::Trigger(int time)
{
    out_desiredSteeringWheelAngle = steeringController.CalculateSteeringAngleTUDresden(time);
}
