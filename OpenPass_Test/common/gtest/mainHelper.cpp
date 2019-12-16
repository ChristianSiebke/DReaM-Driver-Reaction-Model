/********************************************************
* Copyright (c) 2019 in-tech GmbH on behalf of BMW AG
*********************************************************/

#include <gtest/gtest.h>
#include <string>

#include "mainHelper.h"

void SetDefaultXmlOutput(int* argc, char **argv)
{
    for (int i = 1; i < *argc; ++i)
    {
        if (std::string(argv[i]) == "--default-xml")
        {
            std::string basename(argv[0]);

#ifdef WIN32
            basename = basename.substr(0, basename.length() - 4);   // strip extension
#endif
            ::testing::GTEST_FLAG(output) = std::string("xml:") + basename + ".xml";

            // remove argument from list
            for (int j = i; j != *argc; ++j)
            {
                argv[j] = argv[j + 1];
            }

            // decrement argument count
            *argc -= 1;

            break;
        }
    }
}
