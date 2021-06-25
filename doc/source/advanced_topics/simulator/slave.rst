..
  ************************************************************
  Copyright (c) 2021 in-tech GmbH

  This program and the accompanying materials are made
  available under the terms of the Eclipse Public License 2.0
  which is available at https://www.eclipse.org/legal/epl-2.0/

  SPDX-License-Identifier: EPL-2.0
  ************************************************************

.. _slave:

Slave
==========

.. _slave_commandlinearguments:

Command Line Arguments
-----------------------

The slave can be configured by applying the following command line arguments.  
Unspecified arguments will be defaulted (*default values in []*).

The slave supports the following arguments:

* *--logLevel* [0] :
  Logging level between 0 (minimum) and 5 (maximum - debug core)
* *--logFile* [OpenPassSlave.log]* :
  Name of the log file
* *--lib* [lib] :
  Path of the libraries (relative or absolute)
* *--configs* [configs] :
  Path for writing outputs (relative or absolute)
* *--results* [results] :
  Path for writing outputs (relative or absolute)

.. _slave_scheduler:

Scheduler
---------

The Scheduler handles the sequence of agent based and common tasks before, during and after simulation.

Executing phases
~~~~~~~~~~~~~~~~

Each simulation run splits up into 3 phases: Bootstrap tasks, Stepping and Finalize tasks.

1. Initial task are grouped within the bootstrap tasks and are executed first and only once.
   This includes updating the Observation modules and triggering all PreRun Spawners.

2. After this Spawning, PreAgent, NonRecurring, Recurring and Syncronize Tasks are processed for each timestep.
   First all Runtime Spawners are triggered.
   Whenever a new agent is spawned, all its components are parsed to *Trigger* and *Update* tasks, grouped and sorted by execution time as non-recurring and recurring tasks.
   Next all PreAgents task are executed, which includes publishing the world state to the DataStore in triggering all EventDetectors and Manipulators
   Non-recurring tasks are executed just once and will be deleted afterwards (*init* flag in ComponentRepository).
   Right after execution of all agent-based tasks, the Syncronize phase synchronizes all changes to the world as world state for the next timestep.

3. The last phase is finalizing task, executed when an end condition is reached, such as end of simulation duration. Within this phase all EventDetectors and Manipulators are triggered and the Observation modules are updated one last time.

Task type description
~~~~~~~~~~~~~~~~~~~~~

The scheduler handles 8 different task types:

* **Spawning** - triggers agent spawning
* **EventDetector** - execute event detector
* **Manipulator** - execute manipulator
* **Observation** - update observation modules
* **Trigger** - execute trigger function of connected component
* **Update** - execute update output of connected component and resulting input
* **SyncWorld** - update world

The following table gives an overview to all phases.

.. table::
   :class: tight-table
   
   ================== ========== ======================================================== ===================================================================================
   Phase              Changeable Task types                                               Notes
   ================== ========== ======================================================== ===================================================================================
   Bootstrap          no         Spawning (PreRun), Observation
   Spawning           no         Spawning (Runtime), Observation 
   PreAgent           no         SyncWorld (publishing), EventDetector, Manipulator 
   Non recurring      yes        Trigger, Update                                          Only components defined as "Init"
   Recurring          yes        Trigger, Update                                          Non Init components
   Syncronize         no         SyncWorld                                                Update the state of the agents and the virtual world (e.g. due to agent movements).
   Finalize           no         EventDetector, Manipulator, Observation 
   ================== ========== ======================================================== ===================================================================================

Task priorities
~~~~~~~~~~~~~~~

The order of the tasks within one phase depends on component priority and task type.
All tasks for a given timestamp are ordered by the SchedulerTasks priority, and divided into the aformentioned phases.

The following table shows the priorities for the non-component task types:

.. table::
   :class: tight-table
   
   =============== ======== =================================================================================================
   Task type       Priority Notes
   =============== ======== =================================================================================================
   Spawning        4        Highest priority, nothing can happen before any agent is instantiated. 
   EventDetector   3  
   Manipulator     2        The manipulator task uses the event detector tasks as input. Therefore it is called afterwards. 
   SyncGlobalData  1 
   Observation     0        Observation tasks have to be execute at the end.
   =============== ======== =================================================================================================

The priority of trigger and update tasks depend on the priority within the SystemConfig. 
They are independent of non-component priorities.
Since trigger tasks prepare output signals, output tasks are right called after the corresponding trigger tasks.
