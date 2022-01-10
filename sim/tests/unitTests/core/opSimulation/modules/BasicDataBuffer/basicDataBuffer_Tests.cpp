/********************************************************************************
 * Copyright (c) 2020-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "common/openPassTypes.h"
#include "common/openPassUtils.h"

#include "common/runtimeInformation.h"

#include "basicDataBufferImplementation.h"

#include "fakeAgentDataPublisher.h"
#include "fakeCallback.h"
#include "fakeParameter.h"
#include "fakeStochastics.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::_;
using ::testing::ElementsAre;
using ::testing::Eq;
using ::testing::NiceMock;
using ::testing::NotNull;
using ::testing::Pair;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::SizeIs;
using ::testing::UnorderedElementsAre;

using namespace openpass::type;

const openpass::common::RuntimeInformation fakeRti{{openpass::common::Version{0, 0, 0}}, {"", "", ""}};

class TestBasicDataBuffer : public BasicDataBufferImplementation
{
public:
    TestBasicDataBuffer(const openpass::common::RuntimeInformation *runtimeInformation, const CallbackInterface *callbacks) :
        BasicDataBufferImplementation(runtimeInformation, callbacks)
    {
    }

    AcyclicStore &GetAcyclicStore()
    {
        return acyclicStore;
    }

    void SetAcyclicStore(AcyclicStore &newStore)
    {
        acyclicStore = newStore;
    }

    StaticStore &GetStaticStore()
    {
        return staticStore;
    }

    void SetStaticStore(StaticStore &newStore)
    {
        staticStore = newStore;
    }

    CyclicStore &GetStore()
    {
        return cyclicStore;
    }

    void SetStore(CyclicStore &newStore)
    {
        cyclicStore = newStore;
    }

    StoreIndex<EntityId> &GetEntityIdIndex()
    {
        return entityIdIndex;
    }
};

class BasicDataBuffer_GetCyclic_Test : public ::testing::Test
{
public:
    BasicDataBuffer_GetCyclic_Test()
    {
        implementation = std::make_unique<BasicDataBufferImplementation>(&fakeRti, &fakeCallback);

        // fill some test data

        // value types according to openpass::databuffer::Value
        // bool, char, int, size_t, float, double, std::string and std::vector of each type

        implementation->PutCyclic(0, "boolVal",   true);
        implementation->PutCyclic(0, "charVal",   'a');
        implementation->PutCyclic(0, "intVal",    1);
        implementation->PutCyclic(0, "sizeVal",   static_cast<size_t>(2));
        implementation->PutCyclic(0, "floatVal",  3.3f);
        implementation->PutCyclic(0, "doubleVal", 4.4);
        implementation->PutCyclic(0, "stringVal", "str");

        implementation->PutCyclic(0, "boolVec",   std::vector<bool>{true, false});
        implementation->PutCyclic(0, "charVec",   std::vector<char>{'a'});
        implementation->PutCyclic(0, "intVec",    std::vector<char>{1, 2});
        implementation->PutCyclic(0, "sizeVec",   std::vector<size_t>{3, 4});
        implementation->PutCyclic(0, "floatVec",  std::vector<float>{5.5f, 6.6f});
        implementation->PutCyclic(0, "doubleVec", std::vector<double>{7.7, 8.8});
        implementation->PutCyclic(0, "stringVec", std::vector<std::string>{"str1", "str2"});

        // data with additional entity ids

        implementation->PutCyclic(1, "intVal", 2);
        implementation->PutCyclic(2, "intVal", 3);

        implementation->PutCyclic(1, "intVal2", 22);
        implementation->PutCyclic(2, "intVal2", 23);
    }

    std::unique_ptr<DataBufferInterface> implementation;

private:
    NiceMock<FakeCallback> fakeCallback;
};

class BasicDataBuffer_GetAcyclic_Test : public ::testing::Test
{
public:
    BasicDataBuffer_GetAcyclic_Test()
    {
        implementation = std::make_unique<BasicDataBufferImplementation>(&fakeRti, &fakeCallback);

        // fill some test data
        Parameter params{{"param1", 1},
                         {"param2", 2.0}
                        };

        implementation->PutAcyclic(0, "key1", Acyclic{"name", {{0, 1}}, {{2, 3}}, {{"param1", 1}}});
        implementation->PutAcyclic(1, "key2", Acyclic{"name", {{1, 2}}, {{3, 4}}, {{"param2", 2.0}}});
        implementation->PutAcyclic(0, "key1", Acyclic{"name", {{2, 3}}, {{4, 5}}, {{"param3", "paramVal3"}}});
        implementation->PutAcyclic(0, "key1", Acyclic{"name", {{3, 4}}, {{5, 6}}, {{"param4", std::vector<int>{0, 1, 2}}}});
    }

    std::unique_ptr<DataBufferInterface> implementation;

private:
    NiceMock<FakeCallback> fakeCallback;
};

class BasicDataBuffer_Persistence_Test : public ::testing::Test
{
public:
    BasicDataBuffer_Persistence_Test()
    {
        implementation = std::make_unique<BasicDataBufferImplementation>(&fakeRti, &fakeCallback);

        implementation->PutStatic("nonpersistentKey", 0, false);
        implementation->PutStatic("persistentKey", 1, true);
        implementation->PutCyclic(0, "cyclicKey", 2);
        implementation->PutAcyclic(0, "acyclicKey", Acyclic{"name", {{0, 1}}, {{2, 3}}, {{"param1", 1}}});
    }

    std::unique_ptr<DataBufferInterface> implementation;

private:
    NiceMock<FakeCallback> fakeCallback;
};

class BasicDataBuffer_GetStatic_Test : public ::testing::Test
{
public:
    BasicDataBuffer_GetStatic_Test()
    {
        implementation = std::make_unique<BasicDataBufferImplementation>(&fakeRti, &fakeCallback);

        // fill some test data
        implementation->PutStatic("boolVal", true);
        implementation->PutStatic("intVal", 1);
        implementation->PutStatic("doubleVal", 2.0);
    }

    std::unique_ptr<DataBufferInterface> implementation;

private:
    NiceMock<FakeCallback> fakeCallback;
};

class BasicDataBuffer_GetKeys_Test : public ::testing::Test
{
public:
    BasicDataBuffer_GetKeys_Test()
    {
        implementation = std::make_unique<BasicDataBufferImplementation>(&fakeRti, &fakeCallback);

        // fill some test data
        implementation->PutStatic("level1a", 0);
        implementation->PutStatic("level1b/level2a", 0);
        implementation->PutStatic("level1b/level2b", 0);
        implementation->PutStatic("level1c/level2c/level3a", 0);
        implementation->PutStatic("level1c/level2c/level3b", 0);

        implementation->PutCyclic(0, "level1c", 0);
        implementation->PutCyclic(0, "level1d/level2a", 0);
        implementation->PutCyclic(0, "level1d/level2b", 0);
        implementation->PutCyclic(0, "level1e/level2c", 0);
        implementation->PutCyclic(0, "level1e/level2d", 0);
        implementation->PutCyclic(1, "level1f", 0);

        //implementation->PutAcyclic(0, 0, "level1i",  Acyclic{"name", {{0, 1}}, {{2, 3}}, {}});
    }

    std::unique_ptr<DataBufferInterface> implementation;

private:
    NiceMock<FakeCallback> fakeCallback;
};

namespace openpass::databuffer {

std::ostream& operator<<(std::ostream& stream, CyclicRow const& row)
{
  stream << ", entityId = " << row.entityId << ", key = " << row.key << " , value = ";

  std::visit(openpass::utils::FlatParameter::to_string([&stream](const std::string& valueStr)
  {
      stream << valueStr;
  }, "|"), row.value);

  return stream << "}";
}

}

TEST(BasicDataBuffer, PutCyclicData_StoresData)
{
    FakeCallback fakeCallback;
    FakeStochastics fakeStochastics;

    const EntityId agentId = 1;
    const int value = 2;
    const std::string key{"key/one/two"};
    const CyclicRow expectedRow{agentId, key, value};

    auto ds = std::make_unique<TestBasicDataBuffer>(&fakeRti, &fakeCallback);

    ASSERT_THAT(ds, NotNull());

    ds->PutCyclic(agentId, key, value);

    EXPECT_THAT(ds->GetStore().back(), Eq(expectedRow));
}

TEST(BasicDataBuffer, PutCyclicData_CreatesIndices)
{
    FakeCallback fakeCallback;
    FakeStochastics fakeStochastics;

    const EntityId agentId = 1;
    const int value = 2;
    const std::string key{"key/one/two"};
    const CyclicRow expectedRow{agentId, key, value};
    const size_t expectedIndex = 0;

    auto ds = std::make_unique<TestBasicDataBuffer>(&fakeRti, &fakeCallback);

    ASSERT_THAT(ds, NotNull());

    ds->PutCyclic(agentId, key, value);

    auto &entityIdx = ds->GetEntityIdIndex();

    auto firstEntityIdx = entityIdx.equal_range(agentId).first;

    EXPECT_THAT(firstEntityIdx->second, Eq(expectedIndex));
}

TEST_F(BasicDataBuffer_GetCyclic_Test, GivenEntityIdAndKey_ReturnsCorrectData)
{
    const auto result1 = implementation->GetCyclic(1, "intVal");
    const auto result2 = implementation->GetCyclic(0, "intVal");

    ASSERT_THAT(result1->size(), Eq(1));
    ASSERT_THAT(result2->size(), Eq(1));
    EXPECT_THAT(result1->at(0), Eq(CyclicRow{1, "intVal", 2}));
    EXPECT_THAT(result2->at(0), Eq(CyclicRow{0, "intVal", 1}));
}

TEST_F(BasicDataBuffer_GetCyclic_Test, GivenKey_ReturnsCorrectResult)
{
    const auto result = implementation->GetCyclic(std::nullopt, "intVal");

    ASSERT_THAT(result->size(), Eq(3));
    EXPECT_THAT(result->at(0), Eq(CyclicRow{0, "intVal", 1}));
    EXPECT_THAT(result->at(1), Eq(CyclicRow{1, "intVal", 2}));
    EXPECT_THAT(result->at(2), Eq(CyclicRow{2, "intVal", 3}));
}

TEST_F(BasicDataBuffer_GetCyclic_Test, GivenEntityIdAndWildcard_ReturnsCorrectResult)
{
    const auto result = implementation->GetCyclic(2, "*");

    ASSERT_THAT(result->size(), Eq(2));
    EXPECT_THAT(result->at(0), Eq(CyclicRow{2, "intVal", 3}));
    EXPECT_THAT(result->at(1), Eq(CyclicRow{2, "intVal2", 23}));
}

TEST_F(BasicDataBuffer_GetCyclic_Test, GivenWildcard_ReturnsCorrectResult)
{
    const auto result = implementation->GetCyclic(std::nullopt, "*");

    ASSERT_THAT(result->size(), Eq(18));

    EXPECT_THAT(result->at(0),  Eq(CyclicRow{0, "boolVal",   true}));
    EXPECT_THAT(result->at(1),  Eq(CyclicRow{0, "charVal",   'a'}));
    EXPECT_THAT(result->at(2),  Eq(CyclicRow{0, "intVal",    1}));
    EXPECT_THAT(result->at(3),  Eq(CyclicRow{0, "sizeVal",   static_cast<size_t>(2)}));
    EXPECT_THAT(result->at(4),  Eq(CyclicRow{0, "floatVal",  3.3f}));
    EXPECT_THAT(result->at(5),  Eq(CyclicRow{0, "doubleVal", 4.4}));
    EXPECT_THAT(result->at(6),  Eq(CyclicRow{0, "stringVal", "str"}));

    EXPECT_THAT(result->at(7),  Eq(CyclicRow{0, "boolVec",   std::vector<bool>{true, false}}));
    EXPECT_THAT(result->at(8),  Eq(CyclicRow{0, "charVec",   std::vector<char>{'a'}}));
    EXPECT_THAT(result->at(9),  Eq(CyclicRow{0, "intVec",    std::vector<char>{1, 2}}));
    EXPECT_THAT(result->at(10), Eq(CyclicRow{0, "sizeVec",   std::vector<size_t>{3, 4}}));
    EXPECT_THAT(result->at(11), Eq(CyclicRow{0, "floatVec",  std::vector<float>{5.5f, 6.6f}}));
    EXPECT_THAT(result->at(12), Eq(CyclicRow{0, "doubleVec", std::vector<double>{7.7, 8.8}}));
    EXPECT_THAT(result->at(13), Eq(CyclicRow{0, "stringVec", std::vector<std::string>{"str1", "str2"}}));

    EXPECT_THAT(result->at(14), Eq(CyclicRow{1, "intVal", 2}));
    EXPECT_THAT(result->at(15), Eq(CyclicRow{2, "intVal", 3}));

    EXPECT_THAT(result->at(16), Eq(CyclicRow{1, "intVal2", 22}));
    EXPECT_THAT(result->at(17), Eq(CyclicRow{2, "intVal2", 23}));

}

TEST_F(BasicDataBuffer_GetCyclic_Test, Result_IsIteratable)
{
    const auto result = implementation->GetCyclic(std::nullopt, "intVal2");

    ASSERT_THAT(result->size(), Eq(2));

    int expectedValue = 22;
    int id = 1;
    for (const auto& entryRef : *result)
    {
        const auto& entry = entryRef.get();
        EXPECT_THAT(entry, Eq(CyclicRow{id, "intVal2", expectedValue}));
        expectedValue += 1;
        id += 1;
    }
}

TEST(BasicDataBuffer, PutAcyclicData_StoresData)
{
    FakeCallback fakeCallback;
    FakeStochastics fakeStochastics;

    const EntityId agentId = 1;

    const std::string name{"name"};
    const openpass::type::TriggeringEntities triggeringEntities{{0, 1}};
    const openpass::type::AffectedEntities affectedEntities{{2, 3}};
    const std::string key{"key/one/two"};
    const Parameter params{{"key", "value"}};
    const Acyclic acyclic{name, triggeringEntities, affectedEntities, params};
    const AcyclicRow expectedRow{agentId, key, acyclic};

    auto ds = std::make_unique<TestBasicDataBuffer>(&fakeRti, &fakeCallback);

    ASSERT_THAT(ds, NotNull());

    ds->PutAcyclic(agentId, key, acyclic);

    EXPECT_THAT(ds->GetAcyclicStore().back(), Eq(expectedRow));
}

TEST_F(BasicDataBuffer_GetAcyclic_Test, GivenKey_ReturnsCorrectResult)
{
    const auto result = implementation->GetAcyclic(std::nullopt, "key1");

    ASSERT_THAT(result->size(), Eq(3));

    EXPECT_THAT(result->at(0), Eq(AcyclicRow{0, "key1", Acyclic{"name", {{0, 1}}, {{2, 3}}, {{"param1", 1}}}}));
    EXPECT_THAT(result->at(1), Eq(AcyclicRow{0, "key1", Acyclic{"name", {{2, 3}}, {{4, 5}}, {{"param3", "paramVal3"}}}}));
    EXPECT_THAT(result->at(2), Eq(AcyclicRow{0, "key1", Acyclic{"name", {{3, 4}}, {{5, 6}}, {{"param4", std::vector<int>{0, 1, 2}}}}}));
}

TEST_F(BasicDataBuffer_GetAcyclic_Test, GivenWildcard_ReturnsCorrectResult)
{
    const auto result = implementation->GetAcyclic(std::nullopt, "*");

    ASSERT_THAT(result->size(), Eq(4));

    EXPECT_THAT(result->at(0), Eq(AcyclicRow{0, "key1", Acyclic{"name", {{0, 1}}, {{2, 3}}, {{"param1", 1}}}}));
    EXPECT_THAT(result->at(1), Eq(AcyclicRow{1, "key2", Acyclic{"name", {{1, 2}}, {{3, 4}}, {{"param2", 2.0}}}}));
    EXPECT_THAT(result->at(2), Eq(AcyclicRow{0, "key1", Acyclic{"name", {{2, 3}}, {{4, 5}}, {{"param3", "paramVal3"}}}}));
    EXPECT_THAT(result->at(3), Eq(AcyclicRow{0, "key1", Acyclic{"name", {{3, 4}}, {{5, 6}}, {{"param4", std::vector<int>{0, 1, 2}}}}}));
}

TEST(BasicDataBuffer, PutStaticData_StoresData)
{
    FakeCallback fakeCallback;

    const int value = 1;
    const std::string key{"key/one/two"};

    auto ds = std::make_unique<TestBasicDataBuffer>(&fakeRti, &fakeCallback);

    ASSERT_THAT(ds, NotNull());

    ds->PutStatic(key, value);

    Value result;
    ASSERT_NO_THROW(result = std::get<Value>(ds->GetStaticStore().at(key)));
    EXPECT_THAT(result, Eq(Value{value}));
}

TEST(BasicDataBuffer, PutStaticDataTwiceUsingSameKey_IgnoresSecondCall)
{
    FakeCallback fakeCallback;

    const int value1 = 1;
    const int value2 = 2;
    const std::string key{"key/one/two"};

    auto ds = std::make_unique<TestBasicDataBuffer>(&fakeRti, &fakeCallback);

    ASSERT_THAT(ds, NotNull());

    ds->PutStatic(key, value1);
    ds->PutStatic(key, value2);

    Value result;
    ASSERT_NO_THROW(result = std::get<Value>(ds->GetStaticStore().at(key)));
    EXPECT_THAT(result, Eq(Value{value1}));
}

TEST_F(BasicDataBuffer_Persistence_Test, FixtureSetup_StoresData)
{
    const auto acyclicResult = implementation->GetAcyclic(std::nullopt, "acyclicKey");
    const auto cyclicResult = implementation->GetCyclic(std::nullopt, "cyclicKey");
    const auto staticPersistentResult = implementation->GetStatic("persistentKey");
    const auto staticNonPersistentResult = implementation->GetStatic("nonpersistentKey");

    ASSERT_THAT(acyclicResult->size(), Eq(1));
    ASSERT_THAT(cyclicResult->size(), Eq(1));
    ASSERT_THAT(staticPersistentResult, SizeIs(1));
    ASSERT_THAT(staticNonPersistentResult, SizeIs(1));
}

TEST_F(BasicDataBuffer_Persistence_Test, ClearRun_KeepsPersistentStaticData)
{
    implementation->ClearRun();

    const auto staticPersistentResult = implementation->GetStatic("persistentKey");

    ASSERT_THAT(staticPersistentResult, SizeIs(1));
    EXPECT_THAT(staticPersistentResult.at(0), Eq(Value{1}));
}

TEST_F(BasicDataBuffer_Persistence_Test, ClearRun_DeletesNonPersisentData)
{
    implementation->ClearRun();

    const auto acyclicResult = implementation->GetAcyclic(std::nullopt, "acyclicKey");
    const auto cyclicResult = implementation->GetCyclic(std::nullopt, "cyclicKey");
    const auto staticNonPersistentResult = implementation->GetStatic("nonpersistentKey");

    ASSERT_THAT(acyclicResult->size(), Eq(0));
    ASSERT_THAT(cyclicResult->size(), Eq(0));
    ASSERT_THAT(staticNonPersistentResult, SizeIs(0));
}

TEST_F(BasicDataBuffer_Persistence_Test, ClearTimeStep_KeepsStaticData)
{
    implementation->ClearTimeStep();

    const auto staticPersistentResult = implementation->GetStatic("persistentKey");
    const auto staticNonPersistentResult = implementation->GetStatic("nonpersistentKey");

    ASSERT_THAT(staticPersistentResult, SizeIs(1));
    EXPECT_THAT(staticPersistentResult.at(0), Eq(Value{1}));
    ASSERT_THAT(staticNonPersistentResult, SizeIs(1));
    EXPECT_THAT(staticNonPersistentResult.at(0), Eq(Value{0}));
}

TEST_F(BasicDataBuffer_Persistence_Test, ClearTimeStep_DeletesNonStaticData)
{
    implementation->ClearTimeStep();

    const auto acyclicResult = implementation->GetAcyclic(std::nullopt, "acyclicKey");
    const auto cyclicResult = implementation->GetCyclic(std::nullopt, "cyclicKey");

    ASSERT_THAT(acyclicResult->size(), Eq(0));
    ASSERT_THAT(cyclicResult->size(), Eq(0));
}

TEST_F(BasicDataBuffer_GetStatic_Test, GivenKey_ReturnsValue)
{
    const auto boolResult = implementation->GetStatic("boolVal");
    const auto intResult = implementation->GetStatic("intVal");
    const auto doubleResult = implementation->GetStatic("doubleVal");

    ASSERT_THAT(boolResult,   SizeIs(1));
    ASSERT_THAT(intResult,    SizeIs(1));
    ASSERT_THAT(doubleResult, SizeIs(1));

    EXPECT_THAT(boolResult.at(0),   Eq(Value{true}));
    EXPECT_THAT(intResult.at(0),    Eq(Value{1}));
    EXPECT_THAT(doubleResult.at(0), Eq(Value{2.0}));
}

TEST_F(BasicDataBuffer_GetStatic_Test, GivenWildcard_ReturnsEmptyResult)
{
    const auto result = implementation->GetStatic("*");

    ASSERT_THAT(result, SizeIs(0));
}

TEST_F(BasicDataBuffer_GetKeys_Test, StaticsKey_ReturnsCorrectKeys)
{
    const auto keys1 = implementation->GetKeys("Statics");
    const auto keys2 = implementation->GetKeys("Statics/level1a");
    const auto keys3 = implementation->GetKeys("Statics/level1b");
    const auto keys4 = implementation->GetKeys("Statics/level1c/level2c");

    EXPECT_THAT(keys1, ElementsAre("level1a", "level1b", "level1c"));
    EXPECT_THAT(keys2, SizeIs(0));
    EXPECT_THAT(keys3, ElementsAre("level2a", "level2b"));
    EXPECT_THAT(keys4, ElementsAre("level3a", "level3b"));
}

TEST_F(BasicDataBuffer_GetKeys_Test, CyclicsKeyWithoutTimestampAndEntityId_ReturnsTimestamps)
{
    const auto keys1 = implementation->GetKeys("Cyclics");

    EXPECT_THAT(keys1, ElementsAre("0", "1"));

}

TEST_F(BasicDataBuffer_GetKeys_Test, CyclicsKeyWithTimestampAndEntityId_ReturnsKeys)
{
    const auto keys1 = implementation->GetKeys("Cyclics/0");
    const auto keys2 = implementation->GetKeys("Cyclics/0/level1e");
    const auto keys3 = implementation->GetKeys("Cyclics/1");

    EXPECT_THAT(keys1, ElementsAre("level1c", "level1d", "level1e"));
    EXPECT_THAT(keys2, ElementsAre("level2c", "level2d"));
    EXPECT_THAT(keys3, ElementsAre("level1f"));

}

TEST(BasicDataBuffer_Constants, Wildcard_IsSize1)
{
    ASSERT_THAT(WILDCARD, SizeIs(1));
}
