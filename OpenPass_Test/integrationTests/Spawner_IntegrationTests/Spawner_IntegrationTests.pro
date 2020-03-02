#/*******************************************************************************
#* Copyright (c) 2020 in-tech GmbH
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

#-----------------------------------------------------------------------------
# \file  OpenPassSlave_IntegationTests.pro
# \brief This file contains tests for the OpenPassSlave module
#-----------------------------------------------------------------------------/
QT += xml

CONFIG += OPENPASS_GTEST \
          OPENPASS_GTEST_DEFAULT_MAIN
include(../../testing.pri)

SPAWNER_COMMON = $$OPEN_SRC/CoreModules/SpawnPoints/Common
SPAWNER_PRERUN = $$OPEN_SRC/CoreModules/SpawnPoints/SpawnPointPreRunCommon

INCLUDEPATH += . \
    $$OPEN_SRC/CoreModules/SpawnPoints \
    $$SPAWNER_COMMON \
    $$SPAWNER_PRERUN \
    $$OPEN_SRC/CoreFramework/CoreShare/ \
    $$OPEN_SRC/CoreFramework/OpenPassSlave/ \
    $$OPEN_SRC/CoreFramework/OpenPassSlave/modelInterface/ \
    $$OPEN_SRC/CoreFramework/OpenPassSlave/modelElements/ \
    $$OPEN_SRC/CoreFramework/OpenPassSlave/observationInterface/ \
    $$OPEN_SRC/CoreFramework/OpenPassSlave/spawnPointInterface/ \
    $$OPEN_SRC/CoreFramework/OpenPassSlave/framework/

SOURCES += \
    $$SPAWNER_COMMON/WorldAnalyzer.cpp \
    $$SPAWNER_PRERUN/SpawnPointPreRunCommon.cpp \
    Spawner_IntegrationTests.cpp \
    $$OPEN_SRC/CoreFramework/CoreShare/parameters.cpp \
    $$OPEN_SRC/CoreFramework/OpenPassSlave/modelElements/agentBlueprint.cpp \
    $$OPEN_SRC/CoreFramework/OpenPassSlave/modelElements/agent.cpp \
    $$OPEN_SRC/CoreFramework/OpenPassSlave/modelElements/channel.cpp \
    $$OPEN_SRC/CoreFramework/OpenPassSlave/modelElements/component.cpp \
    $$OPEN_SRC/CoreFramework/OpenPassSlave/modelInterface/modelBinding.cpp \
    $$OPEN_SRC/CoreFramework/OpenPassSlave/modelInterface/modelLibrary.cpp \
    $$OPEN_SRC/CoreFramework/OpenPassSlave/framework/agentDataPublisher.cpp \
    $$OPEN_SRC/CoreFramework/OpenPassSlave/framework/sampler.cpp \
    $$OPEN_SRC/CoreFramework/CoreShare/log.cpp

HEADERS += \
    $$SPAWNER_COMMON/WorldAnalyzer.h \
    $$SPAWNER_PRERUN/SpawnPointPreRunCommon.h \
    $$OPEN_SRC/CoreFramework/OpenPassSlave/framework/sampler.h \

