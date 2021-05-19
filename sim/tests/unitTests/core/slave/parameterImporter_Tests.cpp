#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "common/helper/importerHelper.h"

#include "parameterImporter.h"
#include "modelElements/parameters.h"

using ::testing::Eq;
using ::testing::SizeIs;

namespace op = openpass::parameter;

TEST(ParameterImporter, ImportParametersSuccessfully)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
              "<Profiles>"
                 "<ProfileGroup Type = \"TestGroup\">"
                    "<Profile Name = \"TestName\">"
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
                    "</Profile>"
                 "</ProfileGroup>"
              "</Profiles>"
                );

    QDomElement fakeParameter = documentRootFromString(
                    "<Profile Name = \"TestName\">"
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
                    "</Profile>"
                );

    namespace op = openpass::parameter;

    op::ParameterSetLevel1 parameter;
    EXPECT_NO_THROW(parameter = op::Import(fakeParameter, fakeDocumentRoot));
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
              "<Profiles>"
                 "<ProfileGroup Type = \"TestGroup\">"
                    "<Profile Name = \"TestName\">"
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
                    "</Profile>"
                 "</ProfileGroup>"
              "</Profiles>"
                );

    QDomElement fakeParameter = documentRootFromString(
                    "<Profile Name = \"TestName\">"
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
                    "</Profile>"
                );

    op::ParameterSetLevel1 parameter;
    EXPECT_NO_THROW(parameter = op::Import(fakeParameter, fakeDocumentRoot));

    EXPECT_THAT(op::Get<bool>(parameter, "FakeBoolOne").value(), true);
    EXPECT_THAT(op::Get<std::string>(parameter, "FakeStringOne").value(), Eq("FakeOne"));

    auto testList = op::Get<op::internal::ParameterListLevel1>(parameter, "TestList");
    ASSERT_THAT(testList.value(), SizeIs(2));

    EXPECT_THAT(op::Get<bool>(testList->at(0), "FakeBoolTwo").value(), true);
    EXPECT_THAT(op::Get<int>(testList->at(0), "FakeIntOne").value(), 1);

    EXPECT_THAT(op::Get<int>(testList->at(1), "FakeIntTwo").value(), 2);
    EXPECT_THAT(op::Get<double>(testList->at(1), "FakeDoubleOne").value(), 1.0);
}

TEST(ParameterImporter, ImportParametersWithTwoSimpleParameterListsSuccessfully)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
              "<Profiles>"
                 "<ProfileGroup Type = \"TestGroup\">"
                    "<Profile Name = \"TestName\">"
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
                    "</Profile>"
                 "</ProfileGroup>"
              "</Profiles>"
                );

    QDomElement fakeParameter = documentRootFromString(
                    "<Profile Name = \"TestName\">"
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
                    "</Profile>"
                );

    op::ParameterSetLevel1 parameter;
    EXPECT_NO_THROW(parameter = op::Import(fakeParameter, fakeDocumentRoot));

    auto testListOne = op::Get<op::internal::ParameterListLevel1>(parameter, "TestListOne");
    auto testListTwo = op::Get<op::internal::ParameterListLevel1>(parameter, "TestListTwo");

    ASSERT_THAT(testListOne->at(0), SizeIs(2));
    ASSERT_THAT(testListTwo->at(0), SizeIs(2));

    EXPECT_THAT(op::Get<bool>(testListOne->at(0), "FakeBoolOne").value(), true);
    EXPECT_THAT(op::Get<int>(testListTwo->at(0), "FakeIntTwo").value(), 2);
}

