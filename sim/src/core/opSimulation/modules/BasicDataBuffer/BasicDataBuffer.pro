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
# \file  BasicDataBuffer.pro
# \brief This file contains the information for the QtCreator-project of the
# module BasicDataBuffer
#-----------------------------------------------------------------------------/

CONFIG += OPENPASS_LIBRARY
DEFINES += BASIC_DATABUFFER_LIBRARY

include(../../../../../global.pri)

INCLUDEPATH += \
    ../../../.. \
    ../../../../..

SOURCES += \
    basicDataBuffer.cpp \
    basicDataBufferImplementation.cpp

HEADERS += \
    basicDataBuffer.h \
    basicDataBufferImplementation.h
