# /*********************************************************************
# * Copyright (c) 2021 ITK Engineering GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

CONFIG += OPENPASS_GTEST \
          OPENPASS_GTEST_DEFAULT_MAIN

include(../../../../testing.pri)

TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG += thread
CONFIG += qt

MAIN_SRC_DIR = $$PWD/../../../../../src
include($$MAIN_SRC_DIR/../global.pri)

INCLUDEPATH += c:/OpenPASS/thirdParty/include

INCLUDEPATH += . \
               $$MAIN_SRC_DIR/common/pcm \
               $$MAIN_SRC_DIR/components/Algorithm_Switch

HEADERS += $$MAIN_SRC_DIR/components/Algorithm_Switch/switch.h \
           tst_ut_switch.h


SOURCES += $$MAIN_SRC_DIR/components/Algorithm_Switch/switch.cpp \
           tst_ut_switch.cpp

Release:DESTDIR = $$DIR_RELEASE
Debug:DESTDIR = $$DIR_DEBUG
