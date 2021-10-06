/********************************************************************************
 * Copyright (c) 2019 AMFD GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "lateralImpl.h"

#include "fakeStochastics.h"
#include "fakeParameter.h"
#include "fakeAgent.h"
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

    void SetDesiredSteeringWheelAngle(double Angle) {out_desiredSteeringWheelAngle = Angle;}
    void SetIsActive(bool active) {isActive = active;}

    void SetLateralInput(const LateralSignal lateralSignal) {steeringController.SetLateralInput(lateralSignal);}
    void SetVehicleParameter(const double &steeringRatio,
                             const double &maximumSteeringWheelAngleAmplitude,
                             const double &wheelbase)
    {
        steeringController.SetVehicleParameter(steeringRatio,
                                               maximumSteeringWheelAngleAmplitude,
                                               wheelbase);
    }
    void SetVelocityAndSteeringWheelAngle(const double &velocity,
                                          const double &steeringWheelAngle)
    {
        steeringController.SetVelocityAndSteeringWheelAngle(velocity, steeringWheelAngle);
    }

    double GetDesiredSteeringWheelAngle() {return out_desiredSteeringWheelAngle;}
};

