..
  *******************************************************************************
  Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
                2021 in-tech GmbH

  This program and the accompanying materials are made available under the
  terms of the Eclipse Public License 2.0 which is available at
  http://www.eclipse.org/legal/epl-2.0.

  SPDX-License-Identifier: EPL-2.0
  *******************************************************************************

Scenario-based simulation
=========================

This guide describes how to execute an exemplary simulation with openPASS.

The simulation runs a scenario which is set up for triggering an AEB system. 
The scenario contains two agents: The ego agent and a scenario agent. 
The ego vehicle is equipped with an AEB system (the system under test). 
The scenario agent performs a cut-in and triggers as a result the AEB system of the ego agent. 
To force this exact maneuver, the scenario agent is set up to follow a predefined trajectory.

The GUI is not yet capable of configuring and executing a scenario based simulation as this is currently work in progress. 
Thus, an existing simulation configuration is used and the simulation is started manually.

**Step-by-step instructions**

#. Navigate into |op| install directory (e.g. ``C:\OpenPASS``)

#. Install directory should look like the following

    ::

        OpenPASS-Install-Dir
        ├── bin
        └── configs <-- this directory will be created in the next step
        └── examples
        │   ├── Configurations
        │   └── DefaultConfigurations
        └── doc
        └── examples
        └── lib 
        │   ...
        └── opSimulation.exe

#. Create a new folder named "configs" for the configuration files within the |op| install directory 

#. Copy configuration files

    Simulations for |op| are fully configured through configuration files.
    Examples for |op| configuration files are located in the directory ``examples`` and are divided into two groups: *Default configurations* and *specific configurations*.

    a. The *default configuration* files located under ``examples/DefaultConfigurations`` contain the basis for any simulation. 

        i. Copy all files from the directory ``examples/DefaultConfigurations`` to the previously created folder ``configs``.

    b. The *specific configuration* files located under ``examples/Configurations`` complement the default configuration files. Examples are provided for different simulations.

        i. Ensure all files from directory ``examples/DefaultConfigurations`` have been copied to ``configs`` (this has been done in the previous step). Repeat this step every time another simulation is chosen. 
                
        ii. Navigate to the directory ``examples/Configurations``. Here, all folders contain special pre-configured simulations (which either can be used as demo cases or as end-to-end test cases primarily used for development). 

        iii. Copy all files from the directory specifying the specific scenario you want to simulate (in our case all files from directory ``examples/Configurations/AEB``) to the directory ``configs``. Overwrite existing files when prompted. 

    .. note::

	A detailed description of the various configuration files can be found under see :ref:`configs_in_depth`.

#. Learn how to modify configuration files 

    If one wants to edit the configurations (e.g. experiment set-up or parameters of specific scenario) placed under ``configs``, one can do so by changing any of the following files:
    
    ::

        configs
        ├── PedestrianModelsCatalog.xosc
        ├── ProfilesCatalog.xml
        ├── Scenario.xosc
        ├── SceneryConfiguration.xodr
        ├── simulationConfig.xml
        ├── systemConfigBlueprint.xml
        └── VehicleModelsCatalog.xosc


   As an example, modifications of the configuration files that describe the AEB scenario are given.
   The following list describes some relevant adjustments that may be useful.

    a. ``ProfilesCatalog.xml``:
    
        * The AEB system and sensor parameters, e.g. „TTC“, „DetectionRange“, can be adjusted.
        * The spawner parameters, such as the traffic volume or the velocity of the surrounding traffic, can be modified in the existing spawner profiles. 

    b. ``Scenario.xosc``:

        * The scenario set-up contains the initial positions und velocities of the ego and scenario agent.
        * The trajectory that defines the cut-in maneuver of the scenario agent is defined and editable here.
        * The overall simulation time, which determines the end condition of the simulation in seconds, can be adjusted.

    c. ``simulationConfig.xml``:

        * The number of invocations can be changed in case more than one run is desired to be simulated. This will incorporate stochastic variation (i.e. initial constellation of surrounding traffic)
        * If surrounding traffic is not desired in the experiment, the spawner libraries "SpawnPointPreRunCommon" and "SpawnPointRuntimeCommon" can be deactivated by removing the corresponding sections. Only the "SpawnPointScenario" is mandatory to spawn the ego and scenario agent. More information on the functionality of spawners can be found in :ref:`components_spawner`.
        * The output format can be modified by setting the parameter "LoggingCyclicsToCsv" to true.

#. Start the simulation by double-clicking ``opSimulation.exe`` or from the console by calling the executable.

#. Once the simulation is successfully completed, the following results can be found in the directory ``results``:

    * ``simulationOutput.xml``: Contains general information about the experiment and an overview on all agents from the simulation. Further, an event log is contained. If the csv-output is set to false in the ``simulationConfig.xml``, the ``simulationOutput.xml`` will also include the „cyclics” (state in each time step) of the simulation.
    * ``Cyclics_Run_xxx.csv``: In case the csv-output is activated, the „cyclics” of each run in the simulation are logged to a separated csv-file. This file is missing, if "cyclics" are written directly to the ``simulationOutput.xml`` (i.e. when "LoggingCyclicsToCsv" is set to false).
    * ``Repository_Run_xxx.csv``: Overview of the agents and objects from the simulation as well as some details on scenery components like lane markings, guard rails, etc.

