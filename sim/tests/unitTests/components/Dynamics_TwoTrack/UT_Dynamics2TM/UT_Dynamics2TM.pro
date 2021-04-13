# /*********************************************************************
# * Copyright (c) 2021 ITK Engineering GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

QT += testlib

TARGET = tst_ut_dynamics2tmtest

MAIN_SRC_DIR = $$PWD/../../../../../src
include($$MAIN_SRC_DIR/../global.pri)

INCLUDEPATH += . \
               $$MAIN_SRC_DIR/components/Dynamics_TwoTrack/src \
               $$MAIN_SRC_DIR \

HEADERS += \
    $$MAIN_SRC_DIR/components/Dynamics_TwoTrack/src/dynamics_twotrack_vehicle.h \
    $$MAIN_SRC_DIR/components/Dynamics_TwoTrack/src/dynamics_twotrack_tire.h \
    $$MAIN_SRC_DIR/common/vector2d.h \

SOURCES += \
    $$MAIN_SRC_DIR/components/Dynamics_TwoTrack/src/dynamics_twotrack_vehicle.cpp \
    $$MAIN_SRC_DIR/components/Dynamics_TwoTrack/src/dynamics_twotrack_tire.cpp \
    tst_ut_dynamics2tmtest.cpp \

DEFINES += SRCDIR=\\\"$$PWD/\\\"
