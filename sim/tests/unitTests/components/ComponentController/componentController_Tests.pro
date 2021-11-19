################################################################################
# Copyright (c) 2019 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

CONFIG += OPENPASS_GTEST \
          OPENPASS_GTEST_DEFAULT_MAIN

include(../../../testing.pri)

UNIT_UNDER_TEST = $$OPEN_SRC/components/ComponentController/src

win32:QMAKE_CXXFLAGS += -Wa,-mbig-obj

INCLUDEPATH += \
    . \
    $$UNIT_UNDER_TEST \
    ../../../..

HEADERS += \
    $$OPEN_SRC/Common/agentBasedManipulatorEvent.h \
    $$OPEN_SRC/Common/componentWarningEvent.h \
    $$UNIT_UNDER_TEST/condition.h \
    $$UNIT_UNDER_TEST/componentStateInformation.h \
    $$UNIT_UNDER_TEST/stateManager.h \
    $$UNIT_UNDER_TEST/componentControllerImpl.h

SOURCES += \
    $$UNIT_UNDER_TEST/condition.cpp \
    $$UNIT_UNDER_TEST/componentStateInformation.cpp \
    $$UNIT_UNDER_TEST/stateManager.cpp \
    $$UNIT_UNDER_TEST/componentControllerImpl.cpp \
    componentController_Tests.cpp
