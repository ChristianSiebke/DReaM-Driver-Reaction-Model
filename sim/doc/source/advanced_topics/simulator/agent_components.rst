..
  *******************************************************************************
  Copyright (c) 2021 in-tech GmbH

  This program and the accompanying materials are made available under the
  terms of the Eclipse Public License 2.0 which is available at
  http://www.eclipse.org/legal/epl-2.0.

  SPDX-License-Identifier: EPL-2.0
  *******************************************************************************

.. _agentcomponents:

Agent Components
================

An agent in openPASS is composed of multiple modules, which are connected by corresponding signals.
As shown in the next figure, the modules can be roughly divided into the groups drivers, vehicle components, algorithms, dynamic modules, and prioritizers. 
Thereby, modules can consist of several submodules, such as sensor (reading from an interface) and action (writing to an interface).

.. figure:: ./images/DynamicsModules.png

    Modules for longitudinal and lateral dynamics

By statistic means, based on corresponding probabilities defined in the :ref:`profilescatalog`, each individual agent is composed from a superset of all possible (valid) combinations, which is defined by the :ref:`systemconfigblueprint`.
This config defines all available framework modules and agent modules and connects them by corresponding channels, which in turn have a specific signal type.

The next figure gives an exhaustive overview over the current superset:

.. figure:: ./images/ComponentsChannelCommunicationDiagram.svg

    Components and channel communication
    
:download:`./images/ComponentsChannelCommunicationDiagram.svg`

Modules that can be paramerized by the user are explained in the :ref:`Simulation User Guide <simuserguide_components>`.
Therefor the following section only contains the components not listed in the user guide.

Action_LongitudinalDriver
-------------------------

Updates the agents pedal positions and gear.
The input for this module is prepared by the AlgorithmLongitudinal Module.

Action_SecondaryDriver
-----------------------

Updates the agents BrakingLight, Indicator, Horn and all Lightswitches (Headlight, HighBeam, Flasher).
The input for this module is prepared by the driver module.

AgentUpdater
------------

The AgentUpdater executes all Set-Methods of the agent dynamics after the DynamicsPrioritizer. This includes position, velocity, acceleration and rotation.

Algorithm_Lateral
-----------------

This module converts the lateral input of the driver module into a steeringwheel angle.

Algorithm_Longitudinal
----------------------

This module converts the acceleration input of the driver module into pedal positions and gear.

Dynamics_Collision
------------------

If the number of collision partners of the agent is bigger than in the previous timestep, the DynamicsCollision module calculates the collision. 
Currently the collision is implemented fully inelastic, i.e. all agents will have the same velocity after the collision, while the momentum is conserved. 
After the collision the agents slow down with a fixed deceleration until fully stopped.

Dynamics_RegularDriving
------------------------

The module takes care that the motion of the agent fit to the physical limitations, such as friction or maximum possible acceleration based on the current gear.
This module uses both the world friction and the vehiclemodelparameter friction.
Thereby it calculates the dynamics of the agents in every time step.
The currently covered  dynamics are *Acceleration*, *Velocity*, and consequently *Position*, *Yaw angle* and *Yaw rate*.
The input for this module is the steeringwheel angle and the new acceleration of the vehicle.

LimiterAccelerationVehicleComponents
-------------------------------------

This module limits the AccelerationSignal from the PrioritizerAccelerationVehicleComponents to the constraints given by the vehicle. The DynamicsTrajectoryFollower can then use this signal to calculate a trajectory.

The limit is calculated by :math:`a_{\text{lim}} = \frac {F_{\text{wheel}} - F_{\text{roll}} - F_{\text{air}}} {m_{\text{veh}}}`, where the symbols meanings are:

======================== ================================================
Symbol                   Decription                                     
======================== ================================================
:math:`a_{\text{lim}}`   Resulting acceleration limit [m/s²] 
:math:`F_{\text{wheel}}` Force at wheel (provided by drivetrain) [N]    
:math:`F_{\text{roll}}`  Force resulting from rolling resistance [N]    
:math:`F_{\text{air}}`   Force resulting from air drag [N]              
:math:`m_{\text{veh}}`   Mass of the vehicle [kg]                       
======================== ================================================

The components are calculated as follows:

**Driving force**

:math:`F_{\text{wheel}} = \frac {T_{\text{engine}} \cdot r_{\text{axle}}} {r_{\text{wheel}}}`

========================= ============================================================================================
Symbol                    Decription                                                                                 
========================= ============================================================================================
:math:`T_{\text{engine}}` Resulting torque from drivetrain at current velocity (assumung best gearing selected) [Nm] 
:math:`r_{\text{axle}}`   Axle transmission ratio [1]                                                                
:math:`r_{\text{wheel}}`  Static radius of the wheels [m]                                                            
========================= ============================================================================================

The engine torque :math:`T_{\text{engine}}` is calculated by a simple model, where the torque scales proportional with the current engine speed between 1350 and 5000 rpm, up to maximum engine torque.
From minimum engine speed up to 1350 rpm the torque scales proportional with the engine speed up to half the maximum torque.
From 5000 rpm up to maximum engine speed, the torque scales with 5000 / maxEngineSpeed, up to maximum torque.

**Rolling resistance**

:math:`F_{\text{roll}} = m_{\text{veh}} \cdot c_{\text{fric}} \cdot g`

========================= ============================================================================================
Symbol                    Decription                                        
========================= ============================================================================================
:math:`m_{\text{veh}}`    Mass of the vehicle [kg]                          
:math:`c_{\text{fric}}`   Rolling friction coefficient (constant 0.015) [1] 
========================= ============================================================================================


**Air drag**

:math:`F_{\text{air}} = \frac {\rho_{\text{air}}} {2} \cdot A_{\text{front}} \cdot c_w \cdot v^2`

========================= ============================================================================================
Symbol                    Decription                                 
========================= ============================================================================================
:math:`\rho_{\text{air}}` Densitiy of air [kg/m³]         
:math:`A_{\text{front}}`  Vehicle front surface area [m²] 
:math:`c_w`               Drag coefficient [1]                       
:math:`v`                 Vehicle's current velocity [m/s]           
========================= ============================================================================================

OpenScenarioActions
-------------------

As defined by openSCENARIO, OpenScenarioActions is the relaying module for:

- Trajectory-actions
- LaneChange-actions 
- UserDefined-actions.
  
If a

- TrajectoryManipulator
- LaneChangeManipulator

or a user defined manipulator
  
raises such an event for the specified agent, the module forwards it as signal to all interested module of the corresponding agent. The modules can than react on the signals content without time delay.

Parameters_Vehicle
-------------------

The ParametersVehicle module forwards the VehicleModelParamters to all other moduls that need them via the ParametersVehicleSignal

Sensor_Driver
--------------

The SensorDriver performs queries on the AgentInterface to gather information about the own agent and its surroundings. These are forwarded to the driver modules and the Algorithm modules, which use them for their calculations.

SensorFusionOSI
---------------

The SensorFusionOSI module allows unsorted aggregation of any data provided by sensors. All sampled detected objects can then be broadcasted to connected ADAS.

It collects all SensorDataSignals and merges them into a single SensorDataSignal.

SignalPrioritizer
-----------------

All channels can only have one source.
If one modul can have the same input type from multiple sources a prioritizer modul is needed in between.
All sources then get an output channel to the prioritizer modul and the prioritizer gets an output to the modul, which uses this signal.
If more than an component sends an active signal during the same timestep, the prioritizer forwards only the signal from the input channel with the highest priority.
These priorities are set as parameters in the systemconfigblueprint.xml, where the key corresponds the the id of the input channel and the value is the priority (higher value is prioritized).
In the following example the channel with id 102 has the highest priority (3) and the channel with id 100 has the lowest priority (1).

.. code-block:: xml

   <component>
     <id>PrioritizerName</id>
     <schedule>
       <priority>150</priority>
       <offset>0</offset>
       <cycle>100</cycle>
       <response>0</response>
     </schedule>
     <library>SignalPrioritizer</library>
     <parameters>
       <parameter>
         <id>100</id>
         <type>int</type>
         <unit/>
         <value>1</value>
       </parameter>
       <parameter>
         <id>101</id>
         <type>int</type>
         <unit/>
         <value>2</value>
       </parameter>
       <parameter>
         <id>102</id>
         <type>int</type>
         <unit/>
         <value>3</value>
       </parameter>
     </parameters>
   </component>

One prioritizer modul can only handle signals of the same type and the signal class must be derived from ComponentStateSignal.
If there is no signal in one timestep, then the signal of the previos timestep is hold.

**Existing prioritizer modules**

* PrioritizerAccelerationVehicleComponents
* PrioritizerSteeringVehicleComponents
* PrioritizerTurningIndicator
* PrioritizerLongitudinal
* PrioritizerSteering
* PrioritizerDynamics


ComponentController
-------------------

Overview
~~~~~~~~~

The ComponentControoler (CC) is used to configure and handle dependencies between other vehicle components.

Example use cases could be:

- Cruise control:

  - driver requesting higher acceleration than cruise control overrides the latter
  - driver braking deactivates cruise control
  
- Lane keeping assistant:

  - cannot be activated by driver, if emergency braking is currently active
  - stays active, when emergency braking occours (i. e. by other ADAS)

The responsibilies of the CC are:

- Handling of all dependencies between *VehicleComponents* in case a component wants to activate
- Make information about driver, *TrajectoryFollower* and other *VehicleComponents* available to each other
- Determine the highest allowed activation state of a component and notify the affected component about this state

To achieve this tasks, each component is assigned a maximum allowed state in each timestep. This state is of type ComponentState,
which defines *Disabled*, *Armed* or *Active* as allowed states.
Drivers can be in a state of either *Active* or *Passive*.

State handling inside Vehicle Component
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Within a vehicle component, the information flow should be implemented as follows:

1. The vehicle component retrieves the information of other components and the current maximum allowed state from the CC.
   Other components include drivers, trajectory followers and all other vehicle components connected to the CC.
2. Based on that information the vehicle component determines its current desired state.
3. The desired state is sent to the CC.

The CC handles all the dependencies between different components and determines the maximum allowed state for each component based
on the configuration of the CC.

Used signals
~~~~~~~~~~~~~

The CC communicates with the controlled components via framework signals.

Inputs to the ComponentController:

.. table::
   :class: tight-table

   ================== ===================================================== =============================
   Source             Contents                                              Signal                      
   ================== ===================================================== =============================
   TrajectoryFollower Current state                                         ComponentStateSignal        
   Driver             Current state, pedal activity                         DriverStateSignal           
   VehicleComponent   Current state, desired state, generic ADAS parameters VehicleCompToCompCtrlSignal 
   ================== ===================================================== =============================


Output to other components:

.. table::
   :class: tight-table

   ================== ========================================================================== =============================
   Destination        Contents                                                                   Signal                      
   ================== ========================================================================== =============================
   TrajectoryFollower Current max. reachable state                                               ComponentStateSignal        
   Driver             List of all ADAS with names, stati and types                               AdasStateSignal             
   VehicleComponent   Current max. reachable state, list of all ADAS with names, stati and types CompCtrlToVehicleCompSignal 
   ================== ========================================================================== =============================
