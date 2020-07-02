#/*******************************************************************************
#* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

#-----------------------------------------------------------------------------
# \file DynamicsTrajectoryFollower_Tests.pro
# \brief This file contains tests for the TrajectoryFollower module
#-----------------------------------------------------------------------------/
CONFIG += OPENPASS_GTEST OPENPASS_GTEST_DEFAULT_MAIN
include(../../../testing.pri)

win32:QMAKE_CXXFLAGS += -Wa,-mbig-obj

TEST_SRC = dynamicsTrajectoryFollower_Tests.cpp \
           trajectoryTester.cpp

TRAJECTORYFOLLOWER_SRC = \
    $$relative_path($$OPEN_SRC)/Components/Dynamics_TrajectoryFollower/trajectoryFollowerImplementation.cpp

TRAJECTORYFOLLOWER_HDR = \
    $$relative_path($$OPEN_SRC)/Components/Dynamics_TrajectoryFollower/trajectoryFollowerImplementation.h

COMMON_HDR = \
    $$relative_path($$OPEN_SRC)/Common/vector2d.h

INCLUDEPATH += . \
    $$relative_path($$OPEN_SRC)/Components/Dynamics_TrajectoryFollower

HEADERS += \
    $$TRAJECTORYFOLLOWER_HDR \
    $$COMMON_HDR

SOURCES += \
    $$TEST_SRC \
    $$TRAJECTORYFOLLOWER_SRC
