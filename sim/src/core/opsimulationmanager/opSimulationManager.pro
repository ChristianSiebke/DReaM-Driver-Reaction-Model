################################################################################
# Copyright (c) 2021 ITK Engineering GmbH
#               2017-2019 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

#-----------------------------------------------------------------------------
# \file  opSimulationManager.pro
# \brief This file contains the information for the QtCreator-project of the
#        opSimulationManager
#-----------------------------------------------------------------------------

CONFIG += OPENPASS_EXECUTABLE
include(../../../global.pri)

QT += concurrent widgets xml

SRC_CORESHARE = ../../common/xmlParser.cpp \
                ../common/log.cpp

INC_CORESHARE = ../../common/xmlParser.h \
                ../common/log.h

INCLUDEPATH += \
    framework \
    importer \
    .. \
    ../.. \
    ../../.. \
    ../../../..

SOURCES += \
    $$SRC_CORESHARE \
    framework/main.cpp \
    framework/processManager.cpp \
    importer/opSimulationManagerConfigImporter.cpp

HEADERS += \
    $$INC_CORESHARE \
    opSimulationManager.h \
    framework/processManager.h \
    importer/opSimulationManagerConfigImporter.h \
    framework/opSimulationManagerConfig.h \
    framework/simulationConfig.h
