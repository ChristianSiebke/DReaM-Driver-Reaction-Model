/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*               2019 AMFD GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "Common/commonTools.h"
#include "Common/parametersVehicleSignal.h"
#include "Common/lateralSignal.h"
#include "Common/steeringSignal.h"
#include "Components/Sensor_Driver/Signals/sensorDriverSignal.h"
#include "testResourceManager.h"

using ::testing::Return;
using ::testing::An;
using ::testing::_;
using ::testing::ReturnRef;


/*******************************************
 * CHECK Trigger                           *
 *******************************************/

/// \brief Data table for definition of individual test cases for AreaOfInterest::EGO_FRONT
struct DataFor_AlgorithmLateralDriverImplementation_Trigger
{
    double      input_LongitudinalVelocity;
    double      input_LateralDeviation;
    double      input_HeadingError;
    double      input_KappaSet;
    double      input_LastSteeringWheelAngle;
    double      result_SteeringWheelAngle;

    /// \brief This stream will be shown in case the test fails
    friend std::ostream& operator<<(std::ostream& os, const DataFor_AlgorithmLateralDriverImplementation_Trigger& obj)
     {
        return os
         << "  input_LongitudinalVelocity (double): "   << obj.input_LongitudinalVelocity
         << "| input_LateralDeviation (double): "       << obj.input_LateralDeviation
         << "| input_HeadingError (double): "           << obj.input_HeadingError
         << "| input_KappaSet (double): "               << obj.input_KappaSet
         << "| input_LastSteeringWheelAngle (double): " << obj.input_LastSteeringWheelAngle
         << "| result_SteeringWheelAngle (double): "    << obj.result_SteeringWheelAngle;
     }
};

class LateralDriverTrigger : public ::testing::Test,
                            public ::testing::WithParamInterface<DataFor_AlgorithmLateralDriverImplementation_Trigger>
{
};

TEST_P(LateralDriverTrigger, LateralDriver_CheckTriggerFunction)
{
    // Get Resources for testing
    DataFor_AlgorithmLateralDriverImplementation_Trigger data = GetParam();

    TestResourceManager resourceManager;
    TestAlgorithmLateralImplementation* stubLateralDriver = resourceManager.stubLateralDriver;

    // Set data for test
    stubLateralDriver->SetTimeLast(0.);
    stubLateralDriver->SetGainLateralDeviation(20.);
    stubLateralDriver->SetSteeringRatio(10.);
    stubLateralDriver->SetWheelBase(3.);
    stubLateralDriver->SetLongitudinalVelocity(data.input_LongitudinalVelocity);
    stubLateralDriver->SetLateralDeviation(data.input_LateralDeviation);
    stubLateralDriver->SetGainHeadingError(10.);
    stubLateralDriver->SetHeadingError(data.input_HeadingError);
    stubLateralDriver->SetCurvature(data.input_KappaSet);
    stubLateralDriver->SetSteeringMax(360.0);
    stubLateralDriver->SetActualSteeringWheelAngle(data.input_LastSteeringWheelAngle);

    // Call test
    stubLateralDriver->Trigger(100);
    double result = stubLateralDriver->GetDesiredSteeringWheelAngle();

    // Results must be within 1% of analytical results (since excact matches can't be guaranteed)
    bool resultLegit = std::fabs(data.result_SteeringWheelAngle - result) <= .01 * std::fabs(data.result_SteeringWheelAngle);

    // Evaluate result
    ASSERT_TRUE(resultLegit);
}

/**********************************************************
 * The test data (must be defined below test)             *
 **********************************************************/
INSTANTIATE_TEST_CASE_P(Default, LateralDriverTrigger,testing::Values
(
   /*
        double      input_LongitudinalVelocity;
        double      input_LateralDeviation;
        double      input_HeadingError;
        double      input_KappaSet;
        double      input_LastSteeringWheelAngle;
        double      result_SteeringWheelAngle;
   */

    DataFor_AlgorithmLateralDriverImplementation_Trigger{ 50., 0., 0., 0.,   0.,        0.},  // Driving straight
    DataFor_AlgorithmLateralDriverImplementation_Trigger{ 50., 1., 0., 0.,   0., 12.375888},  // Lateral deviation from trajectory
    DataFor_AlgorithmLateralDriverImplementation_Trigger{ 50., 0., 1., 0., 300., 309.39721},  // Heading error from trajectory
    DataFor_AlgorithmLateralDriverImplementation_Trigger{ 50., 0., 0., 1., 350.,      360.},  // Curvature of trajectory, 715.65051° capped at 360°
    DataFor_AlgorithmLateralDriverImplementation_Trigger{ 50., 1., 1., 1., 350.,      360.}  // Total steering wheel angle, 1073.17618° capped at 360°
)
);


