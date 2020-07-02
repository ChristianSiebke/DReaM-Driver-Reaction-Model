\page dev_framework_modules_spawnpoints SpawnPoints

\tableofcontents

[//]: <> "Please sort by alphabet."

\section dev_framework_modules_spawnpoints_framework Framework

The current SpawnPoint setup allows users to generically add or remove SpawnPoints.
The SpawnPointNetwork can hold multiple SpawnPointLibraries. 
These SpawnPointLibraries can generate multiple SpawnPoint instances.
All SpawnPoint instances are instantiated before each invocation.

![SpawnPoint Classes ](SpawnPointClasses.svg)

The SpawnPoints are responsible for interacting directly with the SpawnPoint dependencies (e.g. World, AgentFactory) in order to instantiate new Agents.
The SpawnPoints are configured through the SpawnPointsConfig in the SlaveConfig file. 
If the Profile tag is existing, the related SpawnPoint profile with the specified name from the ProfilesCatalog is passed to the SpawnPoint as a ParameterInterface.
If the Profile tag does not exist, the Scenario file is passed to the SpawnPoint instead.

![SpawnPoint Dependencies ](SpawnPointDependencies.svg)

**Example SpawnPointsConfig (within slaveConfig.xml):**

```xml
<SpawnPointsConfig>
	<SpawnPoint>
		<Library>SpawnPointScenario_OSI</Library>
		<Type>PreRun</Type>
		<Priority>1</Priority>
	</SpawnPoint>
</SpawnPointsConfig>

<SpawnPointsConfig>
	<SpawnPoint>
		<Library>SpawnPointPreRunCommon_OSI</Library>
		<Type>PreRun</Type>
		<Priority>0</Priority>
		<Profile>ExamplePreRunProfile</Profile>
	</SpawnPoint>
</SpawnPointsConfig>

<SpawnPointsConfig>
	<SpawnPoint>
		<Library>SpawnPointRuntimeCommon_OSI</Library>
		<Type>Runtime</Type>
		<Priority>0</Priority>
		<Profile>ExampleRuntimeProfile</Profile>
	</SpawnPoint>
</SpawnPointsConfig>
```


| Name     | Description |
|----------|-------------|
| Library  | Library name of the SpawnPoint |
| Type     | Either "PreRun" or "RunTime". PreRun triggers only once before the invocation starts. RunTime continuously triggers during the run. |
| Priority | Determines the order in which SpawnPoints are triggered. SpawnPoints with higher priority are triggered first. Greater values equal higher priority. |
| Profile  | Required only for the [PreRunCommon](\ref dev_framework_modules_spawnpoints_preruncommonspawnpoint) and [RuntimeCommon](\ref dev_framework_modules_spawnpoints_runtimecommonspawnpoint) SpawnPoints. Do not define for the [Scenario](\ref dev_framework_modules_spawnpoints_scenariospawnpoint) SpawnPoint.|

There are 3 types of SpawnPoints:
1. [Scenario](\ref dev_framework_modules_spawnpoints_scenariospawnpoint)
2. [PreRunCommon](\ref dev_framework_modules_spawnpoints_preruncommonspawnpoint)
3. [RuntimeCommon](\ref dev_framework_modules_spawnpoints_runtimecommonspawnpoint)

\section dev_framework_modules_spawnpoints_scenariospawnpoint ScenarioSpawnPoint

The Scenario SpawnPoint (included in library "SpawnPointScenario_OSI") is responsible for spawning Ego and Scenario vehicles as defined in the Scenario configuration file. 
It is only called once initially and is recommended for each simulation.
This SpawnPoint should trigger before any other SpawnPoints.
A position and velocity are required for each Ego and Scenario agent in the scenario file.
The portion of the Scenario file relevant to this SpawnPoint can be found [here](\ref scenario_entities).

\section dev_framework_modules_spawnpoints_preruncommonspawnpoint PreRunSpawnPoint
The PreRun Spawnpoint (included in library "SpawnPointPreRunCommon_OSI") is responsible for populating the scenery/world with Common-Agents before the simulator starts.
This SpawnPoint only acts once before the simulator starts and not during the simulation run.
A traffic config is required for each PreRun SpawnPoint.

The PreRunCommon SpawnPoint requires a Spawner-Profile in the ProfilesCatalog. The SpawnerProfile contains information specifying the SpawnAreas and TrafficGroups

```xml
<ProfileGroup Type="TrafficGroup">
    <Profile Name="ExampleTrafficGroup">
      <List Name="AgentProfiles">
        <ListItem>
          <String Key="Name" Value="LuxuryClassCarAgent"/>
          <Double Key="Weight" Value="0.4"/>
        </ListItem>
        <ListItem>
          <String Key="Name" Value="MiddleClassCarAgent"/>
          <Double Key="Weight" Value="0.6"/>
        </ListItem>
        <ListItem>
          <String Key="Name" Value="TruckAgent"/>
          <Double Key="Weight" Value="0.0"/>
        </ListItem>
      </List>
      <NormalDistribution Key="Velocity" Max="68.5" Mean="43.5" Min="18.5" SD="5.0"/>
      <LogNormalDistribution Key="TGap" Max="0.5" Min="0.5" Mu="0.5" Sigma="0.5"/>
      <DoubleVector Key="Homogeneity" Value="0.8, 0.7"/>
      <Bool Key="RightLaneOnly" Value="true"/>
    </Profile>
</ProfileGroup>
<ProfileGroup Type="Spawner">
    <Profile Name="ExamplePreRunSpawner">
      <List Name="SpawnPoints">
        <ListItem>
          <String Key="Road" Value="1"/>
          <IntVector Key="Lanes" Value="-1,-2,-3,-4,-5"/>
          <Double Key="SStart" Value="0.0"/>
          <Double Key="SEnd" Value="1000.0"/>
        </ListItem>
      </List>
      <List Name="TrafficGroups">
        <ListItem>
          <Double Key="Weight" Value="1"/>
          <Reference Type="TrafficGroup" Name="ExampleTrafficGroup"/>
        </ListItem>
      </List>
    </Profile>
</ProfileGroup>
```

The SpawnAreas are defined by the following parameter:

| Name            | Description |
|-----------------|-------------|
| Road            | The RoadID of the Road on which to spawn Agents |
| Lanes           | The LaneIDs of the Lanes of the Road on which to spawn Agents |
| SStart         | The S position specifying the minimum S for the range within which to spawn Agents |
| SEnd           | The S position specifying the maximum S for the range within which to spawn Agents |

The TrafficGroups are defined by the following parameter:

| Name            | Description |
|-----------------|-------------|
| AgentProfiles   | A set of <ListItem>s which define potential AgentProfile values for the Agents spawned in the SpawnArea and the probability at which the TrafficVolume will be selected |
| Velocity    	  | A stochastic distribution describing the velocity in m/s of the spawned Agents |
| TGap      	  | A stochastic distribution describing the gap in seconds between spawned Agents |
| Homogeneity     | OPTIONAL: A vector describing the velocity increments for left lanes |
| RightLaneOnly   | OPTIONAL: A flag determining whether this TrafficGroup can only be applied to the right most lane |

The PreRunCommon SpawnPoint will spawn common agents on the specified Lanes of the specified Road from the s position S-Start to the s position S-End with Traffic Parameters sampled from the specified TrafficVolumes, PlatoonRates, Velocities, and AgentProfiles lists with the following restrictions:

- If the Scenario Spawn Point spawned Scenario Agents (including the Ego agent) before this Spawn Point is triggered (in the intended order of these Spawn Points), ranges between the Scenario Agents are invalid for spawning by this Spawn Point.
The spawn ranges will only be augmented by Scenario Agents on the same Road and Lane.
As such, there are 7 different potential scenarios that may arise in terms of how the spawn point's valid spawn ranges may be impacted:
    1. Two Scenario Agents on the same Road and Lane - one before S-Start position and one after S-End position
        - This invalidates the entirety of the spawning range; no agents may be spawned here
    1. Two Scenario Agents on the same Road and Lane - one between S-Start position and S-End position and one either before S-Start or after S-End
        - The only valid spawn range is that on the opposite side of the in-specified-range Agent from the outside-specified-range agent
    1. Two Scenario Agents on the same Road and Lane - both within the specified S-Start and S-End positions
        - The valid spawn ranges are between S-Start and the first car and between the second car and S-End
    1. Two Scenario Agents on the same Road and Lane - both outside the specified S-Start and S-End positions on the same side (both before S-Start or both after S-End)
        - The specified spawn range is entirely valid
    1. One Scenario Agent on the same Road and Lane - within specified S-Start and S-End positions
        - The valid spawn ranges include all but the bounds of the Agent within the specified S-Start and S-End positions
    1. One Scenario Agent on the same Road and Lane - outside specified S-Start and S-End positions
        - The specified spawn range is entirely valid
    1. No Scenario Agents on the same Road and Lane
        - The specified spawn range is entirely valid
- If a non-existent road is specified, no spawning will occur
- If only non-existent lanes for an existent road are specified, no spawning will occur
- If some specified lanes exist and some are non-existent for an existent road, spawning will occur for the lanes which do exist
- If the specified S-Start and S-End positions are either beyond or before the S positions at which the specified Road and Lane combination exists, no spawning will occur
- In the situation where a section of a Road adds a new lane to the left of the currently existing lanes, one should be aware that the laneId "-1" will shift to the newest left lane and should adjust SpawnPoint profiles with this in mind

\section dev_framework_modules_spawnpoints_runtimecommonspawnpoint RuntimeSpawnPoint
The Runtime SpawnPoint (included in library "SpawnPointRuntimeCommon_OSI") is responsible for maintaining a populated scenery throughout the simulation runtime.
This SpawnPoint acts at each timestep throughout the simulation run and attempts to spawn Common Agents at the specified location(s).

The PreRunCommon SpawnPoint requires a Spawner-Profile to be defined in the ProfilesCatalog. The Spawner-Profile contains all SpawnPoints and all possible TrafficGroups:

```xml
<ProfileGroup Type="TrafficGroup">
    <Profile Name="ExampleTrafficGroup">
      <List Name="AgentProfiles">
        <ListItem>
          <String Key="Name" Value="LuxuryClassCarAgent"/>
          <Double Key="Weight" Value="0.4"/>
        </ListItem>
        <ListItem>
          <String Key="Name" Value="MiddleClassCarAgent"/>
          <Double Key="Weight" Value="0.6"/>
        </ListItem>
        <ListItem>
          <String Key="Name" Value="TruckAgent"/>
          <Double Key="Weight" Value="0.0"/>
        </ListItem>
      </List>
      <NormalDistribution Key="Velocity" Max="68.5" Mean="43.5" Min="18.5" SD="5.0"/>
      <LogNormalDistribution Key="TGap" Max="0.5" Min="0.5" Mu="0.5" Sigma="0.5"/>
      <DoubleVector Key="Homogeneity" Value="0.8, 0.7"/>
      <Bool Key="RightLaneOnly" Value="true"/>
    </Profile>
</ProfileGroup>
<ProfileGroup Type="Spawner">
    <Profile Name="ExamplePreRunSpawner">
      <List Name="SpawnPoints">
        <ListItem>
          <String Key="Road" Value="1"/>
          <IntVector Key="Lanes" Value="-1,-2,-3,-4,-5"/>
          <Double Key="SCoordinate" Value="0.0"/>
        </ListItem>
      </List>
      <List Name="TrafficGroups">
        <ListItem>
          <Double Key="Weight" Value="1"/>
          <Reference Type="TrafficGroup" Name="ExampleTrafficGroup"/>
        </ListItem>
      </List>
    </Profile>
</ProfileGroup>
```

The SpawnPoints are defined by the following parameter:

| Name            | Description |
|-----------------|-------------|
| Road            | The RoadID of the Road on which to spawn Agents |
| Lanes           | The LaneIDs of the Lanes of the Road on which to spawn Agents |
| S-Position      | The S position specifying at which point to spawn Agents |

The TrafficGroups are defined by the following parameter:

| Name            | Description |
|-----------------|-------------|
| AgentProfiles   | A set of <ListItem>s which define potential AgentProfile values for the Agents spawned in the SpawnArea and the probability at which the TrafficVolume will be selected |
| Velocity    	  | A stochastic distribution describing the velocity in m/s of the spawned Agents |
| TGap      	  | A stochastic distribution describing the gap in seconds between spawned Agents |
| Homogeneity     | OPTIONAL: A vector describing the velocity increments for left lanes |
| RightLaneOnly   | OPTIONAL: A flag determining whether this TrafficGroup can only be applied to the right most lane |

The RuntimeCommon SpawnPoint will spawn a generated Agent under the following conditions:

- the distance between the spawn position and the next agent in the lane satisfies the "gap in seconds" such that the specified traffic volume is met
- the spawning of the agent would not cause a crash: either of the spawned agent to a car further down the lane, or of a car before the spawned agent into the spawned agent

If the above conditions are not met, the agent is queued to be spawned the next time both of the conditions *are* met.
