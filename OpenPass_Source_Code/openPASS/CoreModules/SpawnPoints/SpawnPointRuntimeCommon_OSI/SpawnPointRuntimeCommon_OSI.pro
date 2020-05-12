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
# \file  SpawnPoint_OSI.pro
# \brief This file contains the information for the QtCreator-project of the
# module SpawnPoint_OSI
#-----------------------------------------------------------------------------/

DEFINES += SPAWNPOINT_COMMON_RUNTIME_LIBRARY
CONFIG += OPENPASS_LIBRARY
include(../../../../global.pri)

SUBDIRS += \
    . \

INCLUDEPATH += \
    $$SUBDIRS \
        ../../.. \
        ../../../CoreFramework/OpenPassSlave/modelElements \
        ../../../CoreFramework/OpenPassSlave/framework \
        ../../../CoreFramework/OpenPassSlave/importer \
        ../../../CoreFramework/CoreShare \
        ..

DEPENDENCIES_HEADERS = \
                       ../Common/SpawnPointDefinitions.h \
                       ../Common/WorldAnalyzer.h \
                       ../../../CoreFramework/CoreShare/log.h \
                       ../../../CoreFramework/OpenPassSlave/modelElements/agentBlueprint.h \
                       ../../../CoreFramework/OpenPassSlave/framework/sampler.h

DEPENDENCIES_SOURCES = \
                       ../Common/WorldAnalyzer.cpp \
                       ../../../Common/vector2d.cpp \
                       ../../../CoreFramework/CoreShare/log.cpp \
                       ../../../CoreFramework/OpenPassSlave/modelElements/agentBlueprint.cpp \
                       ../../../CoreFramework/OpenPassSlave/framework/sampler.cpp

SOURCES += \
    $$DEPENDENCIES_SOURCES \
    SpawnPointRuntimeCommon.cpp \
    SpawnPointRuntimeCommonExport.cpp

HEADERS += \
    $$DEPENDENCIES_HEADERS \
    SpawnPointRuntimeCommon.h \
    SpawnPointRuntimeCommonExport.h \
    SpawnPointRuntimeCommonGlobal.h \
    SpawnPointRuntimeCommonParameterExtractor.h \
    SpawnPointRuntimeCommonDefinitions.h
