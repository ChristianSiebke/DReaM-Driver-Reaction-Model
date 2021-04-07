..
  ************************************************************
  Copyright (c) 2021 in-tech GmbH

  This program and the accompanying materials are made
  available under the terms of the Eclipse Public License 2.0
  which is available at https://www.eclipse.org/legal/epl-2.0/

  SPDX-License-Identifier: EPL-2.0
  ************************************************************

.. _scenario:

Scenario
========

.. _scenario_overview:

Overview
--------

The scenario configuration file describes all dynamic configuration of a simulation run, i.e. the position of an agent as well as conditional events that alter the behavior of the simulation during the run.
It adheres to the **ASAM OpenSCENARIO 1.0** standard, although we support only a subset of the features of the standard.

The different parts of the scenario configuration are described in the following chapters.

.. _scenario_parameterdeclaration:

ParameterDeclarations
---------------------

The tag ``ParameterDeclarations`` allow to define generic parameters, which can be referenced later the file by its name, prefixed with ``$``.

**Example declaration**

.. literalinclude:: @OP_REL_SIM@/contrib/examples/DefaultConfigurations/Scenario.xosc
   :language: xml
   :dedent: 2
   :start-at: <ParameterDeclarations>
   :end-at: </ParameterDeclarations>

**Example usage**

.. literalinclude:: @OP_REL_SIM@/contrib/examples/DefaultConfigurations/Scenario.xosc
   :language: xml
   :dedent: 12
   :start-at: <SimulationTime
   :lines: 1

**Supported parameterTypes**

- string
- integer
- double

.. note:: Only ``string`` allows empty values.

.. warning::

   The parameter ``OP_OSC_SchemaVersion`` is used to check for schema compatibility and is mandatory within |op|.

.. _scenario_catalogs:

Catalogs
--------

The ``Catalogs`` tag defines references to various other files containing sub features of OpenSCENARIO (and written in the same format).

The following catalogs interpreted:

- Mandatory: :ref:`scenario_vehiclemodels`
- Mandatory: :ref:`scenario_pedestrianmodels`
- Optional: TrajectoryCatalog for the case that a *FollowTrajectory action* is defined

**Example**

.. literalinclude:: @OP_REL_SIM@/contrib/examples/DefaultConfigurations/Scenario.xosc
   :language: xml
   :dedent: 2
   :start-at: <CatalogLocations>
   :end-at: </CatalogLocations>

.. _scenario_vehiclemodels:

VehicleCatalog
~~~~~~~~~~~~~~

This file is mandatory.
It contains the available vehicle models for the simulation.
For each vehicle the physical parameters are stored here.

**Supported models:**

- car_bmw_i3
- car_bmw_3
- car_bmw_7_1
- car_bmw_7_2
- car_mini_cooper
- bus
- truck
- bicycle

**Full Example:** :download:`@OP_REL_SIM@/contrib/examples/DefaultConfigurations/VehicleModelsCatalog.xosc`

.. _scenario_pedestrianmodels:

PedestrianCatalog
~~~~~~~~~~~~~~~~~

This file is mandatory.
It contains the available pedestrian models for the simulation.
For each pedestrian the physical parameters are stored here.

.. note::

  Currently, pedestrian models are internally handled the same way as vehicle models, i.e. they behave like simplified vehicles.
  For convenience, |op| internally sets *meaningless* parameters, such as *number of gears*, to operational defaults.

**Supported models:**

- pedestrian_child
- pedestrian_adult

**Full Example:** :download:`@OP_REL_SIM@/contrib/examples/DefaultConfigurations/PedestrianModelsCatalog.xosc`

.. _scenario_roadnetwork:

RoadNetwork
-----------

The ``RoadNetwork`` tag contains the mandatory reference to the OpenDRIVE :ref:`scenery`.

**Example**

.. literalinclude:: @OP_REL_SIM@/contrib/examples/DefaultConfigurations/Scenario.xosc
   :language: xml
   :dedent: 2
   :start-at: <RoadNetwork>
   :end-at: </RoadNetwork>

