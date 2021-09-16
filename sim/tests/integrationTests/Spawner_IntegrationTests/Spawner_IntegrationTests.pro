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
# \file  slave_IntegationTests.pro
# \brief This file contains tests for the slave module
#-----------------------------------------------------------------------------/
QT += xml

CONFIG += OPENPASS_GTEST \
          OPENPASS_GTEST_DEFAULT_MAIN
include(../../testing.pri)

SPAWNER_COMMON = $$OPEN_SRC/core/slave/modules/Spawners/common
SPAWNER_PRERUN = $$OPEN_SRC/core/slave/modules/Spawners/PreRunCommon

INCLUDEPATH += . \
    $$OPEN_SRC/core/slave/modules/Spawners \
    $$SPAWNER_PRERUN \
    $$OPEN_SRC/core/ \
    $$OPEN_SRC/core/slave/ \
    $$OPEN_SRC/core/slave/modelInterface/ \
    $$OPEN_SRC/core/slave/modelElements/ \
    $$OPEN_SRC/core/slave/bindings/ \
    $$OPEN_SRC/core/slave/framework/ \
    $$OPEN_SRC/.. \
    $$OPEN_SRC/../..

SOURCES += \
    $$SPAWNER_COMMON/WorldAnalyzer.cpp \
    $$SPAWNER_PRERUN/SpawnerPreRunCommon.cpp \
    Spawner_IntegrationTests.cpp \
    $$OPEN_SRC/core/slave/modelElements/agentBlueprint.cpp \
    $$OPEN_SRC/core/slave/modelElements/agent.cpp \
    $$OPEN_SRC/core/slave/modelElements/channel.cpp \
    $$OPEN_SRC/core/slave/modelElements/component.cpp \
    $$OPEN_SRC/core/slave/modelElements/parameters.cpp \
    $$OPEN_SRC/core/slave/bindings/modelBinding.cpp \
    $$OPEN_SRC/core/slave/bindings/modelLibrary.cpp \
    $$OPEN_SRC/core/slave/framework/agentDataPublisher.cpp \
    $$OPEN_SRC/core/slave/framework/sampler.cpp \
    $$OPEN_SRC/core/common/log.cpp

HEADERS += \
    $$SPAWNER_COMMON/WorldAnalyzer.h \
    $$SPAWNER_PRERUN/SpawnerPreRunCommon.h \
    $$OPEN_SRC/core/slave/framework/sampler.h \

