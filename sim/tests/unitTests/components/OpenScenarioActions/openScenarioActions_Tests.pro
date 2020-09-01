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

UNIT_UNDER_TEST = $$OPEN_SRC/components/OpenScenarioActions/src

INCLUDEPATH += \
    . \
    $$UNIT_UNDER_TEST \
    ../../../..

HEADERS += \
    $$OPEN_SRC/common/laneChangeEvent.h \
    $$OPEN_SRC/common/trajectoryEvent.h \
    $$OPEN_SRC/common/trajectorySignal.h \
    $$UNIT_UNDER_TEST/openScenarioActionsImpl.h \
    $$UNIT_UNDER_TEST/oscActionsCalculation.h

SOURCES += \
    $$UNIT_UNDER_TEST/transformLaneChange.cpp \
    $$UNIT_UNDER_TEST/transformSpeedAction.cpp \
    $$UNIT_UNDER_TEST/openScenarioActionsImpl.cpp \
    $$UNIT_UNDER_TEST/oscActionsCalculation.cpp \
    openScenarioActions_Tests.cpp
