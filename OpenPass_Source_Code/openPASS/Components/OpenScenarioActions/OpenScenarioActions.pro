# /*********************************************************************
# * Copyright (c) 2020 in-tech GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

#-----------------------------------------------------------------------------
# \file  OpenScenarioActions.pro
# \brief This file contains the information for the QtCreator-project of the
# module OpenScenarioActions
#-----------------------------------------------------------------------------/

DEFINES += OPENSCENARIO_ACTIONS_LIBRARY
CONFIG += OPENPASS_LIBRARY
include(../../../global.pri)

SUBDIRS +=  .

INCLUDEPATH += $$SUBDIRS \
            ../../Common \
            ../../Interfaces \
            ..

SOURCES += \
        openScenarioActions.cpp \
        openScenarioActionsImplementation.cpp \
        oscActionsCalculation.cpp \
        transformLaneChange.cpp \
        transformSpeedAction.cpp

HEADERS += \
        openScenarioActions.h \
        openScenarioActionsGlobal.h \
        openScenarioActionsImplementation.h \
        oscActionsCalculation.h \
        actionTransformRepository.h \
        transformerBase.h \
        transformTrajectory.h \
        transformLaneChange.h \
        transformSpeedAction.h
