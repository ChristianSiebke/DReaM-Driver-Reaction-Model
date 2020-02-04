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

UNIT_UNDER_TEST = $$OPEN_SRC/CoreModules/EventDetector
CONDITIONS = $$OPEN_SRC/CoreModules/EventDetector/Conditions

# commons
HDR_COMMONS = $$OPEN_SRC/Common/Vector2D.h \
              $$OPEN_SRC/Common/openScenarioDefinitions.h
SRC_COMMONS = $$OPEN_SRC/Common/Vector2D.cpp

INCLUDEPATH += \
    . \
    $$UNIT_UNDER_TEST \
    $$OPEN_SRC/Common

HEADERS += \
    $$HDR_COMMONS \
    $$OPEN_SRC/CoreFramework/CoreShare/log.h \
    $$OPEN_SRC/CoreFramework/CoreShare/parameters.h \
    $$UNIT_UNDER_TEST/CollisionDetector.h \
    $$UNIT_UNDER_TEST/ConditionalEventDetector.h \
    $$UNIT_UNDER_TEST/EventDetectorCommonBase.h \
    $$CONDITIONS/ConditionCommonBase.h \
    $$OPEN_SRC/Common/eventDetectorDefinitions.h

SOURCES += \
    $$SRC_COMMONS \
    $$OPEN_SRC/CoreFramework/CoreShare/log.cpp \
    $$OPEN_SRC/CoreFramework/CoreShare/parameters.cpp \
    $$UNIT_UNDER_TEST/CollisionDetector.cpp \
    $$UNIT_UNDER_TEST/ConditionalEventDetector.cpp \
    $$UNIT_UNDER_TEST/EventDetectorCommonBase.cpp \
    $$OPEN_SRC/Common/eventDetectorDefinitions.cpp \
    ./CollisionDetectorUnitTests.cpp \
    ./ConditionalEventDetector_Tests.cpp
