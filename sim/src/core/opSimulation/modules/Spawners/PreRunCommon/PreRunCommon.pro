################################################################################
# Copyright (c) 2019-2020 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

#-----------------------------------------------------------------------------
# \file  PreRunCommon.pro
# \brief This file contains the information for the QtCreator-project of the
# module PreRunCommon
#-----------------------------------------------------------------------------/

# shortened .pro file name due to MinGW path length problems
TARGET = PreRunCommon

DEFINES += SPAWNER_COMMON_PRERUN_LIBRARY
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
                     SpawnerPreRunCommonDefinitions.h \
                     SpawnerPreRunCommonParameterExtractor.h \
                     SpawnerPreRunCommon.h \
                     SpawnerPreRunCommonExport.h \
                     SpawnerPreRunCommonGlobal.h

SPAWNPOINT_SOURCES = \
                     SpawnerPreRunCommon.cpp \
                     SpawnerPreRunCommonExport.cpp

SOURCES += \
    $$DEPENDENCY_SOURCES \
    $$SPAWNER_SOURCES

HEADERS += \
    $$DEPENDENCY_HEADERS \
    $$SPAWNER_HEADERS
