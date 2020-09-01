# /*********************************************************************
# * Copyright (c) 2017, 2018, 2019 in-tech GmbH
# *               2019 BMW AG
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/
QT += xml

CONFIG += OPENPASS_GTEST \
          OPENPASS_GTEST_DEFAULT_MAIN

include(../../testing.pri)

win32:QMAKE_CXXFLAGS += -Wa,-mbig-obj

UNIT_UNDER_TEST = $$OPEN_SRC/common

INCLUDEPATH += \
    $$UNIT_UNDER_TEST \
    $$OPEN_SRC \
    $$OPEN_SRC/.. \
    $$OPEN_SRC/core/slave/modelElements

HEADERS += \
    $$UNIT_UNDER_TEST/commonTools.h

SOURCES += \
    tokenizeString_Tests.cpp \
    ttcCalculation_Tests.cpp \
    vectorToString_Tests.cpp
