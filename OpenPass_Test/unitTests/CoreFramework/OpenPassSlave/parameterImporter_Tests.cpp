#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "common/helper/importerHelper.h"

#include "parameterImporter.h"
#include "parameters.h"

using namespace SimulationCommon;

TEST(ParameterImporter_UnitTests, ImportParametersSuccessfully)
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

    SimulationCommon::ModelParameters resultModelParameters;
    ASSERT_TRUE(ParameterImporter::ImportParameters(fakeDocumentRoot, resultModelParameters));

    auto resultBools = resultModelParameters.GetParametersBool();
    auto resultInts = resultModelParameters.GetParametersInt();
    auto resultIntVectors = resultModelParameters.GetParametersIntVector();
    auto resultDoubles = resultModelParameters.GetParametersDouble();
    auto resultDoubleVectors = resultModelParameters.GetParametersDoubleVector();
    auto resultStrings = resultModelParameters.GetParametersString();

    ASSERT_EQ(resultBools.at("FakeBoolOne"), true);
    ASSERT_EQ(resultBools.at("FakeBoolTwo"), false);
    ASSERT_EQ(resultInts.at("FakeIntOne"), 1);
    ASSERT_EQ(resultInts.at("FakeIntTwo"), 2);
    ASSERT_EQ(resultIntVectors.at("FakeIntVector").at(0), 5);
    ASSERT_EQ(resultIntVectors.at("FakeIntVector").at(1), 6);
    ASSERT_EQ(resultIntVectors.at("FakeIntVector").at(2), 7);
    ASSERT_EQ(resultDoubles.at("FakeDoubleOne"), 1.0);
    ASSERT_EQ(resultDoubles.at("FakeDoubleTwo"), 2.5);
    ASSERT_EQ(resultDoubleVectors.at("FakeDoubleVector").at(0), 1.5);
    ASSERT_EQ(resultDoubleVectors.at("FakeDoubleVector").at(1), 2.6);
    ASSERT_EQ(resultDoubleVectors.at("FakeDoubleVector").at(2), 3.7);
    ASSERT_EQ(resultStrings.at("FakeStringOne"), "FakeOne");
    ASSERT_EQ(resultStrings.at("FakeStringTwo"), "FakeTwo");
}

TEST(ParameterImporter_UnitTests, ImportParametersWithOneSimpleParameterListSuccessfully)
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

    SimulationCommon::ModelParameters resultModelParameters;

    ASSERT_TRUE(ParameterImporter::ImportParameters(fakeDocumentRoot, resultModelParameters));

    auto resultBools = resultModelParameters.GetParametersBool();
    auto resultStrings = resultModelParameters.GetParametersString();
    auto resultParameterLists = resultModelParameters.GetParameterLists();

    auto resultList = resultParameterLists.at("TestList");
    auto resultBoolsOfFirstListEntry = resultList.at(0)->GetParametersBool();
    auto resultIntsOfFirstListEntry = resultList.at(0)->GetParametersInt();

    auto resultIntsOfSecondListEntry = resultList.at(1)->GetParametersInt();
    auto resultDoublesOfSecondListEntry = resultList.at(1)->GetParametersDouble();

    ASSERT_EQ(resultBools.at("FakeBoolOne"), true);
    ASSERT_EQ(resultStrings.at("FakeStringOne"), "FakeOne");

    ASSERT_EQ(resultBoolsOfFirstListEntry.at("FakeBoolTwo"), true);
    ASSERT_EQ(resultIntsOfFirstListEntry.at("FakeIntOne"), 1);

    ASSERT_EQ(resultIntsOfSecondListEntry.at("FakeIntTwo"), 2);
    ASSERT_EQ(resultDoublesOfSecondListEntry.at("FakeDoubleOne"), 1.0);
}

