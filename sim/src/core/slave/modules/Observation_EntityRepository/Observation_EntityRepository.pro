# /*********************************************************************
# * Copyright (c) 2020 in-tech GmbH
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

#-----------------------------------------------------------------------------
# \file  Observation_Log.pro
# \brief This file contains the information for the QtCreator-project of the
# module Observation_Log
#-----------------------------------------------------------------------------/

DEFINES += OBSERVATION_ENTITYREPOSITORY_LIBRARY
CONFIG += OPENPASS_LIBRARY
include(../../../../../global.pri)

INCLUDEPATH += \
    ../../../.. \
    ../../../../..\

SOURCES += \
    observation_entityRepository.cpp \
    observation_entityRepositoryImplementation.cpp

HEADERS += \
    observation_entityRepositoryGlobal.h \
    observation_entityRepository.h \
    observation_entityRepositoryImplementation.h
