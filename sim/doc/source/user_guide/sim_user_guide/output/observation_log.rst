..
  *******************************************************************************
  Copyright (c) 2021 in-tech GmbH

  This program and the accompanying materials are made available under the
  terms of the Eclipse Public License 2.0 which is available at
  http://www.eclipse.org/legal/epl-2.0.

  SPDX-License-Identifier: EPL-2.0
  *******************************************************************************

.. _observation_log:

Observation_Log
###############

This section describes the parameters and outputs of the ``Observation_Log`` observer.

.. _observationlog_paramerization:

Paramerization
==============

Following parameters are supported:

.. table::
   :class: tight-table

   =================== ============= ==========================================================================
   Parameter           Type          Description
   =================== ============= ==========================================================================
   OutputFilename      String        Name of the output file (normally ``simulationOutput.xml``)
   LoggingCyclicsToCsv Bool          If ``true``, cyclics are written an additional file (one CSV file per run)
   LoggingGroup_<NAME> StringVector  Defines which columns belong to the logging group named NAME
   LoggingGroups       StringVector  Defines active logging groups
   =================== ============= ==========================================================================

The columns, defined by the ``LoggingGroup_<NAME>`` correspond to cyclic data entries written by the core and other components.
At time or writing, the core publishes the following cyclics (see source of ``AgentNetwork.cpp``):

- XPosition
- YPosition
- VelocityEgo
- AccelerationEgo
- YawAngle
- YawRate
- SteeringAngle
- TotalDistanceTraveled
- PositionRoute
- TCoordinate
- Lane
- Road
- SecondaryLanes
- AgentInFront

Please refer to the individual components, for information about their published cyclics.

.. todo::

   The concept Cyclics and the DataBuffer needs further explanation.
   We also need a way to better communicate, who is publishing what.
   This should directly come out of the source code, to keep the documentation up to date.

.. admonition:: **Wildcards in LoggingGroup definitions**

   | It is possible to use the wildcard character ``*`` in a ``LoggingGroup_<NAME>`` entry.
   | The wildcard can be used only once per column reference.

**Example**

.. literalinclude:: @OP_REL_SIM@/contrib/examples/DefaultConfigurations/slaveConfig.xml
   :language: xml
   :start-at: <Library>Observation_Log</Library>
   :end-at: </Parameters>

Output Files
============

.. _observationlog_simout:

SimulationOutput
~~~~~~~~~~~~~~~~

Every successful run (single experiment) generates a single file, normally called ``simulationOutput.xml`` (see :ref:'observationlog_paramerization').
The output can contain multiple invocations of the same configuration with different random seeds.
For each invocation a RunResult is stored, which contains information about the agents and their parameters.
As well as run specific events and parameters.

.. code-block:: xml

   <SimulationOutput>
       <RunResults>
           <RunResult RunId="0">
               ...
           </RunResult>
           <RunResult RunId="1">
               ...
           </RunResult>
       </RunResults>
   </SimulationOutput>

The RunResult consist out of the following parts:

- :ref:`observationlog_runstatistics`
- :ref:`observationlog_events`
- :ref:`observationlog_agents`
- :ref:`observationlog_cyclics`

.. _observationlog_runstatistics:

RunStatistics
-------------

This section contains the RandomSeed and general statistics of the invocation.

.. table::
   :class: tight-table

   ======================= ==================================================================
   Tag                     Description
   ======================= ==================================================================
   RandomSeed              Random seed used for this invocation
   VisibilityDistance      Visibility distance of the world
   StopReason              Displays the reason why the simulation stopped.
                           Currently only due to time out.
   StopTime                Currently not used and set to -1
   EgoAccident             Flag which shows whether the ego agent was involved in an accident
   TotalDistanceTraveled   Total traveled distance of all agents
   EgoDistanceTraveled     Total traveled distance of ego vehicle only
   ======================= ==================================================================

.. _observationlog_events:

Events
------

This section contains all events that occurred during the invocation.
Event can either be triggered by an EventDetector, Manipulator or by certain vehicle components.
They are used to track special behavior in the simulation.

.. table::
   :class: tight-table

   ====================== =====================================================================================
   Attribute              Description
   ====================== =====================================================================================
   Time                   Time in ms when the event occurred.
   Source                 Name of the component which created the event.
                          ``OpenSCENARIO``, if triggered by an OpenSCENARIO condition.
   Name                   In case of an OpenSCENARIO event, a path expression Story/Act/Sequence/Maneuver/Event
                          using the names of the corresponding tags as described in the OpenSCENARIO file.
                          Otherwise determined by the triggering component.
   TriggeringEntities     List of entity IDs triggering this event.
   AffectedEntities       List of entity IDs affected by this event.
   Parameters             List of generic key/value string pairs.
   ====================== =====================================================================================

.. _observationlog_agents:

Agents
------

