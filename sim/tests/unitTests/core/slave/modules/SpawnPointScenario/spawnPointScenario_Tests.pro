# /*********************************************************************
# * Copyright (c) 2017 2018 2019 in-tech GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

CONFIG += OPENPASS_GTEST \
          OPENPASS_GTEST_DEFAULT_MAIN

include(../../../../../testing.pri)

SPAWNPOINTS = $$OPEN_SRC/core/slave/modules/Spawners

UNIT_UNDER_TEST = $$SPAWNPOINTS/Scenario

OPENPASS_SLAVE = $$OPEN_SRC/core/slave

CORE_SHARE = $$OPEN_SRC/core/common

FRAMEWORK = $$OPENPASS_SLAVE/framework

MODEL_ELEMENTS = $$OPENPASS_SLAVE/modelElements

win32:QMAKE_CXXFLAGS += -Wa,-mbig-obj

INCLUDEPATH += \
    $$SPAWNPOINTS \
    $$UNIT_UNDER_TEST \
    $$FRAMEWORK \
    $$MODEL_ELEMENTS \
    $$OPENPASS_SLAVE \
    $$OPENPASS_SLAVE/importer \
    $$OPENPASS_SLAVE/bindings \
    $$OPEN_SRC \
    $$OPEN_SRC/core \
    ../../../../../.. \
    ../../../../../../..

HEADERS += \
    $$UNIT_UNDER_TEST/SpawnPointScenario.h \
    $$FRAMEWORK/agentBlueprintProvider.h \
    $$FRAMEWORK/dynamicProfileSampler.h \
    $$FRAMEWORK/dynamicParametersSampler.h \
    $$FRAMEWORK/dynamicAgentTypeGenerator.h \
    $$OPENPASS_SLAVE/bindings/modelBinding.h \
    $$MODEL_ELEMENTS/agentBlueprint.h \
    $$MODEL_ELEMENTS/componentType.h \
    $$MODEL_ELEMENTS/agentType.h \
    $$MODEL_ELEMENTS/agent.h \
    $$MODEL_ELEMENTS/component.h \
    $$MODEL_ELEMENTS/channel.h \
    $$MODEL_ELEMENTS/parameters.h \
    $$CORE_SHARE/log.h \
    $$OPENPASS_SLAVE/bindings/modelLibrary.h \
    $$SPAWNPOINTS/common/WorldAnalyzer.h

SOURCES += \
    $$UNIT_UNDER_TEST/SpawnPointScenario.cpp \
    $$FRAMEWORK/agentBlueprintProvider.cpp \
    $$FRAMEWORK/agentDataPublisher.cpp \
    $$FRAMEWORK/dynamicProfileSampler.cpp \
    $$FRAMEWORK/dynamicParametersSampler.cpp \
    $$FRAMEWORK/dynamicAgentTypeGenerator.cpp \
    $$FRAMEWORK/sampler.cpp \
    $$OPENPASS_SLAVE/bindings/modelBinding.cpp \
    $$MODEL_ELEMENTS/agentBlueprint.cpp \
    $$MODEL_ELEMENTS/componentType.cpp \
    $$MODEL_ELEMENTS/agentType.cpp \
    $$MODEL_ELEMENTS/channel.cpp \
    $$MODEL_ELEMENTS/agent.cpp \
    $$MODEL_ELEMENTS/component.cpp \
    $$MODEL_ELEMENTS/parameters.cpp \
    $$CORE_SHARE/log.cpp \
    $$OPENPASS_SLAVE/bindings/modelLibrary.cpp \
    $$SPAWNPOINTS/common/WorldAnalyzer.cpp \
    spawnPointScenario_Tests.cpp

