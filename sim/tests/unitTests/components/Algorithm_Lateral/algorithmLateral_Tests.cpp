/********************************************************************************
 * Copyright (c) 2019 AMFD GmbH
 *               2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "common/commonTools.h"
#include "common/parametersVehicleSignal.h"
#include "common/lateralSignal.h"
#include "common/steeringSignal.h"
#include "components/Sensor_Driver/src/Signals/sensorDriverSignal.h"
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
    double                  input_LongitudinalVelocity;
    double                  input_LateralDeviation;
    double                  input_HeadingError;
    double                  input_LastSteeringWheelAngle;
    std::vector<double>     input_CurvatureSegmentsNear;
    std::vector<double>     input_CurvatureSegmentsFar;
    double                  input_KappaRoad;
    double                  input_KappaManeuver;
    double                  result_SteeringWheelAngle;

    /// \brief This stream will be shown in case the test fails
    friend std::ostream& operator<<(std::ostream& os, const DataFor_AlgorithmLateralDriverImplementation_Trigger& obj)
     {
        return os
         << "  input_LongitudinalVelocity (double): "   << obj.input_LongitudinalVelocity
         << "| input_LateralDeviation (double): "       << obj.input_LateralDeviation
         << "| input_HeadingError (double): "           << obj.input_HeadingError
         << "| input_LastSteeringWheelAngle (double): " << obj.input_LastSteeringWheelAngle
         << "| input_KappaRoad (double): "              << obj.input_KappaRoad
         << "| input_KappaManeuver (double): "          << obj.input_KappaManeuver
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
    LateralSignal lateralSignal {ComponentState::Acting,
                                0.0,
                                data.input_LateralDeviation,
                                20.,
                                data.input_HeadingError,
                                10.,
                                data.input_KappaManeuver,
                                data.input_KappaRoad,
                                data.input_CurvatureSegmentsNear,
                                data.input_CurvatureSegmentsFar};
    stubLateralDriver->SetLateralInput(lateralSignal);
    stubLateralDriver->SetVehicleParameter(10.,
                                           2 * M_PI,
                                           3.);
    stubLateralDriver->SetVelocityAndSteeringWheelAngle(data.input_LongitudinalVelocity,
                                                        data.input_LastSteeringWheelAngle);

    // Call test
    stubLateralDriver->Trigger(0);
    double result = stubLateralDriver->GetDesiredSteeringWheelAngle();

    // Results must be within 1% of analytical results (since excact matches can't be guaranteed)
    bool resultLegit = std::fabs(data.result_SteeringWheelAngle - result) <= .01 * std::fabs(data.result_SteeringWheelAngle);

    // Evaluate result
    ASSERT_TRUE(resultLegit) << "SteeringWheelAngle: " << result << std::endl;
}

/**********************************************************
 * The test data (must be defined below test)             *
 **********************************************************/
INSTANTIATE_TEST_CASE_P(Default, LateralDriverTrigger,testing::Values
(
   /*
        double                  input_LongitudinalVelocity;
        double                  input_LateralDeviation;
        double                  input_HeadingError;
        double                  input_LastSteeringWheelAngle;
        std::vector<double>     input_CurvatureSegmentsNear;
        std::vector<double>     input_CurvatureSegmentsFar;
        double                  input_KappaRoad;
        double                  input_KappaManeuver;
        double                  result_SteeringWheelAngle;
   */

    DataFor_AlgorithmLateralDriverImplementation_Trigger{50., 0., 0.,   0. * M_PI / 180.0, {0., 0., 0., 0., 0.}, {0., 0., 0., 0., 0.}, 0., 0., 000.000000 * M_PI / 180.}, // Driving straight
    DataFor_AlgorithmLateralDriverImplementation_Trigger{50., 1., 0.,   0. * M_PI / 180.0, {0., 0., 0., 0., 0.}, {0., 0., 0., 0., 0.}, 0., 0., 013.750987 * M_PI / 180.}, // Lateral deviation from trajectory
    DataFor_AlgorithmLateralDriverImplementation_Trigger{50., 0., 1., 300. * M_PI / 180.0, {0., 0., 0., 0., 0.}, {0., 0., 0., 0., 0.}, 0., 0., 332.000000 * M_PI / 180.}, // Lateral deviation from trajectory with non central steering wheel capped to 320°/s (actual 343.77467)
    DataFor_AlgorithmLateralDriverImplementation_Trigger{50., 0., 2., 350. * M_PI / 180.0, {0., 0., 0., 0., 0.}, {0., 0., 0., 0., 0.}, 0., 0., 360.000000 * M_PI / 180.}, // Curvature of trajectory, 687.54935° capped at 360°
    DataFor_AlgorithmLateralDriverImplementation_Trigger{50., 2., 1., 350. * M_PI / 180.0, {0., 0., 0., 0., 0.}, {0., 0., 0., 0., 0.}, 0., 0., 360.000000 * M_PI / 180.}  // Total steering wheel angle, 371.27665° capped at 360°
)
);


/*******************************************
 * CHECK Update input                      *
 *******************************************/

/// \brief Data table for definition of individual test cases for AreaOfInterest::EGO_FRONT
struct DataFor_AlgorithmLateralDriverImplementation_UpdateInput
{
    bool                    input_NotifyCollistion;
    double                  input_LateralDeviation;
    double                  input_GainLateralDeviation;
    double                  input_HeadingError;
    double                  input_GainHeadingError;
    double                  input_KappaManeuver;
    double                  input_KappaRoad;
    double                  input_SteeringRatio;
    double                  input_MaximumSteeringWheelAngleAmplitude;
    double                  input_WheelBase;
    double                  input_AbsoluteVelocity;
    std::vector<double>     input_CurvatureOfSegmentsToNearPoint;
    std::vector<double>     input_CurvatureOfSegmentsToFarPoint;

    /// \brief This stream will be shown in case the test fails
    friend std::ostream& operator<<(std::ostream& os, const DataFor_AlgorithmLateralDriverImplementation_UpdateInput& obj)
     {
        return os
                << "  input_NotifyCollistion (bool): "                      << obj.input_NotifyCollistion
                << "| input_LateralDeviation (double): "                    << obj.input_LateralDeviation
                << "| input_GainLateralDeviation (double): "                << obj.input_GainLateralDeviation
                << "| input_HeadingError (double): "                        << obj.input_HeadingError
                << "| input_GainHeadingError (double): "                    << obj.input_GainHeadingError
                << "| input_KappaManeuver (double): "                       << obj.input_KappaManeuver
                << "| input_KappaRoad (double): "                           << obj.input_KappaRoad
                << "| input_SteeringRatio (double): "                       << obj.input_SteeringRatio
                << "| input_MaximumSteeringWheelAngleAmplitude (double): "  << obj.input_MaximumSteeringWheelAngleAmplitude
                << "| input_WheelBase (double): "                           << obj.input_WheelBase
                << "| input_AbsoluteVelocity (double): "                    << obj.input_AbsoluteVelocity;
     }
};

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

    DataFor_AlgorithmLateralDriverImplementation_UpdateOutput{0.27 * M_PI / 180., true},
    DataFor_AlgorithmLateralDriverImplementation_UpdateOutput{0.74 * M_PI / 180., false}
)
);
