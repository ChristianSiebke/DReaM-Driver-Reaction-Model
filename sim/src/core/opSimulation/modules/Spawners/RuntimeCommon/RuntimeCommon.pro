# /*********************************************************************
# * Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

#-----------------------------------------------------------------------------
# \file  RuntimeCommon.pro
# \brief This file contains the information for the QtCreator-project of the
# module RuntimeCommon
#-----------------------------------------------------------------------------/

# shortened .pro file name due to MinGW path length problems
TARGET = RuntimeCommon

DEFINES += SPAWNER_COMMON_RUNTIME_LIBRARY
CONFIG += OPENPASS_LIBRARY
include(../../../../../../global.pri)

SUBDIRS += \
    . \

INCLUDEPATH += \
    .. \
    ../../../ \
    ../../../.. \
    ../../../../.. \
    ../../../../../.. \
    ../../../modelElements

DEPENDENCY_HEADERS = \
                     ../common/WorldAnalyzer.h \
                     ../../../modelElements/agentBlueprint.h \
                     ../../../framework/sampler.h

DEPENDENCY_SOURCES = \
                     ../common/WorldAnalyzer.cpp \
                     ../../../modelElements/agentBlueprint.cpp \
                     ../../../framework/sampler.cpp

SOURCES += \
    $$DEPENDENCY_SOURCES \
    SpawnerRuntimeCommon.cpp \
    SpawnerRuntimeCommonExport.cpp

HEADERS += \
    $$DEPENDENCYS_HEADERS \
    SpawnerRuntimeCommon.h \
    SpawnerRuntimeCommonExport.h \
    SpawnerRuntimeCommonGlobal.h \
    SpawnerRuntimeCommonParameterExtractor.h \
    SpawnerRuntimeCommonDefinitions.h
