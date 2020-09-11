# /*********************************************************************
# * Copyright (c) 2017, 2018, 2019 in-tech GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

#-----------------------------------------------------------------------------
# \file  Algorithm_Lateral.pro
# \brief This file contains the information for the QtCreator-project of the
# module Algorithm_Lateral
#-----------------------------------------------------------------------------/

DEFINES += ALGORITHM_LATERAL_LIBRARY
CONFIG += OPENPASS_LIBRARY
include(../../../global.pri)

SUBDIRS += .\
    src

INCLUDEPATH += \
    $$SUBDIRS \
    ../../.. \
    ../..

SOURCES += \
    algorithm_lateral.cpp \
    src/lateralImpl.cpp \
    src/steeringController.cpp

HEADERS += \
    algorithm_lateral.h \
    src/lateralImpl.h \
    src/steeringController.h
