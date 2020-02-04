#pragma once

#include "gmock/gmock.h"
#include "Interfaces/runResultInterface.h"

class FakeRunResult : public RunResultInterface {
 public:
    MOCK_METHOD0(SetEndCondition, void());
    MOCK_CONST_METHOD0(GetCollisionIds, const std::list<int>*());
    MOCK_CONST_METHOD0(GetPositions, const std::map<const AgentInterface*, std::tuple<double, double>>*());
    MOCK_CONST_METHOD0(GetVelocities, const std::map<const AgentInterface*, std::tuple<double, double>>*());
    MOCK_CONST_METHOD0(GetYaws, const std::map<const AgentInterface*, double>*());
    MOCK_CONST_METHOD0(GetDistances, const std::map<const AgentInterface*, double>*());
    MOCK_CONST_METHOD0(IsCollision, bool());
    MOCK_CONST_METHOD0(IsTimeOver, bool());
    MOCK_CONST_METHOD0(IsEndCondition, bool());
    MOCK_METHOD1(AddCollisionId, void(const int));
};
