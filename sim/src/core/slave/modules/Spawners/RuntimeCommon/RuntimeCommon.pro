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
# \file  SpawnPointRuntimeCommon.pro
# \brief This file contains the information for the QtCreator-project of the
# module SpawnPointRuntimeCommon
#-----------------------------------------------------------------------------/

# shortened .pro file name due to MinGW path length problems
TARGET = SpawnPointRuntimeCommon

DEFINES += SPAWNPOINT_COMMON_RUNTIME_LIBRARY
CONFIG += OPENPASS_LIBRARY
include(../../../../../../global.pri)

SUBDIRS += \
    . \

INCLUDEPATH += \
    .. \
    ../../../ \
    ../../../.. \
    ../../../../.. \
    ../../../../../.. \
    ../../../modelElements

DEPENDENCY_HEADERS = \
                     ../common/WorldAnalyzer.h \
                     ../../../../common/log.h \
                     ../../../modelElements/agentBlueprint.h \
                     ../../../framework/sampler.h

DEPENDENCY_SOURCES = \
                     ../common/WorldAnalyzer.cpp \
                     ../../../../common/log.cpp \
                     ../../../modelElements/agentBlueprint.cpp \
                     ../../../framework/sampler.cpp

SOURCES += \
    $$DEPENDENCY_SOURCES \
    SpawnPointRuntimeCommon.cpp \
    SpawnPointRuntimeCommonExport.cpp

HEADERS += \
    $$DEPENDENCYS_HEADERS \
    SpawnPointRuntimeCommon.h \
    SpawnPointRuntimeCommonExport.h \
    SpawnPointRuntimeCommonGlobal.h \
    SpawnPointRuntimeCommonParameterExtractor.h \
    SpawnPointRuntimeCommonDefinitions.h
