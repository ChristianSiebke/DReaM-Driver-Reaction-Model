# /*********************************************************************
# * Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

#-----------------------------------------------------------------------------
# \file  Dynamics_TrajectoryFollower.pro
# \brief This file contains the information for the QtCreator-project of the
# module Dynamics_TrajectoryFollower
#-----------------------------------------------------------------------------/

# shortened .pro file name due to MinGW path length problems
TARGET = Dynamics_TrajectoryFollower

DEFINES += DYNAMICS_TRAJECTORY_FOLLOWER_LIBRARY
CONFIG += OPENPASS_LIBRARY
include(../../../global.pri)

SUBDIRS += .\
    src

INCLUDEPATH += \
    $$SUBDIRS \
    ../../.. \
    ../..

SOURCES += \
    dynamics_tf.cpp \
    src/tfImplementation.cpp

HEADERS += \
    dynamics_tf.h \
    src/tfImplementation.h

