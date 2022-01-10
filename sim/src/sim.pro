################################################################################
# Copyright (c) 2020 ITK Engineering GmbH
#               2017-2020 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

DEFINES += USE_PROTOBUF_ARENA

TEMPLATE = subdirs
SUBDIRS = \
    components \
    opsimulationmanager \
    core/opSimulation \
    core/opSimulation/modules/BasicDataBuffer \
    core/opSimulation/modules/EventDetector \
    core/opSimulation/modules/Manipulator \
    core/opSimulation/modules/Observation_EntityRepository \
    core/opSimulation/modules/Observation_Log \
    core/opSimulation/modules/Observation_LogAgent \
    core/opSimulation/modules/Spawners/Scenario \
    core/opSimulation/modules/Spawners/PreRunCommon \
    core/opSimulation/modules/Spawners/RuntimeCommon \
    core/opSimulation/modules/Stochastics \
    core/opSimulation/modules/World_OSI \

opSimulation.file = core/opSimulation/opSimulation.pro
opsimulationmanager.file = core/opsimulationmanager/opSimulationManager.pro
