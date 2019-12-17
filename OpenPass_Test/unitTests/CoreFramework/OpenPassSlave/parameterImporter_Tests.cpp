#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "common/helper/importerHelper.h"

#include "parameterImporter.h"
#include "parameters.h"

using ::testing::Eq;
using ::testing::SizeIs;

namespace op = openpass::parameter;

TEST(ParameterImporter, ImportParametersSuccessfully)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
                "<ComponentParameterSet Name = \"Regular\">"
                    "<Bool Key=\"FakeBoolOne\" Value=\"true\"/>"
                    "<Bool Key=\"FakeBoolTwo\" Value=\"false\"/>"
                    "<Int Key=\"FakeIntOne\" Value=\"1\"/>"
                    "<Int Key=\"FakeIntTwo\" Value=\"2\"/>"
                    "<IntVector Key=\"FakeIntVector\" Value=\"5, 6, 7\" />"
                    "<Double Key=\"FakeDoubleOne\" Value=\"1.0\"/>"
                    "<Double Key=\"FakeDoubleTwo\" Value=\"2.5\"/>"
                    "<DoubleVector Key=\"FakeDoubleVector\" Value=\"1.5, 2.6, 3.7\" />"
                    "<String Key=\"FakeStringOne\" Value=\"FakeOne\"/>"
                    "<String Key=\"FakeStringTwo\" Value=\"FakeTwo\"/>"
                "</ComponentParameterSet>"
                );

    namespace op = openpass::parameter;

    op::Container parameter;
    EXPECT_NO_THROW(parameter = op::Import(fakeDocumentRoot));
    EXPECT_THAT(op::Get<bool>(parameter, "FakeBoolOne").value(), Eq(true));
    EXPECT_THAT(op::Get<bool>(parameter, "FakeBoolTwo").value(), Eq(false));
    EXPECT_THAT(op::Get<int>(parameter, "FakeIntOne").value(), Eq(1));
    EXPECT_THAT(op::Get<int>(parameter, "FakeIntTwo").value(), Eq(2));
    EXPECT_THAT(op::Get<std::vector<int>>(parameter, "FakeIntVector").value().at(0), Eq(5));
    EXPECT_THAT(op::Get<std::vector<int>>(parameter, "FakeIntVector").value().at(1), Eq(6));
    EXPECT_THAT(op::Get<std::vector<int>>(parameter, "FakeIntVector").value().at(2), Eq(7));
    EXPECT_THAT(op::Get<double>(parameter, "FakeDoubleOne").value(), Eq(1.0));
    EXPECT_THAT(op::Get<double>(parameter, "FakeDoubleTwo").value(), Eq(2.5));
    EXPECT_THAT(op::Get<std::vector<double>>(parameter, "FakeDoubleVector").value().at(0), Eq(1.5));
    EXPECT_THAT(op::Get<std::vector<double>>(parameter, "FakeDoubleVector").value().at(1), Eq(2.6));
    EXPECT_THAT(op::Get<std::vector<double>>(parameter, "FakeDoubleVector").value().at(2), Eq(3.7));
    EXPECT_THAT(op::Get<std::string>(parameter, "FakeStringOne").value(), Eq("FakeOne"));
    EXPECT_THAT(op::Get<std::string>(parameter, "FakeStringTwo").value(), Eq("FakeTwo"));
}