/*******************************************
 * CHECK Update input                      *
 *******************************************/

/// \brief Data table for definition of individual test cases for AreaOfInterest::EGO_FRONT
struct DataFor_AlgorithmLateralDriverImplementation_UpdateInput
{
    bool        input_NotifyCollistion;
    double      input_LateralDeviation;
    double      input_GainLateralDeviation;
    double      input_HeadingError;
    double      input_GainHeadingError;
    double      input_KappaSet;
    double      input_SteeringRatio;
    double      input_MaximumSteeringWheelAngleAmplitude;
    double      input_WheelBase;
    double      input_AbsoluteVelocity;

    /// \brief This stream will be shown in case the test fails
    friend std::ostream& operator<<(std::ostream& os, const DataFor_AlgorithmLateralDriverImplementation_UpdateInput& obj)
     {
        return os
                << "  input_NotifyCollistion (bool): "                      << obj.input_NotifyCollistion
                << "| input_LateralDeviation (double): "                    << obj.input_LateralDeviation
                << "| input_GainLateralDeviation (double): "                << obj.input_GainLateralDeviation
                << "| input_HeadingError (double): "                        << obj.input_HeadingError
                << "| input_GainHeadingError (double): "                    << obj.input_GainHeadingError
                << "| input_KappaSet (double): "                            << obj.input_KappaSet
                << "| input_SteeringRatio (double): "                       << obj.input_SteeringRatio
                << "| input_MaximumSteeringWheelAngleAmplitude (double): "  << obj.input_MaximumSteeringWheelAngleAmplitude
                << "| input_WheelBase (double): "                           << obj.input_WheelBase
                << "| input_AbsoluteVelocity (double): "                    << obj.input_AbsoluteVelocity;
     }
};

class LateralDriverUpdateInput : public ::testing::Test,
                            public ::testing::WithParamInterface<DataFor_AlgorithmLateralDriverImplementation_UpdateInput>
{
};

TEST_P(LateralDriverUpdateInput, LateralDriver_CheckUpdateInputFunction)
{
    // Get Resources for testing
    DataFor_AlgorithmLateralDriverImplementation_UpdateInput data = GetParam();

    TestResourceManager resourceManager;
    TestAlgorithmLateralImplementation* stubLateralDriver = resourceManager.stubLateralDriver;

    // Create Signals
    // Signal from Driver to module AlgorithmLateralDriver
    const std::shared_ptr<SignalInterface const> signal0 = std::make_shared<LateralSignal const>(
                0.,
                data.input_LateralDeviation, data.input_GainLateralDeviation, data.input_HeadingError,
                data.input_GainHeadingError, data.input_KappaSet, ComponentState::Acting);
    int localLinkId0{0};
    // Signal from Parameters_Vehicle (vehicle model parameters) to module AlgorithmLateralDriver
    VehicleModelParameters vehicleParameters;
    vehicleParameters.steeringRatio = data.input_SteeringRatio;
    vehicleParameters.maximumSteeringWheelAngleAmplitude = data.input_MaximumSteeringWheelAngleAmplitude;
    vehicleParameters.wheelbase = data.input_WheelBase;
    const std::shared_ptr<SignalInterface const> signal1 = std::make_shared<ParametersVehicleSignal const>(vehicleParameters);
    int localLinkId1{100};
    // Signal from SensorDriver (own vehicle) to module AlgorithmLateralDriver
    OwnVehicleInformation ownVehicle;
    GeometryInformation geometryInfo;
    TrafficRuleInformation trafficRuleInfo;
    SurroundingObjects surroundingObjects;
    ownVehicle.velocity = data.input_AbsoluteVelocity;
    const std::shared_ptr<SignalInterface const> signal2 = std::make_shared<SensorDriverSignal const>(ownVehicle, trafficRuleInfo, geometryInfo, surroundingObjects);
    int localLinkId2{101};

    // Call tests
    int time{100};
    stubLateralDriver->UpdateInput(localLinkId0, signal0, time);
    stubLateralDriver->UpdateInput(localLinkId1, signal1, time);
    stubLateralDriver->UpdateInput(localLinkId2, signal2, time);

    // Evaluate result
    ASSERT_EQ(data.input_GainHeadingError, stubLateralDriver->GetGainHeadingError());
    ASSERT_EQ(data.input_GainLateralDeviation, stubLateralDriver->GetGainLateralDeviation());
    ASSERT_EQ(data.input_HeadingError, stubLateralDriver->GetHeadingError());
    ASSERT_EQ(data.input_KappaSet, stubLateralDriver->GetKappaSet());
    ASSERT_EQ(data.input_LateralDeviation, stubLateralDriver->GetLateralDeviation());
    ASSERT_EQ(data.input_SteeringRatio, stubLateralDriver->GetSteeringRatio());
    ASSERT_EQ(data.input_MaximumSteeringWheelAngleAmplitude, stubLateralDriver->GetMaximumSteeringWheelAngleAmplitude());
    ASSERT_EQ(data.input_WheelBase, stubLateralDriver->GetWheelBase());
    ASSERT_EQ(data.input_AbsoluteVelocity, stubLateralDriver->GetAbsoluteVelocity());
}