TEST(ParameterImporter, ImportParametersWithNestedParameterListSuccessfully)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
              "<Profiles>"
                 "<ProfileGroup Type = \"TestGroup\">"
                    "<Profile Name = \"TestName\">"
                        "<Bool Key=\"FakeBoolOne\" Value=\"true\"/>"
                        "<List Name=\"TestList\">"
                            "<ListItem>"
                                "<Bool Key=\"FakeBoolTwo\" Value=\"true\"/>"
                            "</ListItem>"
                            "<ListItem>"
                                "<Int Key=\"FakeIntTwo\" Value=\"2\"/>"
                                "<List Name=\"TestListLevel2\">"
                                    "<ListItem>"
                                        "<Bool Key=\"FakeBoolThree\" Value=\"true\"/>"
                                        "<Int Key=\"FakeIntThree\" Value=\"3\"/>"
                                    "</ListItem>"
                                    "<ListItem>"
                                        "<Bool Key=\"FakeBoolThree\" Value=\"false\"/>"
                                        "<Int Key=\"FakeIntThree\" Value=\"4\"/>"
                                    "</ListItem>"
                                 "</List>"
                            "</ListItem>"
                        "</List>"
                        "<String Key=\"FakeStringOne\" Value=\"FakeOne\"/>"
                    "</Profile>"
                 "</ProfileGroup>"
              "</Profiles>"
                );

    QDomElement fakeParameter = documentRootFromString(
                    "<Profile Name = \"TestName\">"
                        "<Bool Key=\"FakeBoolOne\" Value=\"true\"/>"
                        "<List Name=\"TestList\">"
                            "<ListItem>"
                                "<Bool Key=\"FakeBoolTwo\" Value=\"true\"/>"
                            "</ListItem>"
                            "<ListItem>"
                                "<Int Key=\"FakeIntTwo\" Value=\"2\"/>"
                                "<List Name=\"TestListLevel2\">"
                                    "<ListItem>"
                                        "<Bool Key=\"FakeBoolThree\" Value=\"true\"/>"
                                        "<Int Key=\"FakeIntThree\" Value=\"3\"/>"
                                    "</ListItem>"
                                    "<ListItem>"
                                        "<Bool Key=\"FakeBoolThree\" Value=\"false\"/>"
                                        "<Int Key=\"FakeIntThree\" Value=\"4\"/>"
                                    "</ListItem>"
                                 "</List>"
                            "</ListItem>"
                        "</List>"
                        "<String Key=\"FakeStringOne\" Value=\"FakeOne\"/>"
                    "</Profile>"
                );

    op::ParameterSetLevel1 parameter;
    EXPECT_NO_THROW(parameter = op::Import(fakeParameter, fakeDocumentRoot));

    auto testListLevel1 = op::Get<op::internal::ParameterListLevel1>(parameter, "TestList");
    ASSERT_THAT(testListLevel1.value(), SizeIs(2));

    auto testListLevel2 = op::Get<op::internal::ParameterListLevel2>(testListLevel1->at(1), "TestListLevel2");

    ASSERT_THAT(testListLevel2.value(), SizeIs(2));
    EXPECT_THAT(op::Get<bool>(testListLevel2->at(0), "FakeBoolThree").value(), true);
    EXPECT_THAT(op::Get<int>(testListLevel2->at(0), "FakeIntThree").value(), 3);

    EXPECT_THAT(op::Get<bool>(testListLevel2->at(1), "FakeBoolThree").value(), false);
    EXPECT_THAT(op::Get<int>(testListLevel2->at(1), "FakeIntThree").value(), 4);
}

