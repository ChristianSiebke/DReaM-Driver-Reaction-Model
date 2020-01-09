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

DEFINES += DYNAMICS_TRAJECTORY_FOLLOWER_LIBRARY
CONFIG += OPENPASS_LIBRARY
include(../../../global.pri)

QT += xml

SUBDIRS +=  . \
            ../../CoreFramework/CoreShare/Cephes

INCLUDEPATH += $$SUBDIRS \
            ../../Common \
            ../../CoreFramework/CoreShare \
            ../../CoreFramework/OpenPassSlave/modelElements \
            ../../Interfaces \
            ..

SOURCES += $$getFiles(SUBDIRS, cpp) \
           $$getFiles(SUBDIRS, c) \
            ../../Common/vector2d.cpp \
            ../../CoreFramework/CoreShare/log.cpp \

HEADERS += $$getFiles(SUBDIRS, h) \
            ../../Common/vector2d.h \
            ../../CoreFramework/CoreShare/log.h \