/**********************************************************
 * The test data (must be defined below test)             *
 **********************************************************/
INSTANTIATE_TEST_CASE_P(Default, LateralDriverUpdateInput,testing::Values
(
   /*
        bool        input_NotifyCollistion;
        double      input_LateralDeviation;
        double      input_GainLateralDeviation;
        double      input_HeadingError;
        double      input_GainHeadingError;
        double      input_KappaSet;
        double      input_SteeringRatio;
        double      input_MaximumSteeringWheelAngleAmplitude;
        double      input_WheelBase;
        double      input_AbsoluteVelocity;
   */

    DataFor_AlgorithmLateralDriverImplementation_UpdateInput{ true,  2.,  3.,  4.,  5.,  6.,  7.,  8.,  9., 10.},
    DataFor_AlgorithmLateralDriverImplementation_UpdateInput{false, 98., 97., 96., 95., 94., 93., 92., 91., 90.}
)
);


/********************************************
 * CHECK Update output                      *
 ********************************************/

struct DataFor_AlgorithmLateralDriverImplementation_UpdateOutput
{
    double input_DesiredSteeringWheelAngle;
    bool input_IsActive;

    /// \brief This stream will be shown in case the test fails
    friend std::ostream& operator<<(std::ostream& os, const DataFor_AlgorithmLateralDriverImplementation_UpdateOutput& obj)
     {
        return os
         << "  input_DesiredSteeringWheelAngle (double): "   << obj.input_DesiredSteeringWheelAngle;
     }
};

class LateralDriverUpdateOutput : public ::testing::Test,
                            public ::testing::WithParamInterface<DataFor_AlgorithmLateralDriverImplementation_UpdateOutput>
{
};

TEST_P(LateralDriverUpdateOutput, LateralDriver_CheckFunction_UpdateOutput)
{
    // Get Resources for testing
    DataFor_AlgorithmLateralDriverImplementation_UpdateOutput data = GetParam();

    TestResourceManager resourceManager;
    TestAlgorithmLateralImplementation* stubLateralDriver = resourceManager.stubLateralDriver;

    // Create Signals
    stubLateralDriver->SetDesiredSteeringWheelAngle(data.input_DesiredSteeringWheelAngle);
    stubLateralDriver->SetIsActive(data.input_IsActive);

    int localLinkId0{0};
    int time{100};
    std::shared_ptr<SignalInterface const> signal1;

    // Call tests
    stubLateralDriver->UpdateOutput(localLinkId0, signal1, time);

    const std::shared_ptr<SteeringSignal const> signal = std::dynamic_pointer_cast<SteeringSignal const>(signal1);

    // Evaluate result
    if(data.input_IsActive)
    {
        ASSERT_EQ(signal->componentState, ComponentState::Acting);
        ASSERT_EQ(signal->steeringWheelAngle, data.input_DesiredSteeringWheelAngle);
    } else
    {
        ASSERT_EQ(signal->componentState, ComponentState::Disabled);
        ASSERT_EQ(signal->steeringWheelAngle, 0.);
    }

}

/**********************************************************
 * The test data (must be defined below test)             *
 **********************************************************/
INSTANTIATE_TEST_CASE_P(Default, LateralDriverUpdateOutput,testing::Values
(
   /*
        double input_DesiredSteeringWheelAngle;
        bool input_IsActive;
   */

    DataFor_AlgorithmLateralDriverImplementation_UpdateOutput{0.27, true},
    DataFor_AlgorithmLateralDriverImplementation_UpdateOutput{0.74, false}
)
);
