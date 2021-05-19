# /*********************************************************************
# * Copyright (c) 2019, 2020 in-tech GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

CONFIG += OPENPASS_GTEST \
          OPENPASS_GTEST_DEFAULT_MAIN

include(../../../../../testing.pri)

win32:QMAKE_CXXFLAGS += -Wa,-mbig-obj

UNIT_UNDER_TEST = $$OPEN_SRC/core/slave/modules/EventDetector
CONDITIONS = $$OPEN_SRC/core/slave/modules/EventDetector/Conditions

# commons
HDR_COMMONS = $$OPEN_SRC/Common/Vector2D.h \
              $$OPEN_SRC/Common/openScenarioDefinitions.h

INCLUDEPATH += \
    . \
    $$UNIT_UNDER_TEST \
    $$OPEN_SRC/core \
    ../../../../../..

HEADERS += \
    $$HDR_COMMONS \
    $$UNIT_UNDER_TEST/CollisionDetector.h \
    $$UNIT_UNDER_TEST/ConditionalEventDetector.h \
    $$UNIT_UNDER_TEST/EventDetectorCommonBase.h \
    $$CONDITIONS/ConditionCommonBase.h \
    $$OPEN_SRC/common/eventDetectorDefinitions.h

SOURCES += \
    $$UNIT_UNDER_TEST/CollisionDetector.cpp \
    $$UNIT_UNDER_TEST/ConditionalEventDetector.cpp \
    $$UNIT_UNDER_TEST/EventDetectorCommonBase.cpp \
    $$OPEN_SRC/common/eventDetectorDefinitions.cpp \
    CollisionDetectorUnitTests.cpp \
    ConditionalEventDetector_Tests.cpp
