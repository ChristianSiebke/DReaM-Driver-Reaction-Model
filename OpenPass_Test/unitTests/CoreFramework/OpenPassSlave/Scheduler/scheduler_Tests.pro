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

OPEN_PASS_SLAVE = $$OPEN_SRC/CoreFramework/OpenPassSlave
UNIT_UNDER_TEST = $$OPEN_PASS_SLAVE/scheduler

win32:QMAKE_CXXFLAGS += -Wa,-mbig-obj

INCLUDEPATH += \
    . \
    $$OPEN_SRC/CoreFramework/CoreShare \
    $$OPEN_PASS_SLAVE \
    $$OPEN_PASS_SLAVE/modelElements \
    $$OPEN_PASS_SLAVE/framework \
    $$OPEN_PASS_SLAVE/eventDetectorInterface \
    $$OPEN_PASS_SLAVE/manipulatorInterface \
    $$OPEN_PASS_SLAVE/modelInterface \
    $$OPEN_PASS_SLAVE/spawnPointInterface \
    $$UNIT_UNDER_TEST

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
    $$OPEN_PASS_SLAVE/eventDetectorInterface/eventDetectorBinding.cpp \
    $$OPEN_PASS_SLAVE/eventDetectorInterface/eventDetectorLibrary.cpp \
    $$OPEN_PASS_SLAVE/modelElements/agent.cpp \
    $$OPEN_PASS_SLAVE/modelElements/agentBlueprint.cpp \
    $$OPEN_PASS_SLAVE/modelElements/channel.cpp \
    $$OPEN_PASS_SLAVE/modelElements/component.cpp \
    $$OPEN_PASS_SLAVE/modelInterface/modelBinding.cpp \
    $$OPEN_PASS_SLAVE/modelInterface/modelLibrary.cpp \
    $$OPEN_SRC/Common/eventDetectorDefinitions.cpp \
    $$OPEN_SRC/Common/vector2d.cpp \
    $$OPEN_SRC/CoreFramework/CoreShare/log.cpp \
    $$OPEN_SRC/CoreFramework/CoreShare/parameters.cpp \
    taskBuilder_Tests.cpp \
    schedulerTasks_Tests.cpp \
    agentParser_Tests.cpp \
    scheduler_Tests.cpp \
    \ # unknown dependency (@reinhard)!
    $$OPEN_PASS_SLAVE/framework/agentDataPublisher.cpp
