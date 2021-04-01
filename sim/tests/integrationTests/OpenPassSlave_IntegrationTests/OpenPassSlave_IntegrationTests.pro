#/*******************************************************************************
#* Copyright (c) 2019, 2020 in-tech GmbH
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

#-----------------------------------------------------------------------------
# \file  OpenPassSlave_IntegationTests.pro
# \brief This file contains tests for the OpenPassSlave module
#-----------------------------------------------------------------------------/
QT += xml

CONFIG += OPENPASS_GTEST \
          OPENPASS_GTEST_DEFAULT_MAIN
include(../../testing.pri)

OPENPASS_SLAVE = $$OPEN_SRC/core/slave
CORE_SHARE = $$OPEN_SRC/core/common
WORLD_OSI = $$OPEN_SRC/core/slave/modules/World_OSI

INCLUDEPATH += . \
    $$OPENPASS_SLAVE \
    $$OPENPASS_SLAVE/framework \
    $$OPENPASS_SLAVE/modelElements \
    $$OPENPASS_SLAVE/bindings \
    $$OPENPASS_SLAVE/importer/road \
    $$OPEN_SRC/core \
    $$CORE_SHARE/cephes \
    $$WORLD_OSI \
    $$WORLD_OSI/OWL \
    $$WORLD_OSI/Localization \
    $$OPEN_SRC/.. \
    $$OPEN_SRC/../..

SRC_SCENARIO =  $$OPENPASS_SLAVE/importer/eventDetectorImporter.cpp \
                $$OPENPASS_SLAVE/importer/scenario.cpp \
                $$OPENPASS_SLAVE/importer/scenarioImporter.cpp \
                $$OPENPASS_SLAVE/importer/scenarioImporterHelper.cpp \
                $$OPENPASS_SLAVE/importer/oscImporterCommon.cpp

INC_SCENARIO =  $$OPENPASS_SLAVE/importer/scenario.h \
                $$OPENPASS_SLAVE/importer/scenarioImporter.h \
                $$OPENPASS_SLAVE/importer/scenarioImporterHelper.h \
                $$OPEN_SRC/common/openScenarioDefinitions.h \
                $$OPENPASS_SLAVE/importer/oscImporterCommon.h

SRC_SCENERY =   $$OPENPASS_SLAVE/importer/scenery.cpp \
                $$OPENPASS_SLAVE/importer/sceneryImporter.cpp \
                $$OPENPASS_SLAVE/importer/road.cpp \
                $$OPENPASS_SLAVE/importer/junction.cpp \
                $$OPENPASS_SLAVE/importer/connection.cpp \
                $$OPENPASS_SLAVE/importer/road/roadSignal.cpp \
                $$OPENPASS_SLAVE/importer/road/roadObject.cpp \
                $$OPENPASS_SLAVE/bindings/worldBinding.cpp \
                $$OPENPASS_SLAVE/bindings/worldLibrary.cpp \
                $$OPENPASS_SLAVE/modules/Stochastics/stochastics_implementation.cpp

INC_SCENERY =   $$OPENPASS_SLAVE/importer/scenery.h \
                $$OPENPASS_SLAVE/importer/sceneryImporter.h \
                $$OPENPASS_SLAVE/importer/road.h \
                $$OPENPASS_SLAVE/importer/junction.h \
                $$OPENPASS_SLAVE/importer/connection.h \
                $$OPENPASS_SLAVE/importer/road/roadSignal.h \
                $$OPENPASS_SLAVE/importer/road/roadObject.h \
                $$OPENPASS_SLAVE/bindings/world.h \
                $$OPENPASS_SLAVE/bindings/worldBinding.h \
                $$OPENPASS_SLAVE/bindings/worldLibrary.h \
                $$OPENPASS_SLAVE/modules/Stochastics/stochastics_implementation.h

SRC_SYSTEMCONFIG =  $$OPENPASS_SLAVE/importer/systemConfig.cpp \
                    $$OPENPASS_SLAVE/importer/systemConfigImporter.cpp \
                    $$OPENPASS_SLAVE/modelElements/agentType.cpp \
                    $$OPENPASS_SLAVE/modelElements/componentType.cpp

INC_SYSTEMCONFIG =  $$OPENPASS_SLAVE/importer/systemConfig.h \
                    $$OPENPASS_SLAVE/importer/systemConfigImporter.h \
                    $$OPENPASS_SLAVE/modelElements/agentType.h \
                    $$OPENPASS_SLAVE/modelElements/componentType.h

