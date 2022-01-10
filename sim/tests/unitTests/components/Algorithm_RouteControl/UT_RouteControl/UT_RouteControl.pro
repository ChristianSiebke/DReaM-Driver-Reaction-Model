################################################################################
# Copyright (c) 2021 ITK Engineering GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

QT += testlib

TARGET = tst_ut_RouteControl

MAIN_SRC_DIR = $$PWD/../../../../../src
include($$MAIN_SRC_DIR/../global.pri)

INCLUDEPATH += . \
               $$MAIN_SRC_DIR \
               $$MAIN_SRC_DIR/common \
               $$MAIN_SRC_DIR/components/Algorithm_Routecontrol \

HEADERS += $$MAIN_SRC_DIR/common/vector2d.h \
           $$MAIN_SRC_DIR/components/Algorithm_Routecontrol/routeControl.h \

SOURCES += $$MAIN_SRC_DIR/components/Algorithm_Routecontrol/routeControl.cpp \
           tst_ut_RouteControl.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

