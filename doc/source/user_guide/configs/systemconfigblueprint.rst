..
  ************************************************************
  Copyright (c) 2021 in-tech GmbH

  This program and the accompanying materials are made
  available under the terms of the Eclipse Public License 2.0
  which is available at https://www.eclipse.org/legal/epl-2.0/

  SPDX-License-Identifier: EPL-2.0
  ************************************************************

.. _systemconfigblueprint:

SystemConfigBlueprint
=====================

This file contains the possible agent modules and channels of a dynamically generated agent. The content of this file should only be adjusted by experienced users with knowledge of the simulation architecture. The SystemConfigBlueprint is a special SystemConfig and has the same schema. Only the system with id 0 is used for generating dynamic agents.
If the simulation uses only statically configured agents (AgentProfile Type attribute is "Static"), this file isn't required.

.. _systemconfigblueprint_agentComponents:

AgentComponents
---------------
All components are listed here. An agent consists of a subset of this components.

.. table::
   :class: tight-table

   =========== ===============================================================================
   Attribute   Description                                            
   =========== ===============================================================================
   Id          Used as key by the simulation to find the component    
   Priority    The module with the highest priority value gets executed first by the scheduler                        
   Offset      Delay for the trigger method of each component in ms   
   Cycle       Interval in which the component gets triggered by the scheduler in ms                                        
   Response    Delay for the UpdateOutput method of each component in ms                                                     
   Library     Library name of this component                         
   Parameters  Parameters used by the component                       
   =========== ===============================================================================

Example:
This example describes the Sensor_Driver module.

.. literalinclude:: @OP_REL_SIM@/contrib/examples/Common/systemConfigBlueprint.xml
   :language: xml
   :dedent: 12
   :lines: 28-38

.. _systemconfigblueprint_priorities:

Priorities
----------

Please refer to the [Components and channel communication diagram]]\ref dev_concepts_modulecomposition) for assignment of a proper priority.
Based on the signal flow, input relevant components like sensors need to be executed first. They provide data for consuming components (algorithms) like ADAS and drivers.
Data is then handled by algorithms like Algorithm_Lateral.
Output-relevant modules like dynamics and actions are executed last.

Prioritizer can be applied on different levels depending on the modules/data they need to handle levels as following can be defined:
Level 1 describes data produced by ADAS and drivers. 
Level 2 describes data output by vehicle dynamic controllers.
Level 3 describes data delivered by dynamics.

Priorities can be grouped (coarse) as following:
Highest number indicates highest priority.

.. table::
   :class: tight-table
   
   =========================== ===========
   Scope                       Range     
   =========================== ===========
   Parameters                  500       
   OpenScenarioActions         400       
   Sensor                      350...399 
   Event                       330       
   DriverMode                  310       
   ADAS                        250...299 
   ComponentController         200       
   Prioritizer (Lvl. 1)        150...199 
   VehicleDynamicsControllers  100...149 
   Prioritizer (Lvl. 2)        75...99   
   Dynamics                    50...74   
   Prioritizer (Lvl. 3)        25...49   
   Updater                     0...24    
   =========================== ===========

The table below can be used as orientation when a new module is introduced.

.. table::
   :class: tight-table

   ========================================= ======================================= ========= =========================== =======================================================================================================================
   Name                                      Library                                 Priority  Scope                       Note            
   ========================================= ======================================= ========= =========================== =======================================================================================================================
   ParametersAgentModules                    ParametersAgent                         500       Parameters                  Sets all init-data and is updated cyclically      
   OpenScenarioActions                       OpenScenarioActions                     400       ADAS                        Reads events from OpenScenario Actions and forwards them to other components      
   SensorObjectDetector                      Sensor_OSI                              398       Sensor                      Gets instantiated multiple times (one time per sensor)         
   SensorAggregation                         SensorAggregation_OSI                   351       Sensor                                     
   SensorFusionErrorless                     SensorFusionErrorless_OSI               350       Sensor                                     
   AlgorithmAgentFollowingDriverModel        AlgorithmAgentFollowingDriverModel      310       DriverModels                               
   AEB                                       AlgorithmAutonomousEmergencyBraking     250       ADAS                                       
   ComponentController                       ComponentController                     200       ADAS                        Manages vehicle component states with regard to other vehicle component states and conditions and in response to events.         
   PrioritizerLaterDriver                    SignalPrioritizer                       150       Prioritizer                                
   PrioritizerAccelerationDriver             SignalPrioritizer                       150       Prioritizer                                
   PrioritizerTurningIndicator               SignalPrioritizer                       150       Prioritizer                                
   PrioritizerSteeringVehicleComponents      SignalPrioritizer                       150       Prioritizer                                
   PrioritizerAccelerationVehicleComponents  SignalPrioritizer                       150       Prioritizer                                
   LimiterAccelerationVehicleComponents      LimiterAccelerationVehicleComponents    120       VehicleDynamicsControllers                 
   AlgorithmLateralDriver                    AlgorithmLateralDriver                  100       VehicleDynamicsControllers                 
   AlgorithmLongitudinalVehicleComponents    AlgorithmLongitudinalVehicleComponents  100       VehicleDynamicsControllers                 
   AlgorithmLongitudinalDriver               AlgorithmLongitudinalDriver             100       VehicleDynamicsControllers                 
   PrioritizerSteering                       SignalPrioritizer                       75        Prioritizer                                
   PrioritizerLongitudinal                   SignalPrioritizer                       75        Prioritizer                                
   DynamicsCollision                         DynamicsCollision                       50        Dynamics                                   
   DynamicsRegularDriving                    DynamicsRegularDriving                  50        Dynamics                                   
   DynamicsTrajectoryFollower                DynamicsTrajectoryFollower              50        Dynamics                                   
   PrioritizerDynamics                       SignalPrioritizer                       25        Prioritizer                                
   SensorRecordStateModule                   SensorRecordState                       2         Updater                     Since values are "frozen" for current time step, logging can be placed anywhere        
   ActionLongitudinalDriverModules           ActionLongitudinalDriver                3         Updater                     Will be expanded to ActionPrimary DriverTasks     
   ActionSecondaryDriverTasksModules         ActionSecondaryDriverTasks              3         Updater                                    
   AgentUpdater                              AgentUpdater                            1         Updater                                    
   ========================================= ======================================= ========= =========================== ======================================================================================================================= 

