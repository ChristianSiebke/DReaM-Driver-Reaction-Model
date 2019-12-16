/**********************************************
* Copyright (c) 2018 in-tech GmbH             *
* on behalf of BMW AG                         *
***********************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "fakeAgent.h"
#include "fakeObservation.h"
#include "fakeParameter.h"
#include "fakeStochastics.h"
#include "fakeWorld.h"
#include "fakeWorldObject.h"
#include "Interfaces/observationInterface.h"
#include "algorithm_autonomousEmergencyBrakingImplementation.h"

using ::testing::NiceMock;

class AlgorithmAutonomousEmergencyBraking_UnitTest : public ::testing::Test
{
public:
    AlgorithmAutonomousEmergencyBraking_UnitTest();

    ~AlgorithmAutonomousEmergencyBraking_UnitTest()
    {
        delete implementation;
    }

    void SetEgoValues (double velocity, double acceleration, double yawRate);

    AlgorithmAutonomousEmergencyBrakingImplementation* implementation;
    NiceMock<FakeWorld> fakeWorldInterface;
    NiceMock<FakeAgent> fakeEgoAgent;
    NiceMock<FakeObservation> fakeObservation;
};