.. _scenario_entities:

Entities
--------

The ``Entities`` tag defines all agents that are present at the start of the simulation at predefined positions.
There may be any number of these so called **Scenario Agents** (also zero).
Each entity is described by a name and a reference to a vehicle profile within the :ref:`profilescatalog`.

.. note:: This deviates from the OpenSCENARIO standard.

.. todo:: In what way do scenario agents deviate from the standard?

To specify an entity as the **Ego Agent**, the it must be named "Ego".

Entities can also be grouped into selections.

.. todo:: What is an EntitySelection?

**Example**

.. literalinclude:: @OP_REL_SIM@/contrib/examples/DefaultConfigurations/Scenario.xosc
   :language: xml
   :dedent: 2
   :start-at: <Entities>
   :end-at: </Entities>

**Overriding Parameters**

Parameters defined in the VehicleCatalog can be assigned inside the CatalogReference element.
In this case the assigned parameter will overwrite the definition in the VehicleCatalog.
Otherwise the value defined in the VehicleCatalog is used.
The type of the parameter must match that in the VehicleCatalog.

**Example**

.. code-block:: xml

   <CatalogReference catalogName="ProfilesCatalog.xml" entryName="MiddleClassCarAgent">
       <ParameterAssignments>
           <ParameterAssignment parameterRef="Length" value="4.0" />
       </ParameterAssignments>
   </CatalogReference>

.. admonition:: AgentProfile vs. VehicleModel

   |Op| references AgentProfiles instead of a VehicleModels.
   Such an AgentProfile may have a stochastic distribution of different VehicleModels, each with potentially different parameters.
   After the random selection of the vehicle model, some parameters may not be available and are therefore not assigned (ignored).

.. _scenario_storyboard:

Storyboard
----------

The ``Storyboard`` tag contains the initial setup of the scenario and manipulator actions.
These actions control the behavior of the simulation at runtime based on predefined conditions.

.. _scenario_init:

Init
~~~~

The content of the ``Init`` tag is forwarded to the SpawnPoint and define *if* and *where* it should place the ego and scenario agents.
The position can either be defined as global coordinates (x and y) with the ``World`` tag or as lane coordinates (s and t) with the ``Lane`` tag.

The schema is defined as follows:

.. table::
   :class: tight-table

   ==================== =================== ========================= ==============
   Tag                  Parent              Attributes                Occurrences
   ==================== =================== ========================= ==============
   Actions              Init                *none*                    1
   Private              Actions             entityRef                 1 per entity
   PrivateAction        Private             *none*                    1+
   TeleportAction       PrivateAction       *none*                    1
   Position             TeleportAction      *none*                    1
   LanePosition         Position            roadId, laneId, s, offset 0 or 1
   WorldPosition        Position            x, y, h                   0 or 1
   Orientation          LanePosition        type, h                   0 or 1
   LongitudinalAction   PrivateAction       *none*                    1
   SpeedAction          LongitudinalAction  *none*                    1
   SpeedActionDynamics  SpeedAction         rate, dynamicsShape       1
   SpeedActionTarget    SpeedAction         *none*                    1
   AbsoluteTargetSpeed  SpeedActionTarget   value                     1
   RoutingAction        PrivateAction       *none*                    0 or 1
   AssignRouteAction    RoutingAction       *none*                    1
   Route                AssignRouteAction   *none*                    1
   Waypoint             Route               *none*                    1+
   Position             Waypoint            *none*                    1
   RoadPosition         Position            roadId, t                 1
   VisibilityAction     PrivateAction       traffic                   1
   ==================== =================== ========================= ==============

All listed attributes are required.
The attributes have the following meaning:

.. table::
   :class: tight-table

   ==================== ================== ============================================================================================
   Tag                  Attribute          Description
   ==================== ================== ============================================================================================
   Private              entityRef          name of the entity for which the initial values are described in the sub-tags
   LanePosition         roadId             Id of the road in the Scenery
   LanePosition         laneId             Id of the lane in the Scenery
   LanePosition         s                  start position on the lane (i.e. distance from the start of the road) of the reference point
   LanePosition         offset             lateral distance of the reference point to the middle of the road (i.e. t coordinate)
   WorldPosition        x                  x coordinate of the reference point
   WorldPosition        y                  y coordinate of the reference point
   WorldPosition        h                  heading
   Orientation          type               has to be "relative"
   Orientation          h                  heading angle in radiant relative to the lane
   SpeedActionDynamics  rate               acceleration
   SpeedActionDynamics  dynamicsShape      "linear" for constant acceleration, "step" for immediate transition
   SpeedActionDynamics  dynamicsDimension  has to be "rate"
   AbsoluteTargetSpeed  value              velocity
   RoadPosition         roadId             Id of the road in the Scenery
   RoadPosition         t                  negative for driving in roadDirection (i.e on lanes with negative Id) else positive
   RoadPosition         s                  ignored
   VisibilityAction     traffic            Flag deciding if the scenario agent will be spawned (true = spawned, false = not spawned)
   ==================== ================== ============================================================================================

**Defining a Position**

|Op| only supports ``LanePosition`` and ``WorldPosition`` from the OpenSCENARIO standard.

**Controlling Visibility**

VisibilityAction is an optional attribute, which allows easy parameterization of certain entities, e.g. via :ref:`scenario_parameterdeclaration`.

.. note:: If VisibilityAction **is not defined** the according entity agent **will be spawned**.

**Stochastic Values**

Unlike OpenSCENARIO, |op| allows some of the aforementioned values to be stochastic.
This is achieved by using the sub-tag ``Stochastics`` within the tags ``LanePosition`` or ``SpeedAction``:

.. code-block:: xml

   <Stochastics value="..." stdDeviation="..." lowerBound="..." upperBound="..."/>

The stochastics tag normally defines a normal distribution, but it is up to each component to interpret its values.
Here, value is a selector, which can be either ``s``, ``offset`` (if inside LanePosition), ``velocity`` or ``rate`` (if inside SpeedAction).
Depending on the selected type, the actual mean value is then taken from the attribute of the corresponding tag ``LanePosition``, ``SpeedActionDynamics``, or ``AbsoluteTargetSpeed``.

**Full Example**

.. code-block:: xml

   <Init>
     <Actions>
       <Private entityRef="Ego">
         <PrivateAction>
           <TeleportAction>
             <Position>
               <LanePosition roadId="1" s="20.0" laneId="-2" offset="0.0">
                 <Orientation type="relative" h="0.2"/>
                 <Stochastics value="s" stdDeviation="5.0" lowerBound="10.0"
                                                           upperBound="30.0"/>
               </LanePosition>
             </Position>
           </TeleportAction>
         </PrivateAction>
         <PrivateAction>
           <LongitudinalAction>
             <SpeedAction>
               <SpeedActionDynamics rate="0.0" dynamicsShape="linear"
                                               dynamicsDimension="rate"/>
               <SpeedActionTarget>
                 <AbsoluteTargetSpeed value="10.0" />
               </SpeedActionTarget>
               <Stochastics value="velocity" stdDeviation="2.0" lowerBound="5.0"
                                                                upperBound="15.0"/>
             </SpeedAction>
           </LongitudinalAction>
         </PrivateAction>
         <PrivateAction>
           <RoutingAction>
             <AssignRouteAction>
               <Route>
                 <Waypoint>
                   <Position>
                     <RoadPosition roadId="1" t="-1.0" s="0"/>
                   </Position>
                 </Waypoint>
                 <Waypoint>
                   <Position>
                     <RoadPosition roadId="2" t="-1.0" s="0"/>
                   </Position>
                 </Waypoint>
               </Route>
             </AssignRouteAction>
           </RoutingAction>
         </PrivateAction>
       </Private>
       <Private entityRef="ScenarioAgent">
         <PrivateAction>
           <TeleportAction>
             <Position>
               <LanePosition roadId="1" s="50.0" laneId="-3" offset="0.0"/>
             </Position>
           </TeleportAction>
         </PrivateAction>
         <PrivateAction>
           <LongitudinalAction>
             <SpeedAction>
               <SpeedActionDynamics rate="0.0" dynamicsShape="linear"
                                               dynamicsDimension="rate"/>
               <SpeedActionTarget>
                 <AbsoluteTargetSpeed value="10.0" />
               </SpeedActionTarget>
             </SpeedAction>
           </LongitudinalAction>
         </PrivateAction>
       </Private>
     </Actions>
   </Init>

