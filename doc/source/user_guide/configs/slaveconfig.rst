..
  ************************************************************
  Copyright (c) 2021 in-tech GmbH

  This program and the accompanying materials are made
  available under the terms of the Eclipse Public License 2.0
  which is available at https://www.eclipse.org/legal/epl-2.0/

  SPDX-License-Identifier: EPL-2.0
  ************************************************************

.. _slaveconfig:

SlaveConfig
===========

.. todo: Write a nicer introduction for the slaveconfig, similar to "Overview" section in scenery

This file describes the user configurable parameters of an experiment. 
Several parameters depend on probabilities. 
Each invocation then rolls for said probabilities. 
All probabilities need to add up to 1.0.

The slaveConfig.xml consists of the following sections:

* :ref:`slaveconfig_profilescatalog`
* :ref:`slaveconfig_experiment`
* :ref:`slaveconfig_scenario`
* :ref:`slaveconfig_environment`
* :ref:`slaveconfig_observations`
* :ref:`slaveconfig_spawners`

.. _slaveconfig_profilescatalog:

ProfilesCatalog
---------------

Specifies the :ref:`profilescatalog` for the experiment.

.. literalinclude:: @OP_REL_SIM@/contrib/examples/Common/slaveConfig.xml
   :language: xml
   :start-at: <ProfilesCatalog> 
   :end-at: </ProfilesCatalog>

.. _slaveconfig_experiment:

Experiment
----------

Specifies the general experiment setup, not specific to a single invocation.

.. table::
   :class: tight-table

   ===================== ================================================== =========
   Tag                   Description                                        Mandatory 
   ===================== ================================================== =========
   ExperimentId          Id of the experiment                               yes
   NumberOfInvocations   Number of invocation in the experiment.                                   
                         For each invocation probabilities are rerolled.    yes
   RandomSeed            Random seed for the entire experiment.                                   
                         Must be within the bounds of an unsigned integer.  yes
   Libraries             Name of the core module Libraries to use.                                  
                         If not specified the default name is assumed.      yes
   ===================== ================================================== =========

.. literalinclude:: @OP_REL_SIM@/contrib/examples/Common/slaveConfig.xml
   :language: xml
   :start-at: <Experiment> 
   :end-at: </Experiment>

.. _slaveconfig_scenario:

Scenario
--------

This section contains information about the scenario setup for the experiment. This information does not change between invocations.

.. table::
   :class: tight-table

   ================ ========================= =========     
   Tag              Description               Mandatory
   ================ ========================= =========
   OpenScenarioFile Name of the scenario file yes
   ================ ========================= =========

**Example**

This experiment uses the "HighwayScenario.xosc" scenario file.

.. literalinclude:: @OP_REL_SIM@/contrib/examples/Common/slaveConfig.xml
   :language: xml
   :start-at: <Scenario> 
   :end-at: </Scenario>

.. _slaveconfig_environment:

Environment
-----------

This section contains information about the world and the general environment inside the simulation. Every invocation re-rolls the environment probabilities.
All probabilities need to add up to 1.0.

.. table::
   :class: tight-table

   =================== ============================================================================================== =========
   Tag                 Description                                                                                    Mandatory
   =================== ============================================================================================== =========
   TimeOfDay           **Currently unused.** Time of day ranging from 1-24 [h].                                       1+ entry
   VisibilityDistance  Defines how far a human driver can see [m].                                                    1+ entry
   Friction            Friction on the road. Used by DynamicsRegularDriving and LimiterAccelerationVehicleComponents. 1+ entry
   Weather             **Currently unused.** Weather as string                                                        1+ entry
   =================== ============================================================================================== =========

**Example**

Every invocation has the time set to 15:00.
In 70% of all invocation drivers can see 125 meter and for the other 30% of invocations the drivers can see 250 meter.
Every invocation has a friction of 0.3.
Every invocation has sunny weather.

.. literalinclude:: @OP_REL_SIM@/contrib/examples/Common/slaveConfig.xml
   :language: xml
   :start-at: <Environment> 
   :end-at: </Environment>

.. _slaveconfig_observations:

Observations
------------

In this section all observation libraries are defined with their parameters.
A specific library is loaded by adding an entry to the `Observations` tag:

.. code-block:: xml

   <Observations>
     <!-- first observer -->
     <Observation> 
       <Library>THE_OBSERVATION_LIBRARY</Library>
       <!-- observer specific parameter -->
       <Parameters>
         <String Key="THE_KEY" Value="THE_VALUE"/>
         <Bool Key="ANOTHER_KEY" Value="false"/>
         ...
       </Parameters>
     </Observation>
     <!-- second observer -->
     <Observation> 
        ...
     </Observation>
   </Observations>

Here, the ``Library`` tag contains the name of the library, and ``Parameters`` contain an optional list of key/value pairs, specific for each observer.

Please refer to the documentation of the individual observers for available parameters:

.. toctree::
   :glob:
   :maxdepth: 1

   ../outputs/*

.. _slaveconfig_spawners:

Spawners
--------

In this section the spawners are defined with their Profile (defined in the ProfilesCatalog). 
The same library can be loaded multiple times with different profiles.
A spawner is either of type "PreRun", meaning it is triggered only once at the start of the simulation, or "Runtime", meaning it is triggered in every timestep.
If different spawners are to be triggered at the same time the spawner with the highest priority is triggered first.

.. literalinclude:: @OP_REL_SIM@/contrib/examples/Common/slaveConfig.xml
   :language: xml
   :start-at: <Spawners> 
   :end-at: </Spawners>


Full Example
------------

.. literalinclude:: @OP_REL_SIM@/contrib/examples/Common/slaveConfig.xml
   :language: xml
   :caption: slaveConfig.xodr
   :linenos:
