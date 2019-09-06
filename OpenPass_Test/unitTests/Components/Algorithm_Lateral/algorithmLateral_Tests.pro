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

UNIT_UNDER_TEST = $$OPEN_SRC/Components/Algorithm_Lateral

INCLUDEPATH += . \
    $$UNIT_UNDER_TEST

HEADERS += \
    $$UNIT_UNDER_TEST/algorithm_lateralImplementation.h \
    testResourceManager.h

SOURCES += \
    $$UNIT_UNDER_TEST/algorithm_lateralImplementation.cpp \
    testResourceManager.cpp \
    algorithmLateral_Tests.cpp

