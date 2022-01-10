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

#include "longCalcs.h"

#include <cmath>
#include <limits>

using ::testing::Return;
using ::testing::_;
using ::testing::ReturnRef;
using ::testing::DoubleEq;

/********************************************
 * CHECK GetEngineTorqueMax                 *
 ********************************************/

struct DataFor_AlgorithmLongitudinalCalculations_GetEngineTorqueMax
{
    double      input_EngineSpeed;
    double      result_EngineTorqueMax;

    /// \brief This stream will be shown in case the test fails
    friend std::ostream& operator<<(std::ostream& os, const DataFor_AlgorithmLongitudinalCalculations_GetEngineTorqueMax& obj)
     {
        return os
         << "  input_Moment (double): "                 << obj.input_EngineSpeed
         << "| result_IsTorqueWithinLimits (double): "  << obj.result_EngineTorqueMax;
     }
};

class AlgorithmLongitudinalCalculationsGetEngineTorqueMax : public ::testing::Test,
                            public ::testing::WithParamInterface<DataFor_AlgorithmLongitudinalCalculations_GetEngineTorqueMax>
{
};

TEST_P(AlgorithmLongitudinalCalculationsGetEngineTorqueMax, AlgorithmLongitudinalCalculations_CheckFunction_GetEngineTorqueMax)
{
    // Get Resources for testing
    DataFor_AlgorithmLongitudinalCalculations_GetEngineTorqueMax data = GetParam();

    // Set up test
    VehicleModelParameters vehicleParameters;
    vehicleParameters.properties = {{"MaximumEngineTorque", 270.},
                                    {"MinimumEngineSpeed", 800.},
                                    {"MaximumEngineSpeed", 4000.}};

    std::function<void (CbkLogLevel, const char*, int, const std::string&)> Log;
    AlgorithmLongitudinalCalculations calculation{0.0, 0.0, vehicleParameters, Log};

    // Call test
    double result = calculation.GetEngineTorqueMax(data.input_EngineSpeed);

    // Evaluate result
    ASSERT_EQ(result, data.result_EngineTorqueMax);
}

/**********************************************************
 * The test data (must be defined below test)             *
 **********************************************************/
INSTANTIATE_TEST_CASE_P(Default, AlgorithmLongitudinalCalculationsGetEngineTorqueMax,testing::Values
(
   /*
        double      input_EngineSpeed;
        double      result_EngineTorqueMax;
   */

    DataFor_AlgorithmLongitudinalCalculations_GetEngineTorqueMax{ 700., 170.},  // Speed below minimum
    DataFor_AlgorithmLongitudinalCalculations_GetEngineTorqueMax{1000., 190.},  // Speed below minimum + 1000 but above minimum
    DataFor_AlgorithmLongitudinalCalculations_GetEngineTorqueMax{5000., 230.},  // Speed above maximum
    DataFor_AlgorithmLongitudinalCalculations_GetEngineTorqueMax{3500., 250.},  // Speed above maximum - 1000 but below maximum
    DataFor_AlgorithmLongitudinalCalculations_GetEngineTorqueMax{3000., 270.}  // Speed within threshold
)
);


/********************************************
 * CHECK GetEngineTorqueMin                 *
 ********************************************/

struct DataFor_AlgorithmLongitudinalCalculations_GetEngineTorqueMin
{
    double      input_EngineSpeed;
    double      result_EngineTorqueMax;

    /// \brief This stream will be shown in case the test fails
    friend std::ostream& operator<<(std::ostream& os, const DataFor_AlgorithmLongitudinalCalculations_GetEngineTorqueMin& obj)
     {
        return os
         << "  input_Moment (double): "                 << obj.input_EngineSpeed
         << "| result_IsTorqueWithinLimits (double): "  << obj.result_EngineTorqueMax;
     }
};

class AlgorithmLongitudinalCalculationsGetEngineTorqueMin : public ::testing::Test,
                            public ::testing::WithParamInterface<DataFor_AlgorithmLongitudinalCalculations_GetEngineTorqueMin>
{
};

TEST_P(AlgorithmLongitudinalCalculationsGetEngineTorqueMin, AlgorithmLongitudinalCalculations_CheckFunction_GetEngineTorqueMin)
{
    // Get Resources for testing
    DataFor_AlgorithmLongitudinalCalculations_GetEngineTorqueMin data = GetParam();

    // Set up test
    VehicleModelParameters vehicleParameters;
    vehicleParameters.properties = {{"MaximumEngineTorque", 270.},
                                    {"MinimumEngineTorque", 30.},
                                    {"MinimumEngineSpeed", 800.},
                                    {"MaximumEngineSpeed", 4000.}};

    std::function<void (CbkLogLevel, const char*, int, const std::string&)> Log;
    AlgorithmLongitudinalCalculations calculation{0.0, 0.0, vehicleParameters, Log};

    // Call test
    double result = calculation.GetEngineTorqueMin(data.input_EngineSpeed);

    // Evaluate result
    ASSERT_EQ(result, data.result_EngineTorqueMax);
}

