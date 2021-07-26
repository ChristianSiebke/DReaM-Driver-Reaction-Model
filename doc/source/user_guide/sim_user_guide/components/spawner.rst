..
  ************************************************************
  Copyright (c) 2021 in-tech GmbH

  This program and the accompanying materials are made
  available under the terms of the Eclipse Public License 2.0
  which is available at https://www.eclipse.org/legal/epl-2.0/

  SPDX-License-Identifier: EPL-2.0
  ************************************************************

.. _components_spawner:

Spawner
-------

Spawners are responsible to populate the world.
In order to do so, several spawners can be used, whereas only the **ScenarioSpawner** is mandatory.

.. _components_scenariospawner:

ScenarioSpawner
~~~~~~~~~~~~~~~

The ScenarioSpawner is responsible for spawning Ego and Scenario vehicles as defined in the Scenario configuration file.
It is only called once initially and is **mandatory** for each simulation.
This Spawner should trigger before any other Spawners.

The ScenarioSpawner has no parameters.

Behavior:

- In order to spawn correctly, a position for each Ego and Scenario vehicle is necessary, normally coming from the scenario (recommended).
  As the position is *optional* in openSCENARIO it is possible that it is **not defined**.
  In such cases spawning takes place at ``x = 0`` and ``y = 0``.
- If there is no route defined in the Scenario, the Spawner will set a random route starting at the spawning position.
- If there are multiple lanes at this position (this is only possible on junctions) it will take the lane with the lowest relative heading.

.. _components_prerunspawner:

PreRunSpawner
~~~~~~~~~~~~~

The PreRunSpawner is responsible for populating the scenery/world with Common-Agents before the simulator starts.
This Spawner only acts once before the simulator starts and not during the simulation run.
The PreRunSpawner needs a list of SpawnZones that define where it will spawn agents and a list of weighted :ref:`components_trafficgroups` that defines the type and parameters of the spawned agents.
The SpawnZones have the following parameters:

.. table::
   :class: tight-table

   ========= ============ ==== ======== ==================================================================================
   Parameter Type         Unit optional Description
   ========= ============ ==== ======== ==================================================================================
   Roads     StringVector      no       List of linked Roads on which to spawn Agents.
                                        Intermediate roads may be omitted.
                                        If a road is not linked to the previous in the list, this road and the roads after it are ignored
   Lanes     IntVector         yes      The LaneIDs of the Lanes of the Road on which to spawn Agents (given on SStart).
                                        Inexistent lanes are ignored.
                                        If omitted all lanes are used.
   SStart    Double       m    yes      The S position on the first road specifying the start of the range within which to spawn Agents
                                        If omitted the whole road is included
   SEnd      Double       m    yes      The S position on the last road specifying the start of the range within which to spawn Agents
   SLength   Double       m    yes      Length of spawned area calculated from SStart      
                                        This is ignored if SEnd is explicitly defined.
                                        If neither is given the whole road is included
   ========= ============ ==== ======== ==================================================================================
   
It is also possible to define the minimum gap in meters either as fixed parameter of type double or as stochastic distribution.
If it isn't defined the default value of 5m is used.

.. literalinclude:: @OP_REL_SIM@/contrib/examples/Common/ProfilesCatalog.xml
   :language: xml
   :dedent: 4
   :start-at: <Profile Name="DefaultPreRunCommon">
   :end-at: </Profile>

The PreRunCommonSpawner will spawn common agents on the specified Lanes of the specified continuous Road sequence (= RoadStream) inside the specified s interval based on the parameters of the TrafficGroups.
The following restrictions apply:

- The PreRunCommonSpawner only spawns on the following OpenDRIVE lane types: 
    - Driving
    - OnRamp
    - OffRamp
    - ConnectingRamp

- If the ScenarioSpawner spawned Scenario Agents (including the Ego agent) before this Spawner is triggered (in the intended order of these Spawners), ranges between the Scenario Agents are invalid for spawning by this Spawner.
  The spawn ranges will only be augmented by Scenario Agents on the same Lane.
  As such, there are 7 different potential scenarios that may arise in terms of how the valid spawn ranges of the spawner may be impacted:
  
  #) **Two Scenario Agents on the same Lane**

     One before S-Start position and one after S-End position:
     This invalidates the entirety of the spawning range; no agents may be spawned here

  #) **Two Scenario Agents on the same Lane**
  
     One between S-Start position and S-End position and one either before S-Start or after S-End:
     The only valid spawn range is that on the opposite side of the in-specified-range Agent from the outside-specified-range agent

  #) **Two Scenario Agents on the same Lane**
  
     Both within the specified S-Start and S-End positions
     The valid spawn ranges are between S-Start and the first car and between the second car and S-End

  #) **Two Scenario Agents on the same Lane**
  
     Both outside the specified S-Start and S-End positions on the same side (both before S-Start or both after S-End):
     The specified spawn range is entirely valid
  
  #) **One Scenario Agent on the same Lane**
  
     Within specified S-Start and S-End positions:
     The valid spawn ranges include all but the bounds of the Agent within the specified S-Start and S-End positions
     
  #) **One Scenario Agent on the same Lane**
     
     Outside specified S-Start and S-End positions:
     The specified spawn range is entirely valid

  #) **No Scenario Agents on the same Lane**
    
     The specified spawn range is entirely valid