.. _scenario_story:

Story
~~~~~

The tag ``Story`` is optional and defines all conditional interventions during runtime.
The story consists of multiple acts, which itself can consist of multiple ``ManeuverGroup`` tags.
A maneuver groups must consist of exactly one ``Actors`` tag and one ``Maneuver`` tag.
The ``maximumExecutionCount`` attribute states the maximum number of times that each maneuver will by triggered.
To trigger an maneuver for an unlimited number of times, set the value to ``-1,``.

.. note:: While multiple stories can be added, grouping of maneuver groups into acts currently has no effect in the simulator.

**Example**

.. code-block:: xml

   <Story name="MyStory">
     <Act name="Act1">
       <ManeuverGroup name="MySequence" maximumExecutionCount="-1">
         <Actors>
           ...
         </Actors>
         <Maneuver name="MyManeuver">
           ...
         </Maneuver>
       </ManeuverGroup>
     </Act>
   </Story>

.. _scenario_actors:

Actors
******

The ``Actors`` tag defines the agents that are mainly affected by this sequence.
These agents can either be defined directly by stating their name within a sub-tag ``<EntityRef entityRef="entityName"/>`` or as the agent that triggered the event itself via ``<Actors selectTriggeringEntities="true"/>``.

.. _scenario_maneuver:

Maneuver
********

The ``Maneuver`` tag defines the conditions, when an action shall take place.
It contains one or more events, each consisting of **one** action and **one or more** start conditions.

.. note:: Only a single action is currently supported, although the standard allows several.

.. warning:: Due to internal handling, event names must be unique.

**Example**

.. code-block:: xml

   <Maneuver name="MyManeuver">
     <Event name="MyEvent" priority="overwrite">
       <Action name="MyManipulator">
         ...
       </Action>
       <StartTrigger>
         <ConditionGroup>
           <Condition name="Conditional">
             ...
           </Condition>
         </ConditionGroup>
       </StartTrigger>
     </Event>
   </Maneuver>

.. _scenario_conditions:

