# /*********************************************************************
# * Copyright (c) 2017, 2018, 2019 in-tech GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

#-----------------------------------------------------------------------------
# \file  EventDetector.pro
# \brief This file contains the information for the QtCreator-project of the
# module EventDetector
#-----------------------------------------------------------------------------/

DEFINES += EVENT_DETECTOR_LIBRARY
CONFIG += OPENPASS_LIBRARY
include(../../../global.pri)

INCLUDEPATH += . \
            ../../Interfaces \
            ../../CoreFramework/CoreShare \
            ../../Common \
            ..

SOURCES += \
    CollisionDetector.cpp \
    ConditionalEventDetector.cpp \
    EventDetectorCommonBase.cpp \
    EventDetectorExport.cpp \
    ../../CoreFramework/CoreShare/log.cpp \
    ../../Common/eventDetectorDefinitions.cpp

HEADERS += \
    ../../Common/vector2d.h \
    ../../Common/eventDetectorDefinitions.h \
    ../../CoreFramework/CoreShare/log.h