/**********************************************************
 * The test data (must be defined below test)             *
 **********************************************************/
INSTANTIATE_TEST_CASE_P(Default, AlgorithmLongitudinalCalculationsGetEngineTorqueMin,testing::Values
(
   /*
        double      input_EngineSpeed;
        double      result_EngineTorqueMax;
   */

    DataFor_AlgorithmLongitudinalCalculations_GetEngineTorqueMin{5000., -23.},  // Speed above max engine speed
    DataFor_AlgorithmLongitudinalCalculations_GetEngineTorqueMin{3000., -27.},  // Speed within threshold
    DataFor_AlgorithmLongitudinalCalculations_GetEngineTorqueMin{ 700., -17.},  // Speed below minimum
    DataFor_AlgorithmLongitudinalCalculations_GetEngineTorqueMin{1000., -19.}  // Speed below minimum - 1000 but not below minimum
)
);

/********************************************
 * CHECK GetAccFromEngineTorque             *
 ********************************************/

struct DataFor_AlgorithmLongitudinalCalculations_GetAccFromEngineTorque
{
    double      input_EngineTorque;
    int         input_ChosenGear;
    double      result_AccelerationFromEngineTorque;

    /// \brief This stream will be shown in case the test fails
    friend std::ostream& operator<<(std::ostream& os, const DataFor_AlgorithmLongitudinalCalculations_GetAccFromEngineTorque& obj)
     {
        return os
         << "  input_EngineTorque (double): "                   << obj.input_EngineTorque
         << "| input_ChosenGear (int): "                        << obj.input_ChosenGear
         << "| result_AccelerationFromEngineTorque (double): "  << obj.result_AccelerationFromEngineTorque;
     }
};

class AlgorithmLongitudinalCalculationsGetAccFromEngineTorque : public ::testing::Test,
                            public ::testing::WithParamInterface<DataFor_AlgorithmLongitudinalCalculations_GetAccFromEngineTorque>
{
};

TEST_P(AlgorithmLongitudinalCalculationsGetAccFromEngineTorque, AlgorithmLongitudinalCalculations_CheckFunction_GetAccFromEngineTorque)
{
    // Get Resources for testing
    DataFor_AlgorithmLongitudinalCalculations_GetAccFromEngineTorque data = GetParam();

    // Set up test
    VehicleModelParameters vehicleParameters;
    vehicleParameters.properties = {{"AxleRatio", 2.8},
                                    {"GearRatio0", 0},
                                    {"GearRatio1", 4.1},
                                    {"GearRatio2", 2.5},
                                    {"GearRatio3", 1.4},
                                    {"GearRatio4", 1.},
                                    {"GearRatio5", .9},
                                    {"GearRatio6", .7},
                                    {"Mass", 800}};
    vehicleParameters.rearAxle.wheelDiameter = 0.7;

    std::function<void (CbkLogLevel, const char*, int, const std::string&)> Log;
    AlgorithmLongitudinalCalculations calculation{0.0, 0.0, vehicleParameters, Log};

    // Call test
    double result = calculation.GetAccFromEngineTorque(data.input_EngineTorque, data.input_ChosenGear);

    // Evaluate result
    ASSERT_EQ(result, data.result_AccelerationFromEngineTorque);
}

/**********************************************************
 * The test data (must be defined below test)             *
 **********************************************************/
INSTANTIATE_TEST_CASE_P(Default, AlgorithmLongitudinalCalculationsGetAccFromEngineTorque,testing::Values
(
   /*
        double      input_EngineTorque;
        int         input_ChosenGear;
        double      result_AccelerationFromEngineTorque;
   */

    DataFor_AlgorithmLongitudinalCalculations_GetAccFromEngineTorque{  0., 0,  0.},  // No Torque
    DataFor_AlgorithmLongitudinalCalculations_GetAccFromEngineTorque{100., 4,  1.},  // Normal, 4th gear
    DataFor_AlgorithmLongitudinalCalculations_GetAccFromEngineTorque{200., 0,  0.},  // Neutral gear
    DataFor_AlgorithmLongitudinalCalculations_GetAccFromEngineTorque{300., 5, 2.7}  // Normal, 5th gear
)
);

/********************************************
 * CHECK GetEngineSpeedByVelocity           *
 ********************************************/

struct DataFor_AlgorithmLongitudinalCalculations_GetEngineSpeedByVelocity
{
    double      input_Velocity;
    double      result_EngineSpeed;

