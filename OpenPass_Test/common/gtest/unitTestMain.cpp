#include <gtest/gtest.h>

#include "mainHelper.h"

int main(int argc, char **argv)
{
    SetDefaultXmlOutput(&argc, argv);
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

