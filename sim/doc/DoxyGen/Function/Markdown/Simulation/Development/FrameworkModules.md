\page dev_framework_modules Framework Modules

\tableofcontents

[//]: <> "Please sort by alphabet."

The modules of openPASS are split the two major categories `Framework Modules` and `Agent Modules` and defined within [SystemConfigBlueprint](\ref io_input_systemconfigblueprint).
`Framework Modules` are instantiated once for the entire simulation  and **all of them are necessary**.
`Agent Modules` are instantiated individually for each agent and as such, it depends on the agents configuration which are necessary.

This section covers the **Framework Modules**, the **Agent Modules** can be found [here](@ref dev_agent_modules).

\section dev_framework_modules_agentfactory AgentFactory

The AgentFactory instantiates and manages all Agent instances.
When a SpawnPoint wants to create a new Agent, it forwards an AgentBlueprint to the AgentFactory.
This AgentBlueprint is then forwarded to the AgentFactory, which instantiates the Agent and all its components.
In order to instantiate an Agent, all required Channels and Components which are stored in the AgentBlueprint, need to be instantiated.
After the Channels and Components are created, the AgentFactory establishes the links between the different components.

\section dev_framework_modules_agentBlueprintProvider AgentBlueprintProvider

The AgentBlueprintProvider is responsible for configuring agents and gathering the required components based on the [ProfilesCatalog](\ref io_input_profilescatalog) and stochastic values.

When creating a new Agent the [SpawnPoint](\ref dev_framework_modules_spawnpoint) is responsible for filling out the first part of the AgentBlueprint.
Initially the SpawnPoint determines in which lane the agent is supposed to be spawned. 
After that, the SpawnPoint calls the AgentBlueprintProvider to fill out the second part of the AgentBlueprint.

\subsection Sequence
First the AgentProfilesSampler samples the name of the AgentProfile. An AgentProfile can either be static or dynamic. In the case of a static AgentProfile the components and channels of the agent are defined
by a system config. In this cause the AgentBlueprintProvider loads the specified system via fileName and system id. Additionally the AgentProfile defines the VehicleModel.

In the case of a dynamic AgentProfile additional profiles need to be sampled. This is handled by the DynamicProfileSampler. After that the DynamicParameterSampler samples probabilistic parameters like sensor latencies.
Then the AgentType with the components and channels is build by the DynamicAgentTypeGenerator. The VehicleModel is in this case defined via the VehicleProfile.

![AgentBlueprintProviderSequenceDiagram](AgentBlueprintProviderSequence.svg)

\section dev_framework_modules_databuffer DataBuffer

The DataBuffer accepts arbitrary values from different components (from framework as well as agents).
The stored values if cyclic and acyclic type are associated with an entity id and a key (topic) and stored for one timestep.
Static values are only associated with a key but stored for the entire run.

Typically, Observers are using the DataBuffer to retrieve information about the simulation.
This can happen during the simulation run and/or at the end of a run.
For the keys, a generic datatype is used (strings).
For the values, a set of predefined datatypes is used (bool, char, int, size_t, float, double, string).
A value can also hold a vector of one of the mentioned datatypes.

For information about the usage, please refer to the implementation's inline documentation.


\section dev_framework_modules_eventdetectors EventDetectors

[//]: <> "Please refer to each section!"
* [ConditionalEventDetector](\ref dev_conditionaleventdetector)
* [CollisionDetector](\ref dev_framework_modules_eventdetectors_collision)

EventDetectors are used to trigger Manipulators. They are waiting for specific behaviors or settings during the simulation. Once an Event occurs it is forwarded to the related Manipulator.

EventDetectors can be specified to only trigger for certain Agents in the Scenario.xosc.

```xml
    <Actors>
        <Entity name="Ego"/>
    </Actors>
```

The "Actors"-tag is mandatory, but the "Entity"-tags are optional.
If no Actors are specified, the Event is triggered for all Agents.

With the "name"-Attribute of an "Entity" a specific Agent can be targeted by the event. This attribute relates to the Ego and Scenario Agents defined in the Scenario.xosc.

The class structure of the EventDetectors is showed in the following diagram:

![EventDetectors](EventDetectors.svg)

---

\subsection dev_framework_modules_eventdetectors_collision CollisionDetector
* **Description:**
    Checks if a collision occured. It is always activated and **cannot be deactivated**.

* **Trigger:** 
    Always active, with cycle time 100 [ms]

* **Configuration:**

    |ParameterName|Type|Description|
    |-----------|------|-----------|
    |-|-|-|

* **Event:** CollisionEvent (CollisionEvent)
  Parameter:

  |Name|Description|
  |-----|------------|
  |CollisionWithAgent|Determines whether this collision involved two agents or one agent and one traffic object.|
  |CollisionAgentId|Agent id of the first agent.|
  |CollisionOpponentId|Id of either the second agent or the traffic object.|

---

\subsection dev_framework_modules_eventdetectors_conditional ConditionalEventDetector

For a detailed description on ConditionalEventDetectors, and the Conditions which trigger them, please refer to the [Scenario](\ref scenario_storyboard_story_maneuver_conditions) page.

---



\section dev_framework_modules_eventnetwork EventNetwork

The EventNetwork is the central module which manages all Events. It makes the Events available for multiple other framework modules.
All new Events are passed to the EventNetwork where they are stored as active events.
After each time step the EventNetwork archives the currently active events.
Additionally the EventNetwork offers several methods and filters to access the events.


List of Modules which use the EventNetwork:
1. ObservationLog
3. EventDetector
4. Manipulator
5. VehicleControlUnit


![EventNetwork](EventNetworkClassdiagram.svg)

---

\section dev_framework_modules_manipulators Manipulators

[//]: <> "Please refer to each section!"
* [CollisionManipulator](\ref dev_framework_modules_manipulators_collision)
* [ComponentStateChangeManipulator](\ref dev_framework_modules_manipulators_componentstatechange)

The class structure of the Manipulators is showed in the following diagram:

![Manipulators](ManipulatorsOS.svg)

---

\subsection dev_framework_modules_manipulators_collision CollisionManipulator
* **Description:**
    This manipulator notifies the two collided agents and recursively all of their other collisionspartners about the new collision

* **Trigger:** This manipulator is always triggered by CollisionDetector.

* **Event:** -

* **Configuration:**

    |ParameterName|Type|Description|
    |-----------|------|-----------|
    |-|-|-|

---

\subsection dev_framework_modules_manipulators_componentstatechange ComponentStateChangeManipulator
* **Description:**
    This manipulator modifies the state of a vehicle component.

* **Trigger:** This manipulator can be triggered by any detector. This has to be parameterized in scenario configuration (see [Input/Output documentation](\ref io_input_scenario)).

* **Event:** Creates an event of type ComponentStateChangeEvent for the specified component name.

* **Configuration:**
```xml
<Action name="ExampleComponentStateChangeManipulator">
	<UserDefined>
		<Command>SetComponentState DynamicsTrajectoryFollower Acting</Command>
	</UserDefined>
</Action>
```

|ElementName|Attribute|Type  |Description                                                   |
|-----------|---------|------|--------------------------------------------------------------|
|Command    |-        |string|This string must be formatted as "SetComponentState <TargetComponent> <DesiredState>", where <TargetComponent> is the target component name and <DesiredState> is one of "Acting", "Armed", or "Disabled".|

---

\section dev_framework_modules_observationlog ObservationLog

The ObservationLog is responsible for generating the [simulation output](\ref io_output_simout). This module handles the file stream to the output file.
Which parameters are logged in the cyclics tag is specified by the [LoggingGroups tag](\ref io_input_slaveconfig_experimentconfig_logginggroups) in the SlaveConfig

The output is initially written into a temporary simulation output file. The temporary output is updated at the end of every invocation. Once all invocations completed successfully the temporary file is renamed to SimulationOutput.xml.

All information which needs to be written into the log is forwarded to an instance of ObservationInterface. This includes RunStatistics, Events and Agent parameters.
Additionally to the logging the ObservationLog is also used by other modules to add their Events to the EventNetwork.

---

\section dev_framework_modules_sampler Sampler

The Sampler contains the entire stochastic functionality of the AgentBlueprintProvider and the SpawnPoint. 
Additionally it prepares the World-Parameters, which are defined based on probabilities in the SlaveConfig.
The AgentBlueprintProvider uses this module to pick values based on a set of probabilities, which are defined in the SlaveConfig.
The SpawnPoint uses this module to generate values based on different distributions.

The Sampler uses the Stochastics module to generate random values.

![Sampler](SamplerClassdiagram.svg)

---

\section dev_framework_modules_spawnpoint SpawnPoint

For information on the current SpawnPoints, please refer the SpawnPoint [documentation](\ref dev_framework_modules_spawners).

---

\section dev_framework_modules_stochastics Stochastics

The Stochastics Module is used to created random numbers. A random seed is used to initialize a Mersenne Twister 19937 generator.
Afterwards numbers are drawn from said generator.

The following types of random numbers are currently available:

[//]: <> "Please refer to each section!"
* Uniform: Expects upper and lower boundary
* Normal: Expects means and standard deviation
* Binomial: Expects upper range number and probability
* Exponential: Expects lambda
* Gamma: Expects mean and standard deviation
* LogNormal: Expects mean and standard deviation
* RandomCdfLogNormal: Expects mean and standard deviation
* PercentileLogNormal: Expects mean, standard deviation and probability

![Stochastics](StochasticsClassdiagram.svg)

---

\section dev_framework_modules_world World
\subsection dev_framework_modules_world_interfacesadapter Interfaces and Adapter

![World Interfaces and Adapter](World.svg)

\subsection dev_framework_modules_world_sampling Sampling of World Geometries

Roads are described following the [OpenDRIVE](http://www.opendrive.org/) specification.
There the geometry of a road is defined algebraically as lines, clothoids and by means of cubic polynomials, whereby primarily only one reference line is defined.
The lanes can be understood as a stack of parallel lanes, which are given by the width and the offset, now in relation to this reference line, which acts as the underlying coordinate system ('s/t' road coordinates with s along the road).
Here, too, width and offset are defined algebraically, which means that almost any boundary lines can be defined.

When the world is initialized, these boundary definitions (i.e. algebraic geometries) are converted into piecewise linear elements, which is called sampling.
The road is scanned at a constant interval along 's', which leads to four-sided (quadrangular) sections of the lanes at common 's' coordinates, so-called lane elements (see LaneElement).
The scanning is carried out at a scanning rate of 10 centimeters with the aim of achieving a total scanning error of less than 5 centimeters, as required by the representation used internally (c.f. [open simulation interface](https://github.com/OpenSimulationInterface)).
Note that this error is only guaranteed if geometries do not exhibit extreme curvatures, i.e. a deviation of more than 5 cm within two sampling points (10 cm along s).
The scanned points define so-called joints, which contain all scanned points at an 's' coordinate across all lane boundaries of the given road.
The number of these joints is reduced by a [Ramer-Douglas-Peucker algorithm](https://en.wikipedia.org/wiki/Ramer%E2%80%93Douglas%E2%80%93Peucker_algorithm), which ensures that the maximum lateral error of each individual point within a joint is less than 5 cm compared to the originally scanned points.
Note that (a) the boundary points of geometries are always retained and (b) additional points for lane marking transitions are also retained to ensure the maximum accuracy of these edge cases.
The lane elements are generated with two successive connections, which are ultimately used in the localization at runtime (see [below](\ref dev_framework_modules_world_localization)).

Internally, each lane element receives a constant direction, which is defined by the direction of the vector spanned between the centers of the corresponding connections.
Each joint also holds the corresponding curvature of the road, so that the curvature can be interpolated linearly within a lane element along the 's' coordinate.

\subsection dev_framework_modules_world_localization Localization

Generally, the position of an agent is stored with respect to [world coordinates (x,y)](\ref dev_concepts_coordinatesystems_world).
As queries on the world operates in [road coordinates (s,t)](\ref dev_concepts_coordinatesystems_road), the position of the agent needs to be transformed.
The transformation is performed by the [localization algorithm](\ref localization).

\subsection dev_framework_modules_world_trafficsigns Traffic Signs, Road Markings and TrafficLights

The world currently supports a variety of traffic signs, road markings and traffic lights.
All of these are defined in OpenDRIVE as "RoadSignal".
At the moment it can only interpret traffic signs and road markings according to the German regulations "StVo" and traffic lights according the the OpenDRIVE appendix.
Traffic signs can contain optional supplementary traffic signs. Supplementary signs are dependent on a main traffic sign and contain additional information.
The following traffic signs are supported:

| TrafficSign                                   | StVo Type | Subtype     | Value and Units   |
|-----------------------------------------------|-----------|-------------|-------------------|
| GiveWay                                       | 205       | -           | -                 |
| Stop                                          | 206       | -           | -                 |
| DoNotEnter                                    | 267       | -           | -                 |
| EnvironmentalZoneBegin                        | 270.1     | -           | -                 |
| EnvironmentalZoneEnd                          | 270.2     | -           | -                 |
| MaximumSpeedLimit                             | 274       | X           | The subtype "X" is used to define the speedlimit in km/h. Afterwards the world converts it to m/s. |
| SpeedLimitZoneBegin                           | 274.1     | -/20        | The subtype is used to define the speedlimit in km/h. Afterwards the world converts it to m/s. No subtype = 30km/h, 20 = 20km/h |
| SpeedLimitZoneEnd                             | 274.2     | -/20        | The subtype is used to define the speedlimit in km/h. Afterwards the world converts it to m/s. No subtype = 30km/h, 20 = 20km/h |
| MinimumSpeedLimit                             | 275       | X           | The subtype is used to define the speedlimit in km/h. Afterwards the world converts it to m/s. |
| OvertakingBanBegin                            | 276       | -           | -                 |
| OvertakingBanTrucksBegin                      | 277       | -           | -                 |
| EndOfMaximumSpeedLimit                        | 278       | X           | The subtype "X" is used to define the speedlimit in km/h. Afterwards the world converts it to m/s. |
| EndOfMinimumSpeedLimit                        | 279       | X           | The subtype "X" is used to define the speedlimit in km/h. Afterwards the world converts it to m/s. |
| OvertakingBanEnd                              | 280       | -           | -                 |
| OvertakingBanTrucksEnd                        | 281       | -           | -                 |
| EndOffAllSpeedLimitsAndOvertakingRestrictions | 282       | -           | -                 |
| RightOfWayNextIntersection                    | 301       | -           | -                 |
| RightOfWayBegin                               | 306       | -           | -                 |
| RightOfWayEnd                                 | 307       | -           | -                 |
| TownBegin                                     | 310       | -           | This sign contains a text describing the name of the town |
| TownEnd                                       | 311       | -           | This sign contains a text describing the name of the town |
| TrafficCalmedDistrictBegin                    | 325.1     | -           | -                 |
| TrafficCalmedDistrictEnd                      | 325.2     | -           | -                 |
| HighWayBegin                                  | 330.1     | -           | -                 |
| HighWayEnd                                    | 330.2     | -           | -                 |
| HighWayExit                                   | 333       | -           | -                 |
| AnnounceHighwayExit                           | 448       | -           | -                 |
| HighwayExitPole                               | 450       | 50/51/52    | The subtype describes the distance to the highway exit in m. 50 = 100m, 51 = 200m, 52 = 300m |
| AnnounceRightLaneEnd                          | 531       | 10/11/12/13 | The subtype describes the number of continuing lanes after the right lane ends. 10 = 1 lane, 11 = 2 lanes, 12 = 3 lanes, 13 = 4 lanes |
| AnnounceLeftLaneEnd                           | 531       | 20/21/22/23 | The subtype describes the number of continuing lanes after the left lane ends. 10 = 1 lane, 11 = 2 lanes, 12 = 3 lanes, 13 = 4 lanes |
| DistanceIndication                            | 1004      | 30/31/32	  | For subtype 30 the value describes the distance in m. For subtype 31 the value describes the distance in km. Subtype 32 has a STOP in 100m |

The following road markings are supported:

| RoadMarking                                   | StVo Type | Subtype     | Value and Units   |
|-----------------------------------------------|-----------|-------------|-------------------|
| PedestrianCrossing                            | 293       | -           | -                 |
| Stop line                                     | 294       | -           | -                 |

The pedestrian crossing can also be defined in OpenDRIVE as object with type "crosswalk".


The following traffic lights are supported:

| TrafficLight                                  | OpenDRIVE Type | Subtype     | Value and Units   |
|-----------------------------------------------|----------------|-------------|-------------------|
| Standard traffic light (red, yellow, green)   | 1.000.001      | -           | -                 |
| Left arrows                                   | 1.000.011      | 10          | -                 |
| Right arrows                                  | 1.000.011      | 20          | -                 |
| Upwards arrows                                | 1.000.011      | 30          | -                 |
| Left und upwards arrows                       | 1.000.011      | 40          | -                 |
| Right und upwards arrows                      | 1.000.011      | 50          | -                 |

These traffic lights are controlled by OpenScenario.
\subsection dev_framework_modules_world_lanemarking Lane Markings

The world also supports lane markings (i.e. printed lines between two lanes) according to the OpenDRIVE standard.
The following attributes of the "roadMark" tag in the scenery file are stored in the world and can be retrieved by the GetLaneMarkings query: sOffset, type, weight, color.
The weight is converted into a width in meter: 0.15 for standard and 0.3 for bold. Lane markings are also converted to OSI LaneBoundaries.
For the OpenDRIVE type "solid solid", "solid broken", "broken solid", and "broken broken" two LaneBoundaries are created in OSI with a fixed lateral distance of 0.15m.

\subsection dev_framework_modules_world_getobstruction GetObstruction

The GetObstruction function calculates the lateral distance an agent must travel in order to align with either the left or right boundary of a target object occupying the same lane.

The calculation adheres to the following process:

1. Project the agent's MainLaneLocator along the lane to the nearest and furthest s-coordinate of the target object, capturing the projected points
2. Create a straight line from the two captured points
3. Calculate the Euclidean distance of each of the target object's corners to the created line
4. Return the left-most and right-most points with respect to the created line


![Example for the calculation of GetObstruction](GetObstruction.png)
