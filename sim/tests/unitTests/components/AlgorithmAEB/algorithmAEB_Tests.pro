# /*********************************************************************
# * Copyright (c) 2019 in-tech GmbH
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

INCLUDEPATH += \
    $$relative_path($$OPEN_SRC)/components/Algorithm_AEB/src \
    $$relative_path($$OPEN_SRC)/components \
    ../../../..

HEADERS += \
    AlgorithmAebOSIUnitTests.h \
    $$relative_path($$OPEN_SRC)/common/boostGeometryCommon.h \
    $$relative_path($$OPEN_SRC)/common/vector2d.h \
    $$relative_path($$OPEN_SRC)/components/Algorithm_AEB/src/autonomousEmergencyBraking.h

SOURCES += \
    AlgorithmAeb_Tests.cpp \
    $$relative_path($$OPEN_SRC)/components/Algorithm_AEB/src/autonomousEmergencyBraking.cpp

LIBS += \
    -lopen_simulation_interface \
    -lprotobuf
