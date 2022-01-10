################################################################################
# Copyright (c) 2021 ITK Engineering GmbH
#               2019-2021 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

#-----------------------------------------------------------------------------
# \file  opSimulation_IntegrationTests.pro
# \brief This file contains tests for the opSimulation module
#-----------------------------------------------------------------------------/
QT += xml

CONFIG += OPENPASS_GTEST \
          OPENPASS_GTEST_DEFAULT_MAIN
include(../../testing.pri)

OPENPASS_SIMULATION = $$OPEN_SRC/core/opSimulation
CORE_SHARE = $$OPEN_SRC/core/common
WORLD_OSI = $$OPEN_SRC/core/opSimulation/modules/World_OSI

INCLUDEPATH += . \
    $$OPENPASS_SIMULATION \
    $$OPENPASS_SIMULATION/framework \
    $$OPENPASS_SIMULATION/modelElements \
    $$OPENPASS_SIMULATION/bindings \
    $$OPENPASS_SIMULATION/importer/road \
    $$OPEN_SRC/core \
    $$CORE_SHARE/cephes \
    $$WORLD_OSI \
    $$WORLD_OSI/OWL \
    $$WORLD_OSI/Localization \
    $$OPEN_SRC/.. \
    $$OPEN_SRC/../..

SRC_SCENARIO =  $$OPENPASS_SIMULATION/importer/eventDetectorImporter.cpp \
                $$OPENPASS_SIMULATION/importer/scenario.cpp \
                $$OPENPASS_SIMULATION/importer/scenarioImporter.cpp \
                $$OPENPASS_SIMULATION/importer/scenarioImporterHelper.cpp \
                $$OPENPASS_SIMULATION/importer/sceneryDynamics.cpp \
                $$OPENPASS_SIMULATION/importer/oscImporterCommon.cpp

INC_SCENARIO =  $$OPENPASS_SIMULATION/importer/scenario.h \
                $$OPENPASS_SIMULATION/importer/scenarioImporter.h \
                $$OPENPASS_SIMULATION/importer/scenarioImporterHelper.h \
                $$OPENPASS_SIMULATION/importer/sceneryDynamics.h \
                $$OPEN_SRC/common/openScenarioDefinitions.h \
                $$OPENPASS_SIMULATION/importer/oscImporterCommon.h

SRC_SCENERY =   $$OPENPASS_SIMULATION/importer/scenery.cpp \
                $$OPENPASS_SIMULATION/importer/sceneryImporter.cpp \
                $$OPENPASS_SIMULATION/importer/road.cpp \
                $$OPENPASS_SIMULATION/importer/junction.cpp \
                $$OPENPASS_SIMULATION/importer/connection.cpp \
                $$OPENPASS_SIMULATION/importer/road/roadSignal.cpp \
                $$OPENPASS_SIMULATION/importer/road/roadObject.cpp \
                $$OPENPASS_SIMULATION/bindings/worldBinding.cpp \
                $$OPENPASS_SIMULATION/bindings/worldLibrary.cpp \
                $$OPENPASS_SIMULATION/modules/Stochastics/stochastics_implementation.cpp

INC_SCENERY =   $$OPENPASS_SIMULATION/importer/scenery.h \
                $$OPENPASS_SIMULATION/importer/sceneryImporter.h \
                $$OPENPASS_SIMULATION/importer/road.h \
                $$OPENPASS_SIMULATION/importer/junction.h \
                $$OPENPASS_SIMULATION/importer/connection.h \
                $$OPENPASS_SIMULATION/importer/road/roadSignal.h \
                $$OPENPASS_SIMULATION/importer/road/roadObject.h \
                $$OPENPASS_SIMULATION/bindings/world.h \
                $$OPENPASS_SIMULATION/bindings/worldBinding.h \
                $$OPENPASS_SIMULATION/bindings/worldLibrary.h \
                $$OPENPASS_SIMULATION/modules/Stochastics/stochastics_implementation.h

SRC_SYSTEMCONFIG =  $$OPENPASS_SIMULATION/importer/systemConfig.cpp \
                    $$OPENPASS_SIMULATION/importer/systemConfigImporter.cpp \
                    $$OPENPASS_SIMULATION/modelElements/agentType.cpp \
                    $$OPENPASS_SIMULATION/modelElements/componentType.cpp

