#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#* Copyright (c) 2020 in-tech GmbH
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

set(FOLDER "CoreComponents")

#add_subdirectory(Action_BrakeLight_Basic)
add_subdirectory(Action_LongitudinalDriver)
add_subdirectory(Action_SecondaryDriverTasks)
add_subdirectory(AgentUpdater)
add_subdirectory(AlgorithmAgentFollowingDriverModel)
add_subdirectory(Algorithm_AEB)
#add_subdirectory(Algorithm_CruiseControlByDistance)
add_subdirectory(Algorithm_FmuWrapper)
add_subdirectory(Algorithm_Lateral)
add_subdirectory(Algorithm_Longitudinal)
add_subdirectory(ComponentController)
add_subdirectory(Dynamics_Collision)
add_subdirectory(Dynamics_Longitudinal_Basic)
add_subdirectory(Dynamics_RegularDriving)
add_subdirectory(Dynamics_TrajectoryFollower)
add_subdirectory(LimiterAccelerationVehicleComponents)
add_subdirectory(OpenScenarioActions)
add_subdirectory(Parameters_Vehicle)
add_subdirectory(SensorFusion_OSI)
#add_subdirectory(Sensor_Distance)
add_subdirectory(Sensor_Driver)
add_subdirectory(Sensor_OSI)
add_subdirectory(Sensor_RecordState)
add_subdirectory(SignalPrioritizer)