TEST(ParameterImporter, ImportStochasticDistributionSuccessfully)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
              "<Profiles>"
                 "<ProfileGroup Type = \"TestGroup\">"
                    "<Profile Name = \"TestName\">"
                        "<NormalDistribution Key=\"FakeOne\" Mean=\"2.5\" SD=\"0.3\" Min=\"1.91\" Max=\"3.09\"/>"
                        "<LogNormalDistribution Key=\"FakeTwo\" Mean=\"2.0\" SD=\"0.3\" Min=\"1.0\" Max=\"3.0\"/>"
                        "<LogNormalDistribution Key=\"FakeThree\" Mu=\"0.5\" Sigma=\"1\" Min=\"1.0\" Max=\"2.0\"/>"
                        "<UniformDistribution Key=\"FakeFour\" Min=\"1.\" Max=\"10.\"/>"
                        "<ExponentialDistribution Key=\"FakeFive\" Lambda=\"2.0\" Min=\"1.0\" Max=\"3.0\"/>"
                        "<ExponentialDistribution Key=\"FakeSix\" Mean=\"3.0\" Min=\"1.0\" Max=\"3.0\"/>"
                        "<GammaDistribution Key=\"FakeSeven\" Mean=\"2.0\" SD=\"0.5\" Min=\"1.0\" Max=\"3.0\"/>"
                        "<GammaDistribution Key=\"FakeEight\" Shape=\"2.2\" Scale=\"0.7\" Min=\"1.0\" Max=\"3.0\"/>"
                    "</Profile>"
                 "</ProfileGroup>"
              "</Profiles>"
                );

    QDomElement fakeParameter = documentRootFromString(
                    "<Profile Name = \"TestName\">"
                        "<NormalDistribution Key=\"FakeOne\" Mean=\"2.5\" SD=\"0.3\" Min=\"1.91\" Max=\"3.09\"/>"
                        "<LogNormalDistribution Key=\"FakeTwo\" Mean=\"2.0\" SD=\"0.3\" Min=\"1.0\" Max=\"3.0\"/>"
                        "<LogNormalDistribution Key=\"FakeThree\" Mu=\"0.5\" Sigma=\"1\" Min=\"1.0\" Max=\"2.0\"/>"
                        "<UniformDistribution Key=\"FakeFour\" Min=\"1.\" Max=\"10.\"/>"
                        "<ExponentialDistribution Key=\"FakeFive\" Lambda=\"2.0\" Min=\"1.0\" Max=\"3.0\"/>"
                        "<ExponentialDistribution Key=\"FakeSix\" Mean=\"3.0\" Min=\"1.0\" Max=\"3.0\"/>"
                        "<GammaDistribution Key=\"FakeSeven\" Mean=\"2.0\" SD=\"0.5\" Min=\"1.0\" Max=\"3.0\"/>"
                        "<GammaDistribution Key=\"FakeEight\" Shape=\"2.2\" Scale=\"0.7\" Min=\"1.0\" Max=\"3.0\"/>"
                    "</Profile>"
                );

    op::ParameterSetLevel1 parameter;
    EXPECT_NO_THROW(parameter = op::Import(fakeParameter, fakeDocumentRoot));

    auto opt1 = op::Get<op::StochasticDistribution>(parameter, "FakeOne");
    auto opt2 = op::Get<op::StochasticDistribution>(parameter, "FakeTwo");
    auto opt3 = op::Get<op::StochasticDistribution>(parameter, "FakeThree");
    auto opt4 = op::Get<op::StochasticDistribution>(parameter, "FakeFour");
    auto opt5 = op::Get<op::StochasticDistribution>(parameter, "FakeFive");
    auto opt6 = op::Get<op::StochasticDistribution>(parameter, "FakeSix");
    auto opt7 = op::Get<op::StochasticDistribution>(parameter, "FakeSeven");
    auto opt8 = op::Get<op::StochasticDistribution>(parameter, "FakeEight");

    ASSERT_TRUE(opt1.has_value() && std::holds_alternative<op::NormalDistribution>(opt1.value()));
    ASSERT_TRUE(opt2.has_value() && std::holds_alternative<op::LogNormalDistribution>(opt2.value()));
    ASSERT_TRUE(opt3.has_value() && std::holds_alternative<op::LogNormalDistribution>(opt3.value()));
    ASSERT_TRUE(opt4.has_value() && std::holds_alternative<op::UniformDistribution>(opt4.value()));
    ASSERT_TRUE(opt5.has_value() && std::holds_alternative<op::ExponentialDistribution>(opt5.value()));
    ASSERT_TRUE(opt6.has_value() && std::holds_alternative<op::ExponentialDistribution>(opt6.value()));
    ASSERT_TRUE(opt7.has_value() && std::holds_alternative<op::GammaDistribution>(opt7.value()));
    ASSERT_TRUE(opt8.has_value() && std::holds_alternative<op::GammaDistribution>(opt8.value()));

    auto normalDist = std::get<op::NormalDistribution>(opt1.value());
    auto logNormalDistTwo = std::get<op::LogNormalDistribution>(opt2.value());
    auto logNormalDistThree = std::get<op::LogNormalDistribution>(opt3.value());
    auto uniformDist = std::get<op::UniformDistribution>(opt4.value());
    auto expDistOne = std::get<op::ExponentialDistribution>(opt5.value());
    auto expDistTwo = std::get<op::ExponentialDistribution>(opt6.value());
    auto gammaDistOne = std::get<op::GammaDistribution>(opt7.value());
    auto gammaDistTwo = std::get<op::GammaDistribution>(opt8.value());

    EXPECT_DOUBLE_EQ(normalDist.GetMean(), 2.5);
    EXPECT_DOUBLE_EQ(normalDist.GetStandardDeviation(), 0.3);
    EXPECT_DOUBLE_EQ(normalDist.GetMin(), 1.91);
    EXPECT_DOUBLE_EQ(normalDist.GetMax(), 3.09);

    EXPECT_DOUBLE_EQ(logNormalDistTwo.GetMean(), 2.0);
    EXPECT_DOUBLE_EQ(logNormalDistTwo.GetStandardDeviation(), 0.3);
    EXPECT_DOUBLE_EQ(logNormalDistTwo.GetMin(), 1);
    EXPECT_DOUBLE_EQ(logNormalDistTwo.GetMax(), 3.0);

    EXPECT_DOUBLE_EQ(logNormalDistThree.GetMu(), 0.5);
    EXPECT_DOUBLE_EQ(logNormalDistThree.GetSigma(), 1);
    EXPECT_DOUBLE_EQ(logNormalDistThree.GetMin(), 1);
    EXPECT_DOUBLE_EQ(logNormalDistThree.GetMax(), 2.0);

    EXPECT_DOUBLE_EQ(uniformDist.GetMin(), 1);
    EXPECT_DOUBLE_EQ(uniformDist.GetMax(), 10);

    EXPECT_DOUBLE_EQ(expDistOne.GetLambda(), 2);
    EXPECT_DOUBLE_EQ(expDistOne.GetMin(), 1);
    EXPECT_DOUBLE_EQ(expDistOne.GetMax(), 3);

    EXPECT_DOUBLE_EQ(expDistTwo.GetMean(), 3);
    EXPECT_DOUBLE_EQ(expDistTwo.GetMin(), 1);
    EXPECT_DOUBLE_EQ(expDistTwo.GetMax(), 3);

    EXPECT_DOUBLE_EQ(gammaDistOne.GetMean(), 2);
    EXPECT_DOUBLE_EQ(gammaDistOne.GetStandardDeviation(), 0.5);
    EXPECT_DOUBLE_EQ(gammaDistOne.GetMin(), 1);
    EXPECT_DOUBLE_EQ(gammaDistOne.GetMax(), 3);

    EXPECT_DOUBLE_EQ(gammaDistTwo.GetShape(), 2.2);
    EXPECT_DOUBLE_EQ(gammaDistTwo.GetScale(), 0.7);
    EXPECT_DOUBLE_EQ(gammaDistTwo.GetMin(), 1);
    EXPECT_DOUBLE_EQ(gammaDistTwo.GetMax(), 3);
}

