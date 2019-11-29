/**********************************************
* Copyright (c) 2018 in-tech GmbH             *
* on behalf of BMW AG                         *
***********************************************/

#include "gtest/gtest.h"

#include "fakeAgent.h"
#include "fakeObservation.h"
#include "fakeWorld.h"
#include "fakeWorldObject.h"
#include "Interfaces/observationInterface.h"
#include "fakeParameter.h"
#include "fakeStochastics.h"

#include "SensorGeometric2D.h"

class SensorGeometric2DWithObstruction_UnitTests : public ::testing::Test
{
public:
    SensorGeometric2DWithObstruction_UnitTests();
    virtual ~SensorGeometric2DWithObstruction_UnitTests() = 0;

    void PlaceEgo(double x, double y, double yaw);
    void PlaceAgent(double x, double y, double length, double width, double yaw);
    void InitSensor(double range, double openingAngleH, bool obstruction, double minAreaPercentage);
    const std::vector<DetectedObject> DetectAndSortObjects();

    const polygon_t& GetBBByAgentId(size_t id);

    SensorGeometric2D* sensor = nullptr;
    FakeObservation fakeObservation;

private:
    ::testing::NiceMock<FakeWorld> fakeWorldInterface;
    ::testing::NiceMock<TestStochasticsInterface> fakeStochastics;
    TestParameterInterface* fakeParameters;

    std::map<std::string, int> fakeIntegers = { {"SensorModelType", 1 }
                                              };

    std::map<std::string, double> fakeDoubles = { {"FailureProbability", 0 },
                                                  {"Latency",            0 }
                                                };

    std::map<std::string, const std::string> fakeStrings = { {"SensorName", "TestSensor" }
                                                           };

    std::map<std::string, bool> fakeBools;

    std::map<int, ObservationInterface*> observations = { { 0, &fakeObservation } };

    ::testing::NiceMock<FakeAgent> fakeEgoAgent;
    std::vector<const WorldObjectInterface*> fakeObjects;
    std::vector<const polygon_t*> boundingBoxes;
    int agentCount{1};      // account for ego
};