TEST(ParameterImporter_UnitTests, ImportParametersWithTwoSimpleParameterListsSuccessfully)
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

    SimulationCommon::ModelParameters resultModelParameters;
    ASSERT_TRUE(ParameterImporter::ImportParameters(fakeDocumentRoot, resultModelParameters));

    auto resultParameterLists = resultModelParameters.GetParameterLists();

    auto resultListOne = resultParameterLists.at("TestListOne");
    auto resultBoolsOfFirstList = resultListOne.at(0)->GetParametersBool();
    auto resultIntsOfFirstList = resultListOne.at(0)->GetParametersInt();

    auto resultListTwo = resultParameterLists.at("TestListTwo");
    auto resultBoolsOfSecondList = resultListTwo.at(0)->GetParametersBool();
    auto resultIntsOfSecondList = resultListTwo.at(0)->GetParametersInt();

    ASSERT_EQ(resultBoolsOfFirstList.at("FakeBoolOne"), true);
    ASSERT_EQ(resultIntsOfFirstList.at("FakeIntOne"), 1);

    ASSERT_EQ(resultBoolsOfSecondList.at("FakeBoolTwo"), false);
    ASSERT_EQ(resultIntsOfSecondList.at("FakeIntTwo"), 2);
}

TEST(ParameterImporter_UnitTests, ImportParametersWithRecursiveParameterListsSuccessfully)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
                "<ComponentParameterSet Name = \"Regular\">"
                    "<List Name=\"MainList\">"
                        "<ListItem>"
                            "<List Name=\"SubList\">"
                                "<ListItem>"
                                    "<Bool Key=\"FakeBoolOne\" Value=\"true\"/>"
                                    "<Int Key=\"FakeIntOne\" Value=\"1\"/>"
                                "</ListItem>"
                            "</List>"
                        "</ListItem>"
                    "</List>"
                "</ComponentParameterSet>"
                );

    SimulationCommon::ModelParameters resultModelParameters;

    ASSERT_TRUE(ParameterImporter::ImportParameters(fakeDocumentRoot, resultModelParameters));
    auto resultParameterLists = resultModelParameters.GetParameterLists();

    auto resultMainList = resultParameterLists.at("MainList");
    auto subParameterLists = resultMainList.at(0)->GetParameterLists();

    auto resultSubList = subParameterLists.at("SubList");
    auto resultBoolsOfSubList = resultSubList.at(0)->GetParametersBool();
    auto resultIntsOfSubList = resultSubList.at(0)->GetParametersInt();

    ASSERT_EQ(resultBoolsOfSubList.at("FakeBoolOne"), true);
    ASSERT_EQ(resultIntsOfSubList.at("FakeIntOne"), 1);
}

TEST(ParameterImporter_UnitTests, ImportStochasticDsitributionSuccessfully)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
                "<ComponentParameterSet Name = \"Regular\">"
                    "<NormalDistribution Key=\"FakeOne\" Mean=\"2.5\" SD=\"0.3\" Min=\"1.91\" Max=\"3.09\"/>"
                    "<NormalDistribution Key=\"FakeTwo\" Mean=\"0.5\" SD=\"1\" Min=\"1.0\" Max=\"2.0\"/>"
                "</ComponentParameterSet>"
                );

    SimulationCommon::ModelParameters resultModelParameters;

    ASSERT_TRUE(ParameterImporter::ImportParameters(fakeDocumentRoot, resultModelParameters));

    const auto &resultNormalDitributions = resultModelParameters.GetParametersNormalDistribution();

    const auto resultFakeOne = resultNormalDitributions.at("FakeOne");
    const auto resultFakeTwo = resultNormalDitributions.at("FakeTwo");

    ASSERT_DOUBLE_EQ(resultFakeOne.mean, 2.5);
    ASSERT_DOUBLE_EQ(resultFakeOne.standardDeviation, 0.3);
    ASSERT_DOUBLE_EQ(resultFakeOne.min, 1.91);
    ASSERT_DOUBLE_EQ(resultFakeOne.max, 3.09);

    ASSERT_DOUBLE_EQ(resultFakeTwo.mean, 0.5);
    ASSERT_DOUBLE_EQ(resultFakeTwo.standardDeviation, 1);
    ASSERT_DOUBLE_EQ(resultFakeTwo.min, 1);
    ASSERT_DOUBLE_EQ(resultFakeTwo.max, 2.0);
}
