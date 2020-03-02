# /*********************************************************************
# * Copyright (c) 2017, 2018, 2019 in-tech GmbH
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

win32:QMAKE_CXXFLAGS += -Wa,-mbig-obj

UNIT_UNDER_TEST = $$OPEN_SRC/Components/AlgorithmAFDM

INCLUDEPATH += \
    . \
    $$OPEN_SRC/Components/Sensor_Driver/Signals \
    $$UNIT_UNDER_TEST

HEADERS += \
    $$OPEN_SRC/Components/Sensor_Driver/Signals/sensorDriverSignal.h \
    $$UNIT_UNDER_TEST/AlgorithmAFDMImplementation.h

SOURCES += \
    $$UNIT_UNDER_TEST/AlgorithmAFDMImplementation.cpp \
    Afdm_Tests.cpp
    