This section contains some information on how each agent is configured.

.. table::
   :class: tight-table

   ====================== ==============================================================
   Attribute              Description
   ====================== ==============================================================
   Id                     Identification number
   AgentTypeGroupName     The agent category. This can either be Ego, Scenario or Common
   AgentTypeName          Name of the agent profile
   VehicleModelType       Name of the vehicle model
   DriverProfileName      Name of the driver profile
   ====================== ==============================================================

.. code-block:: xml

   <Agent Id="0"
          AgentTypeGroupName="Ego"
          AgentTypeName="MiddleClassCarAgent"
          VehicleModelType="car_bmw_7"
          DriverProfileName="Regular">

The VehicleAttributes tag lists basic information of the vehicle parameters.

.. table::
   :class: tight-table

   ======================= =============================================================================
   Attribute               Description
   ======================= =============================================================================
   Width                   Width of the vehicles bounding box
   Length                  Length of the vehicles bounding box
   Height                  Height of the vehicles bounding box
   LongitudinalPivotOffset Distance between center of the bounding box and reference point of the agent.
                           Positive distances are closer to the front of the vehicle.
                           Negative distances are closer to the rear of the vehicle.
   ======================= =============================================================================

The Sensors tag lists all sensors of the agent and their parameters.

.. table::
   :class: tight-table

   ======================= =====================================================================
   Attribute               Description
   ======================= =====================================================================
   Name                    Name of the component
   Model                   Type of the sensor
   MountingPosLongitudinal Relative longitudinal position of the sensor in relation to the agent
   MountingPosLateral      Relative lateral position of the sensor in relation to the agent
   MountingPosHeight       Relative height of the sensor in relation to the agent
   OrientationPitch        Pitch rotation of the sensor in relation to the agent
   OrientationYaw          Yaw rotation of the sensor in relation to the agent
   OpeningAngleH           Horizontal opening angle of the sensor
   OpeningAngleV           Vertical opening angle of the sensor
   DetectionRange          Range how far the sensor reaches in m
   ======================= =====================================================================

.. note::

   Calculation of visual obstruction is currently supported only by the Geometric2D sensor.
   See :ref:`systemconfigblueprint` for configuration.

.. code-block:: xml

   <Sensor Name="Geometric2DFront"
           Model="Geometric2D"
           MountingPosLongitudinal="0"
           MountingPosLateral="0"
           MountingPosHeight="0.5"
           OrientationPitch="0"
           OrientationYaw="0"
           OpeningAngleH="20"
           OpeningAngleV="-999"
           DetectionRange="300" />

.. _observationlog_cyclics:

Cyclics
-------

If the parameter ``LoggingCyclicsToCsv`` is set to ``false``, this section contains all logged parameters of the agents per time step.
The tag ``header`` defines the layout of all samples.
Each entry of the header consists of the agent id and the name of the logged value, in the form ``ID:SAMPLE_NAME``.
A sample contains all information for one specific time step.
If an agent does not exist at the current time step, the value is ' '.

**Example**

In this example exist two agents with the ids 0 and 1.
Two time steps were being tracked, one at 0 seconds and one at 100 ms.
Agent 0 has a constant velocity of 30 m/s and starts at the position X: 100 and Y: 50.
Agent 1 has a initial velocity of 40 m/s and starts at the position X: 200 and Y: 50.

.. code-block:: xml

   <Cyclics>
       <Header>00:VelocityEgo, 00:XPosition, 00:YPosition, 00:YawAngle, 01:VelocityEgo, 01:XPosition, 01:YPosition, 01:YawAngle</Header>
       <Samples>
           <Sample Time="0">30, 100, 50, 0, 40, 200, 50, 0</Sample>
           <Sample Time="100">30, 103, 50, 0, 40, 204, 50, 0</Sample>
       </Samples>
   </Cyclics>

If the parameter ``LoggingCyclicsToCsv`` is set to ``true``, this section only contains a reference to the corresponding CSV file, e.g.

.. code-block:: xml

   <Cyclics>
       <CyclicsFile>Cyclics_Run_000.csv</CyclicsFile>
   </Cyclics>

For each invocation, the file number of the filename is incremented, starting with ``Cyclics_Run_000.csv``.

Cyclics_Run_###
~~~~~~~~~~~~~~~

This file contains the samples as described in :ref:'observationlog_cyclics', but with the time step as leading column.

**Example**

======== ============== ============ ============ =========== ============== ============ ============ ===========
Timestep 00:VelocityEgo 00:XPosition 00:YPosition 00:YawAngle 01:VelocityEgo 01:XPosition 01:YPosition 01:YawAngle
======== ============== ============ ============ =========== ============== ============ ============ ===========
0        30             100          50           0           40             200          50           0
100      30             103          50           0           40             204          50           0
======== ============== ============ ============ =========== ============== ============ ============ ===========
