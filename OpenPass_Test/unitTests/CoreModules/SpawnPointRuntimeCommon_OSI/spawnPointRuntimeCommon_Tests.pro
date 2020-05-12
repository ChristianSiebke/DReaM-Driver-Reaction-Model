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

include(../../../testing.pri)

SPAWNPOINTS = $$OPEN_SRC/CoreModules/SpawnPoints

UNIT_UNDER_TEST = $$SPAWNPOINTS/SpawnPointRuntimeCommon_OSI

OPENPASS_SLAVE = $$OPEN_SRC/CoreFramework/OpenPassSlave

CORE_SHARE = $$OPEN_SRC/CoreFramework/CoreShare

FRAMEWORK = $$OPENPASS_SLAVE/framework

MODEL_INTERFACE = $$OPENPASS_SLAVE/modelInterface

MODEL_ELEMENTS = $$OPENPASS_SLAVE/modelElements

win32:QMAKE_CXXFLAGS += -Wa,-mbig-obj

INCLUDEPATH += \
    $$SPAWNPOINTS \
    $$UNIT_UNDER_TEST \
    $$FRAMEWORK \
    $$MODEL_INTERFACE \
    $$MODEL_ELEMENTS \
    $$OPENPASS_SLAVE \
    $$OPENPASS_SLAVE/spawnPointInterface \
    $$OPENPASS_SLAVE/importer \
    $$CORE_SHARE \
    $$OPEN_SRC/Common

HEADERS += \
    $$UNIT_UNDER_TEST/SpawnPointRuntimeCommon.h \
    $$UNIT_UNDER_TEST/SpawnPointRuntimeCommonDefinitions.h \
    $$UNIT_UNDER_TEST/SpawnPointRuntimeCommonParameterExtractor.h \
    $$FRAMEWORK/agentBlueprintProvider.h \
    $$FRAMEWORK/dynamicProfileSampler.h \
    $$FRAMEWORK/dynamicParametersSampler.h \
    $$FRAMEWORK/dynamicAgentTypeGenerator.h \
    $$MODEL_INTERFACE/modelBinding.h \
    $$MODEL_ELEMENTS/agentBlueprint.h \
    $$MODEL_ELEMENTS/componentType.h \
    $$MODEL_ELEMENTS/agentType.h \
    $$MODEL_ELEMENTS/agent.h \
    $$MODEL_ELEMENTS/component.h \
    $$MODEL_ELEMENTS/channel.h \
    $$CORE_SHARE/log.h \
    $$CORE_SHARE/parameters.h \
    $$OPENPASS_SLAVE/modelInterface/modelLibrary.h \
    $$SPAWNPOINTS/Common/WorldAnalyzer.h

SOURCES += \
    $$relative_path($$OPEN_SRC)/Common/vector2d.cpp \
    $$UNIT_UNDER_TEST/SpawnPointRuntimeCommon.cpp \
    $$FRAMEWORK/agentBlueprintProvider.cpp \
    $$FRAMEWORK/dynamicProfileSampler.cpp \
    $$FRAMEWORK/dynamicParametersSampler.cpp \
    $$FRAMEWORK/dynamicAgentTypeGenerator.cpp \
    $$FRAMEWORK/sampler.cpp \
    $$MODEL_INTERFACE/modelBinding.cpp \
    $$MODEL_ELEMENTS/agentBlueprint.cpp \
    $$MODEL_ELEMENTS/componentType.cpp \
    $$MODEL_ELEMENTS/agentType.cpp \
    $$MODEL_ELEMENTS/channel.cpp \
    $$MODEL_ELEMENTS/agent.cpp \
    $$MODEL_ELEMENTS/component.cpp \
    $$CORE_SHARE/log.cpp \
    $$CORE_SHARE/parameters.cpp \
    $$OPENPASS_SLAVE/modelInterface/modelLibrary.cpp \
    $$SPAWNPOINTS/Common/WorldAnalyzer.cpp \
    spawnPointRuntimeCommon_Tests.cpp

