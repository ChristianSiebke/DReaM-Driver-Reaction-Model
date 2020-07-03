/*********************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "Common/openPassUtils.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

TEST(vectorToString, Empty_ReturnsEmtpy)
{
    ASSERT_THAT(openpass::utils::vector::to_string(std::vector<bool>{}), "");
    ASSERT_THAT(openpass::utils::vector::to_string(std::vector<int>{}), "");
    ASSERT_THAT(openpass::utils::vector::to_string(std::vector<double>{}), "");
    ASSERT_THAT(openpass::utils::vector::to_string(std::vector<std::string>{}), "");
}

TEST(vectorToString, OneElement_ReturnsSingleElementWithoutDelimiter)
{
    ASSERT_THAT(openpass::utils::vector::to_string(std::vector<bool>{true}), "1");
    ASSERT_THAT(openpass::utils::vector::to_string(std::vector<int>{123}), "123");
    ASSERT_THAT(openpass::utils::vector::to_string(std::vector<double>{12.3}), "12.3");
    ASSERT_THAT(openpass::utils::vector::to_string(std::vector<std::string>{"Test"}), "Test");
}

TEST(vectorToString, SeveralElementsAndNoDelimiter_ReturnsElementsWithDefaultDelimiter)
{
    ASSERT_THAT(openpass::utils::vector::to_string(std::vector<bool>{true, false, true}), "1,0,1");
    ASSERT_THAT(openpass::utils::vector::to_string(std::vector<int>{123, 456, 789}), "123,456,789");
    ASSERT_THAT(openpass::utils::vector::to_string(std::vector<double>{12.3, 45.6, 78.9}), "12.3,45.6,78.9");
    ASSERT_THAT(openpass::utils::vector::to_string(std::vector<std::string>{"Test1", "Test2", "Test3"}), "Test1,Test2,Test3");
}

TEST(vectorToString, SeveralElementsAndCustomDelimiter_ReturnsElementsWithCustomDelimiter)
{
    ASSERT_THAT(openpass::utils::vector::to_string(std::vector<int>{123, 456, 789}, "/"), "123/456/789");
    ASSERT_THAT(openpass::utils::vector::to_string(std::vector<int>{123, 456, 789}, "/ "), "123/ 456/ 789");
    ASSERT_THAT(openpass::utils::vector::to_string(std::vector<int>{123, 456, 789}, "_THIS_IS_A_WEIRD_DELIMITER_"), "123_THIS_IS_A_WEIRD_DELIMITER_456_THIS_IS_A_WEIRD_DELIMITER_789");
}

TEST(vectorToString, DoublesWithLargeBandwidthAndCustomDelimiter_ReturnsElementsWithCustomDelimiter)
{
    // let see if compilers behave in the same way w.r.t std::to_string
    ASSERT_THAT(openpass::utils::vector::to_string(std::vector<double>{1.23e-6, 1e-3, 1, 1000.00001, 1.23e6}, " ### "), "1.23e-06 ### 0.001 ### 1 ### 1000 ### 1.23e+06");
}