- If only non-existent lanes on SStart are specified, no spawning will occur

- If some specified lanes exist and some are non-existent, spawning will occur for the lanes which do exist

Once the spawning ranges are determined the PreRunSpawner will spawn for each spawning area based on the following logic:

#) First the agentprofile needs to be determined. If the current spawn position evaluate to a right lane, the pool from which the agentprofile is drafted is extended by all traffic groups which contain the RightLaneOnly flag set to true.

#) Then the time gap between the new agent and the closest existing agent is sampled.

#) Afterwards the velocity of the new agent is being sampled under consideration of the homogeneity.

#) The gap and velocity are used to calculate the distance between the new agent and the next agent in this spawnarea. Here a minimum distance of 5m between agents is required.

#) A random route is sampled starting at the appropriate road

#) Based on the distance and the velocity the TTC (2s) conditions are evaluated.If the TTC is critical the spawn velocity is reduced to fulfill the TTC requirements.

#) As a final step the spawnzone evaluates the spawncoordinates. If they are valid the agent is created, else the spawnzone moves on to the next spawning range.

.. _components_runtimespawner:

RuntimeSpawner
~~~~~~~~~~~~~~~~~~~

The RuntimeSpawner (included in library "SpawnerRuntimeCommon_OSI") is responsible for maintaining a populated scenery throughout the simulation runtime.
It acts at each timestep throughout the simulation run and attempts to spawn Common Agents at the specified location(s).
The RuntimeSpawner needs a list of Spawners that define where it will spawn agents and a list of weighted :ref:`components_trafficgroups` that defines the type and parameters of the spawned agents.
The Spawners have the following parameters:

.. table::
   :class: tight-table

   ========== ============ ==== ==============================================================
   Parameter  Type         Unit Description
   ========== ============ ==== ==============================================================
   Roads      StringVector      The RoadIDs of the Roads on which to spawn Agents
   Lanes      IntVector         The LaneIDs of the Lanes of the Road on which to spawn Agents
   S-Position Double       m    The S position specifying at which point to spawn Agents
   ========== ============ ==== ==============================================================

It is also possible to define the minimum gap in meters either as fixed parameter of type double or as stochastic distribution.
If it isn't defined the default value of 5m is used.

.. literalinclude:: @OP_REL_SIM@/contrib/examples/Common/ProfilesCatalog.xml
   :language: xml
   :dedent: 4
   :start-at: <Profile Name="DefaultRuntimeCommon">
   :end-at: </Profile>

The RuntimeSpawner will spawn based on the following logic:

- First the agentprofile needs to be determined. If the current spawn position evaluates to a right lane, the pool from which the agentprofile is drafted is extended by all traffic groups which contain the RightLaneOnly flag set to true.
- Then the time gap between the new agent and the closest existing agent is sampled.
- Afterwards the velocity of the new agent is being sampled under consideration of the homogeneity.
- A random route is sampled starting at the appropriate road
- Once the timely gap expires, the spawnpoint evaluate if the TTC (2s) conditions and a minimum required distance between agents (5m) are met. If the TTC is critical the spawn velocity is reduced to fullfill the TTC requriements. If the minimum distance is not fullfilled, the agent will be held back.
- If all requirements were fullfilled the agent is spawned.

The RuntimeSpawner only spawns on the following OpenDRIVE lane types: Driving, OnRamp

.. _components_trafficgroups:

TrafficGroups
~~~~~~~~~~~~~

Both the :ref:`components_prerunspawner` and the :ref:`components_runtimespawner` need one or more TrafficGroup.
These are typically defined in a separate ProfileGroup of type "TrafficGroup" and then reference by the spawner profile.
In this way both spawner can use the same TrafficGroups.

.. table::
   :class: tight-table

   ============= ============= ==== =======================================================================================================================================================================
   Parameter     Type          Unit Description
   ============= ============= ==== =======================================================================================================================================================================
   AgentProfiles <List>             A set of <ListItem>s which define potential AgentProfile values for the Agents spawned in the SpawnArea and the probability at which the TrafficVolume will be selected
   Velocity      Distribution  m/s  A stochastic distribution describing the velocity of the spawned Agents
   TGap          Distribution  s    A stochastic distribution describing the time gap between spawned Agents
   Homogeneity   DoubleVector       OPTIONAL: A vector describing the velocity increments for left lanes
   RightLaneOnly Bool               OPTIONAL: A flag determining whether this TrafficGroup can only be applied to the right most lane
   ============= ============= ==== =======================================================================================================================================================================

.. literalinclude:: @OP_REL_SIM@/contrib/examples/Common/ProfilesCatalog.xml
   :language: xml
   :dedent: 4
   :start-at: <Profile Name="LightVehicles">
   :end-at: </Profile>