.. _systemconfigblueprint_channelids:

Channel-Ids
-----------

Channels allow components to communicate with each other.
The signalflow is set explicitly via a channel-Id of 4 digits (see also [Components and channels communication diagram](/ref dev_concepts_modulecomposition)).

The first two numbers define the sending module (XX 00).
The other two digits define the type of signal that is sent (00 XX).

Signals as well as modules can be grouped to allow explicit numbering (see tables below).

Channel-Ids between Sensor and SensorFusion are an exception to this rule. For sensor/sensor fusion communication channel-ids are 9900 + x (incremented for every new sensor)

Example:
PrioritizerAccelerationDriver -> AlgorithmLongitudinalDriver with signal of type AccelerationSignal: 1813.


**Ids for Modules (first two digits)**


Index range for module groups:

.. table::
   :class: tight-table
   
   ================= ==========
   Group             Id      
   ================= ==========
   Dynamics          1...10  
   Algorithm         11...30 
   DriverTasks       31...40 
   Driver            41...50 
   VehicleComponent  51...80 
   Special           81...89 
   Sensor            91...99 
   ================= ========== 

With corresponding defined indices:

.. table::
   :class: tight-table
  
   ========================================= ================= =====
   Module                                    Group             Id 
   ========================================= ================= =====
   AgentUpdater                              Dynamics          1  
   Dynamics_TrajectoryFollower               Dynamics          2  
   Dynamics_RegularDriving                   Dynamics          3  
   Dynamics_Collision                        Dynamics          4  
   PrioritizerDynamics                       Dynamics          5  
   Algorithm_LongitudinalVehicleComponent    Algorithm         11 
   Algorithm_LongitudinalAfdm                Algorithm         12 
   Algorithm_SteeringVehicleComponent        Algorithm         14 
   Algorithm_LateralVehicleAfdm              Algorithm         15 
   LimiterVehicleLongitudinal                Algorithm         17 
   PrioritizerLongitudinal                   Algorithm         21 
   PrioritizerSteering                       Algorithm         22 
   PrioritizerAccelerationVehicleComponents  Algorithm         23 
   PrioritizerSteeringVehicleComponents      Algorithm         24 
   Action_LongitudinalDriver                 DriverTasks       31 
   Action_SecondaryDriverTasks               DriverTasks       32 
   PrioritizerTurningIndicator               DriverTasks       33 
   AlgorithmAgentFollowingDriver             Driver            41 
   AEB                                       VehicleComponent  52 
   ComponentController                       Special           83 
   OpenScenarioActions                       Special           84 
   Parameter_Vehicle                         Sensor            92 
   SensorAggregation                         Sensor            93 
   SensorFusion                              Sensor            94 
   Sensor_Driver                             Sensor            95 
   ========================================= ================= =====

**Ids for Signals (last two digits)**

Index range for signal groups:


.. table::
   :class: tight-table

   ==================== ========== 
   Group                Id      
   ==================== ==========
   Dynamics             1...10  
   Algorithm            11...30 
   OpenScenarioActions  61...70 
   Special              71...80 
   Sensor               81...90 
   Parameters           91...99 
   ==================== ==========
 
With corresponding defined indices:

.. table::
   :class: tight-table
 
   ======================================= ==================== =====
   Signal                                  Group                Id 
   ======================================= ==================== =====
   Dynamics                                Dynamics             01 
   Longitudinal                            Algorithm            11 
   Steering                                Algorithm            12 
   Acceleration                            Algorithm            13 
   Lateral                                 Algorithm            14 
   SecondaryDriverTasks                    Algorithm            19 
   Trajectory                              OpenScenarioActions  71 
   AcquireGlobalPosition                   OpenScenarioActions  62 
   CustomParameters (CustomCommandAction)  OpenScenarioActions  63 
   SensorDriver                            Sensor               81 
   SensorData                              Sensor               90 
   ParametersVehicle                       Parameters           92 
   ======================================= ==================== =====
  
.. _systemconfigblueprint_parameters:

Parameters
----------

For more information on the type of parameters (escpecially stochastic distributions), please refer to the [ProfilesGroup section](\ref io_input_profilescatalog_profileGroups).

**Important Note:** The syntax for defining parameters in the SystemConfigBlueprint file differs from the ProfilesCatalog syntax.
See the following example:

.. code-block:: xml

   <parameters>
       <parameter>
           <id>StringParameter</id>
           <type>string</type>
           <unit/>
           <value>Lorem ipsum</value>
       </parameter>
       <parameter>
           <id>RandomParameter</id>
           <type>normalDistribution</type>
           <unit/>
           <value>
               <mean>15.0</mean>
               <sd>2.5</sd>
               <min>10.0</min>
               <max>20.0</max>
           </value>
       </parameter>
   </parameters>
