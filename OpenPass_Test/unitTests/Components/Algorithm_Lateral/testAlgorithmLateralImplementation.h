/*******************************************************************************
* Copyright (c) 2019 AMFD GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "algorithm_lateral.h"

#include "fakeStochastics.h"
#include "fakeParameter.h"
#include "fakeAgent.h"
#include "algorithm_lateralImplementation.h"
#include "fakePublisher.h"

/**********************************************************/
// Define fake classes necessary for testing
/**********************************************************/

class TestAlgorithmLateralImplementation : public AlgorithmLateralImplementation
{
public:
    TestAlgorithmLateralImplementation(
            std::string componentName,
            bool isInit,
            int priority,
            int offsetTime,
            int responseTime,
            int cycleTime,
            StochasticsInterface* stochastics,
            const ParameterInterface* parameters,
            PublisherInterface * const publisher,
            const CallbackInterface* callbacks,
            AgentInterface* agent) :
            AlgorithmLateralImplementation(
                componentName,
                isInit,
                priority,
                offsetTime,
                responseTime,
                cycleTime,
                stochastics,
                parameters,
                publisher,
                callbacks,
                agent){}

    ~TestAlgorithmLateralImplementation(){}

    void SetGainLateralDeviation(double gainLateralDeviation) {in_gainLateralDeviation = gainLateralDeviation;}
    void SetSteeringRatio(double steeringRatio) {in_steeringRatio = steeringRatio;}
    void SetWheelBase(double WheelBase) {in_wheelBase = WheelBase;}
    void SetLongitudinalVelocity(double longitudinalVelocity) {velocity = longitudinalVelocity;}
    void SetLateralDeviation(double lateralDeviation) {in_lateralDeviation = lateralDeviation;}
    void SetGainHeadingError(double gainHeadingError) {in_gainHeadingError = gainHeadingError;}
    void SetHeadingError(double headingError) {in_headingError = headingError;}
    void SetCurvature(double curvature) {in_kappaSet = curvature;}
    void SetSteeringMax(double steeringMax) {in_steeringMax =  steeringMax;}
    void SetActualSteeringWheelAngle(double angle) {this->steeringWheelAngle = angle;}
    void SetDesiredSteeringWheelAngle(double Angle) {out_desiredSteeringWheelAngle = Angle;}
    void SetIsActive(bool active) {isActive = active;}
    void SetTimeLast(int timeLast) {this->timeLast = timeLast;}

    double GetDesiredSteeringWheelAngle() {return out_desiredSteeringWheelAngle;}
    double GetGainHeadingError() {return in_gainHeadingError;}
    double GetGainLateralDeviation() {return in_gainLateralDeviation;}
    double GetHeadingError() {return in_headingError;}
    double GetKappaSet() {return in_kappaSet;}
    double GetLateralDeviation() {return in_lateralDeviation;}
    double GetLongitudinalVelocity() {return velocity;}
    double GetSteeringRatio() {return in_steeringRatio;}
    double GetMaximumSteeringWheelAngleAmplitude() {return in_steeringMax;}
    double GetWheelBase() {return in_wheelBase;}
    double GetAbsoluteVelocity() {return velocity;}
};

