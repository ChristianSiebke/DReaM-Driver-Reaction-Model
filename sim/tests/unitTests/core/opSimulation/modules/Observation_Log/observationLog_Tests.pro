# /*********************************************************************
# * Copyright (c) 2018 2019 in-tech GmbH
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

UNIT_UNDER_TEST = $$OPEN_SRC/core/opSimulation/modules/Observation_Log

win32:QMAKE_CXXFLAGS += -Wa,-mbig-obj

INCLUDEPATH += \
    . \
    $$UNIT_UNDER_TEST \
    ../../../../../..

HEADERS += \
    $$UNIT_UNDER_TEST/observation_logImplementation.h \
    $$UNIT_UNDER_TEST/observationCyclics.h \
    $$UNIT_UNDER_TEST/observationFileHandler.h \
    $$UNIT_UNDER_TEST/runStatistic.h \
    $$UNIT_UNDER_TEST/runStatisticCalculation.h

SOURCES += \
    $$UNIT_UNDER_TEST/observation_logImplementation.cpp \
    $$UNIT_UNDER_TEST/observationCyclics.cpp \
    $$UNIT_UNDER_TEST/observationFileHandler.cpp \
    $$UNIT_UNDER_TEST/runStatistic.cpp \
    $$UNIT_UNDER_TEST/runStatisticCalculation.cpp \
    observationLog_Tests.cpp
