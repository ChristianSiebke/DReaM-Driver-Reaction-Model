/********************************************************************************
 * Copyright (c) 2019-2020 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#include <gtest/gtest.h>

#include "mainHelper.h"

int main(int argc, char **argv)
{
    SetDefaultXmlOutput(&argc, argv);
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

