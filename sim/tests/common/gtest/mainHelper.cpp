/********************************************************************************
 * Copyright (c) 2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

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
