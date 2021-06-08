# /*********************************************************************
# * Copyright (c) 2019, 2020 in-tech GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

#-----------------------------------------------------------------------------
# \file  SpawnPointCommonPreRun.pro
# \brief This file contains the information for the QtCreator-project of the
# module SpawnPointCommonPreRun
#-----------------------------------------------------------------------------/

# shortened .pro file name due to MinGW path length problems
TARGET = SpawnPointPreRunCommon

DEFINES += SPAWNPOINT_COMMON_PRERUN_LIBRARY
CONFIG += OPENPASS_LIBRARY
include(../../../../../../global.pri)

INCLUDEPATH += \
    .. \
    ../../../ \
    ../../../.. \
    ../../../../.. \
    ../../../../../.. \
    ../../../modelElements

DEPENDENCY_HEADERS = \
                     ../common/WorldAnalyzer.h \
                     ../../../modelElements/agentBlueprint.h \
                     ../../../framework/sampler.h

DEPENDENCY_SOURCES = \
                     ../common/WorldAnalyzer.cpp \
                     ../../../modelElements/agentBlueprint.cpp \
                     ../../../framework/sampler.cpp

SPAWNPOINT_HEADERS = \
                     SpawnPointPreRunCommonDefinitions.h \
                     SpawnPointPreRunCommonParameterExtractor.h \
                     SpawnPointPreRunCommon.h \
                     SpawnPointPreRunCommonExport.h \
                     SpawnPointPreRunCommonGlobal.h

SPAWNPOINT_SOURCES = \
                     SpawnPointPreRunCommon.cpp \
                     SpawnPointPreRunCommonExport.cpp

SOURCES += \
    $$DEPENDENCY_SOURCES \
    $$SPAWNPOINT_SOURCES

HEADERS += \
    $$DEPENDENCY_HEADERS \
    $$SPAWNPOINT_HEADERS
