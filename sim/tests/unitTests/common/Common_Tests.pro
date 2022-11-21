################################################################################
# Copyright (c) 2019 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#               2017-2019 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
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
    $$OPEN_SRC/core/opSimulation/modelElements

HEADERS += \
    $$UNIT_UNDER_TEST/commonTools.h

SOURCES += \
    $$UNIT_UNDER_TEST/commonTools.cpp \
    commonHelper_Tests.cpp \
    tokenizeString_Tests.cpp \
    ttcCalculation_Tests.cpp \
    vectorToString_Tests.cpp
