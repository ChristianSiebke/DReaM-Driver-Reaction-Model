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
# \file  SignalPrioritizer.pro
# \brief This file contains the information for the QtCreator-project of the
# module SignalPrioritizer
#-----------------------------------------------------------------------------/

DEFINES += SIGNAL_PRIORITIZER_LIBRARY
CONFIG += OPENPASS_LIBRARY
include(../../../global.pri)

SUBDIRS += .\
    src

INCLUDEPATH += \
    $$SUBDIRS \
    ../../.. \
    ../..

SOURCES += \
    signalPrioritizer.cpp \
    src/signalPrioritizerImpl.cpp

HEADERS += \
    signalPrioritizer.h \
    src/signalPrioritizerImpl.h