TEST(ParameterImporter, AbortReferenceLoops)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
              "<Profiles>"
                 "<ProfileGroup Type = \"TestGroup1\">"
                    "<Profile Name = \"TestProfile1\">"
                        "<NormalDistribution Key=\"TestDistribution1\" Mean=\"0.5\" SD=\"1\" Min=\"1.0\" Max=\"2.0\"/>"
                        "<Reference Name=\"TestProfile2\" Type=\"TestGroup2\"/>"
                    "</Profile>"
                 "</ProfileGroup>"
                 "<ProfileGroup Type = \"TestGroup2\">"
                    "<Profile Name = \"TestProfile2\">"
                        "<NormalDistribution Key=\"TestDistribution2\" Mean=\"2.5\" SD=\"0.3\" Min=\"1.91\" Max=\"3.09\"/>"
                        "<Reference Name=\"TestProfile1\" Type=\"TestGroup1\"/>"
                    "</Profile>"
                "</ProfileGroup>"
             "</Profiles>"
             );

    QDomElement fakeParameter = documentRootFromString(
                "<Profile Name = \"TestProfile1\">"
                    "<NormalDistribution Key=\"TestDistribution1\" Mean=\"0.5\" SD=\"1\" Min=\"1.0\" Max=\"2.0\"/>"
                    "<Reference Name=\"TestProfile2\" Type=\"TestGroup2\"/>"
                "</Profile>"
                );

    op::ParameterSetLevel1 parameter;
    EXPECT_THROW(parameter = op::Import(fakeParameter, fakeDocumentRoot), std::runtime_error);
}

