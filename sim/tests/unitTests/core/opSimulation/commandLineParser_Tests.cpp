/********************************************************************************
 * Copyright (c) 2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "commandLineParser.h"

#include "QStringList"
#include "QString"

#include <list>
#include <algorithm>

using ::testing::SizeIs;

QStringList CREATE_ARGUMENTS(const std::list<std::string>& arguments)
{
    // prepend an element, where the executable name would be
    QStringList qArguments = QStringList() << "placeholder_excutableName";

    // insert argument
    std::transform(begin(arguments), end(arguments), std::back_inserter(qArguments),
    [](const std::string & a) { return QString::fromStdString(a); });

    // append a NULL, as expected to define the end of the arguments
    qArguments << NULL;

    return qArguments;
}

QStringList CREATE_EMPTY_ARGUMENTS()
{
    return CREATE_ARGUMENTS(std::list<std::string> {});
}

TEST(CommandLineParser, GivenAllValues_ParsesValuesAppropriately)
{
    auto qArguments = CREATE_ARGUMENTS(
    {
        "--logFile", "testLogFile",
        "--logLevel", "1234",
        "--lib", "testLibraryPath",
        "--configs", "testConfigPath",
        "--results", "testResultPath",
    });

    auto parsedArguments = CommandLineParser::Parse(qArguments);

    EXPECT_THAT(parsedArguments.logFile, "testLogFile");
    EXPECT_THAT(parsedArguments.logLevel, 1234);
    EXPECT_THAT(parsedArguments.libPath, "testLibraryPath");
    EXPECT_THAT(parsedArguments.configsPath, "testConfigPath");
    EXPECT_THAT(parsedArguments.resultsPath, "testResultPath");
}

TEST(CommandLineParser, GivenNoValues_SetDefaultsAndLogsEntryForEachDefaultedValue)
{
    auto parsedArguments = CommandLineParser::Parse(CREATE_EMPTY_ARGUMENTS());

    EXPECT_THAT(parsedArguments.logFile, "opSimulation.log");
    EXPECT_THAT(parsedArguments.logLevel, 0);
    EXPECT_THAT(parsedArguments.libPath, "modules");
    EXPECT_THAT(parsedArguments.configsPath, "configs");
    EXPECT_THAT(parsedArguments.resultsPath, "results");

    EXPECT_THAT(CommandLineParser::GetParsingLog(), SizeIs(5));
}
