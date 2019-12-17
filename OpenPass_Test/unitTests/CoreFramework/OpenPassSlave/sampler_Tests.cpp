#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "fakeStochastics.h"
#include "sampler.h"

using ::testing::_;
using ::testing::Return;
using ::testing::NiceMock;

const openpass::common::RuntimeInformation fakeRti{openpass::common::Version{0,0,0}, {"", "", ""}};

TEST(Sampler_UnitTests, RollForReturnsTrue)
{
    NiceMock<FakeStochastics> fakeStochastic;
    ON_CALL(fakeStochastic, GetUniformDistributed(_,_)).WillByDefault(Return(0.5));

    Sampler sampler(fakeStochastic, fakeRti);

    ASSERT_TRUE(sampler.RollFor(0.5));
}

TEST(Sampler_UnitTests, RollForReturnsFalse)
{
    NiceMock<FakeStochastics> fakeStochastic;
    ON_CALL(fakeStochastic, GetUniformDistributed(_,_)).WillByDefault(Return(0.6));

    Sampler sampler(fakeStochastic, fakeRti);

    ASSERT_TRUE(!sampler.RollFor(0.5));
}

TEST(Sampler_UnitTests, RollForWithChanceZeroReturnsFalse)
{
    NiceMock<FakeStochastics> fakeStochastic;
    Sampler sampler(fakeStochastic, fakeRti);
    ASSERT_TRUE(!sampler.RollFor(0.0));
}

TEST(Sampler_UnitTests, RollUniformDistributedVectorIndex)
{
    size_t fakeSize = 5;

    NiceMock<FakeStochastics> fakeStochastic;
    ON_CALL(fakeStochastic, GetUniformDistributed(_,_)).WillByDefault(Return(0.5));

    Sampler sampler(fakeStochastic, fakeRti);
    unsigned int result = sampler.RollUniformDistributedVectorIndex(fakeSize);

    ASSERT_EQ(result, (unsigned int) 2);
}

TEST(Sampler_UnitTests, RollUniformDistributedVectorIndexWithRollEqualVectorSize)
{
    size_t fakeSize = 4;

    NiceMock<FakeStochastics> fakeStochastic;
    ON_CALL(fakeStochastic, GetUniformDistributed(_,_)).WillByDefault(Return(1.0));

    Sampler sampler(fakeStochastic, fakeRti);
    unsigned int result = sampler.RollUniformDistributedVectorIndex(fakeSize);

    ASSERT_EQ(result, (unsigned int) 3);
}

TEST(Sampler_UnitTests, SampleIntProbabilityReturnThirdElement)
{
    IntProbabilities fakeProbabilities = {{0, 0.0}, {1, 0.3}, {2, 0.7}};

    NiceMock<FakeStochastics> fakeStochastic;
    ON_CALL(fakeStochastic, GetUniformDistributed(_,_)).WillByDefault(Return(0.4));

    Sampler sampler(fakeStochastic, fakeRti);
    int result = sampler.SampleIntProbability(fakeProbabilities);

    ASSERT_EQ(result, 2);
}

TEST(Sampler_UnitTests, SampleIntProbabilityInvalidProbabilities)
{
    IntProbabilities fakeProbabilities = {{0, 0.0}, {1, 0.3}};

    NiceMock<FakeStochastics> fakeStochastic;
    ON_CALL(fakeStochastic, GetUniformDistributed(_,_)).WillByDefault(Return(0.4));

    Sampler sampler(fakeStochastic, fakeRti);

    try
    {
        sampler.SampleIntProbability(fakeProbabilities);
        ASSERT_TRUE(false);

    }
    catch(...)
    {
        ASSERT_TRUE(true);
    }
}
