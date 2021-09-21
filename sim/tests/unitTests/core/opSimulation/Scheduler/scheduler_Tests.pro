# /*********************************************************************
# * Copyright (c) 2018, 2019 in-tech GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

CONFIG += OPENPASS_GTEST \
          OPENPASS_GTEST_DEFAULT_MAIN

include(../../../../testing.pri)

OPEN_PASS_SIMULATION = $$OPEN_SRC/core/opSimulation
UNIT_UNDER_TEST = $$OPEN_PASS_SIMULATION/framework/scheduler

win32:QMAKE_CXXFLAGS += -Wa,-mbig-obj

INCLUDEPATH += \
    . \
    $$OPEN_SRC/core \
    $$OPEN_PASS_SIMULATION \
    $$OPEN_PASS_SIMULATION/modelElements \
    $$OPEN_PASS_SIMULATION/framework \
    $$OPEN_PASS_SIMULATION/bindings \
    $$UNIT_UNDER_TEST \
    ../../../../..

HEADERS += \
    $$UNIT_UNDER_TEST/agentParser.h \
    $$UNIT_UNDER_TEST/runResult.h \
    $$UNIT_UNDER_TEST/scheduler.h \
    $$UNIT_UNDER_TEST/schedulerTasks.h \
    $$UNIT_UNDER_TEST/taskBuilder.h \
    $$UNIT_UNDER_TEST/tasks.h

SOURCES += \
    $$UNIT_UNDER_TEST/agentParser.cpp \
    $$UNIT_UNDER_TEST/runResult.cpp \
    $$UNIT_UNDER_TEST/scheduler.cpp \
    $$UNIT_UNDER_TEST/schedulerTasks.cpp \
    $$UNIT_UNDER_TEST/taskBuilder.cpp \
    $$UNIT_UNDER_TEST/tasks.cpp \
    $$OPEN_PASS_SIMULATION/bindings/eventDetectorBinding.cpp \
    $$OPEN_PASS_SIMULATION/bindings/eventDetectorLibrary.cpp \
    $$OPEN_PASS_SIMULATION/modelElements/agent.cpp \
    $$OPEN_PASS_SIMULATION/modelElements/agentBlueprint.cpp \
    $$OPEN_PASS_SIMULATION/modelElements/channel.cpp \
    $$OPEN_PASS_SIMULATION/modelElements/component.cpp \
    $$OPEN_PASS_SIMULATION/modelElements/parameters.cpp \
    $$OPEN_PASS_SIMULATION/bindings/modelBinding.cpp \
    $$OPEN_PASS_SIMULATION/bindings/modelLibrary.cpp \
    $$OPEN_SRC/common/eventDetectorDefinitions.cpp \
    $$OPEN_SRC/core/common/log.cpp \
    taskBuilder_Tests.cpp \
    schedulerTasks_Tests.cpp \
    agentParser_Tests.cpp \
    scheduler_Tests.cpp \
    \ # unknown dependency (@reinhard)!
    $$OPEN_PASS_SIMULATION/framework/agentDataPublisher.cpp