    /// \brief This stream will be shown in case the test fails
    friend std::ostream& operator<<(std::ostream& os, const DataFor_AlgorithmLongitudinalCalculations_GetEngineSpeedByVelocity& obj)
     {
        return os
         << "  input_Velocity (double): "       << obj.input_Velocity
         << "| result_EngineSpeed (double): "   << obj.result_EngineSpeed;
     }
};

class AlgorithmLongitudinalCalculationsGetEngineSpeedByVelocity : public ::testing::Test,
                            public ::testing::WithParamInterface<DataFor_AlgorithmLongitudinalCalculations_GetEngineSpeedByVelocity>
{
};

TEST_P(AlgorithmLongitudinalCalculationsGetEngineSpeedByVelocity, AlgorithmLongitudinalCalculations_CheckFunction_GetEngineSpeedByVelocity)
{
    // Get Resources for testing
    DataFor_AlgorithmLongitudinalCalculations_GetEngineSpeedByVelocity data = GetParam();

    // Set up tests
    VehicleModelParameters vehicleParameters;
    vehicleParameters.properties = {{"AxleRatio", 1},
                                    {"GearRatio1", 4.1},
                                    {"GearRatio2", 2.5},
                                    {"GearRatio3", 1.4},
                                    {"GearRatio4", 1.},
                                    {"GearRatio5", .9},
                                    {"GearRatio6", .7},
                                    {"Mass", 800}};
    vehicleParameters.rearAxle.wheelDiameter = 0.5;

    std::function<void (CbkLogLevel, const char*, int, const std::string&)> Log;
    AlgorithmLongitudinalCalculations calculation{0.0, 0.0, vehicleParameters, Log};

    // Call test
    double result = calculation.GetEngineSpeedByVelocity(data.input_Velocity, 4) * 2 * M_PI;

    // Evaluate result
    ASSERT_EQ(result, data.result_EngineSpeed);
}

/**********************************************************
 * The test data (must be defined below test)             *
 **********************************************************/
INSTANTIATE_TEST_CASE_P(Default, AlgorithmLongitudinalCalculationsGetEngineSpeedByVelocity,testing::Values
(
   /*
        double      input_Velocity;
        double      result_EngineSpeed;
   */

    DataFor_AlgorithmLongitudinalCalculations_GetEngineSpeedByVelocity{0., 0.},
    DataFor_AlgorithmLongitudinalCalculations_GetEngineSpeedByVelocity{50., 12000.},
    DataFor_AlgorithmLongitudinalCalculations_GetEngineSpeedByVelocity{20., 4800.}
)
);

/********************************************
 * CHECK GetEngineTorqueAtGear              *
 ********************************************/

struct DataFor_AlgorithmLongitudinalCalculations_GetEngineTorqueAtGear
{
    double      input_Acceleration;
    int         input_Gear;
    double      result_TorqueAtGear;

    /// \brief This stream will be shown in case the test fails
    friend std::ostream& operator<<(std::ostream& os, const DataFor_AlgorithmLongitudinalCalculations_GetEngineTorqueAtGear& obj)
     {
        return os
         << "  input_Acceleration (double): "   << obj.input_Acceleration
         << "| input_Gear (double): "           << obj.input_Gear
         << "| result_TorqueAtGear (double): "  << obj.result_TorqueAtGear;
     }
};

class AlgorithmLongitudinalCalculationsGetEngineTorqueAtGear : public ::testing::Test,
                            public ::testing::WithParamInterface<DataFor_AlgorithmLongitudinalCalculations_GetEngineTorqueAtGear>
{
};

TEST_P(AlgorithmLongitudinalCalculationsGetEngineTorqueAtGear, AlgorithmLongitudinalCalculations_CheckFunction_GetEngineTorqueAtGear)
{
    // Get Resources for testing
    DataFor_AlgorithmLongitudinalCalculations_GetEngineTorqueAtGear data = GetParam();

    // Set up tests
    VehicleModelParameters vehicleParameters;
    vehicleParameters.properties = {{"AxleRatio", 1.},
                                    {"GearRatio1", 4.1},
                                    {"GearRatio2", 2.5},
                                    {"GearRatio3", 1.4},
                                    {"GearRatio4", 1.},
                                    {"GearRatio5", .9},
                                    {"GearRatio6", .7},
                                    {"NumberOfGears", 6},
                                    {"Mass", 500}};
    vehicleParameters.rearAxle.wheelDiameter = 0.5;

    std::function<void (CbkLogLevel, const char*, int, const std::string&)> Log;
    AlgorithmLongitudinalCalculations calculation{0.0, 0.0, vehicleParameters, Log};

    // Call test
    double result = calculation.GetEngineTorqueAtGear(data.input_Gear, data.input_Acceleration);

    // Evaluate result
    ASSERT_EQ(result, data.result_TorqueAtGear);
}

