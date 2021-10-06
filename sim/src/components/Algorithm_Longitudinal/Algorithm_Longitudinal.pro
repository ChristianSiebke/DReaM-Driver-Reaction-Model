################################################################################
# Copyright (c) 2017 AMFD GmbH
#               2017-2019 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

#-----------------------------------------------------------------------------
# \file  Algorithm_Longitudinal.pro
# \brief This file contains the information for the QtCreator-project of the
# module Algorithm_Longitudinal
#-----------------------------------------------------------------------------/

DEFINES += ALGORITHM_LONGITUDINAL_LIBRARY
CONFIG += OPENPASS_LIBRARY
include(../../../global.pri)

SUBDIRS += .\
    src

INCLUDEPATH += \
    $$SUBDIRS \
    ../../.. \
    ../..

SOURCES += \
    algorithm_longitudinal.cpp \
    src/algo_longImpl.cpp \
    src/longCalcs.cpp

HEADERS += \
    algorithm_longitudinal.h \
    src/algo_longImpl.h \
    src/longCalcs.h
