# /*********************************************************************
# * Copyright (c) 2019 in-tech GmbH
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

DEFINES += SPAWNPOINT_COMMON_PRERUN_LIBRARY
CONFIG += OPENPASS_LIBRARY
include(../../../../global.pri)

INCLUDEPATH += \
        . \
        .. \
        ../../../CoreFramework/OpenPassSlave/modelElements \
        ../../../CoreFramework/CoreShare \

DEPENDENCY_HEADERS = \
                     ../Common/WorldAnalyzer.h \
                     ../../../CoreFramework/CoreShare/log.h \
                     ../../../CoreFramework/OpenPassSlave/modelElements/agentBlueprint.h \
                     ../../../CoreFramework/OpenPassSlave/framework/sampler.h

DEPENDENCY_SOURCES = \
                     ../Common/WorldAnalyzer.cpp \
                     ../../../CoreFramework/CoreShare/log.cpp \
                     ../../../CoreFramework/OpenPassSlave/modelElements/agentBlueprint.cpp \
                     ../../../CoreFramework/OpenPassSlave/framework/sampler.cpp

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