TEST(ParameterImporter, ImportSingleReferenceSuccessfully)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
              "<Profiles>"
                 "<ProfileGroup Type = \"TestGroup1\">"
                    "<Profile Name = \"TestProfile1\">"
                        "<NormalDistribution Key=\"TestDistribution1\" Mean=\"1\" SD=\"1\" Min=\"1\" Max=\"1\"/>"
                        "<Reference Name=\"TestProfile2\" Type=\"TestGroup2\"/>"
                    "</Profile>"
                 "</ProfileGroup>"
                 "<ProfileGroup Type = \"TestGroup2\">"
                    "<Profile Name = \"TestProfile2\">"
                        "<NormalDistribution Key=\"TestDistribution2\" Mean=\"2\" SD=\"2\" Min=\"2\" Max=\"2\"/>"
                    "</Profile>"
                "</ProfileGroup>"
             "</Profiles>"
             );

    QDomElement fakeParameter = documentRootFromString(
                "<Profile Name = \"TestProfile1\">"
                    "<NormalDistribution Key=\"TestDistribution1\" Mean=\"1\" SD=\"1\" Min=\"1\" Max=\"1\"/>"
                    "<Reference Name=\"TestProfile2\" Type=\"TestGroup2\"/>"
                "</Profile>"
                );

    op::ParameterSetLevel1 parameter;
    EXPECT_NO_THROW(parameter = op::Import(fakeParameter, fakeDocumentRoot));

    auto opt1 = op::Get<op::StochasticDistribution>(parameter, "TestDistribution1");
    auto opt2 = op::Get<op::StochasticDistribution>(parameter, "TestDistribution2");

    ASSERT_TRUE(opt1.has_value() && std::holds_alternative<op::NormalDistribution>(opt1.value()));
    ASSERT_TRUE(opt2.has_value() && std::holds_alternative<op::NormalDistribution>(opt2.value()));

    auto normalDistOne = std::get<op::NormalDistribution>(opt1.value());
    auto normalDistTwo = std::get<op::NormalDistribution>(opt2.value());

    EXPECT_DOUBLE_EQ(normalDistOne.GetMean(), 1);
    EXPECT_DOUBLE_EQ(normalDistOne.GetStandardDeviation(), 1);
    EXPECT_DOUBLE_EQ(normalDistOne.GetMin(), 1);
    EXPECT_DOUBLE_EQ(normalDistOne.GetMax(), 1);

    EXPECT_DOUBLE_EQ(normalDistTwo.GetMean(), 2);
    EXPECT_DOUBLE_EQ(normalDistTwo.GetStandardDeviation(), 2);
    EXPECT_DOUBLE_EQ(normalDistTwo.GetMin(), 2);
    EXPECT_DOUBLE_EQ(normalDistTwo.GetMax(), 2.0);
}

TEST(Parameter_LogNormalDistribution, GetAndSetMu_MeanAndSdUnchanged)
{
    constexpr double mu{12.3};
    constexpr double sigma{1.2};
    openpass::parameter::LogNormalDistribution distribution{mu, sigma, 0, 100};

    const double mean = distribution.GetMean();
    const double sd = distribution.GetStandardDeviation();

    distribution.SetMu(distribution.GetMu());

    EXPECT_DOUBLE_EQ(mean, distribution.GetMean());
    EXPECT_DOUBLE_EQ(sd, distribution.GetStandardDeviation());
}

TEST(Parameter_LogNormalDistribution, GetAndSetSigma_MeanAndSdUnchanged)
{
    constexpr double mu{12.3};
    constexpr double sigma{1.2};
    openpass::parameter::LogNormalDistribution distribution{mu, sigma, 0, 100};

    const double mean = distribution.GetMean();
    const double sd = distribution.GetStandardDeviation();

    distribution.SetSigma(distribution.GetSigma());

    EXPECT_DOUBLE_EQ(mean, distribution.GetMean());
    EXPECT_DOUBLE_EQ(sd, distribution.GetStandardDeviation());
}

