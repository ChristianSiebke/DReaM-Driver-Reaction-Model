/********************************************************************************
 * Copyright (c) 2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "common/commonTools.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"


using ::testing::ElementsAre;
using ::testing::SizeIs;

TEST(TokenizeString, EmptyString_ReturnsEmptyVector)
{
    const std::string str;
    ASSERT_THAT(CommonHelper::TokenizeString(str, ','), SizeIs(0));
}

TEST(TokenizeString, OneElement_ReturnsElementInVector)
{
    const std::string str{"Elem1"};
    const auto result = CommonHelper::TokenizeString(str, ',');
    ASSERT_THAT(result, SizeIs(1));
    EXPECT_THAT(result, ElementsAre("Elem1"));
}

TEST(TokenizeString, MultipleElements_ReturnsElementsInVector)
{
    const std::string str{"Elem1,Elem2,Elem3"};
    const auto result = CommonHelper::TokenizeString(str, ',');
    ASSERT_THAT(result, SizeIs(3));
    EXPECT_THAT(result, ElementsAre("Elem1", "Elem2", "Elem3"));
}

TEST(TokenizeString, MultipleElementsWithWhitespace_ReturnsElementsInVector)
{
    const std::string str{"Elem 1,Elem 2,Elem 3"};
    const auto result = CommonHelper::TokenizeString(str, ',');
    ASSERT_THAT(result, SizeIs(3));
    EXPECT_THAT(result, ElementsAre("Elem 1", "Elem 2", "Elem 3"));
}

TEST(TokenizeString, MultipleElementsWithWhitespace_ReturnsTrimmedElementsInVector)
{
    const std::string str{" Elem1, Elem2  , Elem3  "};
    const auto result = CommonHelper::TokenizeString(str, ',');
    ASSERT_THAT(result, SizeIs(3));
    EXPECT_THAT(result, ElementsAre("Elem1", "Elem2", "Elem3"));
}

TEST(TokenizeString, MultipleElementsAndEmptyElementWithWhitespaceInMid_ReturnsEmptyElementAsEmptyStringInVector)
{
    const std::string str{" Elem1,  , Elem3  "};
    const auto result = CommonHelper::TokenizeString(str, ',');
    ASSERT_THAT(result, SizeIs(3));
    EXPECT_THAT(result, ElementsAre("Elem1", "", "Elem3"));
}

TEST(TokenizeString, MultipleElementsAndEmptyElementWithoutWhitespaceInMid_ReturnsEmptyElementAsEmptyStringInVector)
{
    const std::string str{"Elem1,,Elem3"};
    const auto result = CommonHelper::TokenizeString(str, ',');
    ASSERT_THAT(result, SizeIs(3));
    EXPECT_THAT(result, ElementsAre("Elem1", "", "Elem3"));
}

TEST(TokenizeString, MultipleElementsAndEmptyElementWithWhitespaceAtBegin_ReturnsEmptyElementAsEmptyStringInVector)
{
    const std::string str{"  , Elem2, Elem3  "};
    const auto result = CommonHelper::TokenizeString(str, ',');
    ASSERT_THAT(result, SizeIs(3));
    EXPECT_THAT(result, ElementsAre("", "Elem2", "Elem3"));
}

TEST(TokenizeString, MultipleElementsAndEmptyElementWithoutWhitespaceAtBegin_ReturnsEmptyElementAsEmptyStringInVector)
{
    const std::string str{",Elem2,Elem3"};
    const auto result = CommonHelper::TokenizeString(str, ',');
    ASSERT_THAT(result, SizeIs(3));
    EXPECT_THAT(result, ElementsAre("", "Elem2", "Elem3"));
}

TEST(TokenizeString, MultipleElementsAndEmptyElementWithWhitespaceAtEnd_ReturnsEmptyElementAsEmptyStringInVector)
{
    const std::string str{" Elem1, Elem2 , "};
    const auto result = CommonHelper::TokenizeString(str, ',');
    ASSERT_THAT(result, SizeIs(3));
    EXPECT_THAT(result, ElementsAre("Elem1", "Elem2", ""));
}

TEST(TokenizeString, MultipleElementsAndEmptyElementWithoutWhitespaceAtEnd_ReturnsEmptyElementAsEmptyStringInVector)
{
    const std::string str{"Elem1,Elem2,"};
    const auto result = CommonHelper::TokenizeString(str, ',');
    ASSERT_THAT(result, SizeIs(3));
    EXPECT_THAT(result, ElementsAre("Elem1", "Elem2", ""));
}