/**********************************************************
 * The test data (must be defined below test)             *
 **********************************************************/
INSTANTIATE_TEST_CASE_P(Default, AlgorithmLongitudinalCalculationsGetEngineTorqueAtGear,testing::Values
(
   /*
        double      input_Acceleration;
        int         input_Gear;
        double      result_TorqueAtGear;
   */

    DataFor_AlgorithmLongitudinalCalculations_GetEngineTorqueAtGear{0., 1, 0.},  // No acceleration
    DataFor_AlgorithmLongitudinalCalculations_GetEngineTorqueAtGear{1., 0, 0.},  // Neutral gear
    DataFor_AlgorithmLongitudinalCalculations_GetEngineTorqueAtGear{1., 4, 125.}, // Normal, gear ratio 1.
    DataFor_AlgorithmLongitudinalCalculations_GetEngineTorqueAtGear{1., 2, 50.} // Normal, gear ratio 2.5
)
);

/********************************************
 * CHECK PedalPosition                      *
 ********************************************/

struct DataFor_AlgorithmLongitudinalCalculations_PedalPosition
{
    double      input_Acceleration;
    double      engineTorqueMax;
    double      result_AcceleratorPedalPosition;
    double      result_BrakePedalPosition;

    /// \brief This stream will be shown in case the test fails
    friend std::ostream& operator<<(std::ostream& os, const DataFor_AlgorithmLongitudinalCalculations_PedalPosition& obj)
     {
        return os
         << "  input_Acceleration (double): "                       << obj.input_Acceleration
         << "| engineTorqueMax (double): "                          << obj.engineTorqueMax
         << "| result_AcceleratorPedalPosition (double): "          << obj.result_AcceleratorPedalPosition
         << "| result_BrakePedalPosition (double): "                << obj.result_BrakePedalPosition;
     }
};

class AlgorithmLongitudinalCalculationsPedalPosition : public ::testing::Test,
                            public ::testing::WithParamInterface<DataFor_AlgorithmLongitudinalCalculations_PedalPosition>
{
};

TEST_P(AlgorithmLongitudinalCalculationsPedalPosition, AlgorithmLongitudinalCalculations_CheckFunction_PedalPosition)
{
    // Get Resources for testing
    DataFor_AlgorithmLongitudinalCalculations_PedalPosition data = GetParam();

    // Set up test
    VehicleModelParameters vehicleParameters;
    vehicleParameters.properties = {{"AxleRatio", 1},
                                    {"GearRatio1", 1},
                                    {"NumberOfGears", 1},
                                    {"MaximumEngineTorque", data.engineTorqueMax},
                                    {"MinimumEngineSpeed", -10000},
                                    {"MaximumEngineSpeed", 10000},
                                    {"Mass", 1000}};
    vehicleParameters.rearAxle.wheelDiameter = 2.;

    std::function<void (CbkLogLevel, const char*, int, const std::string&)> Log;
    AlgorithmLongitudinalCalculations calculation{0.0, data.input_Acceleration, vehicleParameters, Log};

    // Call test
    calculation.CalculatePedalPositions();
    double result_AccelerationPedalPosition = calculation.GetAcceleratorPedalPosition();
    double result_BrakePedalPosition = calculation.GetBrakePedalPosition();

    // Evaluate result
    ASSERT_THAT(result_AccelerationPedalPosition, DoubleEq(data.result_AcceleratorPedalPosition));
    ASSERT_THAT(result_BrakePedalPosition, DoubleEq(data.result_BrakePedalPosition));
}

/**********************************************************
 * The test data (must be defined below test)             *
 **********************************************************/
INSTANTIATE_TEST_CASE_P(Default, AlgorithmLongitudinalCalculationsPedalPosition,testing::Values
(
   /*
             double      input_Acceleration;
             double      engineTorqueMax;
             double      result_AcceleratorPedalPosition;
             double      result_BrakePedalPosition;
   */

    DataFor_AlgorithmLongitudinalCalculations_PedalPosition{  // Brake
                            -5.005,
                            1000.0,
                            0.0, 0.5},
    DataFor_AlgorithmLongitudinalCalculations_PedalPosition{  // Decelerate, no brake
                            -0.05,
                            1000.0,
                            50. / 1100.0, 0.0},
    DataFor_AlgorithmLongitudinalCalculations_PedalPosition{  // No Acceleration
                            0.0,
                            2000.0,
                            200.0 / 2200.0, 0.0},
    DataFor_AlgorithmLongitudinalCalculations_PedalPosition{  // Accelerate
                            1.5,
                            2000.0,
                            1700.0 / 2200.0, 0.0}
)
);
