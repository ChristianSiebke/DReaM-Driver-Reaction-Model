################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
#-----------------------------------------------------------------------------
# \file  AlgorithmFmuWrapper.pro
# \brief This file contains the information for the QtCreator-project of the
# module AlgorithmFmuWrapper
#
# Copyright (c) 2017, 2019 in-tech GmbH
# on behalf of BMW AG
#-----------------------------------------------------------------------------/
DEFINES += ALGORITHM_FMUWRAPPER_LIBRARY \
           USE_EXTENDED_OSI
CONFIG += OPENPASS_LIBRARY
include(../../../global.pri)

QMAKE_CXXFLAGS += -fpermissive

INCLUDEPATH += \
    src \
    src/FmiImporter/include \
    $$relative_path($$EXTRA_INCLUDE_PATH)/FMILibrary \
    $$relative_path($$EXTRA_INCLUDE_PATH)/FMILibrary/FMI \    # required due to FMILibrary internal include structure
    $$relative_path($$EXTRA_INCLUDE_PATH)/FMILibrary/FMI1 \   #  - " -
    $$relative_path($$EXTRA_INCLUDE_PATH)/FMILibrary/FMI2 \   #  - " -
    $$relative_path($$EXTRA_INCLUDE_PATH)/FMILibrary/JM \     #  - " -
    ../../core/opSimulation/modules/World_OSI \
    ../../.. \
    ../..

SOURCES += \
    src/FmiImporter/src/Common/fmuChecker.c \
    src/FmiImporter/src/FMI1/fmi1_check.c \
    src/FmiImporter/src/FMI1/fmi1_cs_sim.c \
    src/FmiImporter/src/FMI1/fmi1_me_sim.c \
    src/FmiImporter/src/FMI2/fmi2_check.c \
    src/FmiImporter/src/FMI2/fmi2_cs_sim.c \
    src/FmiImporter/src/FMI2/fmi2_me_sim.c \
    AlgorithmFmuWrapper.cpp \
    src/fmuWrapper.cpp \
    src/OsmpFmuHandler.cpp

HEADERS += \
    src/FmiImporter/include/fmuChecker.h \
    AlgorithmFmuWrapper.h \
    src/fmuWrapper.h \
    src/OsmpFmuHandler.h \
    src/variant_visitor.h

LIBS += -lfmilib_shared -lopen_simulation_interface -lprotobuf
