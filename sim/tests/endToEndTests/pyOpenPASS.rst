..
  *******************************************************************************
  Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)

  This program and the accompanying materials are made available under the
  terms of the Eclipse Public License 2.0 which is available at
  http://www.eclipse.org/legal/epl-2.0.

  SPDX-License-Identifier: EPL-2.0
  *******************************************************************************

This tool acts as configurable executor for complete sets of configs for the openPASS simulation.
The test framework is located at ``sim/tests/endToEndTests/pyOpenPASS``.

Prerequisites
-------------

Please refer to :ref:`binary_packages` for instructions on installing the prerequisites.

Execution
---------

.. code:: bash

    main.py
      -s SIMULATION                         # path to simulation executable, e.g. /openPASS/bin
      -m MUTUAL_RESOURCES_PATH              # path to mutual config files for all runs, e.g. /examples/common
      -r RESOURCES_PATH                     # path from where configs are retrieved (override common files if necessary)
      -c PYOPENPASS_CONFIG.JSON             # JSON config for pyOpenPASS
      --scope SCOPE_IN_PYOPENPASS_CONFIG    # A user defined scope in the config (such as fast, nightly, ...)
      -d                                    # Debug some pandas dataframes as csv to /tmp or C:\temp

Configuration
-------------

-  Basic tests: Check executability of configs + Determinism test (*1 x n* vs *n x 1* tests).
-  Specialized tests: Execute config and check for specific results in the output of the simulator.

Basic Tests
~~~~~~~~~~~

.. code:: json

    {
        "scopes":
        {
            "THE_SCOPE":
            {
                "baseConfig":
                {
                    "duration": 60,
                    "randomSeed": 0,
                    "invocations": 100
                },
                "configurations":
                [
                  "CONFIG_UNDER_TEST1",
                  "CONFIG_UNDER_TEST2"
                ]
            }
        }
    }

.. note::

    RandomSeed is an optional value.
    If set, it will override the value given with the configuration.

Parameterized Tests
~~~~~~~~~~~~~~~~~~~

.. code:: json

    {
        "scopes":
        {
            "THE_SCOPE":
            {
                "baseConfig":
                {
                    "duration": 60,
                    "randomSeed": 0,
                    "invocations": 100
                },
                "parameterization":
                {
                  "file": "someConfigFile.xml",
                  "xpath": "//some/value",
                  "values": ["value1", "value2", "value3"]
                },
                "configurations":
                [
                  "CONFIG_UNDER_TEST1",
                  "CONFIG_UNDER_TEST2"
                ]
            }
        }
    }

If parameterization specified, the generic tests are executed for each config and each parameter value.
The value is injected into a single configuration file, specified via its filename and an valid xpath.
As values might be contain paths to resources, the variable ``${configFolder}`` can be used as dynamic placeholder.

**Example**

With the core being at ``/OpenPASS/bin`` the scope ``FMU_Test`` shall be executed using two different FMUs:

::

    "values": ["${configFolder}/resources/FMUv1.fmu","${configFolder}/resources/FMUv2.fmu" ]

For each parameter, a different config folder ``configs/FMU_Test_#`` is created, where ``#`` is the parameters index.
Before execution, the value of the first parameter ``${configFolder}/resources/FMUv1.fmu`` is replaced with the absolute path ``/OpenPASS/bin/configs/FMU_Test_0/resources/FMUv1``.

Specialized Tests
~~~~~~~~~~~~~~~~~

.. code:: json

    {
        "scopes":
        {
            "THE_SCOPE":
            {
                "baseConfig":
                {
                    "duration": 30,
                    "randomSeed": 0,
                    "invocations": 100
                },
                "specialized":
                {
                    "description": "A CONCISE DESCRIPTION OF THE SCOPE",
                    "query": "A VALID QUERY STRING (SEE BELOW)",
                    "success_rate": 0.9
                },
                "configurations":
                [
                  "CONFIG_UNDER_TEST1",
                  "CONFIG_UNDER_TEST2"
                ]
            }
        }
    }

Note that ``description`` is an optional value, which is currently not used by the framework, but helps to remember, what the test is about.
It is strongly advised to set it, as it might be used in future releases.


Querying Results
----------------

Specialized Tests allow to define a query for checking the results of a simulation.

Basic Syntax
~~~~~~~~~~~~

::

    [aggregate]([column] | [filter]) [operator] [value]

-  Aggregate:
   Everything pandas supports on dataframes, such as `pandas.DataFrame.count <https://pandas.pydata.org/pandas-docs/stable/reference/api/pandas.DataFrame.count.html?highlight=count#pandas.DataFrame.count>`__, min, max, mean
-  Column:
   A column on which the aggregate should operate.
   Columns are given by the simulation outputs cyclic columns, such as ``PositionRoute``.
   Additionally ``AgentId`` is made available.
-  Filter:
   A filter based on `pandas.DataFrame.filter <https://pandas.pydata.org/pandas-docs/stable/reference/api/pandas.DataFrame.filter.html?highlight=filter#pandas.DataFrame.filter>`__ syntax using the available columns.
-  Operator:
   A comparison operator from the following list: ==, <=, >=, <, >, !=, ~= (approximate).
   The approximate operator allows ``1*e-6 x value`` as maximum deviation from value.
