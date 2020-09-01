# /*********************************************************************
# * Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

TEMPLATE = subdirs
SUBDIRS = \
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
    components/Action_LongitudinalDriver \
    components/Action_SecondaryDriverTasks \
    components/AgentUpdater \
    components/Algorithm_AEB \
    components/Algorithm_FmuWrapper \
    components/Algorithm_Lateral \
    components/Algorithm_Longitudinal \
    components/AlgorithmAFDM \
    components/ComponentController \
    components/Dynamics_Collision \
    components/Dynamics_CollisionPostCrash \
    components/Dynamics_RegularDriving \
    components/Dynamics_RegularTwoTrack \
    components/Dynamics_TF \
    components/LimiterAccVehComp \
    components/OpenScenarioActions \
    components/Parameters_Vehicle \
    components/Sensor_Driver \
    components/Sensor_OSI \
    components/Sensor_RecordState \
    components/SensorFusion_OSI \
    components/SignalPrioritizer

slave.file = core/slave/OpenPassSlave.pro
master.file = core/master/OpenPassMaster.pro
