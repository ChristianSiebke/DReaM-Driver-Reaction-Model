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
        CoreFramework/OpenPassMaster \
        CoreFramework/OpenPassSlave \
        CoreModules/BasicDataStore \
        CoreModules/EventDetector \
        CoreModules/Manipulator \
        CoreModules/Observation_Log \
        CoreModules/SpawnPoints/SpawnPointScenario \
        CoreModules/SpawnPoints/SpawnPointPreRunCommon \
        CoreModules/SpawnPoints/SpawnPointRuntimeCommon \
        CoreModules/Stochastics \
        CoreModules/World_OSI \
        Components/Action_LongitudinalDriver \
        Components/Action_SecondaryDriverTasks \
        Components/AgentUpdater \
        Components/Algorithm_AEB \
        Components/Algorithm_FmuWrapper \
        Components/Algorithm_Lateral \
        Components/Algorithm_Longitudinal \
        Components/AlgorithmAFDM \
        Components/ComponentController \
        Components/Dynamics_Collision \
        Components/Dynamics_RegularDriving \
        Components/Dynamics_TF \
        Components/LimiterAccVehComp \
        Components/OpenScenarioActions \
        Components/Parameters_Vehicle \
        Components/Sensor_Driver \
        Components/Sensor_OSI \
        Components/Sensor_RecordState \
        Components/SensorFusion_OSI \
        Components/SignalPrioritizer
