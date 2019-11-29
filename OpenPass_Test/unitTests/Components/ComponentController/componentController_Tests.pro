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

UNIT_UNDER_TEST = $$OPEN_SRC/Components/ComponentController

INCLUDEPATH += \
    . \
    $$UNIT_UNDER_TEST

HEADERS += \
    $$OPEN_SRC/Common/agentBasedManipulatorEvent.h \
    $$OPEN_SRC/Common/componentWarningEvent.h \
    $$UNIT_UNDER_TEST/condition.h \
    $$UNIT_UNDER_TEST/componentStateInformation.h \
    $$UNIT_UNDER_TEST/stateManager.h \
    $$UNIT_UNDER_TEST/componentControllerImplementation.h

SOURCES += \
    $$UNIT_UNDER_TEST/condition.cpp \
    $$UNIT_UNDER_TEST/componentStateInformation.cpp \
    $$UNIT_UNDER_TEST/stateManager.cpp \
    $$UNIT_UNDER_TEST/componentControllerImplementation.cpp \
    componentController_Tests.cpp
