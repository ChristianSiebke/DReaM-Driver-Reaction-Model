# /*********************************************************************
# * Copyright (c) 2021 ITK Engineering GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

include(../../../components_custom/UT.pri)

TARGET = tst_ut_Chassis3D

INCLUDEPATH += . \
               $$MAIN_SRC_DIR/src/components/Dynamics_Chassis \


HEADERS += $$MAIN_SRC_DIR/src/components/Dynamics_Chassis/VehicleBasics.h \
           $$MAIN_SRC_DIR/src/components/Dynamics_Chassis/ForceWheelZ.h \
           $$MAIN_SRC_DIR/src/components/Dynamics_Chassis/WheelOscillation.h \


SOURCES += $$MAIN_SRC_DIR/src/components/Dynamics_Chassis/ForceWheelZ.cpp \
           $$MAIN_SRC_DIR/src/components/Dynamics_Chassis/WheelOscillation.cpp \
           tst_ut_Chassis3D.cpp \

DEFINES += SRCDIR=\\\"$$PWD/\\\"