Conditions
""""""""""

|Op| support different kind of conditions, belonging either to ``ByValueCondition`` or ``ByEntityCondition``.
While a **by value condition** trigger based on a specified value and is unrelated to any specific agent, a **by entity condition** is bound to an triggering agent, defined by a mandatory section ``TriggeringEntities``.
Entities can be either ego or scenario agents, or all agents if the section is left blank.

.. note::

  OpenSCENARIO specifies that a rule should be applied to the ``TriggeringEntities`` element.
  Currently, only the rule ``any`` is supported, so any of the listed entities may satisfy the condition independent of the others.

.. table::

   ==================================== ==============
   Condition                            Triggered By
   ==================================== ==============
   :ref:`scenario_simulationtime`       Value
   :ref:`scenario_relativelaneposition` Entity
   :ref:`scenario_roadposition`         Entity
   :ref:`scenario_relativelaneposition` Entity
   :ref:`scenario_relativespeed`        Entity
   :ref:`scenario_timetocollision`      Entity
   :ref:`scenario_timeheadway`          Entity
   ==================================== ==============

.. _scenario_simulationtime:

SimulationTime
++++++++++++++

Defines, when the simulation ends.
Triggers at a specified time value, given in seconds.
The rule is required and only ``greaterThan`` is currently supported.

**Example**

.. literalinclude:: @OP_REL_SIM@/contrib/examples/DefaultConfigurations/Scenario.xosc
   :language: xml
   :dedent: 8
   :start-at: <Condition name="EndTime"
   :end-at: </Condition>

.. _scenario_relativelaneposition:

RelativeLanePosition
++++++++++++++++++++

Triggers if an entity is within a relative lane position with respect to the specified object.
Here, *ds* (s-delta) and *dLane* (lane-delta) are applied to a reference *object*, e.g. ``referenceEntity.s + ds``.
The tolerance is then added and subtracted from the resulting s-coordinate to form a triggering zone.
The condition is satisfied, if a TriggeringEntity touches this zone.

**Example**

.. literalinclude:: @OP_REL_SIM@/contrib/examples/Configurations/ByEntityCondition_RelativeLane/Scenario.xosc
   :language: xml
   :dedent: 18
   :start-at: <Condition name="RelativeLanePosition">
   :end-at: </Condition>

.. _scenario_roadposition:

RoadPosition
++++++++++++

This Condition evaluates if a TriggeringEntity touches a given s-coordinate, *s*,  on a specified road, given by *roadId*.
Note that the tolerance is added and subtracted from the s-coordinate to form a triggering zone.
The condition is satisfied, if a TriggeringEntity touches this zone.

**Example**

.. literalinclude:: @OP_REL_SIM@/contrib/examples/Configurations/ByEntityCondition_RoadPosition/Scenario.xosc
   :language: xml
   :dedent: 18
   :start-at: <Condition name="RoadPosition">
   :end-at: </Condition>

.. _scenario_relativespeed:

RelativeSpeed
+++++++++++++

This condition evaluates if the relative velocity (:math:`v_{rel}`) between a TriggeringEntity and a reference entity, is *lessThan*, *equalTo*, or *greaterThan*, a specified value, where :math:`v_{rel} = v_{trig} - v_{ref}`.

**Example**

.. literalinclude:: @OP_REL_SIM@/contrib/examples/Configurations/ByEntityCondition_RelativeSpeed/Scenario.xosc
   :language: xml
   :dedent: 18
   :start-at: <Condition name="RelativeSpeed">
   :end-at: </Condition>

.. _scenario_timetocollision:

TimeToCollision
+++++++++++++++

This condition evaluates if the *Time To Collision* (TTC) between a TriggeringEntity and a reference entity is *lessThan*, *equalTo*, or *greaterThan* a specified value.
The TTC is determined by projecting the movement of the entities using steps of 0.1 seconds and returning the first time step, at which their bounding boxes intersect.

**Example**

.. literalinclude:: @OP_REL_SIM@/contrib/examples/Configurations/ByEntityCondition_TimeToCollision/Scenario.xosc
   :language: xml
   :dedent: 18
   :start-at: <Condition name="TimeToCollision">
   :end-at: </Condition>

.. _scenario_timeheadway:

TimeHeadway
+++++++++++

This condition evaluates if the *Time Headway* (THW) between a TriggeringEntity and a reference entity is *lessThan*, *equalTo*, or *greaterThan* a specified value.
If ``freespace="true"`` the net distance of the bounding boxes is considered, otherwise the distance of the reference points.
The ``alongRoute`` attribute must be ``true``.

**Example**

.. literalinclude:: @OP_REL_SIM@/contrib/examples/Configurations/ByEntityCondition_TimeHeadway/Scenario.xosc
   :language: xml
   :dedent: 18
   :start-at: <Condition name="TimeHeadway">
   :end-at: </Condition>

.. _scenario_actions:

Actions
"""""""

Actions define what happens if all conditions of the maneuver are fulfilled and are applied on the actors defined for this maneuver.

The following openSCENARIO actions are supported:

- :ref:`scenario_lanechange`
- :ref:`scenario_followtrajectory`
- :ref:`scenario_acquireposition`
- :ref:`scenario_removeagent`
- :ref:`scenario_speedaction`

In addition, the following user defined actions are interpreted:

- :ref:`scenario_componentstatechange`
- :ref:`scenario_defaultCustomCommandAction`


.. todo:: Write a paragraph about the concept "Trajectories, OSC Actions, and so" in the developer zone.

.. _scenario_lanechange:

LaneChange
++++++++++

If this action trigger, the actor is forced to perform a trajectory, calculated at the time of triggering.
The target lane be given either absolute or with respect to another entity.
The trajectory can either have a fixed length (in s) or a fixed time.

Currently, "sinusoidal" is the only supported type.

**Example** *with absolute target and fixed length*

.. literalinclude:: @OP_REL_SIM@/contrib/examples/Configurations/OSCAction_SinusoidalLaneChangeLeft_Absolute/Scenario.xosc
   :language: xml
   :dedent: 14
   :start-at: <Action name="LaneChange">
   :end-at: </Action>

**Example** *with relative target and fixed time*

.. literalinclude:: @OP_REL_SIM@/contrib/examples/Configurations/OSCAction_SinusoidalLaneChangeLeft_Relative/Scenario.xosc
   :language: xml
   :dedent: 14
   :start-at: <Action name="LaneChange">
   :end-at: </Action>

.. _scenario_followtrajectory:

Follow Trajectory
+++++++++++++++++

If triggered, the defined trajectory is relayed as signal to listening components.
The trajectory can be defined either directly in the story or as separate TrajectoryCatalog.
For the points (vertices) of the trajectory only world coordinates, given as ``openScenario::WorldPosition``, are supported.
Right now, the mandatory attributes  *z*, *pitch* and *roll* are ignored.

**Example**

.. literalinclude:: @OP_REL_SIM@/contrib/examples/Configurations/Pedestrian_Trajectory/Scenario.xosc
   :language: xml
   :dedent: 14
   :start-at: <Action name="Trajectory">
   :end-at: </Action>

**Example** *using a TrajectoryCatalog*

.. code-block:: xml

   <Action name="FollowTrajectory">
     <Private>
       <Routing>
         <FollowTrajectoryAction>
           <CatalogReference catalogName="TrajectoryCatalog.xosc" entryName="TrajectoryA">
         </FollowTrajectoryAction>
       </Routing>
      </Private>
   </Action>


.. _scenario_acquireposition:

Acquire Position
++++++++++++++++

If triggered, the defined position is relayed as signal to listening components.
Currently only "WorldPosition" and "RelativeObjectPosition" are supported.

.. warning: Currently, this is only the supported by the component *FMU_Wrapper* and for OSMP messages.

**Example** *(WorldPosition)*

.. code-block:: xml

   <Action name="AcquirePosition">
     <PrivateAction>
       <RoutingAction>
         <AcquirePositionAction>
           <Position>
             <WorldPosition x="1.0" y="2.0" z="0.0" h="3.1415" p="0.0" r="0.0" />
           </Position>
         </AcquirePositionAction>
       </RoutingAction>
     </PrivateAction>
   </Action>

**Example** *(RelativeObjectPosition)*

.. code-block:: xml

    <Action name="AcquirePosition">
      <PrivateAction>
        <RoutingAction>
          <AcquirePositionAction>
            <Position>
              <RelativeObjectPosition dx="1.0" dy="1.0" entityRef="S1"/>
            </Position>
          </AcquirePositionAction>
        </RoutingAction>
      </PrivateAction>
    </Action>

.. _scenario_removeagent:

Remove Agent
++++++++++++

The RemoveAgentsManipulator removes the specified entity at the time of triggering.

**Example**

.. literalinclude:: @OP_REL_SIM@/contrib/examples/Configurations/OSCAction_RemoveAgent/Scenario.xosc
   :language: xml
   :dedent: 14
   :start-at: <Action name="RemoveAgents">
   :end-at: </Action>

.. _scenario_speedaction:

SpeedAction
+++++++++++

The SpeedAction adjusts the velocity of an entity based on parameters of the SpeedAction.
Both variants, "RelativeTargetSpeed" and "AbsoluteTargetSpeed", are supported, but with restrictions:

- The SpeedActionDynamics attribute "dynamicsShape" only supports "linear" and "step".
- The SpeedActionDynamics attribute "dynamicsDimension" only supports "rate".
- The RelativeTargetSpeed attribute "continuous" is ignored.

.. note::

   Values defined in the SpeedAction might not reflect actual values used by the simulator due to physical boundaries of the simulator.

**Example** *AbsoulteTargetSpeed*

.. code-block:: xml

   <Action name="SpeedAction">
     <PrivateAction>
       <LongitudinalAction>
         <SpeedAction>
           <SpeedActionDynamics dynamicsShape="step" value="0.0" dynamicsDimension="rate"/>
           <SpeedActionTarget>
             <AbsoluteTargetSpeed value="20"/>
           </SpeedActionTarget>
         </SpeedAction>
       </LongitudinalAction>
     </PrivateAction>
   </Action>

**Example** *RelativeTargetSpeed*

.. code-block:: xml

   <Action name="SpeedAction">
     <PrivateAction>
       <LongitudinalAction>
         <SpeedAction>
           <SpeedActionDynamics dynamicsShape="step" value="0.0" dynamicsDimension="rate"/>
           <SpeedActionTarget>
             <RelativeTargetSpeed entityRef="ScenarioAgent" value="10" 
                                  speedTargetValueType="delta" continuous="false"/>
           </SpeedActionTarget>
           </SpeedAction>
       </LongitudinalAction>
     </PrivateAction>
   </Action>

.. _scenario_defaultCustomCommandAction:

UsedDefinedAction: DefaultCustomCommandAction
+++++++++++++++++++++++++++++++++++++++++++++

.. todo:: Write a paragraph in the developer zone about the concept "wrapping FMUs" and the "OSMP FMU"

This is **the** default custom command, and is always selected, if **the first WORD** (Keyword) in the command string is **not** registered as a special custom command.

The ``DefaultCustomCommandAction`` adds an event to the ``EventNetwork``, which is relayed as a string (``StringSignal``) by the component``OSCActions``.

.. admonition:: FMU_Wrapper component for OSMP messages only

   The linked string signal is set as custom action of the TrafficCommand.

**Example**

.. code-block:: xml

  <Action name="CustomParameters">
      <UserDefinedAction>
          <CustomCommandAction>Arbitrary String, e.g. { "hello": "world"}</CustomCommandAction>
      </UserDefinedAction>
  </Action>

.. note::

   Here, "Arbitrary" is first checked, if it matches any other available Keyword for used defined actions, such as :ref:`scenario_componentstatechange`.

.. _scenario_componentstatechange:

UserDefinedAction: Component State Change
+++++++++++++++++++++++++++++++++++++++++

Command Keyword: SetComponentState

.. todo:: Write a paragraph in the developer zone about the concept "Component State Change"

The ComponentStateChangeManipulator influences the maximum reachable state of an agent component that is handled by the ComponentController, i.e. it can either activate or deactivate a component.

**Example**

.. code-block:: xml

   <Action name="ComponentStateChange">
       <UserDefinedAction>
           <CustomCommandAction>SetComponentState Dynamics_TrajectoryFollower Acting</CustomCommandAction>
       </UserDefinedAction>
   </Action>

.. _scenario_stoptrigger:

StopTrigger
~~~~~~~~~~~

Here, end conditions for the simulation are defined.

Right now, only SimulationTime is supported, given in seconds, with the fixed ``rule="greaterThan"``.

**Example**

.. literalinclude:: @OP_REL_SIM@/contrib/examples/DefaultConfigurations/Scenario.xosc
   :language: xml
   :dedent: 2
   :start-at: <StopTrigger>
   :end-at: </StopTrigger>
