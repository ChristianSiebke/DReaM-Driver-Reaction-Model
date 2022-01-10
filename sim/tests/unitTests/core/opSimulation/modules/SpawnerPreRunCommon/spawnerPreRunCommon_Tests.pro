################################################################################
# Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################


CONFIG += OPENPASS_GTEST \
          OPENPASS_GTEST_DEFAULT_MAIN

include(../../../../../testing.pri)

SPAWNERS = $$OPEN_SRC/core/opSimulation/modules/Spawners

UNIT_UNDER_TEST = $$SPAWNERS/PreRunCommon

OPENPASS_SIMULATION = $$OPEN_SRC/core/opSimulation

CORE_SHARE = $$OPEN_SRC/core/common

FRAMEWORK = $$OPENPASS_SIMULATION/framework

MODEL_ELEMENTS = $$OPENPASS_SIMULATION/modelElements

win32:QMAKE_CXXFLAGS += -Wa,-mbig-obj

INCLUDEPATH += \
    $$SPAWNERS \
    $$UNIT_UNDER_TEST \
    $$FRAMEWORK \
    $$MODEL_ELEMENTS \
    $$OPENPASS_SIMULATION \
    $$OPENPASS_SIMULATION/importer \
    $$OPENPASS_SIMULATION/bindings \
    $$OPEN_SRC \
    $$OPEN_SRC/core \
    ../../../../../.. \
    ../../../../../../..

HEADERS += \
    $$UNIT_UNDER_TEST/SpawnerPreRunCommon.h \
    $$UNIT_UNDER_TEST/SpawnerPreRunCommonDefinitions.h \
    $$UNIT_UNDER_TEST/SpawnerPreRunCommonParameterExtractor.h \
    $$FRAMEWORK/agentBlueprintProvider.h \
    $$FRAMEWORK/dynamicProfileSampler.h \
    $$FRAMEWORK/dynamicParametersSampler.h \
    $$FRAMEWORK/dynamicAgentTypeGenerator.h \
    $$OPENPASS_SIMULATION/bindings/modelBinding.h \
    $$MODEL_ELEMENTS/agentBlueprint.h \
    $$MODEL_ELEMENTS/componentType.h \
    $$MODEL_ELEMENTS/agentType.h \
    $$MODEL_ELEMENTS/agent.h \
    $$MODEL_ELEMENTS/component.h \
    $$MODEL_ELEMENTS/channel.h \
    $$MODEL_ELEMENTS/parameters.h \
    $$CORE_SHARE/log.h \
    $$OPENPASS_SIMULATION/bindings/modelLibrary.h \
    $$SPAWNERS/common/WorldAnalyzer.h

SOURCES += \
    $$UNIT_UNDER_TEST/SpawnerPreRunCommon.cpp \
    $$FRAMEWORK/agentBlueprintProvider.cpp \
    $$FRAMEWORK/agentDataPublisher.cpp \
    $$FRAMEWORK/dynamicProfileSampler.cpp \
    $$FRAMEWORK/dynamicParametersSampler.cpp \
    $$FRAMEWORK/dynamicAgentTypeGenerator.cpp \
    $$FRAMEWORK/sampler.cpp \
    $$OPENPASS_SIMULATION/bindings/modelBinding.cpp \
    $$MODEL_ELEMENTS/agentBlueprint.cpp \
    $$MODEL_ELEMENTS/componentType.cpp \
    $$MODEL_ELEMENTS/agentType.cpp \
    $$MODEL_ELEMENTS/channel.cpp \
    $$MODEL_ELEMENTS/agent.cpp \
    $$MODEL_ELEMENTS/component.cpp \
    $$MODEL_ELEMENTS/parameters.cpp \
    $$CORE_SHARE/log.cpp \
    $$OPENPASS_SIMULATION/bindings/modelLibrary.cpp \
    $$SPAWNERS/common/WorldAnalyzer.cpp \
    SpawnerPreRunCommon_Tests.cpp

