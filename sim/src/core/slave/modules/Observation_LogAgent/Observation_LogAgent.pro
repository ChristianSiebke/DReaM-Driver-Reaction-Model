# /*********************************************************************
# * Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
# *               2016, 2017, 2020 ITK Engineering GmbH
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

DEFINES += OBSERVATION_LOGAGENT_LIBRARY
CONFIG += OPENPASS_LIBRARY
include(../../../../../global.pri)

INCLUDEPATH += \
    ../../../.. \
    ../../../../.. \
    ../../.. \

SOURCES += \
    observationFileHandler.cpp \
    observation_log.cpp \
    observation_logImplementation.cpp \
    observationCyclics.cpp \
    runStatistic.cpp \
    runStatisticCalculation.cpp \

HEADERS += \
    observationFileHandler.h \
    observationLogConstants.h \
    observation_log.h \
    observation_logImplementation.h \
    observationCyclics.h \
    runStatistic.h \
    runStatisticCalculation.h \