TEST(Parameter_LogNormalDistribution, GetAndSetMean_MuAndSigmaUnchanged)
{
    constexpr double mu{12.3};
    constexpr double sigma{1.2};
    openpass::parameter::LogNormalDistribution distribution{mu, sigma, 0, 100};

    distribution.SetMean(distribution.GetMean());

    EXPECT_DOUBLE_EQ(mu, distribution.GetMu());
    EXPECT_DOUBLE_EQ(sigma, distribution.GetSigma());
}

TEST(Parameter_LogNormalDistribution, GetAndSetSd_MuAndSigmaUnchanged)
{
    constexpr double mu{12.3};
    constexpr double sigma{1.2};
    openpass::parameter::LogNormalDistribution distribution{mu, sigma, 0, 100};

    distribution.SetStandardDeviation(distribution.GetStandardDeviation());

    EXPECT_DOUBLE_EQ(mu, distribution.GetMu());
    EXPECT_DOUBLE_EQ(sigma, distribution.GetSigma());
}

TEST(Parameter_LogNormalDistribution, CreateWithMeanSd_MeanAndSdAsSet)
{
    constexpr double mean{12.3};
    constexpr double sd{1.2};
    auto distribution = openpass::parameter::LogNormalDistribution::CreateWithMeanSd(mean, sd, 0, 100);

    EXPECT_DOUBLE_EQ(mean, distribution.GetMean());
    EXPECT_DOUBLE_EQ(sd, distribution.GetStandardDeviation());
}

TEST(Parameter_GammaDistribution, GetAndSetShape_MeanAndSdUnchanged)
{
    constexpr double shape{12.3};
    constexpr double scale{1.2};
    openpass::parameter::GammaDistribution distribution{shape, scale, 0, 100};

    const double mean = distribution.GetMean();
    const double sd = distribution.GetStandardDeviation();

    distribution.SetShape(distribution.GetShape());

    EXPECT_DOUBLE_EQ(mean, distribution.GetMean());
    EXPECT_DOUBLE_EQ(sd, distribution.GetStandardDeviation());
}

TEST(Parameter_GammaDistribution, GetAndSetScale_MeanAndSdUnchanged)
{
    constexpr double shape{12.3};
    constexpr double scale{1.2};
    openpass::parameter::GammaDistribution distribution{shape, scale, 0, 100};

    const double mean = distribution.GetMean();
    const double sd = distribution.GetStandardDeviation();

    distribution.SetScale(distribution.GetScale());

    EXPECT_DOUBLE_EQ(mean, distribution.GetMean());
    EXPECT_DOUBLE_EQ(sd, distribution.GetStandardDeviation());
}

TEST(Parameter_GammaDistribution, GetAndSetMean_ShapeAndScaleUnchanged)
{
    constexpr double shape{12.3};
    constexpr double scale{1.2};
    openpass::parameter::GammaDistribution distribution{shape, scale, 0, 100};

    distribution.SetMean(distribution.GetMean());

    EXPECT_DOUBLE_EQ(shape, distribution.GetShape());
    EXPECT_DOUBLE_EQ(scale, distribution.GetScale());
}

TEST(Parameter_GammaDistribution, GetAndSetSd_ShapeAndScaleUnchanged)
{
    constexpr double shape{12.3};
    constexpr double scale{1.2};
    openpass::parameter::GammaDistribution distribution{shape, scale, 0, 100};

    distribution.SetStandardDeviation(distribution.GetStandardDeviation());

    EXPECT_DOUBLE_EQ(shape, distribution.GetShape());
    EXPECT_DOUBLE_EQ(scale, distribution.GetScale());
}

TEST(Parameter_GammaDistribution, CreateWithMeanSd_MeanAndSdAsSet)
{
    constexpr double mean{12.3};
    constexpr double sd{1.2};
    auto distribution = openpass::parameter::GammaDistribution::CreateWithMeanSd(mean, sd, 0, 100);

    EXPECT_DOUBLE_EQ(mean, distribution.GetMean());
    EXPECT_DOUBLE_EQ(sd, distribution.GetStandardDeviation());
}