-  Value:
   A number

**Example**

::

    count(AgentId | PositionRoute >= 800 and Lane != -3) == 0

Using Events in Filter
~~~~~~~~~~~~~~~~~~~~~~

In order to query for a specific event, use ``#(EVENT)`` within the filter syntax.

**Example**

::

    count(AgentId | PositionRoute >= 800 and #(Collision) == True) == 0

Event Payload
^^^^^^^^^^^^^

Each event is associated with a set of triggering entity ids, affected entity ids, and arbitrary key/value pairs (please refer to the openPASS documentation for details).
This information is transformed into a "per agent" scope.

In the following the ``Collision`` event is taken as example.

**TriggeringEntity**

All agents, flagged as triggering become ``IsTriggering``

Query: ``#(Collision):IsTriggering == True``

**AffectedEntity**

All agents, flagged as affected become ``IsAffected``

Query: ``#(Collision):IsAffected == True``

**Key/Value Pairs**

If an event publishes additional payload with the key ``XYZ``, it will can be queried by ``#(EVENT):XYZ``.

Query: ``#(Collision):WithAgent``

.. warning::

    Keys carrying the event name as prefix, such as in ``#(Collision):CollisionWithAgent``, will be stripped to ``Collision:WithAgent``

Query Example
^^^^^^^^^^^^^

| *No agent should collide with agent 0:*
| ``count(AgentId | AgentId == 0 and #(Collision):WithAgent == 1) == 0``


Using openSCENARIO Events
~~~~~~~~~~~~~~~~~~~~~~~~~

OpenScenario events are processed in the same manner as regular events (see above).

This allows to query for occurrences of openSCENARIO events with a name specified within the following xpath:
``OpenSCENARIO/Story/Act/Sequence/Maneuver/Event/@name``

**openSCENARIO Event Definition**

.. code:: xml

    <Story name="TheStory">
      <Act name="TheAct">
        <Sequence name="TheSequence" numberOfExecutions="1">
          ...
          <Maneuver name="TheManeuver">
            ...
            <!-- example name "ttc_event"-->
            <Event name="ttc_event" priority="overwrite">
            ...
              <StartConditions>
                <ConditionGroup>
                  <Condition name="Conditional">
                    <ByEntity>
                      ...
                      <EntityCondition>
                         <TimeToCollision>
                           ...
                         </TimeToCollision>
                      </EntityCondition>
                    </ByEntity>
                  </Condition>
                </ConditionGroup>
              </StartConditions>
            </Event>
            ...
          </Maneuver>
        </Sequence>
      </Act>
    </Story>

**Example openPASS Output**

.. code:: xml

    <Event Time="0" Source="OpenSCENARIO" Name="TheStory/TheAct/TheSequence/TheManeuver/ttc_event">
        <TriggeringEntities/>
        <AffectedEntities>
            <Entity Id="1"/>
        </AffectedEntities>
        <Parameters/>
    </Event>

**Query**

``count(AgentId | #(TheStory/TheAct/TheSequence/TheManeuver/ttc_event) == True ) > 0``


Querying Transitions
~~~~~~~~~~~~~~~~~~~~

Sometimes it is necessary to check, whether a transition happened, such as counting agents, passing a certain position.

This can be achieved by shifting individual columns by ``N`` time steps.

**Time Shift Syntax**

``Column-Shift`` =>  ``PositionRoute-1`` means PositionRoute at one time step earlier

**Example Use Case**

Counting agents passing ``PositionRoute == 350`` on ``LaneId == -1``

**Query**

``count(AgentId | LaneId == -1 and PositionRoute-1 < 350 and PositionRoute >= 350 ) > 0``

.. warning::

    In seldom cases,  a result column happens to have a name like ``Name-N`` where ``N`` is an integer.
    Querying this column would automatically apply time shifting (default behavior) leading to a parsing error.
    In such cases, escape the column name with single quotes (e.g. ``'Name-1'``).

Explicit Datatypes
~~~~~~~~~~~~~~~~~~

pyOpenPASS uses Pandas DataFrames internally.
Pandas will try to detect the datatype of the individual cyclic columns automatically.
This won't fit the user's intention in some cases, such as when the column holds a semicolon separated list of integers but every list contains just one element.
In such cases it is impossible to distinguish between integers and strings based on the data.

For this reason, datatypes can be specified explicitly in the JSON config file:

.. code:: js

    {
        "datatypes": {
            "Sensor0_DetectedAgents": "str" // string with "missing value" support
        },
        "scopes":
        {
          ...

Test Report Generator
---------------------

Generates an HTML report for results from the pyOpenPASS EndToEndTest Framework tool.

**Command**

.. code:: bash

    report.py
        -r RESOURCE1, RESOURCE2, ...          # One or more resource folders
        -c PYOPENPASS_CONFIG.JSON             # JSON config for pyOpenPASS
        --scope SCOPE1, SCOPE2, ...           # One or more scopes to be considered
        --results SIMULATOR_RESULT_PATH       # Path with results from simulator (should be the same for all scopes)
        --output PATH_FOR_REPORT              # Path where the report should be written to
