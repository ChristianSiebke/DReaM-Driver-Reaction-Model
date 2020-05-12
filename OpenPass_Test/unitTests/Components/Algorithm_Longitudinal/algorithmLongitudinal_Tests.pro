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

UNIT_UNDER_TEST = $$OPEN_SRC/Components/Algorithm_Longitudinal

INCLUDEPATH += \
    . \
    $$UNIT_UNDER_TEST

HEADERS += \
    $$UNIT_UNDER_TEST/algorithm_longitudinalImplementation.h \
    $$UNIT_UNDER_TEST/algorithm_longitudinalCalculations.h \

SOURCES += \
    $$relative_path($$OPEN_SRC)/Common/vector2d.cpp \
    $$UNIT_UNDER_TEST/algorithm_longitudinalImplementation.cpp \
    $$UNIT_UNDER_TEST/algorithm_longitudinalCalculations.cpp \
    algorithmLongitudinal_Tests.cpp