INC_SYSTEMCONFIG =  $$OPENPASS_SIMULATION/importer/systemConfig.h \
                    $$OPENPASS_SIMULATION/importer/systemConfigImporter.h \
                    $$OPENPASS_SIMULATION/modelElements/agentType.h \
                    $$OPENPASS_SIMULATION/modelElements/componentType.h

SRC_VEHICLEMODELS = $$OPENPASS_SIMULATION/importer/vehicleModels.cpp \
                    $$OPENPASS_SIMULATION/importer/vehicleModelsImporter.cpp

INC_VEHICLEMODELS = $$OPENPASS_SIMULATION/importer/vehicleModels.h \
                    $$OPENPASS_SIMULATION/importer/vehicleModelsImporter.h

INC_SIMULATIONCONFIG = $$OPENPASS_SIMULATION/importer/simulationConfig.h \
                  $$OPENPASS_SIMULATION/importer/simulationConfigImporter.h \
                  $$OPENPASS_SIMULATION/framework/directories.h

SRC_SIMULATIONCONFIG = $$OPENPASS_SIMULATION/importer/simulationConfig.cpp \
                  $$OPENPASS_SIMULATION/importer/simulationConfigImporter.cpp \
                  $$OPENPASS_SIMULATION/importer/parameterImporter.cpp \
                  $$OPENPASS_SIMULATION/framework/directories.cpp

SRC_CORESHARE = $$OPEN_SRC/common/xmlParser.cpp \
                $$CORE_SHARE/log.cpp \
                $$CORE_SHARE/callbacks.cpp \
                $$CORE_SHARE/cephes/fresnl.c \
                $$CORE_SHARE/cephes/const.c \
                $$CORE_SHARE/cephes/polevl.c

INC_CORESHARE = $$OPEN_SRC/common/xmlParser.h \
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
            $$WORLD_OSI/egoAgent.cpp \
            $$WORLD_OSI/LaneStream.cpp \
            $$WORLD_OSI/RoadStream.cpp

INC_WORLD = $$WORLD_OSI/WorldData.h \
            $$WORLD_OSI/WorldDataQuery.h \
            $$WORLD_OSI/WorldDataException.h \
            $$WORLD_OSI/WorldObjectAdapter.h \
            $$WORLD_OSI/AgentAdapter.h \
            $$WORLD_OSI/OWL/DataTypes.h \
            $$WORLD_OSI/OWL/OpenDriveTypeMapper.h \
            $$WORLD_OSI/Localization.h \
            $$WORLD_OSI/WorldToRoadCoordinateConverter.h \
            $$WORLD_OSI/egoAgent.h \
            $$WORLD_OSI/LaneStream.h \
            $$WORLD_OSI/RoadStream.h

SOURCES += \
    $$OPEN_SRC/common/commonTools.cpp \
    $$OPEN_SRC/common/eventDetectorDefinitions.cpp \
    $$OPENPASS_SIMULATION/modelElements/agentBlueprint.cpp \
    $$OPENPASS_SIMULATION/modelElements/parameters.cpp \
    ScenarioImporter_IntegrationTests.cpp \
    SceneryImporter_IntegrationTests.cpp \
    SimulationConfigImporter_IntegrationTests.cpp \
    SystemConfigImporter_IntegrationTests.cpp \
    VehicleModelsImporter_IntegrationTests.cpp \
    $$SRC_SCENARIO \
    $$SRC_SCENERY \
    $$SRC_SYSTEMCONFIG \
    $$SRC_VEHICLEMODELS \
    $$SRC_CORESHARE \
    $$SRC_WORLD \
    $$SRC_SIMULATIONCONFIG

HEADERS += \
    $$OPEN_SRC/common/commonTools.h \
    $$INC_SCENARIO \
    $$INC_SCENERY \
    $$INC_SYSTEMCONFIG \
    $$INC_VEHICLEMODELS \
    $$INC_CORESHARE \
    $$INC_WORLD \
    $$INC_SIMULATIONCONFIG


LIBS += -lopen_simulation_interface -lprotobuf

win32: {
    LIBS += -lboost_filesystem-mt
} else {
    LIBS += -lboost_filesystem
}
