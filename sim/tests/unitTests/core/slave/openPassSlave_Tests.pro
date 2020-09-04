# /*********************************************************************
# * Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/
QT += xml

CONFIG += OPENPASS_GTEST \
          OPENPASS_GTEST_DEFAULT_MAIN

include(../../../testing.pri)

win32:QMAKE_CXXFLAGS += -Wa,-mbig-obj

UNIT_UNDER_TEST = $$OPEN_SRC/core/slave

INCLUDEPATH += \
            $$OPEN_SRC/core \
            $$OPEN_SRC/core/common/cephes \
            $$OPEN_SRC \
            $$OPEN_SRC/.. \
            $$OPEN_SRC/../.. \
            $$UNIT_UNDER_TEST \
            $$UNIT_UNDER_TEST/framework \
            $$UNIT_UNDER_TEST/importer \
            $$UNIT_UNDER_TEST/modelElements \
            $$UNIT_UNDER_TEST/modelInterface \
            $$OPEN_PASS_SLAVE/manipulatorInterface \
            $$OPEN_PASS_SLAVE/eventDetectorInterface \
            $$OPEN_PASS_SLAVE/spawnPointInterface

DEPENDENCIES = \
    $$OPEN_SRC/common/eventDetectorDefinitions.cpp \
    $$OPEN_SRC/common/stochasticDefinitions.h \
    $$OPEN_SRC/core/common/cephes/fresnl.c \
    $$OPEN_SRC/core/common/cephes/polevl.c \
    $$OPEN_SRC/core/common/cephes/const.c \
    $$OPEN_SRC/core/common/log.cpp \
    $$OPEN_SRC/../../common/xmlParser.cpp \
    $$OPEN_SRC/core/common/parameters.cpp

AGENTSAMPLER_TESTS = \
    $$UNIT_UNDER_TEST/framework/dynamicProfileSampler.h \
    $$UNIT_UNDER_TEST/framework/dynamicProfileSampler.cpp \
    $$UNIT_UNDER_TEST/framework/dynamicParametersSampler.cpp \
    $$UNIT_UNDER_TEST/framework/dynamicAgentTypeGenerator.cpp \
    $$UNIT_UNDER_TEST/modelElements/componentType.cpp \
    $$UNIT_UNDER_TEST/modelElements/agentType.cpp \
    $$UNIT_UNDER_TEST/importer/systemConfig.cpp \
    \
    agentSampler_Tests.cpp

SYSTEMCONFIGIMPORTER_TESTS = \
    $$UNIT_UNDER_TEST/importer/systemConfigImporter.cpp \
    \
    systemConfigImporter_Tests.cpp

SLAVECONFIGIMPORTER_TESTS = \
    $$UNIT_UNDER_TEST/importer/slaveConfig.cpp \
    $$UNIT_UNDER_TEST/importer/slaveConfigImporter.cpp \
    \
    slaveConfigImporter_Tests.cpp

PROFILESIMPORTER_TESTS = \
    $$UNIT_UNDER_TEST/importer/profiles.cpp \
    $$UNIT_UNDER_TEST/importer/profilesImporter.cpp \
    \
    profilesImporter_Tests.cpp

VEHICLEMODELIMPORTER_TESTS = \
    $$UNIT_UNDER_TEST/importer/vehicleModelsImporter.cpp \
    \
    vehicleModelsImporter_Tests.cpp

EVENTDETECTOR_TESTS = \
    $$UNIT_UNDER_TEST/importer/eventDetectorImporter.cpp \
    $$UNIT_UNDER_TEST/framework/eventNetwork.cpp \
    $$UNIT_UNDER_TEST/../common/coreDataPublisher.cpp \
    \
    eventNetwork_Tests.cpp

PARAMETERIMPORTER_TESTS = \
    $$UNIT_UNDER_TEST/importer/parameterImporter.cpp \
    \
    parameterImporter_Tests.cpp

SAMPLER_TESTS = \
    $$UNIT_UNDER_TEST/framework/sampler.cpp \
    \
    sampler_Tests.cpp

SCENERYIMPORTER_TESTS = \
    $$UNIT_UNDER_TEST/importer/sceneryImporter.cpp \
    $$UNIT_UNDER_TEST/importer/road.cpp \
    $$UNIT_UNDER_TEST/importer/junction.cpp \
    $$UNIT_UNDER_TEST/importer/connection.cpp \
    $$UNIT_UNDER_TEST/importer/road/roadSignal.cpp \
    $$UNIT_UNDER_TEST/importer/road/roadObject.cpp \
    \
    sceneryImporter_Tests.cpp

SCENARIOIMPORTER_TESTS = \
    $$UNIT_UNDER_TEST/importer/scenario.cpp \
    $$UNIT_UNDER_TEST/importer/scenarioImporter.cpp \
    $$UNIT_UNDER_TEST/importer/scenarioImporterHelper.cpp \
    $$UNIT_UNDER_TEST/importer/oscImporterCommon.cpp \
    \
    scenarioImporter_Tests.cpp

MANIPULATORIMPORTER_TESTS = \
    manipulatorImporter_Tests.cpp

COMMANDLINERPARSER_TESTS = \
    $$UNIT_UNDER_TEST/framework/commandLineParser.cpp \
    \
    commandLineParser_Tests.cpp \

DIRECTORIES_TESTS = \
    $$UNIT_UNDER_TEST/framework/directories.cpp \
    \
    directories_Tests.cpp \

PUBLISHER_TESTS = \
    $$UNIT_UNDER_TEST/framework/agentDataPublisher.cpp \
    \
    agentDataPublisher_Tests.cpp

SOURCES += \
    $$DEPENDENCIES \
    $$SLAVECONFIGIMPORTER_TESTS \
    $$SYSTEMCONFIGIMPORTER_TESTS \
    $$PROFILESIMPORTER_TESTS \
    $$EVENTDETECTOR_TESTS \
    $$PARAMETERIMPORTER_TESTS \
    $$SAMPLER_TESTS \
    $$SCENARIOIMPORTER_TESTS \
    $$SCENERYIMPORTER_TESTS \
    $$MANIPULATORIMPORTER_TESTS \
    $$INVOCATIONCONTROL_TESTS \
    $$VEHICLEMODELIMPORTER_TESTS \
    $$DIRECTORIES_TESTS \
    $$COMMANDLINERPARSER_TESTS \
    $$AGENTSAMPLER_TESTS \
    $$PUBLISHER_TESTS
