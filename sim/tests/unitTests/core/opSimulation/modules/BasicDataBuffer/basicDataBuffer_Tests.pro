################################################################################
# Copyright (c) 2020 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

CONFIG += OPENPASS_GTEST \
          OPENPASS_GTEST_DEFAULT_MAIN

include(../../../../../testing.pri)

UNIT_UNDER_TEST = $$OPEN_SRC/core/opSimulation/modules/BasicDataBuffer

#win32:QMAKE_CXXFLAGS += -Wa,-mbig-obj

INCLUDEPATH += \
    $$UNIT_UNDER_TEST \
    ../../../../../..

HEADERS += \
    $$UNIT_UNDER_TEST/basicDataBufferImplementation.h

SOURCES += \
    $$UNIT_UNDER_TEST/basicDataBufferImplementation.cpp \
    basicDataBuffer_Tests.cpp
