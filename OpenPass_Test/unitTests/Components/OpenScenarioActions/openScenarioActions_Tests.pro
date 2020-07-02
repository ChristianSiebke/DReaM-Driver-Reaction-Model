# /*********************************************************************
# * Copyright (c) 2020 in-tech GmbH
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

UNIT_UNDER_TEST = $$OPEN_SRC/Components/OpenScenarioActions

INCLUDEPATH += \
    . \
    $$UNIT_UNDER_TEST

HEADERS += \
    $$OPEN_SRC/Common/laneChangeEvent.h \
    $$OPEN_SRC/Common/trajectoryEvent.h \
    $$OPEN_SRC/Common/trajectorySignal.h \
    $$OPEN_SRC/Common/gazeFollowerEvent.h \
    $$OPEN_SRC/Common/gazeFollowerSignal.h \
    $$OPEN_SRC/Common/customLaneChangeEvent.h \
    $$OPEN_SRC/Common/customLaneChangeSignal.h \
    $$UNIT_UNDER_TEST/openScenarioActionsImplementation.h \
    $$UNIT_UNDER_TEST/oscActionsCalculation.h

SOURCES += \
    $$UNIT_UNDER_TEST/openScenarioActionsImplementation.cpp \
    $$UNIT_UNDER_TEST/oscActionsCalculation.cpp \
    openScenarioActions_Tests.cpp
