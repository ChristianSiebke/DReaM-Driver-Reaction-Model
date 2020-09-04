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
# \file  Manipulator.pro
# \brief This file contains the information for the QtCreator-project of the
# module GenericManipulator
#-----------------------------------------------------------------------------/

DEFINES += MANIPULATOR_LIBRARY
CONFIG   += OPENPASS_LIBRARY
include (../../../../../global.pri)

INCLUDEPATH += \
    . \
    .. \
    ../../.. \
    ../../../.. \
    ../../../../.. \
    srcCollisionPostCrash

SOURCES += \
    CollisionManipulator.cpp \
    ComponentStateChangeManipulator.cpp \
    LaneChangeManipulator.cpp \
    ManipulatorCommonBase.cpp \
    ManipulatorExport.cpp \
    NoOperationManipulator.cpp \
    RemoveAgentsManipulator.cpp \
    SpeedActionManipulator.cpp \
    TrajectoryManipulator.cpp \
    srcCollisionPostCrash/collisionDetection_Impact_implementation.cpp \
    srcCollisionPostCrash/polygon.cpp \
    ../../../../core/common/coreDataPublisher.cpp

EVENT_HEADERS += \
    ../../../../common/events/componentStateChangeEvent.h \
    ../../../../common/events/collisionEvent.h \
    ../../../../common/events/laneChangeEvent.h \
    ../../../../common/events/speedActionEvent.h \
    ../../../../common/events/trajectoryEvent.h

HEADERS += \
    $$EVENT_HEADERS \
    CollisionManipulator.h \
    ComponentStateChangeManipulator.h \
    CustomCommandFactory.h \
    LaneChangeManipulator.h \
    ManipulatorCommonBase.h \
    ManipulatorExport.h \
    ManipulatorGlobal.h \
    NoOperationManipulator.h \
    RemoveAgentsManipulator.h \
    SpeedActionManipulator.h \
    TrajectoryManipulator.h \
    srcCollisionPostCrash/collisionDetection_Impact_implementation.h \
    srcCollisionPostCrash/polygon.h \
    ../../../../core/common/coreDataPublisher.h
