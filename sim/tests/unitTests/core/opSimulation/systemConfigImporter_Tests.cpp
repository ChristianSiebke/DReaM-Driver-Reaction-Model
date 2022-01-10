/********************************************************************************
 * Copyright (c) 2018-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "common/helper/importerHelper.h"
#include "systemConfigImporter.h"

using ::testing::SizeIs;
using ::testing::Eq;
using ::testing::Pair;
using ::testing::UnorderedElementsAre;
using ::testing::EndsWith;
using ::testing::ElementsAre;

using namespace Importer;

TEST(SystemConfigImporter_UnitTests, ImportParameters)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
                                       "<root>"
                                           "<parameter>"
                                               "<id>FakeBool</id>"
                                               "<type>bool</type>"
                                               "<unit/>"
                                               "<value>true</value>"
                                           "</parameter>"
                                           "<parameter>"
                                               "<id>FakeInt</id>"
                                               "<type>int</type>"
                                               "<unit/>"
                                               "<value>10</value>"
                                           "</parameter>"
                                           "<parameter>"
                                               "<id>FakeDouble</id>"
                                               "<type>double</type>"
                                               "<unit/>"
                                               "<value>1.2</value>"
                                           "</parameter>"
                                           "<parameter>"
                                               "<id>FakeString</id>"
                                               "<type>string</type>"
                                               "<unit/>"
                                               "<value>TestString</value>"
                                           "</parameter>"
                                           "<parameter>"
                                               "<id>FakeIntVector</id>"
                                               "<type>intVector</type>"
                                               "<unit/>"
                                               "<value>10,11,12</value>"
                                           "</parameter>"
                                           "<parameter>"
                                               "<id>FakeDoubleVector</id>"
                                               "<type>doubleVector</type>"
                                               "<unit/>"
                                               "<value>1.2,2.3,3.4</value>"
                                           "</parameter>"
                                           "<parameter>"
                                               "<id>FakeBoolVectorAlpha</id>"
                                               "<type>boolVector</type>"
                                               "<unit/>"
                                               "<value>true,false,true</value>"
                                           "</parameter>"
                                           "<parameter>"
                                               "<id>FakeBoolVectorNum</id>"
                                               "<type>boolVector</type>"
                                               "<unit/>"
                                               "<value>1,0,1</value>"
                                           "</parameter>"
                                           "<parameter>"
                                               "<id>FakeBoolVectorMixed</id>"
                                               "<type>boolVector</type>"
                                               "<unit/>"
                                               "<value>true,0,1,false</value>"
                                           "</parameter>"
                                       "</root>"
                                    );

    openpass::parameter::ParameterSetLevel1 parameter;
    EXPECT_NO_THROW(parameter = SystemConfigImporter::ImportSystemParameters(fakeDocumentRoot));

    EXPECT_THAT(parameter, SizeIs(9));
    EXPECT_THAT(openpass::parameter::Get<bool>(parameter, "FakeBool").value(), Eq(true));
    EXPECT_THAT(openpass::parameter::Get<int>(parameter, "FakeInt").value(), Eq(10));
    EXPECT_THAT(openpass::parameter::Get<double>(parameter, "FakeDouble").value(), Eq(1.2));
    EXPECT_THAT(openpass::parameter::Get<std::string>(parameter, "FakeString").value(), Eq("TestString"));
    EXPECT_THAT(openpass::parameter::Get<std::vector<int>>(parameter, "FakeIntVector").value(), Eq(std::vector<int>{10,11,12}));
    EXPECT_THAT(openpass::parameter::Get<std::vector<double>>(parameter, "FakeDoubleVector").value(), Eq(std::vector<double>{1.2,2.3,3.4}));
    EXPECT_THAT(openpass::parameter::Get<std::vector<bool>>(parameter, "FakeBoolVectorAlpha").value(), Eq(std::vector<bool>{true,false,true}));
    EXPECT_THAT(openpass::parameter::Get<std::vector<bool>>(parameter, "FakeBoolVectorNum").value(), Eq(std::vector<bool>{true,false,true}));
    EXPECT_THAT(openpass::parameter::Get<std::vector<bool>>(parameter, "FakeBoolVectorMixed").value(), Eq(std::vector<bool>{true,false,true,false}));
}

TEST(SystemConfigImporter_UnitTests, ImportParameters_ThrowsOnInvalidBoolean)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
       "<root>"
           "<parameter>"
               "<id>FakeBoolVector</id>"
               "<type>boolVector</type>"
               "<unit/>"
               "<value>true,abc,true</value>"
           "</parameter>"
       "</root>"
    );

    openpass::parameter::ParameterSetLevel1 parameter;
    EXPECT_THROW(parameter = SystemConfigImporter::ImportSystemParameters(fakeDocumentRoot), std::runtime_error);

    EXPECT_THAT(parameter, SizeIs(0));
}

TEST(SystemConfigImporter_UnitTests, ImportStochasticDistributionSuccessfully)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
                    "<root>"
                        "<parameter>"
                            "<id>FakeOne</id>"
                            "<type>normalDistribution</type>"
                            "<unit/>"
                            "<value>"
                                "<mean>2.5</mean>"
                                "<sd>0.3</sd>"
                                "<min>1.91</min>"
                                "<max>3.09</max>"
                            "</value>"
                        "</parameter>"
                        "<parameter>"
                            "<id>FakeTwo</id>"
                            "<type>logNormalDistribution</type>"
                            "<unit/>"
                            "<value>"
                                "<mean>2.0</mean>"
                                "<sd>0.3</sd>"
                                "<min>1.0</min>"
                                "<max>3.0</max>"
                            "</value>"
                        "</parameter>"
                        "<parameter>"
                            "<id>FakeThree</id>"
                            "<type>logNormalDistribution</type>"
                            "<unit/>"
                            "<value>"
                                "<mu>0.5</mu>"
                                "<sigma>1</sigma>"
                                "<min>1.0</min>"
                                "<max>2.0</max>"
                            "</value>"
                        "</parameter>"
                        "<parameter>"
                            "<id>FakeFour</id>"
                            "<type>uniformDistribution</type>"
                            "<unit/>"
                            "<value>"
                                "<min>1.0</min>"
                                "<max>10.0</max>"
                            "</value>"
                        "</parameter>"
                        "<parameter>"
                            "<id>FakeFive</id>"
                            "<type>exponentialDistribution</type>"
                            "<unit/>"
                            "<value>"
                                "<lambda>2.0</lambda>"
                                "<min>1.0</min>"
                                "<max>3.0</max>"
                            "</value>"
                        "</parameter>"
                        "<parameter>"
                            "<id>FakeSix</id>"
                            "<type>exponentialDistribution</type>"
                            "<unit/>"
                            "<value>"
                                "<mean>3.0</mean>"
                                "<min>1.0</min>"
                                "<max>3.0</max>"
                            "</value>"
                        "</parameter>"
                        "<parameter>"
                            "<id>FakeSeven</id>"
                            "<type>gammaDistribution</type>"
                            "<unit/>"
                            "<value>"
                                "<mean>2.0</mean>"
                                "<sd>0.5</sd>"
                                "<min>1.0</min>"
                                "<max>3.0</max>"
                            "</value>"
                        "</parameter>"
                        "<parameter>"
                            "<id>FakeEight</id>"
                            "<type>gammaDistribution</type>"
                            "<unit/>"
                            "<value>"
                                "<shape>2.2</shape>"
                                "<scale>0.7</scale>"
                                "<min>1.0</min>"
                                "<max>3.0</max>"
                            "</value>"
                            "<value shape=\"2.2\" scale=\"0.7\" min=\"1.0\" max=\"3.0\">true</value>"
                        "</parameter>"
                    "<root>"
                );

    openpass::parameter::ParameterSetLevel1 parameter;
    EXPECT_NO_THROW(parameter = SystemConfigImporter::ImportSystemParameters(fakeDocumentRoot));

    auto opt1 = openpass::parameter::Get<openpass::parameter::StochasticDistribution>(parameter, "FakeOne");
    auto opt2 = openpass::parameter::Get<openpass::parameter::StochasticDistribution>(parameter, "FakeTwo");
    auto opt3 = openpass::parameter::Get<openpass::parameter::StochasticDistribution>(parameter, "FakeThree");
    auto opt4 = openpass::parameter::Get<openpass::parameter::StochasticDistribution>(parameter, "FakeFour");
    auto opt5 = openpass::parameter::Get<openpass::parameter::StochasticDistribution>(parameter, "FakeFive");
    auto opt6 = openpass::parameter::Get<openpass::parameter::StochasticDistribution>(parameter, "FakeSix");
    auto opt7 = openpass::parameter::Get<openpass::parameter::StochasticDistribution>(parameter, "FakeSeven");
    auto opt8 = openpass::parameter::Get<openpass::parameter::StochasticDistribution>(parameter, "FakeEight");

    ASSERT_TRUE(opt1.has_value() && std::holds_alternative<openpass::parameter::NormalDistribution>(opt1.value()));
    ASSERT_TRUE(opt2.has_value() && std::holds_alternative<openpass::parameter::LogNormalDistribution>(opt2.value()));
    ASSERT_TRUE(opt3.has_value() && std::holds_alternative<openpass::parameter::LogNormalDistribution>(opt3.value()));
    ASSERT_TRUE(opt4.has_value() && std::holds_alternative<openpass::parameter::UniformDistribution>(opt4.value()));
    ASSERT_TRUE(opt5.has_value() && std::holds_alternative<openpass::parameter::ExponentialDistribution>(opt5.value()));
    ASSERT_TRUE(opt6.has_value() && std::holds_alternative<openpass::parameter::ExponentialDistribution>(opt6.value()));
    ASSERT_TRUE(opt7.has_value() && std::holds_alternative<openpass::parameter::GammaDistribution>(opt7.value()));
    ASSERT_TRUE(opt8.has_value() && std::holds_alternative<openpass::parameter::GammaDistribution>(opt8.value()));

    auto normalDist = std::get<openpass::parameter::NormalDistribution>(opt1.value());
    auto logNormalDistTwo = std::get<openpass::parameter::LogNormalDistribution>(opt2.value());
    auto logNormalDistThree = std::get<openpass::parameter::LogNormalDistribution>(opt3.value());
    auto uniformDist = std::get<openpass::parameter::UniformDistribution>(opt4.value());
    auto expDistOne = std::get<openpass::parameter::ExponentialDistribution>(opt5.value());
    auto expDistTwo = std::get<openpass::parameter::ExponentialDistribution>(opt6.value());
    auto gammaDistOne = std::get<openpass::parameter::GammaDistribution>(opt7.value());
    auto gammaDistTwo = std::get<openpass::parameter::GammaDistribution>(opt8.value());

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
