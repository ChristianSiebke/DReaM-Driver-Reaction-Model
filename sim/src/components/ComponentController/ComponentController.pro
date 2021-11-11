################################################################################
# Copyright (c) 2017-2019 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

#-----------------------------------------------------------------------------
# \file  ComponentController.pro
# \brief This file contains the information for the QtCreator-project of the
# module ComponentController
#-----------------------------------------------------------------------------/

DEFINES += COMPONENT_CONTROLLER_LIBRARY
CONFIG += OPENPASS_LIBRARY
include(../../../global.pri)

SUBDIRS += .\
    src

INCLUDEPATH += \
    $$SUBDIRS \
    ../../.. \
    ../..

SOURCES += \
        componentController.cpp \
        src/componentControllerImpl.cpp \
        src/componentStateInformation.cpp \
        src/condition.cpp \
        src/stateManager.cpp

HEADERS += \
        componentController.h \
        src/condition.h \
        src/componentControllerImpl.h \
        src/componentControllerCommon.h \
        src/componentStateInformation.h \
        src/stateManager.h
