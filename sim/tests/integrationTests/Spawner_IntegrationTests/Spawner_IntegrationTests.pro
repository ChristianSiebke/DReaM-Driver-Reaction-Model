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
# \file  Spawner_IntegationTests.pro
# \brief This file contains tests for the spawner module
#-----------------------------------------------------------------------------/
QT += xml

CONFIG += OPENPASS_GTEST \
          OPENPASS_GTEST_DEFAULT_MAIN
include(../../testing.pri)

SPAWNER_COMMON = $$OPEN_SRC/core/opSimulation/modules/Spawners/common
SPAWNER_PRERUN = $$OPEN_SRC/core/opSimulation/modules/Spawners/PreRunCommon

INCLUDEPATH += . \
    $$OPEN_SRC/core/opSimulation/modules/Spawners \
    $$SPAWNER_PRERUN \
    $$OPEN_SRC/core/ \
    $$OPEN_SRC/core/opSimulation/ \
    $$OPEN_SRC/core/opSimulation/modelInterface/ \
    $$OPEN_SRC/core/opSimulation/modelElements/ \
    $$OPEN_SRC/core/opSimulation/bindings/ \
    $$OPEN_SRC/core/opSimulation/framework/ \
    $$OPEN_SRC/.. \
    $$OPEN_SRC/../..

SOURCES += \
    $$SPAWNER_COMMON/WorldAnalyzer.cpp \
    $$SPAWNER_PRERUN/SpawnerPreRunCommon.cpp \
    Spawner_IntegrationTests.cpp \
    $$OPEN_SRC/core/opSimulation/modelElements/agentBlueprint.cpp \
    $$OPEN_SRC/core/opSimulation/modelElements/agent.cpp \
    $$OPEN_SRC/core/opSimulation/modelElements/channel.cpp \
    $$OPEN_SRC/core/opSimulation/modelElements/component.cpp \
    $$OPEN_SRC/core/opSimulation/modelElements/parameters.cpp \
    $$OPEN_SRC/core/opSimulation/bindings/modelBinding.cpp \
    $$OPEN_SRC/core/opSimulation/bindings/modelLibrary.cpp \
    $$OPEN_SRC/core/opSimulation/framework/agentDataPublisher.cpp \
    $$OPEN_SRC/core/opSimulation/framework/sampler.cpp \
    $$OPEN_SRC/core/common/log.cpp

HEADERS += \
    $$SPAWNER_COMMON/WorldAnalyzer.h \
    $$SPAWNER_PRERUN/SpawnerPreRunCommon.h \
    $$OPEN_SRC/core/opSimulation/framework/sampler.h \

