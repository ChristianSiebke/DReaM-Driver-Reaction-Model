# /*********************************************************************
# * Copyright (c) 2018, 2019 in-tech GmbH
# *               2019 AMFD GmbH
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

UNIT_UNDER_TEST = $$OPEN_SRC/components/Algorithm_Longitudinal/src

INCLUDEPATH += \
    . \
    $$UNIT_UNDER_TEST \
    ../../../..

HEADERS += \
    $$UNIT_UNDER_TEST/algo_longImpl.h \
    $$UNIT_UNDER_TEST/longCalcs.h \

SOURCES += \
    $$UNIT_UNDER_TEST/algo_longImpl.cpp \
    $$UNIT_UNDER_TEST/longCalcs.cpp \
    algorithmLongitudinal_Tests.cpp
