/*********************************************************************
* Copyright (c) 2019 in-tech GmbH on behalf of BMW
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "directories.h"

using ::testing::Eq;

using namespace openpass::core;

#ifndef WIN32

TEST(Directories, ParseRelativePaths)
{
    Directories directories("/path", "lib", "configs", "results");
    EXPECT_THAT(directories.baseDir, "/path");
    EXPECT_THAT(directories.libraryDir, "/path/lib");
    EXPECT_THAT(directories.configurationDir, "/path/configs");
    EXPECT_THAT(directories.outputDir, "/path/results");
}

TEST(Directories, ParseRelativePathsWithSlashes)
{
    Directories directories("/path", "lib/", "configs/", "results/");
    EXPECT_THAT(directories.baseDir, "/path");
    EXPECT_THAT(directories.libraryDir, "/path/lib");
    EXPECT_THAT(directories.configurationDir, "/path/configs");
    EXPECT_THAT(directories.outputDir, "/path/results");
}

TEST(Directories, ParseAbsolutePaths)
{
    Directories directories("/path", "/lib", "/configs", "/results");
    EXPECT_THAT(directories.baseDir, "/path");
    EXPECT_THAT(directories.libraryDir, "/lib");
    EXPECT_THAT(directories.configurationDir, "/configs");
    EXPECT_THAT(directories.outputDir, "/results");
}

TEST(Directories, ParseAbsolutePathsWithSlashes)
{
    Directories directories("/path", "/lib/", "/configs/", "/results/");
    EXPECT_THAT(directories.baseDir, "/path");
    EXPECT_THAT(directories.libraryDir, "/lib");
    EXPECT_THAT(directories.configurationDir, "/configs");
    EXPECT_THAT(directories.outputDir, "/results");
}

TEST(Directories, Concat)
{
    Directories directories("/path", "lib", "configs", "results");
    ASSERT_THAT(Directories::Concat(directories.libraryDir, "the_library"), "/path/lib/the_library");
}

TEST(Directories, ConcatVector)
{
    Directories directories("/path", "lib", "configs", "results");
    std::vector<std::string> libraries = {"the_library", "the_other_library"};
    auto concatLibraries = Directories::Concat(directories.libraryDir, libraries);
    ASSERT_THAT(concatLibraries[0], "/path/lib/the_library");
    ASSERT_THAT(concatLibraries[1], "/path/lib/the_other_library");
}

TEST(Directories, SamePath)
{
    Directories directories("/path", ".", ".", ".");
    EXPECT_THAT(directories.baseDir, "/path");
    EXPECT_THAT(directories.libraryDir, "/path");
    EXPECT_THAT(directories.configurationDir, "/path");
    EXPECT_THAT(directories.outputDir, "/path");
}

#else

TEST(Directories, ParseRelativePaths)
{
    Directories directories("C:\\path", "lib", "configs", "results");
    EXPECT_THAT(directories.baseDir, "C:/path");
    EXPECT_THAT(directories.libraryDir, "C:/path/lib");
    EXPECT_THAT(directories.configurationDir, "C:/path/configs");
    EXPECT_THAT(directories.outputDir, "C:/path/results");
}

TEST(Directories, ParseRelativePathsWithBackslashes)
{
    Directories directories("C:\\path", "lib\\", "configs\\", "results\\");
    EXPECT_THAT(directories.baseDir, "C:/path");
    EXPECT_THAT(directories.libraryDir, "C:/path/lib");
    EXPECT_THAT(directories.configurationDir, "C:/path/configs");
    EXPECT_THAT(directories.outputDir, "C:/path/results");
}

TEST(Directories, ParseAbsolutePaths)
{
    Directories directories("C:\\path", "D:\\lib", "E:\\configs", "F:\\results");
    EXPECT_THAT(directories.baseDir, "C:/path");
    EXPECT_THAT(directories.libraryDir, "D:/lib");
    EXPECT_THAT(directories.configurationDir, "E:/configs");
    EXPECT_THAT(directories.outputDir, "F:/results");
}

TEST(Directories, ParseAbsolutePathsWithBackslashes)
{
    Directories directories("C:\\path", "D:\\lib\\", "E:\\configs\\", "F:\\results\\");
    EXPECT_THAT(directories.baseDir, "C:/path");
    EXPECT_THAT(directories.libraryDir, "D:/lib");
    EXPECT_THAT(directories.configurationDir, "E:/configs");
    EXPECT_THAT(directories.outputDir, "F:/results");
}

TEST(Directories, Concat)
{
    Directories directories("C:\\path", "lib", "configs", "results");
    ASSERT_THAT(Directories::Concat(directories.libraryDir, "the_library"), "C:/path/lib/the_library");
}

TEST(Directories, ConcatVector)
{
    Directories directories("C:\\path", "lib", "configs", "results");
    std::vector<std::string> libraries = {"the_library", "the_other_library"};
    auto concatLibraries = Directories::Concat(directories.libraryDir, libraries);
    ASSERT_THAT(concatLibraries[0], "C:/path/lib/the_library");
    ASSERT_THAT(concatLibraries[1], "C:/path/lib/the_other_library");
}

TEST(Directories, SamePath)
{
    Directories directories("C:\\path", ".", ".", ".");
    EXPECT_THAT(directories.baseDir, "C:/path");
    EXPECT_THAT(directories.libraryDir, "C:/path");
    EXPECT_THAT(directories.configurationDir, "C:/path");
    EXPECT_THAT(directories.outputDir, "C:/path");
}

#endif
