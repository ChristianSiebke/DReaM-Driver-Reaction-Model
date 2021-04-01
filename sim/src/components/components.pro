# /*********************************************************************
# * Copyright (c) 2020 ITK Engineering GmbH
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

DEFINES += USE_PROTOBUF_ARENA

TEMPLATE = subdirs
SUBDIRS = \
    Action_LongitudinalDriver \
    Action_SecondaryDriverTasks \
    AgentUpdater \
    Algorithm_AEB \
    #Algorithm_FmuWrapper \
    Algorithm_Lateral \
    Algorithm_Longitudinal \
    AlgorithmAFDM \
    ComponentController \
    Dynamics_Collision \
    Dynamics_CollisionPostCrash \
    Dynamics_RegularDriving \
    Dynamics_RegularTwoTrack \
    Dynamics_TF \
    LimiterAccVehComp \
    OpenScenarioActions \
    Parameters_Vehicle \
    Sensor_Driver \
    #Sensor_OSI \
    Sensor_RecordState \
    SensorAggregation_OSI \
    SensorFusionErrorless_OSI \
    SignalPrioritizer \
    Algorithm_ECU \
    Algorithm_RouteControl \
    Algorithm_Switch \
    Sensor_Collision \
    Dynamics_CollisionPCM \
    Dynamics_TwoTrack \
    Dynamics_CopyTrajectory \
    Dynamics_Chassis \