SRC_VEHICLEMODELS = $$OPENPASS_SLAVE/importer/vehicleModels.cpp \
                    $$OPENPASS_SLAVE/importer/vehicleModelsImporter.cpp

INC_VEHICLEMODELS = $$OPENPASS_SLAVE/importer/vehicleModels.h \
                    $$OPENPASS_SLAVE/importer/vehicleModelsImporter.h

INC_SLAVECONFIG = $$OPENPASS_SLAVE/importer/slaveConfig.h \
                  $$OPENPASS_SLAVE/importer/slaveConfigImporter.h \
                  $$OPENPASS_SLAVE/framework/directories.h

SRC_SLAVECONFIG = $$OPENPASS_SLAVE/importer/slaveConfig.cpp \
                  $$OPENPASS_SLAVE/importer/slaveConfigImporter.cpp \
                  $$OPENPASS_SLAVE/importer/parameterImporter.cpp \
                  $$OPENPASS_SLAVE/framework/directories.cpp

SRC_CORESHARE = $$OPEN_SRC/common/xmlParser.cpp \
                $$CORE_SHARE/parameters.cpp \
                $$CORE_SHARE/log.cpp \
                $$CORE_SHARE/callbacks.cpp \
                $$CORE_SHARE/cephes/fresnl.c \
                $$CORE_SHARE/cephes/const.c \
                $$CORE_SHARE/cephes/polevl.c

INC_CORESHARE = $$OPEN_SRC/common/xmlParser.h \
                $$CORE_SHARE/parameters.h \
                $$CORE_SHARE/log.h \
                $$CORE_SHARE/callbacks.h

SRC_WORLD = $$WORLD_OSI/WorldData.cpp \
            $$WORLD_OSI/WorldDataQuery.cpp \
            $$WORLD_OSI/WorldDataException.cpp \
            $$WORLD_OSI/WorldObjectAdapter.cpp \
            $$WORLD_OSI/AgentAdapter.cpp \
            $$WORLD_OSI/OWL/DataTypes.cpp \
            $$WORLD_OSI/OWL/OpenDriveTypeMapper.cpp \
            $$WORLD_OSI/Localization.cpp \
            $$WORLD_OSI/WorldToRoadCoordinateConverter.cpp \
            $$WORLD_OSI/PointQuery.cpp \
            $$WORLD_OSI/egoAgent.cpp

INC_WORLD = $$WORLD_OSI/WorldData.h \
            $$WORLD_OSI/WorldDataQuery.h \
            $$WORLD_OSI/WorldDataException.h \
            $$WORLD_OSI/WorldObjectAdapter.h \
            $$WORLD_OSI/AgentAdapter.h \
            $$WORLD_OSI/OWL/DataTypes.h \
            $$WORLD_OSI/OWL/OpenDriveTypeMapper.h \
            $$WORLD_OSI/Localization.h \
            $$WORLD_OSI/WorldToRoadCoordinateConverter.h \
            $$WORLD_OSI/PointQuery.h \
            $$WORLD_OSI/egoAgent.h

SOURCES += \
    $$OPEN_SRC/common/eventDetectorDefinitions.cpp \
    $$OPENPASS_SLAVE/modelElements/agentBlueprint.cpp \
    ScenarioImporter_IntegrationTests.cpp \
    SceneryImporter_IntegrationTests.cpp \
    SlaveConfigImporter_IntegrationTests.cpp \
    SystemConfigImporter_IntegrationTests.cpp \
    VehicleModelsImporter_IntegrationTests.cpp \
    $$SRC_SCENARIO \
    $$SRC_SCENERY \
    $$SRC_SYSTEMCONFIG \
    $$SRC_VEHICLEMODELS \
    $$SRC_CORESHARE \
    $$SRC_WORLD \
    $$SRC_SLAVECONFIG

HEADERS += \
    $$INC_SCENARIO \
    $$INC_SCENERY \
    $$INC_SYSTEMCONFIG \
    $$INC_VEHICLEMODELS \
    $$INC_CORESHARE \
    $$INC_WORLD \
    $$INC_SLAVECONFIG


LIBS += -lopen_simulation_interface -lprotobuf

win32: {
    LIBS += -llibboost_filesystem-mgw81-mt-x64-1_72
} else {
    LIBS += -lboost_filesystem
}
