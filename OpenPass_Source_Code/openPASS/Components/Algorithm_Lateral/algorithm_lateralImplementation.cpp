/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*               2018, 2019 AMFD GmbH
*               2016, 2017 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** @file  algorithm_lateralImplementation.cpp */
//-----------------------------------------------------------------------------

#include <memory>
#include <qglobal.h>
#include "algorithm_lateralImplementation.h"
#include "Common/steeringSignal.h"
#include "Common/parametersVehicleSignal.h"
#include "Common/lateralSignal.h"
#include "Common/commonTools.h"
#include "Common/globalDefinitions.h"
#include "Components/Sensor_Driver/Signals/sensorDriverSignal.h"

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

            in_lateralDeviation = signal->lateralDeviation;
            in_gainLateralDeviation = signal->gainLateralDeviation;
            in_headingError = signal->headingError;
            in_gainHeadingError = signal->gainHeadingError;
            in_kappaManoeuvre = signal->kappaManoeuvre;
            in_kappaRoad = signal->kappaRoad;
            in_curvatureOfSegmentsToNearPoint = signal->curvatureOfSegmentsToNearPoint;
            in_curvatureOfSegmentsToFarPoint = signal->curvatureOfSegmentsToFarPoint;
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

        in_steeringRatio = signal->vehicleParameters.steeringRatio;
        in_steeringMax = signal->vehicleParameters.maximumSteeringWheelAngleAmplitude;
        in_wheelBase = signal->vehicleParameters.wheelbase;
    }
    else if (localLinkId == 101 || localLinkId == 102)
    {
        // from SensorDriver
        const std::shared_ptr<SensorDriverSignal const> signal = std::dynamic_pointer_cast<SensorDriverSignal const>(data);

        if (!signal)
        {
            const std::string msg = COMPONENTNAME + " invalid signaltype";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }

         velocity = signal->GetOwnVehicleInformation().absoluteVelocity;
         steeringWheelAngle = signal->GetOwnVehicleInformation().steeringWheelAngle;
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
    // Time step length
    double dt{(time - timeLast) * 0.001};
    tAverage = .05;
    double velocityForCalculations = std::fmax(20. / 3.6, velocity);
    // Scale gains to current velocity. Linear interpolation between 0 and default values at 200km/h.
    double velocityFactor = std::clamp(3.6 / 150. * velocityForCalculations, .15, 1.);
    in_gainLateralDeviation *= velocityFactor;
    in_gainHeadingError *= velocityFactor;
    tAverage = tAverage / velocityFactor;

    // Controller for lateral deviation
    double deltaHLateralDeviation = in_gainLateralDeviation
            * in_steeringRatio * in_wheelBase / (velocityForCalculations * velocityForCalculations)
            * in_lateralDeviation * RadiantToDegree;

    // Controller for heading angle error
    double deltaHHeadingError = in_gainHeadingError
            * in_steeringRatio * in_wheelBase / velocityForCalculations
            * in_headingError * RadiantToDegree;

    // Controller for road curvature
    double meanCurvatureToNearPoint = 0.;
    double meanCurvatureToFarPoint = 0.;
    if (!in_curvatureOfSegmentsToNearPoint.empty())
    {
        for (unsigned int i = 0; i < in_curvatureOfSegmentsToNearPoint.size(); ++i)
        {
            meanCurvatureToNearPoint += in_curvatureOfSegmentsToNearPoint.at(i);
        }

        meanCurvatureToNearPoint = meanCurvatureToNearPoint / in_curvatureOfSegmentsToNearPoint.size();
    }

    if (!in_curvatureOfSegmentsToFarPoint.empty())
    {
        for (unsigned int i = 0; i < in_curvatureOfSegmentsToFarPoint.size(); ++i)
        {
            meanCurvatureToFarPoint += in_curvatureOfSegmentsToFarPoint.at(i);
        }

        meanCurvatureToFarPoint = meanCurvatureToFarPoint / in_curvatureOfSegmentsToFarPoint.size();
    }

    // Smooth curvatures with a running average filter
    double meanCurvatureToNearPointSmooth = (dt * meanCurvatureToNearPoint + (tAverage - dt) *
                                             meanCurvatureToNearPointSmoothLast) / tAverage;
    double meanCurvatureToFarPointSmooth = (dt * meanCurvatureToFarPoint + (tAverage - dt) *
                                            meanCurvatureToFarPointSmoothLast) / tAverage;
    double curvatureRoadSmooth = (dt * in_kappaRoad + (tAverage - dt) * curvatureRoadSmoothLast) / tAverage;

    // Weighting of different curvature Information RoadSmooth, road, nearPointSmooth, farPointSmooth, nearPointMax
    std::vector <double> weighingCurvaturePortions = {.75, 0.25, .15, -.10};
    if (!in_curvatureOfSegmentsToNearPoint.empty())
    {
        weighingCurvaturePortions.at(2) = 0.;
    }

    if (!in_curvatureOfSegmentsToFarPoint.empty())
    {
        weighingCurvaturePortions.at(3) = 0.;
    }

    double calc_kappaRoadAnticipated = (weighingCurvaturePortions.at(0) * curvatureRoadSmooth +
                                        weighingCurvaturePortions.at(1) * in_kappaRoad +
                                        weighingCurvaturePortions.at(2) * meanCurvatureToNearPointSmooth +
                                        weighingCurvaturePortions.at(3) * meanCurvatureToFarPointSmooth) /
            (weighingCurvaturePortions.at(0) + weighingCurvaturePortions.at(1) +
             weighingCurvaturePortions.at(2) + weighingCurvaturePortions.at(3));

    // Controller for road curvaturedelta due to manoeuvre
    double deltaHkappa = std::atan((in_kappaManoeuvre + calc_kappaRoadAnticipated) * in_wheelBase)
            * in_steeringRatio * RadiantToDegree;


    // Total steering wheel angle
    double deltaH = deltaHLateralDeviation + deltaHHeadingError + deltaHkappa;

    // Limit steering wheel velocity. Human limit set to 320°/s.
    if (std::fabs(deltaH - steeringWheelAngle) > (320. / velocityFactor) * dt)
    {
        deltaH = deltaH / std::fabs(deltaH) * (320. / velocityFactor) * dt + steeringWheelAngle;
    }

    out_desiredSteeringWheelAngle = TrafficHelperFunctions::ValueInBounds(-in_steeringMax, deltaH , in_steeringMax);
    deltaHLast = out_desiredSteeringWheelAngle;
    timeLast = time;
    meanCurvatureToNearPointSmoothLast = meanCurvatureToNearPointSmooth;
    meanCurvatureToFarPointSmoothLast = meanCurvatureToFarPointSmooth;
    curvatureRoadSmoothLast = curvatureRoadSmooth;
}
