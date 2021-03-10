# /*********************************************************************
# * Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
# *                                 2020 ITK Engineering GmbH
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

DEFINES += USE_PROTOBUF_ARENA

TEMPLATE = subdirs
SUBDIRS = \
    components \
    master \
    slave \
    core/slave/modules/BasicDataStore \
    core/slave/modules/EventDetector \
    core/slave/modules/Manipulator \
    core/slave/modules/Observation_Log \
    core/slave/modules/Observation_LogAgent \
    core/slave/modules/Spawners/Scenario \
    core/slave/modules/Spawners/PreRunCommon \
    core/slave/modules/Spawners/RuntimeCommon \
    core/slave/modules/Stochastics \
    core/slave/modules/World_OSI \

slave.file = core/slave/OpenPassSlave.pro
master.file = core/master/OpenPassMaster.pro
