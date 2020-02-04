#/*******************************************************************************
#* Copyright (c) 2017, 2018, 2019 in-tech GmbH
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

TEST_SRC = absoluteWorldCoordinates_Tests.cpp \
           dynamicsTrajectoryFollower_Tests.cpp \
           roadCoordinates_Tests.cpp \
           trajectoryFollowerCommonBase_Tests.cpp \
           trajectoryTester.cpp

TRAJECTORYFOLLOWER_SRC = \
    $$relative_path($$OPEN_SRC)/Components/Dynamics_TrajectoryFollower/dynamics_trajectoryFollower.cpp \
    $$relative_path($$OPEN_SRC)/Components/Dynamics_TrajectoryFollower/trajectoryFollowerCommonBase.cpp \
    $$relative_path($$OPEN_SRC)/Components/Dynamics_TrajectoryFollower/roadCoordinateTrajectoryFollower.cpp \
    $$relative_path($$OPEN_SRC)/Components/Dynamics_TrajectoryFollower/absoluteWorldCoordinateTrajectoryFollower.cpp \

TRAJECTORYFOLLOWER_HDR = \
    $$relative_path($$OPEN_SRC)/Components/Dynamics_TrajectoryFollower/dynamics_trajectoryFollower.h \
    $$relative_path($$OPEN_SRC)/Components/Dynamics_TrajectoryFollower/trajectoryFollowerCommonBase.h \
    $$relative_path($$OPEN_SRC)/Components/Dynamics_TrajectoryFollower/roadCoordinateTrajectoryFollower.h \
    $$relative_path($$OPEN_SRC)/Components/Dynamics_TrajectoryFollower/absoluteWorldCoordinateTrajectoryFollower.h \

COMMON_SRC = \
    $$relative_path($$OPEN_SRC)/Common/vector2d.cpp

COMMON_HDR = \
    $$relative_path($$OPEN_SRC)/Common/vector2d.h \
    $$relative_path($$OPEN_SRC)/Common/componentStateChangeEvent.h

INCLUDEPATH += . \
    $$relative_path($$OPEN_SRC)/Components/Dynamics_TrajectoryFollower \
    $$relative_path($$OPEN_SRC)/CoreFramework/CoreShare/Cephes

HEADERS += \
    $$TRAJECTORYFOLLOWER_HDR \
    $$COMMON_HDR \
    $$relative_path($$OPEN_SRC)/CoreFramework/CoreShare/log.h \
    $$relative_path($$OPEN_SRC)/CoreFramework/CoreShare/xmlParser.h \
    $$relative_path($$OPEN_SRC)/CoreFramework/OpenPassSlave/importer/csvParser.h \
    $$relative_path($$OPEN_SRC)/CoreFramework/OpenPassSlave/importer/trajectory.h \
    $$relative_path($$OPEN_SRC)/CoreFramework/OpenPassSlave/importer/trajectoryImporter.h

SOURCES += \
    $$TEST_SRC \
    $$TRAJECTORYFOLLOWER_SRC \
    $$COMMON_SRC \
    $$relative_path($$OPEN_SRC)/CoreFramework/CoreShare/log.cpp \
    $$relative_path($$OPEN_SRC)/CoreFramework/CoreShare/xmlParser.cpp \
    $$relative_path($$OPEN_SRC)/CoreFramework/OpenPassSlave/importer/csvParser.cpp \
    $$relative_path($$OPEN_SRC)/CoreFramework/OpenPassSlave/importer/trajectory.cpp \
    $$relative_path($$OPEN_SRC)/CoreFramework/OpenPassSlave/importer/trajectoryImporter.cpp