TEST(ParameterImporter, ImportParametersWithOneSimpleParameterListSuccessfully)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
                "<ComponentParameterSet Name = \"Regular\">"
                    "<Bool Key=\"FakeBoolOne\" Value=\"true\"/>"
                    "<List Name=\"TestList\">"
                        "<ListItem>"
                            "<Bool Key=\"FakeBoolTwo\" Value=\"true\"/>"
                            "<Int Key=\"FakeIntOne\" Value=\"1\"/>"
                        "</ListItem>"
                        "<ListItem>"
                            "<Int Key=\"FakeIntTwo\" Value=\"2\"/>"
                            "<Double Key=\"FakeDoubleOne\" Value=\"1.0\"/>"
                        "</ListItem>"
                    "</List>"
                    "<String Key=\"FakeStringOne\" Value=\"FakeOne\"/>"
                "</ComponentParameterSet>"
                );

    op::Container parameter;
    EXPECT_NO_THROW(parameter = op::Import(fakeDocumentRoot));

    EXPECT_THAT(op::Get<bool>(parameter, "FakeBoolOne").value(), true);
    EXPECT_THAT(op::Get<std::string>(parameter, "FakeStringOne").value(), Eq("FakeOne"));

    auto testList = op::Get<op::internal::ParameterList>(parameter, "TestList");
    ASSERT_THAT(testList.value(), SizeIs(2));

    EXPECT_THAT(op::Get<bool>(testList->at(0), "FakeBoolTwo").value(), true);
    EXPECT_THAT(op::Get<int>(testList->at(0), "FakeIntOne").value(), 1);

    EXPECT_THAT(op::Get<int>(testList->at(1), "FakeIntTwo").value(), 2);
    EXPECT_THAT(op::Get<double>(testList->at(1), "FakeDoubleOne").value(), 1.0);
}

TEST(ParameterImporter, ImportParametersWithTwoSimpleParameterListsSuccessfully)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
                "<ComponentParameterSet Name = \"Regular\">"
                    "<List Name=\"TestListOne\">"
                        "<ListItem>"
                            "<Bool Key=\"FakeBoolOne\" Value=\"true\"/>"
                            "<Int Key=\"FakeIntOne\" Value=\"1\"/>"
                        "</ListItem>"
                    "</List>"
                    "<List Name=\"TestListTwo\">"
                        "<ListItem>"
                            "<Bool Key=\"FakeBoolTwo\" Value=\"false\"/>"
                            "<Int Key=\"FakeIntTwo\" Value=\"2\"/>"
                        "</ListItem>"
                    "</List>"
                "</ComponentParameterSet>"
                );

    op::Container parameter;
    EXPECT_NO_THROW(parameter = op::Import(fakeDocumentRoot));

    auto testListOne = op::Get<op::internal::ParameterList>(parameter, "TestListOne");
    auto testListTwo = op::Get<op::internal::ParameterList>(parameter, "TestListTwo");

    ASSERT_THAT(testListOne->at(0), SizeIs(2));
    ASSERT_THAT(testListTwo->at(0), SizeIs(2));

    EXPECT_THAT(op::Get<bool>(testListOne->at(0), "FakeBoolOne").value(), true);
    EXPECT_THAT(op::Get<int>(testListTwo->at(0), "FakeIntTwo").value(), 2);
}

TEST(ParameterImporter, ImportStochasticDsitributionSuccessfully)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
                "<ComponentParameterSet Name = \"Regular\">"
                    "<NormalDistribution Key=\"FakeOne\" Mean=\"2.5\" SD=\"0.3\" Min=\"1.91\" Max=\"3.09\"/>"
                    "<NormalDistribution Key=\"FakeTwo\" Mean=\"0.5\" SD=\"1\" Min=\"1.0\" Max=\"2.0\"/>"
                "</ComponentParameterSet>"
                );

    op::Container parameter;
    EXPECT_NO_THROW(parameter = op::Import(fakeDocumentRoot));

    auto normalDistOne = op::Get<op::NormalDistribution>(parameter, "FakeOne");
    auto normalDistTwo = op::Get<op::NormalDistribution>(parameter, "FakeTwo");

    ASSERT_THAT(normalDistOne.has_value(), true);
    ASSERT_THAT(normalDistTwo.has_value(), true);

    EXPECT_DOUBLE_EQ(normalDistOne->mean, 2.5);
    EXPECT_DOUBLE_EQ(normalDistOne->standardDeviation, 0.3);
    EXPECT_DOUBLE_EQ(normalDistOne->min, 1.91);
    EXPECT_DOUBLE_EQ(normalDistOne->max, 3.09);

    EXPECT_DOUBLE_EQ(normalDistTwo->mean, 0.5);
    EXPECT_DOUBLE_EQ(normalDistTwo->standardDeviation, 1);
    EXPECT_DOUBLE_EQ(normalDistTwo->min, 1);
    EXPECT_DOUBLE_EQ(normalDistTwo->max, 2.0);
}
