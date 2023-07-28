/********************************************************************************
 * Copyright (c) 2018-2019 AMFD GmbH
 *               2016-2017 ITK Engineering GmbH
 *               2017-2020 in-tech GmbH
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

#include <algorithm>
#include <memory>
#include <qglobal.h>
#include "steeringController.h"
#include "common/commonTools.h"
#include "common/globalDefinitions.h"

double SteeringController::CalculateSteeringAngle(int time)
{
    // Time step length
    double dt{(time - timeLast) * 0.001};
    tAverage = .05;
    double velocityForCalculations = std::fmax(20. / 3.6, in_velocity);
    // Scale gains to current velocity. Linear interpolation between 0 and default values at 200km/h.
    double velocityFactor = std::clamp(3.6 / 150. * velocityForCalculations, .15, 1.);
    in_lateralSignal.gainLateralDeviation *= velocityFactor;
    in_lateralSignal.gainHeadingError *= velocityFactor;
    tAverage = tAverage / velocityFactor;

    // Controller for lateral deviation
    double deltaHLateralDeviation = in_lateralSignal.gainLateralDeviation
            * in_steeringRatio * in_wheelBase / (velocityForCalculations * velocityForCalculations)
            * in_lateralSignal.lateralDeviation;

    // Controller for heading angle error
    double deltaHHeadingError = in_lateralSignal.gainHeadingError
            * in_steeringRatio * in_wheelBase / velocityForCalculations
            * in_lateralSignal.headingError;

    // Controller for road curvature
    double meanCurvatureToNearPoint = 0.;
    double meanCurvatureToFarPoint = 0.;
    if (!in_lateralSignal.curvatureOfSegmentsToNearPoint.empty())
    {
        for (unsigned int i = 0; i < in_lateralSignal.curvatureOfSegmentsToNearPoint.size(); ++i)
        {
            meanCurvatureToNearPoint += in_lateralSignal.curvatureOfSegmentsToNearPoint.at(i);
        }

        meanCurvatureToNearPoint = meanCurvatureToNearPoint / in_lateralSignal.curvatureOfSegmentsToNearPoint.size();
    }

    if (!in_lateralSignal.curvatureOfSegmentsToFarPoint.empty())
    {
        for (unsigned int i = 0; i < in_lateralSignal.curvatureOfSegmentsToFarPoint.size(); ++i)
        {
            meanCurvatureToFarPoint += in_lateralSignal.curvatureOfSegmentsToFarPoint.at(i);
        }

        meanCurvatureToFarPoint = meanCurvatureToFarPoint / in_lateralSignal.curvatureOfSegmentsToFarPoint.size();
    }

    // Smooth curvatures with a running average filter
    double meanCurvatureToNearPointSmooth = (dt * meanCurvatureToNearPoint + (tAverage - dt) *
                                             meanCurvatureToNearPointSmoothLast) / tAverage;
    double meanCurvatureToFarPointSmooth = (dt * meanCurvatureToFarPoint + (tAverage - dt) *
                                            meanCurvatureToFarPointSmoothLast) / tAverage;
    double curvatureRoadSmooth = (dt * in_lateralSignal.kappaRoad + (tAverage - dt) * curvatureRoadSmoothLast) / tAverage;

    // Weighting of different curvature Information RoadSmooth, road, nearPointSmooth, farPointSmooth, nearPointMax
    std::vector <double> weighingCurvaturePortions = {.75, 0.25, .15, -.10};
    if (!in_lateralSignal.curvatureOfSegmentsToNearPoint.empty())
    {
        weighingCurvaturePortions.at(2) = 0.;
    }

    if (!in_lateralSignal.curvatureOfSegmentsToFarPoint.empty())
    {
        weighingCurvaturePortions.at(3) = 0.;
    }

    double calc_kappaRoadAnticipated = (weighingCurvaturePortions.at(0) * curvatureRoadSmooth +
                                        weighingCurvaturePortions.at(1) * in_lateralSignal.kappaRoad +
                                        weighingCurvaturePortions.at(2) * meanCurvatureToNearPointSmooth +
                                        weighingCurvaturePortions.at(3) * meanCurvatureToFarPointSmooth) /
            (weighingCurvaturePortions.at(0) + weighingCurvaturePortions.at(1) +
             weighingCurvaturePortions.at(2) + weighingCurvaturePortions.at(3));

    // Controller for road curvaturedelta due to manoeuvre
    double deltaHkappa = std::atan((in_lateralSignal.kappaManoeuvre + calc_kappaRoadAnticipated) * in_wheelBase)
            * in_steeringRatio;


    // Total steering wheel angle
    double deltaH = deltaHLateralDeviation + deltaHHeadingError + deltaHkappa;

    // Limit steering wheel velocity. Human limit set to 320°/s.
    constexpr double HUMAN_LIMIT {320.0 * M_PI / 180.0};
    const auto maxDeltaSteeringWheelAngle = (HUMAN_LIMIT / velocityFactor) * dt;
    const auto deltaSteeringWheelAngle = deltaH - in_steeringWheelAngle;
    if (std::fabs(deltaSteeringWheelAngle) > maxDeltaSteeringWheelAngle)
    {
        deltaH = std::copysign(maxDeltaSteeringWheelAngle, deltaSteeringWheelAngle) + in_steeringWheelAngle;
    }

    const auto desiredSteeringWheelAngle = std::clamp(deltaH, -in_steeringMax, in_steeringMax);

    timeLast = time;
    meanCurvatureToNearPointSmoothLast = meanCurvatureToNearPointSmooth;
    meanCurvatureToFarPointSmoothLast = meanCurvatureToFarPointSmooth;
    curvatureRoadSmoothLast = curvatureRoadSmooth;

    return desiredSteeringWheelAngle;
}

double SteeringController::CalculateSteeringAngleTUDresden(int time, AgentInterface *agent) {
    if (in_velocity < 0.1 && in_velocity > -0.1) {
        timeLast = time;
        return in_steeringWheelAngle;
    }

    // Time step length
    double dt{(time - timeLast) * 0.001};

    ////stanley controller
    double k = 1.5;
    double kSoft = 0.5;

    double deltaH = (-1 * in_lateralSignal.headingError + std::atan((k * -1 * in_lateralSignal.lateralDeviation) / (kSoft + in_velocity))) *
                    in_steeringRatio;

    // Limit steering wheel velocity. Human limit set to 400°/s.
    double HUMAN_LIMIT{450.0 * M_PI / 180.0};
    if (agent->GetVehicleModelParameters().vehicleType == AgentVehicleType::Bicycle)
        HUMAN_LIMIT = 1000 * M_PI / 180.0;
    const auto maxDeltaSteeringWheelAngle = HUMAN_LIMIT * dt;
    const auto deltaSteeringWheelAngle = deltaH - in_steeringWheelAngle;
    if (std::fabs(deltaSteeringWheelAngle) > maxDeltaSteeringWheelAngle) {
        deltaH = std::copysign(maxDeltaSteeringWheelAngle, deltaSteeringWheelAngle) + in_steeringWheelAngle;
    }

    timeLast = time;
    return std::clamp(deltaH, -in_steeringMax, in_steeringMax);
}
