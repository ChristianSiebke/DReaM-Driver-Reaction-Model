################################################################################
# Copyright (c) 2020 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

#-----------------------------------------------------------------------------
# \file  OpenScenarioActions.pro
# \brief This file contains the information for the QtCreator-project of the
# module OpenScenarioActions
#-----------------------------------------------------------------------------/

DEFINES += OPENSCENARIO_ACTIONS_LIBRARY
CONFIG += OPENPASS_LIBRARY
include(../../../global.pri)

SUBDIRS += .\
    src

INCLUDEPATH += \
    $$SUBDIRS \
    ../../.. \
    ../..

SOURCES += \
        openScenarioActions.cpp \
        src/openScenarioActionsImpl.cpp \
        src/oscActionsCalculation.cpp \
        src/transformLaneChange.cpp \
        src/transformSpeedAction.cpp \
        src/transformAcquirePosition.cpp \
        src/transformDefaultCustomCommandAction.cpp

HEADERS += \
        openScenarioActions.h \
        src/openScenarioActionsImpl.h \
        src/oscActionsCalculation.h \
        src/actionTransformRepository.h \
        src/transformerBase.h \
        src/transformTrajectory.h \
        src/transformLaneChange.h \
        src/transformSpeedAction.h \
        src/transformAcquirePosition.h \
        src/transformDefaultCustomCommandAction.h
